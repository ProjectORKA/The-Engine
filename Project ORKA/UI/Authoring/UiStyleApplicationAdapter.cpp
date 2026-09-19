#include "UiStyleApplicationAdapter.hpp"

#include "../Layout/UiOverflowController.hpp"

#include <cctype>
#include <cstdlib>

namespace Ui
{
	namespace
	{
		String lower(String value)
		{
			for (Char& character : value)
				character = static_cast<Char>(std::tolower(static_cast<unsigned char>(character)));
			return value;
		}

		Bool sizeValue(const UiStyleValue& value, UiSizeSpec& output)
		{
			if (value.kind == UiStyleValueKind::Auto) { output = {}; return true; }
			if (value.kind == UiStyleValueKind::Content) { output = UiSizeSpec::content(); return true; }
			if (value.kind != UiStyleValueKind::Number) return false;
			const Float number = static_cast<Float>(value.number);
			if (value.unit == UiStyleUnit::Percent) output = UiSizeSpec::percent(number / 100.0f);
			else if (value.unit == UiStyleUnit::Fr) output = UiSizeSpec::fr(number);
			else if (value.unit == UiStyleUnit::None || value.unit == UiStyleUnit::Px ||
				value.unit == UiStyleUnit::Dp) output = UiSizeSpec::fixed(number);
			else return false;
			return true;
		}

		Bool scalar(const UiStyleValue& value, Float& output)
		{
			if (value.kind != UiStyleValueKind::Number ||
				(value.unit != UiStyleUnit::None && value.unit != UiStyleUnit::Px &&
					value.unit != UiStyleUnit::Dp)) return false;
			output = static_cast<Float>(value.number);
			return std::isfinite(output);
		}

		Bool edges(const String& text, UiEdges& output)
		{
			Vector<Float> values;
			SizeT at = 0;
			while (at < text.size())
			{
				while (at < text.size() && std::isspace(static_cast<unsigned char>(text[at]))) ++at;
				if (at >= text.size()) break;
				Char* end = nullptr;
				const Float value = std::strtof(text.c_str() + at, &end);
				if (end == text.c_str() + at || !std::isfinite(value)) return false;
				at = static_cast<SizeT>(end - text.c_str());
				if (text.compare(at, 2, "px") == 0 || text.compare(at, 2, "dp") == 0) at += 2;
				else if (at < text.size() && !std::isspace(static_cast<unsigned char>(text[at]))) return false;
				values.push_back(value);
			}
			if (values.empty() || values.size() > 4) return false;
			if (values.size() == 1) output = {values[0], values[0], values[0], values[0]};
			else if (values.size() == 2) output = {values[1], values[0], values[1], values[0]};
			else if (values.size() == 3) output = {values[1], values[0], values[1], values[2]};
			else output = {values[3], values[0], values[1], values[2]};
			return true;
		}

		Bool addClass(UiNode& node, const String& value)
		{
			if (std::find(node.authored.classes.begin(), node.authored.classes.end(), value) != node.authored.classes.end())
				return false;
			node.authored.classes.push_back(value);
			return true;
		}
	}

	UiStyleApplicationAdapter::UiStyleApplicationAdapter(UiDocument& newDocument, UiLayoutEngine& newLayout)
		: document(newDocument), layout(newLayout)
	{
	}

	void UiStyleApplicationAdapter::diagnose(String message, const SourceSpan& span)
	{
		if (styleDiagnostics.size() < DiagnosticSink::maximumDiagnostics)
			styleDiagnostics.push_back({ DiagnosticSeverity::Warning, std::move(message), span });
	}

	const UiPaintStyleTokens* UiStyleApplicationAdapter::paintTokens(const UiNodeHandle node) const
	{
		const auto found = tokens.find(node);
		return found == tokens.end() ? nullptr : &found->second;
	}

	void UiStyleApplicationAdapter::apply(const UiCompiledStyleSheet& sheet, UiNodeHandle root)
	{
		styleDiagnostics.clear();
		tokens.clear();
		if (!root.isValid()) root = document.root();

		Map<UiNodeHandle, UiStyleNodeDescriptor> descriptors;
		document.traverse([&](const UiNodeHandle handle, UiNode& node)
			{
				UiStyleNodeDescriptor descriptor;
				descriptor.type = node.authored.styleType.empty() ?
					node.authored.typeName : node.authored.styleType;
				descriptor.id = node.authored.id;
				descriptor.classes = node.authored.classes;
				if (node.state.hovered) descriptor.pseudoStates |= static_cast<UInt>(UiStylePseudoState::Hover);
				if (node.state.active) descriptor.pseudoStates |= static_cast<UInt>(UiStylePseudoState::Active);
				if (node.state.focused) descriptor.pseudoStates |= static_cast<UInt>(UiStylePseudoState::Focus);
				if (!node.state.enabled) descriptor.pseudoStates |= static_cast<UInt>(UiStylePseudoState::Disabled);
				if (node.state.semantics.selected)
					descriptor.pseudoStates |= static_cast<UInt>(UiStylePseudoState::Selected);
				if (node.state.semantics.checked)
					descriptor.pseudoStates |= static_cast<UInt>(UiStylePseudoState::Checked);
				descriptors[handle] = std::move(descriptor);
			});
		for (auto& [handle, descriptor] : descriptors)
		{
			const UiNode* node = document.get(handle);
			const auto parent = node ? descriptors.find(node->parentHandle()) : descriptors.end();
			if (parent != descriptors.end()) descriptor.parent = &parent->second;
		}

		Map<UiNodeHandle, UiResolvedStyle> resolved;
		for (auto& [handle, descriptor] : descriptors)
		{
			const UiNode* node = document.get(handle);
			const UiResolvedStyle* parentStyle = nullptr;
			if (node)
			{
				const auto parent = resolved.find(node->parentHandle());
				if (parent != resolved.end()) parentStyle = &parent->second;
			}
			resolved[handle] = UiStyleCompiler::resolve(sheet, descriptor,
				{ parentStyle, [](const String& property)
					{ return property == "color" || property == "font-size"; } });
			UiLayoutStyle style = layout.style(handle);
			UiPaintStyleTokens paint;

			for (const auto& [propertyName, declaration] : resolved[handle].declarations)
			{
				const String property = lower(propertyName);
				const UiStyleValue& value = declaration.typedValue;
				Bool supported = true;
				if (property == "width") supported = sizeValue(value, style.width);
				else if (property == "height") supported = sizeValue(value, style.height);
				else if (property == "min-width") supported = sizeValue(value, style.minimumWidth);
				else if (property == "min-height") supported = sizeValue(value, style.minimumHeight);
				else if (property == "max-width") supported = sizeValue(value, style.maximumWidth);
				else if (property == "max-height") supported = sizeValue(value, style.maximumHeight);
				else if (property == "left") supported = sizeValue(value, style.left);
				else if (property == "top") supported = sizeValue(value, style.top);
				else if (property == "right") supported = sizeValue(value, style.right);
				else if (property == "bottom") supported = sizeValue(value, style.bottom);
				else if (property == "padding") supported = edges(declaration.value, style.padding);
				else if (property == "margin") supported = edges(declaration.value, style.margin);
				else if (property == "gap") { Float gap = 0; supported = scalar(value, gap); if (supported) style.gap = Vec2(gap); }
				else if (property == "flex-grow") supported = scalar(value, style.flexGrow);
				else if (property == "flex-shrink") supported = scalar(value, style.flexShrink);
				else if (property == "flex-basis") supported = sizeValue(value, style.flexBasis);
				else if (property == "display")
				{
					const String name = lower(value.text);
					if (name == "flex") style.display = UiDisplay::Flex;
					else if (name == "grid") style.display = UiDisplay::Grid;
					else if (name == "overlay") style.display = UiDisplay::Overlay;
					else if (name == "none") style.display = UiDisplay::None;
					else supported = false;
				}
				else if (property == "flex-direction")
				{
					const String name = lower(value.text);
					if (name == "row") style.flexDirection = UiAxis::Row;
					else if (name == "column") style.flexDirection = UiAxis::Column;
					else supported = false;
				}
				else if (property == "flex-wrap")
				{
					const String name = lower(value.text);
					if (name == "wrap") style.flexWrap = UiWrap::Wrap;
					else if (name == "balance") style.flexWrap = UiWrap::Balance;
					else style.flexWrap = UiWrap::NoWrap;
				}
				else if (property == "position")
				{
					if (lower(value.text) == "absolute") style.position = UiPositionType::Absolute;
					else if (lower(value.text) == "flow" || lower(value.text) == "relative") style.position = UiPositionType::Flow;
					else supported = false;
				}
				else if (property == "align-items" || property == "align-self")
				{
					UiAlign align = UiAlign::Auto;
					const String name = lower(value.text);
					if (name == "start" || name == "flex-start") align = UiAlign::Start;
					else if (name == "center") align = UiAlign::Center;
					else if (name == "end" || name == "flex-end") align = UiAlign::End;
					else if (name == "stretch") align = UiAlign::Stretch;
					else if (name == "baseline") align = UiAlign::Baseline;
					else supported = false;
					if (supported && property == "align-items") style.alignItems = align;
					if (supported && property == "align-self") style.alignSelf = align;
				}
				else if (property == "justify-content")
				{
					const String name = lower(value.text);
					if (name == "start" || name == "flex-start") style.justifyContent = UiJustify::Start;
					else if (name == "center") style.justifyContent = UiJustify::Center;
					else if (name == "end" || name == "flex-end") style.justifyContent = UiJustify::End;
					else if (name == "space-between") style.justifyContent = UiJustify::SpaceBetween;
					else if (name == "space-around") style.justifyContent = UiJustify::SpaceAround;
					else if (name == "space-evenly") style.justifyContent = UiJustify::SpaceEvenly;
					else supported = false;
				}
				else if (property == "overflow" || property == "overflow-x" || property == "overflow-y")
				{
					Bool keywordSupported = false;
					const UiOverflow overflow = parseOverflowKeyword(lower(value.text), keywordSupported);
					supported = keywordSupported;
					if (supported && property == "overflow")
					{
						style.overflowX = overflow;
						style.overflowY = overflow;
					}
					if (supported && property == "overflow-x") style.overflowX = overflow;
					if (supported && property == "overflow-y") style.overflowY = overflow;
				}
				else if (property == "background-color" && value.kind == UiStyleValueKind::Color)
				{ paint.hasBackground = true; paint.background = value.color; }
				else if (property == "color" && value.kind == UiStyleValueKind::Color)
				{ paint.hasForeground = true; paint.foreground = value.color; }
				else if (property == "border-color" && value.kind == UiStyleValueKind::Color)
				{ paint.hasBorder = true; paint.border = value.color; }
				else if (property == "border-width") supported = scalar(value, paint.borderWidth);
				else if (property == "border-radius") supported = scalar(value, paint.borderRadius);
				else if (property == "font-size") { Float ignored = 0; supported = scalar(value, ignored); }
				else supported = false;

				if (!supported)
					diagnose("Unsupported authored style property or value: " + propertyName + ": " + declaration.value,
						declaration.span);
			}
			layout.setStyle(handle, style);
			tokens[handle] = paint;
			if (UiNode* styledNode = document.get(handle))
			{
				if (paint.hasBackground) (void)addClass(*styledNode, "ui-authored-background");
				if (paint.hasForeground) (void)addClass(*styledNode, "ui-authored-foreground");
				if (paint.hasBorder) (void)addClass(*styledNode, "ui-authored-border");
			}
		}
	}
}
