#include "UiAppTheme.hpp"

#include "../Authoring/UiStyleApplicationAdapter.hpp"
#include "../Core/UiDocument.hpp"
#include "../../Math.hpp"
#include "../Layout/UiScroll.hpp"
#include "../Text/UiIconCatalog.hpp"
#include "../Text/UiSvgIconService.hpp"

namespace Ui
{
	namespace
	{
		constexpr Vec4 squareCorners = Vec4(0);

		[[nodiscard]] UiLinearColor srgb(
			const Float r, const Float g, const Float b, const Float a = 1)
		{
			return UiLinearColor::fromStraightSrgb(Vec4(r, g, b, a));
		}

		[[nodiscard]] Bool hasClass(const UiNode& node, const String& name)
		{
			for (const String& styleClass : node.authored.classes)
				if (styleClass == name) return true;
			return false;
		}

		[[nodiscard]] Vec4 straightLinear(const UiLinearColor color)
		{
			const Vec4 premultiplied = color.premultiplied;
			if (premultiplied.a <= 0) return Vec4(0);
			return Vec4(Vec3(premultiplied) / premultiplied.a, premultiplied.a);
		}

		// Composite one highlight layer over a base fill (parent background + overlay).
		[[nodiscard]] UiLinearColor stackHighlight(
			const UiLinearColor base, const UiLinearColor highlight, const Int layers = 1)
		{
			if (layers <= 0) return base;
			Vec4 result = straightLinear(base);
			const Vec4 overlay = straightLinear(highlight);
			for (Int layer = 0; layer < layers; ++layer)
			{
				const Float overlayAlpha = overlay.a;
				const Float inverseOverlayAlpha = 1.0f - overlayAlpha;
				const Vec3 overlayRgb = Vec3(overlay);
				const Vec3 baseRgb = Vec3(result);
				const Float baseAlpha = result.a;
				const Float outAlpha = overlayAlpha + baseAlpha * inverseOverlayAlpha;
				Vec3 outRgb(0);
				if (outAlpha > 0)
				{
					outRgb = (overlayRgb * overlayAlpha + baseRgb * baseAlpha * inverseOverlayAlpha) /
						outAlpha;
				}
				result = Vec4(outRgb, outAlpha);
			}
			return UiLinearColor::fromStraightLinear(result);
		}

		[[nodiscard]] Bool isSurfaceRoot(const UiNode& node)
		{
			return hasClass(node, UiAppClass::root) || hasClass(node, "launcher-root") ||
				hasClass(node, "root");
		}

		[[nodiscard]] Bool isSectionChrome(const UiNode& node)
		{
			return hasClass(node, UiAppClass::panel) || hasClass(node, UiAppClass::section) ||
				hasClass(node, "dock-panel") || node.authored.typeName == "GameViewport" ||
				node.authored.typeName == "WorldSurface";
		}

		[[nodiscard]] UiLinearColor resolveAncestorBackground(
			const UiDocument* document, UiNodeHandle handle, const UiAppColors& colors)
		{
			if (!document || !handle.isValid()) return colors.surfaceRoot;
			const UiNode* node = document->get(handle);
			if (!node) return colors.surfaceRoot;
			if (isSurfaceRoot(*node)) return colors.surfaceRoot;
			if (isSectionChrome(*node))
				return stackHighlight(
					resolveAncestorBackground(document, node->parentHandle(), colors),
					colors.surfaceHighlight, 1);
			return resolveAncestorBackground(document, node->parentHandle(), colors);
		}

		[[nodiscard]] UiLinearColor resolveSectionFill(
			const UiDocument* document, UiNodeHandle handle, const UiAppColors& colors)
		{
			if (!document || !handle.isValid()) return stackHighlight(colors.surfaceRoot, colors.surfaceHighlight, 1);
			const UiNode* node = document->get(handle);
			if (!node || isSurfaceRoot(*node)) return colors.surfaceRoot;
			return stackHighlight(
				resolveAncestorBackground(document, node->parentHandle(), colors),
				colors.surfaceHighlight, 1);
		}

		[[nodiscard]] UiLinearColor resolveControlFill(
			const UiDocument* document, UiNodeHandle handle, const UiAppColors& colors,
			const UiNode& node)
		{
			const UiLinearColor parentFill = document && handle.isValid()
				? resolveAncestorBackground(document, handle, colors)
				: colors.surfaceRoot;
			const Int extraLayers = node.state.active ? 2 : node.state.hovered ? 1 : 0;
			return stackHighlight(parentFill, colors.surfaceHighlight, 1 + extraLayers);
		}

		void deriveSurfaceTiers(UiAppColors& colors)
		{
			colors.surfacePanel = stackHighlight(colors.surfaceRoot, colors.surfaceHighlight, 1);
			colors.surfaceControl = stackHighlight(colors.surfaceRoot, colors.surfaceHighlight, 2);
			colors.surfaceControlHover = stackHighlight(colors.surfaceRoot, colors.surfaceHighlight, 3);
			colors.surfaceControlActive = stackHighlight(colors.surfaceRoot, colors.surfaceHighlight, 4);
			colors.surfaceListItem = stackHighlight(colors.surfaceRoot, colors.surfaceHighlight, 2);
			colors.surfaceListItemHover = stackHighlight(colors.surfaceRoot, colors.surfaceHighlight, 3);
			colors.surfaceListItemActive = stackHighlight(colors.surfaceRoot, colors.surfaceHighlight, 4);
			colors.surfaceBadge = stackHighlight(colors.surfaceRoot, colors.surfaceHighlight, 3);
			colors.surfaceMenu = stackHighlight(colors.surfaceRoot, colors.surfaceHighlight, 1);
			colors.surfaceMenuHover = stackHighlight(colors.surfaceRoot, colors.surfaceHighlight, 2);
			colors.surfaceMenuSelected = stackHighlight(colors.surfaceRoot, colors.surfaceHighlight, 3);
		}

		[[nodiscard]] Bool needsClip(const UiNode& node)
		{
			// Layout publishes the effective clip for every node. Painters must honor
			// it so chrome such as borders and focus rings cannot bleed past scrollports.
			return node.computed.clipSize.x > 0 && node.computed.clipSize.y > 0;
		}

		[[nodiscard]] Bool paintFocusRing(
			const UiAppColors& colors, const UiNode& node, const UiQuadGeometry& geometry,
			UiDisplayList& output)
		{
			if (!node.state.focused) return true;
			const UiQuadGeometry ring{
				geometry.position - Vec2(2),
				geometry.size + Vec2(4),
				squareCorners};
			return output.addSolidQuad(ring, colors.accentFocus);
		}

		[[nodiscard]] Bool paintControlChrome(
			const UiAppColors& colors, const UiQuadGeometry& geometry, const UiNode& node,
			UiDisplayList& output, const UiDocument* document, const UiNodeHandle handle)
		{
			const UiLinearColor fill = document && handle.isValid()
				? resolveControlFill(document, handle, colors, node)
				: node.state.active ? colors.surfaceControlActive
				: node.state.hovered ? colors.surfaceControlHover : colors.surfaceControl;
			if (!output.addSolidQuad(geometry, fill)) return false;
			if (!hasClass(node, "ui-authored-border"))
			{
				const UiLinearColor border = node.state.hovered ? colors.borderHover : colors.border;
				if (!output.addInsetBorder(geometry, 1, border)) return false;
			}
			return paintFocusRing(colors, node, geometry, output);
		}

		[[nodiscard]] Double parseValue(const String& text)
		{
			if (text.empty()) return 0;
			try { return std::stod(text); }
			catch (...) { return 0; }
		}

		[[nodiscard]] Double parseRangeFraction(const UiNode& node)
		{
			const Double value = parseValue(node.state.semantics.value);
			const String& range = node.state.semantics.label;
			const SizeT separator = range.find(':');
			if (separator == String::npos) return std::clamp(value, 0.0, 1.0);
			const Double minimum = parseValue(range.substr(0, separator));
			const Double maximum = parseValue(range.substr(separator + 1));
			if (maximum <= minimum) return 0;
			return std::clamp((value - minimum) / (maximum - minimum), 0.0, 1.0);
		}

		[[nodiscard]] Bool paintNumberField(
			const UiAppColors& colors, const UiNode& node, const UiQuadGeometry& geometry,
			UiDisplayList& output, const UiDocument* document, const UiNodeHandle handle,
			const UiTextPainter& textPainter, UiSvgIconService* iconService)
		{
			const UiLinearColor fill = document && handle.isValid()
				? resolveControlFill(document, handle, colors, node)
				: node.state.active ? colors.surfaceControlActive
				: node.state.hovered ? colors.surfaceControlHover : colors.surfaceControl;
			if (!output.addSolidQuad(geometry, fill)) return false;

			if (hasClass(node, "is-number-display") && hasClass(node, "has-number-progress"))
			{
				const Float fraction = static_cast<Float>(parseRangeFraction(node));
				const Float fillWidth = std::max(2.0f, geometry.size.x * fraction);
				if (!output.addSolidQuad(
					UiQuadGeometry{geometry.position, Vec2(fillWidth, geometry.size.y), squareCorners},
					colors.surfaceTrackFill)) return false;
			}

			if (!hasClass(node, "ui-authored-border"))
			{
				const UiLinearColor border = node.state.hovered ? colors.borderHover : colors.border;
				if (!output.addInsetBorder(geometry, 1, border)) return false;
			}

			if ((node.state.hovered || node.state.active) && hasClass(node, "is-number-display") &&
				iconService)
			{
				const Float chevronSize = UiAppTypography::bodySize;
				const Float inset = UiAppFlex::p10;
				const Float centerY = geometry.position.y + geometry.size.y * 0.5f;
				const UiLinearColor chevronColor = colors.iconMuted;
				const UiQuadGeometry leftChevron{
					Vec2(geometry.position.x + inset,
						centerY - chevronSize * 0.5f),
					Vec2(chevronSize, chevronSize), squareCorners};
				const UiQuadGeometry rightChevron{
					Vec2(geometry.position.x + geometry.size.x - inset - chevronSize,
						centerY - chevronSize * 0.5f),
					Vec2(chevronSize, chevronSize), squareCorners};
				(void)iconService->paint(
					output, Icons::ChevronLeft, leftChevron, chevronColor,
					static_cast<UInt>(chevronSize));
				(void)iconService->paint(
					output, Icons::ChevronRight, rightChevron, chevronColor,
					static_cast<UInt>(chevronSize));
			}

			if (!paintFocusRing(colors, node, geometry, output)) return false;
			return textPainter.paint(node, output);
		}

		[[nodiscard]] Bool paintSlider(
			const UiAppColors& colors, const UiNode& node, const UiQuadGeometry& geometry,
			UiDisplayList& output)
		{
			const Float fraction = static_cast<Float>(parseRangeFraction(node));
			const Float trackHeight = 4;
			const Float thumbSize = 16;
			const Vec2 trackSize(geometry.size.x, trackHeight);
			const Vec2 trackPos(
				geometry.position.x,
				geometry.position.y + (geometry.size.y - trackHeight) * 0.5f);
			const UiQuadGeometry track{trackPos, trackSize, squareCorners};
			if (!output.addSolidQuad(track, colors.surfaceTrack)) return false;

			const Float fillWidth = max(trackHeight, trackSize.x * fraction);
			if (!output.addSolidQuad(
				UiQuadGeometry{trackPos, Vec2(fillWidth, trackHeight), squareCorners},
				colors.surfaceTrackFill)) return false;

			const Vec2 thumbPos(
				trackPos.x + fraction * max(0.0f, trackSize.x - thumbSize),
				geometry.position.y + (geometry.size.y - thumbSize) * 0.5f);
			const UiQuadGeometry thumb{thumbPos, Vec2(thumbSize, thumbSize), squareCorners};
			if (!output.addSolidQuad(thumb, colors.thumb)) return false;
			if (!output.addInsetBorder(thumb, 1, colors.border)) return false;
			return paintFocusRing(colors, node, geometry, output);
		}

		[[nodiscard]] Bool paintToggle(
			const UiAppColors& colors, const UiNode& node, const UiQuadGeometry& geometry,
			UiDisplayList& output)
		{
			const Bool checked = node.state.semantics.checked || hasClass(node, "is-checked");
			const Float switchWidth = min(geometry.size.x, 48.0f);
			const Float switchHeight = min(geometry.size.y, 26.0f);
			const Float thumbInset = 2.0f;
			const Float thumbSize = max(0.0f, switchHeight - thumbInset * 2);
			const Vec2 switchPos(
				geometry.position.x + geometry.size.x - switchWidth,
				geometry.position.y + (geometry.size.y - switchHeight) * 0.5f);
			const UiQuadGeometry track{switchPos, Vec2(switchWidth, switchHeight), squareCorners};
			if (!output.addSolidQuad(
				track, checked ? colors.toggleOn : colors.toggleOff)) return false;

			const Float thumbTravel = max(0.0f, switchWidth - thumbSize - thumbInset * 2);
			const Vec2 thumbPos(
				switchPos.x + thumbInset + (checked ? thumbTravel : 0),
				switchPos.y + thumbInset);
			if (!output.addSolidQuad(
				UiQuadGeometry{thumbPos, Vec2(thumbSize, thumbSize), squareCorners},
				colors.thumb)) return false;
			return paintFocusRing(colors, node, track, output);
		}

		[[nodiscard]] Bool paintCheckbox(
			const UiAppColors& colors, const UiNode& node, const UiQuadGeometry& geometry,
			UiDisplayList& output, UiSvgIconService* iconService, const UiDocument* document,
			const UiNodeHandle handle)
		{
			const Bool checked = node.state.semantics.checked || hasClass(node, "is-checked");
			const Float boxSize = min(min(geometry.size.x, geometry.size.y), UiAppTypography::bodySize + 4);
			const Vec2 boxPos(
				geometry.position.x,
				geometry.position.y + (geometry.size.y - boxSize) * 0.5f);
			const UiQuadGeometry box{boxPos, Vec2(boxSize, boxSize), squareCorners};
			const UiLinearColor uncheckedFill = document && handle.isValid()
				? resolveControlFill(document, handle, colors, node)
				: colors.surfaceControl;
			if (!output.addSolidQuad(box, checked ? colors.accent : uncheckedFill)) return false;
			if (!output.addInsetBorder(box, 1, colors.border)) return false;
			if (checked &&
				!(iconService && iconService->paint(
					output, Icons::Check, box, colors.textInverse,
					static_cast<UInt>(UiAppTypography::bodySize))))
			{
				if (!output.addSolidQuad(
					UiQuadGeometry{boxPos + Vec2(boxSize * 0.22f, boxSize * 0.48f),
						Vec2(boxSize * 0.22f, boxSize * 0.12f), squareCorners},
					colors.textInverse)) return false;
				if (!output.addSolidQuad(
					UiQuadGeometry{boxPos + Vec2(boxSize * 0.36f, boxSize * 0.24f),
						Vec2(boxSize * 0.42f, boxSize * 0.12f), squareCorners},
					colors.textInverse)) return false;
			}
			return paintFocusRing(colors, node, geometry, output);
		}

		[[nodiscard]] Bool paintSelectField(
			const UiAppColors& colors, const UiNode& node, const UiQuadGeometry& geometry,
			UiDisplayList& output, UiSvgIconService* iconService, const UiDocument* document,
			const UiNodeHandle handle)
		{
			if (!paintControlChrome(colors, geometry, node, output, document, handle)) return false;
			const Float arrowSize = UiAppTypography::bodySize;
			const Vec2 arrowPos(
				geometry.position.x + geometry.size.x - arrowSize - UiAppFlex::p10,
				geometry.position.y + (geometry.size.y - arrowSize) * 0.5f);
			const UiQuadGeometry arrow{arrowPos, Vec2(arrowSize, arrowSize), squareCorners};
			if (iconService && iconService->paint(
				output, Icons::ChevronDown, arrow, colors.iconMuted,
				static_cast<UInt>(arrowSize))) return true;
			return output.addSolidQuad(
				UiQuadGeometry{arrowPos, Vec2(arrowSize, arrowSize * 0.55f), squareCorners},
				colors.iconMuted);
		}

		[[nodiscard]] Bool paintSelectOption(
			const UiAppColors& colors, const UiNode& node, const UiQuadGeometry& geometry,
			UiDisplayList& output, const UiDocument* document, const UiNodeHandle handle)
		{
			if (!node.state.visible) return true;
			const Bool selected = node.state.semantics.selected || hasClass(node, "is-selected");
			const UiLinearColor menuBase = document && handle.isValid()
				? resolveAncestorBackground(document, handle, colors)
				: colors.surfaceMenu;
			const Int extraLayers = selected ? 3 : node.state.hovered ? 2 : 1;
			const UiLinearColor fill = stackHighlight(menuBase, colors.surfaceHighlight, extraLayers);
			if (!output.addSolidQuad(geometry, fill)) return false;
			if (selected && !output.addSolidQuad(
				UiQuadGeometry{geometry.position, Vec2(2, geometry.size.y), squareCorners},
				colors.accent)) return false;
			return true;
		}

		[[nodiscard]] Bool paintIconNode(
			const UiAppColors& colors, const UiNode& node, const UiQuadGeometry& geometry,
			UiDisplayList& output, UiSvgIconService* iconService)
		{
			String resource = node.authored.text;
			if (resource.empty()) resource = node.state.semantics.value;
			if (!iconService || resource.empty()) return true;
			const UiQuadGeometry contentGeometry{
				node.computed.contentPosition, node.computed.contentSize, geometry.cornerRadii};
			if (contentGeometry.size.x <= 0 || contentGeometry.size.y <= 0) return true;
			const Float pixelSize = min(contentGeometry.size.x, contentGeometry.size.y);
			if (pixelSize <= 0) return true;
			UiLinearColor tint = colors.iconPrimary;
			if (hasClass(node, UiAppClass::iconMuted)) tint = colors.iconMuted;
			else if (hasClass(node, "section-icon")) tint = colors.iconPrimary;
			else if (hasClass(node, "header-icon")) tint = colors.iconPrimary;
			else if (hasClass(node, "launcher-entry-icon") || hasClass(node, "list-item-icon"))
				tint = colors.iconOnAccent;
			return iconService->paint(output, resource, contentGeometry, tint,
				static_cast<UInt>(pixelSize));
		}

		[[nodiscard]] Bool paintPanelBorder(
			const UiAppColors& colors, const UiQuadGeometry& geometry, UiDisplayList& output)
		{
			return output.addInsetBorder(geometry, 1, colors.border);
		}

		[[nodiscard]] Bool isDecorativeChild(const UiNode& node)
		{
			return node.authored.typeName == "Track" ||
				node.authored.typeName == "Thumb" ||
				node.authored.typeName == "Indicator" ||
				hasClass(node, "ui-track") ||
				hasClass(node, "ui-thumb") ||
				hasClass(node, "ui-indicator");
		}

		void applyTextStyle(UiTextPaintStyle& style, const Float size, const Float line,
			const UiLinearColor& color, const Bool wrap = false)
		{
			style.fontSize = size;
			style.lineHeight = line;
			style.color = color;
			style.wrap = wrap;
		}
	}

	UiAppTheme UiAppTheme::light()
	{
		UiAppTheme theme;
		theme.mode = UiThemeMode::Light;
		theme.colors.surfaceRoot = srgb(0.96f, 0.96f, 0.97f);
		theme.colors.surfaceHighlight = srgb(0.0f, 0.0f, 0.0f, 0.05f);
		deriveSurfaceTiers(theme.colors);
		theme.colors.surfaceTrack = srgb(0.88f, 0.88f, 0.89f);
		theme.colors.surfaceTrackFill = srgb(0.32f, 0.32f, 0.34f);
		theme.colors.border = srgb(0.82f, 0.82f, 0.84f);
		theme.colors.borderHover = srgb(0.64f, 0.64f, 0.67f);
		theme.colors.borderFocus = srgb(0.32f, 0.32f, 0.34f, 0.55f);
		theme.colors.shadow = srgb(0.0f, 0.0f, 0.0f, 0.08f);
		theme.colors.accent = srgb(0.24f, 0.24f, 0.27f);
		theme.colors.accentFocus = srgb(0.24f, 0.24f, 0.27f, 0.45f);
		theme.colors.toggleOn = srgb(0.32f, 0.32f, 0.34f);
		theme.colors.toggleOff = srgb(0.82f, 0.82f, 0.84f);
		theme.colors.thumb = srgb(1.0f, 1.0f, 1.0f);
		theme.colors.textPrimary = srgb(0.09f, 0.09f, 0.11f);
		theme.colors.textSecondary = srgb(0.32f, 0.32f, 0.35f);
		theme.colors.textMuted = srgb(0.52f, 0.52f, 0.55f);
		theme.colors.textInverse = srgb(0.98f, 0.98f, 0.99f);
		theme.colors.textAccent = srgb(0.24f, 0.24f, 0.27f);
		theme.colors.iconPrimary = srgb(0.32f, 0.32f, 0.35f);
		theme.colors.iconMuted = srgb(0.52f, 0.52f, 0.55f);
		theme.colors.iconOnAccent = srgb(0.98f, 0.98f, 0.99f);
		theme.rebuildTokenCache();
		return theme;
	}

	UiAppTheme UiAppTheme::dark()
	{
		UiAppTheme theme;
		theme.mode = UiThemeMode::Dark;
		theme.colors.surfaceRoot = srgb(0.07f, 0.07f, 0.08f);
		theme.colors.surfaceHighlight = srgb(1.0f, 1.0f, 1.0f, 0.05f);
		deriveSurfaceTiers(theme.colors);
		theme.colors.surfaceTrack = srgb(0.28f, 0.28f, 0.30f);
		theme.colors.surfaceTrackFill = srgb(0.82f, 0.82f, 0.84f);
		theme.colors.border = srgb(0.28f, 0.28f, 0.30f);
		theme.colors.borderHover = srgb(0.42f, 0.42f, 0.45f);
		theme.colors.borderFocus = srgb(0.82f, 0.82f, 0.84f, 0.45f);
		theme.colors.shadow = srgb(0.0f, 0.0f, 0.0f, 0.35f);
		theme.colors.accent = srgb(0.88f, 0.88f, 0.90f);
		theme.colors.accentFocus = srgb(0.88f, 0.88f, 0.90f, 0.45f);
		theme.colors.toggleOn = srgb(0.82f, 0.82f, 0.84f);
		theme.colors.toggleOff = srgb(0.28f, 0.28f, 0.30f);
		theme.colors.thumb = srgb(0.98f, 0.98f, 0.99f);
		theme.colors.textPrimary = srgb(0.96f, 0.96f, 0.97f);
		theme.colors.textSecondary = srgb(0.72f, 0.72f, 0.74f);
		theme.colors.textMuted = srgb(0.52f, 0.52f, 0.55f);
		theme.colors.textInverse = srgb(0.09f, 0.09f, 0.11f);
		theme.colors.textAccent = srgb(0.88f, 0.88f, 0.90f);
		theme.colors.iconPrimary = srgb(0.72f, 0.72f, 0.74f);
		theme.colors.iconMuted = srgb(0.52f, 0.52f, 0.55f);
		theme.colors.iconOnAccent = srgb(0.96f, 0.96f, 0.97f);
		theme.rebuildTokenCache();
		return theme;
	}

	void UiAppTheme::setMode(const UiThemeMode newMode)
	{
		if (mode == newMode) return;
		*this = newMode == UiThemeMode::Light ? light() : dark();
	}

	void UiAppTheme::rebuildTokenCache() const
	{
		cachedTokens = {
			{"color-surface-root", "Surface root", "color", colors.surfaceRoot},
			{"color-surface-highlight", "Surface highlight", "color", colors.surfaceHighlight},
			{"color-surface-panel", "Surface panel", "color", colors.surfacePanel},
			{"color-surface-control", "Surface control", "color", colors.surfaceControl},
			{"color-surface-list-item", "List item", "color", colors.surfaceListItem},
			{"color-surface-badge", "Badge", "color", colors.surfaceBadge},
			{"color-border", "Border", "color", colors.border},
			{"color-border-hover", "Border hover", "color", colors.borderHover},
			{"color-accent", "Accent", "color", colors.accent},
			{"color-accent-focus", "Focus ring", "color", colors.accentFocus},
			{"color-toggle-on", "Toggle on", "color", colors.toggleOn},
			{"color-toggle-off", "Toggle off", "color", colors.toggleOff},
			{"color-text-primary", "Text primary", "color", colors.textPrimary},
			{"color-text-secondary", "Text secondary", "color", colors.textSecondary},
			{"color-text-muted", "Text muted", "color", colors.textMuted},
			{"color-icon-primary", "Icon primary", "color", colors.iconPrimary},
			{"color-icon-muted", "Icon muted", "color", colors.iconMuted},
			{"p5", "Padding p5 (5px)", "padding", {}, UiAppFlex::p5, false},
			{"p10", "Padding p10 (10px)", "padding", {}, UiAppFlex::p10, false},
			{"p20", "Padding p20 (20px)", "padding", {}, UiAppFlex::p20, false},
			{"p40", "Padding p40 (40px)", "padding", {}, UiAppFlex::p40, false},
			{"p80", "Padding p80 (80px)", "padding", {}, UiAppFlex::p80, false},
			{"g5", "Gap g5 (5px)", "gap", {}, UiAppFlex::g5, false},
			{"g10", "Gap g10 (10px)", "gap", {}, UiAppFlex::g10, false},
			{"g20", "Gap g20 (20px)", "gap", {}, UiAppFlex::g20, false},
			{"g40", "Gap g40 (40px)", "gap", {}, UiAppFlex::g40, false},
			{"g80", "Gap g80 (80px)", "gap", {}, UiAppFlex::g80, false},
			{"b50", "Basis b50 (50px)", "basis", {}, UiAppFlex::b50, false},
			{"b100", "Basis b100 (100px)", "basis", {}, UiAppFlex::b100, false},
			{"b150", "Basis b150 (150px)", "basis", {}, UiAppFlex::b150, false},
			{"b200", "Basis b200 (200px)", "basis", {}, UiAppFlex::b200, false},
			{"b250", "Basis b250 (250px)", "basis", {}, UiAppFlex::b250, false},
			{"b300", "Basis b300 (300px)", "basis", {}, UiAppFlex::b300, false},
			{"b350", "Basis b350 (350px)", "basis", {}, UiAppFlex::b350, false},
			{"type-body", "Body (16px)", "typography", {}, UiAppTypography::bodySize, false},
			{"type-title", "Title (28px)", "typography", {}, UiAppTypography::titleSize, false},
			{"layout-control-row", "Control row (body + padding)", "layout", {}, UiAppLayout::controlRowHeight, false},
			{"layout-corner-radius", "Corner radius (0px)", "layout", {}, UiAppLayout::cornerRadius, false}};
	}

	const Vector<UiAppThemeToken>& UiAppTheme::tokens() const
	{
		if (cachedTokens.empty()) rebuildTokenCache();
		return cachedTokens;
	}

	const UiAppThemeToken* UiAppTheme::findToken(const String& id) const
	{
		for (const UiAppThemeToken& token : tokens())
			if (id == token.id) return &token;
		return nullptr;
	}

	UiTextStyleProvider makeAppTextStyleProvider(const UiAppTheme& theme)
	{
		return [colors = theme.colors, theme](const UiNode& node, UiTextPaintStyle& style)
			{
				style.padding = Vec2(0);
				style.wrap = false;
				const String& type = node.authored.typeName;
				if (type == "NumberField")
				{
					style.wrap = false;
					style.color = colors.textPrimary;
					style.caretColor = colors.textPrimary;
					style.selectionColor = srgb(
						straightLinear(colors.accent).r,
						straightLinear(colors.accent).g,
						straightLinear(colors.accent).b,
						0.40f);
					style.fontSize = UiAppTypography::bodySize;
					style.lineHeight = UiAppTypography::bodyLine;
					if (hasClass(node, "is-number-display"))
						style.horizontalAlign = UiTextHorizontalAlign::Center;
					return;
				}
				if (type == "TextField" || type == "TextArea" || type == "Button" ||
					type == "MenuItem")
				{
					style.wrap = type == "TextArea";
					style.color = colors.textPrimary;
					style.caretColor = colors.textPrimary;
					style.selectionColor = srgb(
						straightLinear(colors.accent).r,
						straightLinear(colors.accent).g,
						straightLinear(colors.accent).b,
						0.40f);
					style.fontSize = UiAppTypography::bodySize;
					style.lineHeight = UiAppTypography::bodyLine;
					return;
				}
				if (hasClass(node, UiAppClass::textBody))
					return applyTextStyle(style, UiAppTypography::bodySize, UiAppTypography::bodyLine,
						colors.textPrimary);
				if (hasClass(node, UiAppClass::textBadge) || hasClass(node, UiAppClass::fieldLabel))
					return applyTextStyle(style, UiAppTypography::bodySize, UiAppTypography::bodyLine,
						colors.textSecondary);
				if (hasClass(node, UiAppClass::valueReadout))
					return applyTextStyle(style, UiAppTypography::bodySize, UiAppTypography::bodyLine,
						colors.textAccent);
				if (hasClass(node, UiAppClass::entryDescription))
					return applyTextStyle(style, UiAppTypography::bodySize, UiAppTypography::bodyLine,
						colors.textMuted);
				if (hasClass(node, UiAppClass::typographySample))
				{
					Float size = UiAppTypography::bodySize;
					Float line = UiAppTypography::bodyLine;
					if (const UiAppThemeToken* token = theme.findToken(node.authored.id))
					{
						if (!token->isColor) size = token->numericValue;
						if (size >= UiAppTypography::titleSize) line = UiAppTypography::titleLine;
					}
					return applyTextStyle(style, size, line, colors.textPrimary);
				}
				if (hasClass(node, UiAppClass::statusText))
				{
					applyTextStyle(style, UiAppTypography::bodySize, UiAppTypography::bodyLine,
						colors.textSecondary, true);
					return;
				}
				if (hasClass(node, "palette-label"))
					return applyTextStyle(style, UiAppTypography::bodySize, UiAppTypography::bodyLine,
						colors.textMuted);
				applyTextStyle(style, UiAppTypography::bodySize, UiAppTypography::bodyLine,
					colors.textPrimary);
			};
	}

	UiNodePainter makeAppThemePainter(
		const UiAppTheme& theme,
		const UiDocument* document,
		UiTextStyleProvider textStyleProvider,
		UiTextGlyphProvider glyphProvider,
		UiTextShapeProvider textShapeProvider,
		UiSvgIconService* iconService,
		UiPaintStyleTokenProvider paintStyleTokens)
	{
		const UiAppColors colors = theme.colors;
		const UiAppTheme themeSnapshot = theme;
		if (!textStyleProvider) textStyleProvider = makeAppTextStyleProvider(theme);
		const UiTextPainter textPainter(
			std::move(glyphProvider), std::move(textStyleProvider),
			std::move(textShapeProvider));
		return [colors, themeSnapshot, document, textPainter, iconService, paintStyleTokens](
			const UiNodePaintContext& context, UiDisplayList& output)
			{
				const UiNode& node = context.node;
				if (!node.state.visible) return true;
				const UiQuadGeometry geometry{
					node.computed.position, node.computed.size, squareCorners};
				if (geometry.size.x <= 0 || geometry.size.y <= 0) return true;

				const Bool clipped = needsClip(node);
				if (clipped && !output.pushClip(
					UiClipRect{node.computed.clipPosition, node.computed.clipSize}))
					return false;
				auto finish = [&](const Bool result)
					{ return clipped ? output.popClip() && result : result; };
				auto finishWithAuthoredBorder = [&](Bool result) -> Bool
					{
						if (result && paintStyleTokens && hasClass(node, "ui-authored-border"))
						{
							if (const UiPaintStyleTokens* tokens = paintStyleTokens(context.handle))
							{
								if (tokens->hasBorder)
								{
									const Float width = tokens->borderWidth > 0 ? tokens->borderWidth : 1;
									if (!output.addInsetBorder(
										geometry, width,
										UiLinearColor::fromStraightSrgb(tokens->border)))
										result = false;
								}
							}
						}
						return finish(result);
					};

				const String& type = node.authored.typeName;
				if (isDecorativeChild(node)) return finish(true);

				if (type == "Slider" || type == "Range")
					return finishWithAuthoredBorder(paintSlider(colors, node, geometry, output));
				if (type == "Toggle")
					return finishWithAuthoredBorder(paintToggle(colors, node, geometry, output));
				if (type == "Checkbox")
					return finishWithAuthoredBorder(
						paintCheckbox(
							colors, node, geometry, output, iconService, document, context.handle));
				if (type == "Select" || type == "Combo")
					return finishWithAuthoredBorder(
						paintSelectField(
							colors, node, geometry, output, iconService, document, context.handle));
				if (type == "Icon" || type == "Image")
					return finishWithAuthoredBorder(
						paintIconNode(colors, node, geometry, output, iconService));
				if (type == "MenuItem" && hasClass(node, "ui-select-option"))
					return finishWithAuthoredBorder(paintSelectOption(
						colors, node, geometry, output, document, context.handle) &&
						textPainter.paint(node, output));

				if (hasClass(node, UiAppClass::themeSwatch))
				{
					UiLinearColor fill = colors.surfaceControl;
					if (const UiAppThemeToken* token = themeSnapshot.findToken(node.authored.id))
						if (token->isColor) fill = token->color;
					if (!output.addSolidQuad(geometry, fill)) return finishWithAuthoredBorder(false);
					if (!output.addInsetBorder(geometry, 1, colors.border))
						return finishWithAuthoredBorder(false);
					return finishWithAuthoredBorder(textPainter.paint(node, output));
				}

				if (hasClass(node, UiAppClass::spacingSample))
				{
					Float width = UiAppFlex::p10;
					if (const UiAppThemeToken* token = themeSnapshot.findToken(node.authored.id))
						if (!token->isColor) width = token->numericValue;
					if (!output.addSolidQuad(
						UiQuadGeometry{geometry.position, Vec2(width, 12), squareCorners},
						colors.accent)) return finishWithAuthoredBorder(false);
					return finishWithAuthoredBorder(textPainter.paint(node, output));
				}

				UiLinearColor fill = colors.surfacePanel;
				Bool drawFill = false;
				Bool drawBorder = false;

				if (isSurfaceRoot(node))
				{
					drawFill = true;
					fill = colors.surfaceRoot;
				}
				else if (hasClass(node, UiAppClass::shell) || hasClass(node, "launcher-shell") ||
					hasClass(node, "launcher-section-row") || hasClass(node, UiAppClass::scroll) ||
					hasClass(node, "launcher-scroll"))
				{
					drawFill = false;
				}
				else if (isSectionChrome(node))
				{
					drawFill = true;
					drawBorder = hasClass(node, UiAppClass::panel) || hasClass(node, "launcher-list");
					fill = resolveSectionFill(document, context.handle, colors);
				}
				else if (hasClass(node, "launcher-entry"))
				{
					const UiLinearColor parentFill = document
						? resolveAncestorBackground(document, context.handle, colors)
						: colors.surfaceRoot;
					const Int extraLayers = node.state.active ? 2 : node.state.hovered ? 1 : 0;
					const UiLinearColor itemFill = stackHighlight(
						parentFill, colors.surfaceHighlight, 1 + extraLayers);
					if (!output.addSolidQuad(geometry, itemFill)) return finishWithAuthoredBorder(false);
					if (!hasClass(node, "ui-authored-border"))
					{
						const UiLinearColor border = node.state.hovered ? colors.borderHover : colors.border;
						if (!output.addInsetBorder(geometry, 1, border))
							return finishWithAuthoredBorder(false);
					}
					return finishWithAuthoredBorder(paintFocusRing(colors, node, geometry, output) &&
						textPainter.paint(node, output));
				}
				else if (hasClass(node, UiAppClass::badge) || hasClass(node, "launcher-count"))
				{
					drawFill = true;
					drawBorder = true;
					const UiLinearColor parentFill = document
						? resolveAncestorBackground(document, context.handle, colors)
						: colors.surfaceRoot;
					fill = stackHighlight(parentFill, colors.surfaceHighlight, 2);
				}
				else if (type == "NumberField")
				{
					return finishWithAuthoredBorder(paintNumberField(
						colors, node, geometry, output, document, context.handle, textPainter,
						iconService));
				}
				else if (type == "Button" || type == "TextField" || type == "TextArea")
				{
					return finishWithAuthoredBorder(
						paintControlChrome(colors, geometry, node, output, document, context.handle) &&
						textPainter.paint(node, output));
				}
				else if (hasClass(node, "palette-swatch"))
				{
					return finishWithAuthoredBorder(true);
				}
				else if (hasClass(node, "palette-item") || hasClass(node, "palette-row") ||
					hasClass(node, "palette-grid"))
				{
					drawFill = false;
				}
				else if (hasClass(node, "ui-select-menu"))
				{
					drawFill = true;
					drawBorder = true;
					fill = resolveSectionFill(document, context.handle, colors);
					if (!output.addSolidQuad(
						UiQuadGeometry{geometry.position + Vec2(0, 2), geometry.size, squareCorners},
						colors.shadow)) return finishWithAuthoredBorder(false);
				}

				if (drawFill && !output.addSolidQuad(geometry, fill)) return finishWithAuthoredBorder(false);
				if (drawBorder && !hasClass(node, "ui-authored-border") &&
					!paintPanelBorder(colors, geometry, output)) return finishWithAuthoredBorder(false);
				if (node.state.focused && drawFill && isSectionChrome(node))
					return finishWithAuthoredBorder(paintFocusRing(colors, node, geometry, output) &&
						textPainter.paint(node, output));
				return finishWithAuthoredBorder(textPainter.paint(node, output));
			};
	}

	Bool appendAppScrollbarOverlays(
		const UiAppColors& colors, UiDocument& document, UiDisplayList& output)
	{
		Bool succeeded = true;
		document.traverse([&](const UiNodeHandle, UiNode& node)
			{
				if (!node.state.visible) return;
				if (!node.computed.scroll.showVerticalScrollbar &&
					!node.computed.scroll.showHorizontalScrollbar) return;
				const UiRect contentBox{node.computed.contentPosition, node.computed.contentSize};
				const UiRect scrollport = scrollportRect(
					contentBox,
					node.computed.scroll.verticalGutter,
					node.computed.scroll.horizontalGutter);
				const UiScrollAxisState vertical{
					node.computed.scroll.content.y,
					node.computed.scroll.viewport.y,
					node.computed.scroll.offset.y};
				const UiScrollAxisState horizontal{
					node.computed.scroll.content.x,
					node.computed.scroll.viewport.x,
					node.computed.scroll.offset.x};

				if (node.computed.scroll.showVerticalScrollbar)
				{
					const UiScrollbarGeometry bar = verticalScrollbarGeometry(scrollport, vertical);
					if (bar.visible)
					{
						if (!output.addSolidQuad(
							UiQuadGeometry{bar.track.position, bar.track.size, Vec4(0)},
							colors.border)) succeeded = false;
						if (!output.addSolidQuad(
							UiQuadGeometry{bar.thumb.position, bar.thumb.size, Vec4(0)},
							colors.textMuted)) succeeded = false;
					}
				}
				if (node.computed.scroll.showHorizontalScrollbar)
				{
					const UiScrollbarGeometry bar = horizontalScrollbarGeometry(scrollport, horizontal);
					if (bar.visible)
					{
						if (!output.addSolidQuad(
							UiQuadGeometry{bar.track.position, bar.track.size, Vec4(0)},
							colors.border)) succeeded = false;
						if (!output.addSolidQuad(
							UiQuadGeometry{bar.thumb.position, bar.thumb.size, Vec4(0)},
							colors.textMuted)) succeeded = false;
					}
				}
			});
		return succeeded;
	}
}
