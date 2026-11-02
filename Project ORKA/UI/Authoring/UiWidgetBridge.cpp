#include "UiWidgetBridge.hpp"
#include "UiMarkupSurfaceRegistry.hpp"

#include "../Binding/UiExpression.hpp"
#include "../Inspector/UiDomViewerText.hpp"
#include "../Text/UiIconCatalog.hpp"

#include <cctype>

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

		String tagName(const UiMarkupNode& node)
		{
			return lower(node.name);
		}

		String literalAttribute(const UiMarkupNode& node, const String& name)
		{
			const String target = lower(name);
			for (const UiMarkupAttribute& attribute : node.attributes)
				if (attribute.kind == UiAttributeKind::Literal && lower(attribute.name) == target)
					return attribute.value;
			return {};
		}

		Bool hasLiteralAttribute(const UiMarkupNode& node, const String& name)
		{
			return !literalAttribute(node, name).empty();
		}

		Double parseNumber(const String& text, const Double fallback)
		{
			if (text.empty()) return fallback;
			Char* end = nullptr;
			const Double value = std::strtod(text.c_str(), &end);
			return end != text.c_str() && std::isfinite(value) ? value : fallback;
		}

		Bool parseBool(const String& text, const Bool fallback)
		{
			if (text.empty()) return fallback;
			const String folded = lower(text);
			if (folded == "true" || folded == "1" || folded == "yes") return true;
			if (folded == "false" || folded == "0" || folded == "no") return false;
			return fallback;
		}

		UiWidgetCommon commonFromMarkup(
			const UiMarkupNode& node,
			const UiPropertyRegistry& scope,
			const Function<Int()>& allocateTabIndex)
		{
			UiWidgetCommon common;
			common.id = node.id;
			common.label = literalAttribute(node, "label");
			common.classes = node.classes;
			common.enabled = parseBool(literalAttribute(node, "enabled"), true);
			if (allocateTabIndex) common.tabIndex = allocateTabIndex();
			(void)scope;
			return common;
		}

		UiValue evaluateBinding(
			const UiCompiledBinding& binding,
			const UiPropertyRegistry& scope)
		{
			const UiExpressionResult result = UiExpressionVm::evaluate(
				binding.expression, scope, UiExpressionVm::defaultInstructionBudget);
			return result.valid ? result.value : UiValue::null();
		}

		String bindingPath(
			const UiCompiledBundle& bundle,
			UInt blueprintIndex,
			const String& target)
		{
			const String folded = lower(target);
			for (const UiCompiledBinding& binding : bundle.bindings)
			{
				if (binding.nodeIndex != blueprintIndex ||
					binding.kind == UiAttributeKind::IfDirective) continue;
				if (lower(binding.target) == folded &&
					!binding.expression.properties.empty())
					return binding.expression.properties.front();
			}
			return {};
		}
	}

	UiWidgetBridge::UiWidgetBridge(UiWidgetFactory& newFactory, UiPropertyRegistry& newProperties)
		: factory(newFactory), properties(newProperties)
	{
	}

	UiWidgetBridge::~UiWidgetBridge()
	{
		clear();
	}

	void UiWidgetBridge::setPropertyChangeHandler(PropertyChangeHandler handler)
	{
		onPropertyChange = std::move(handler);
	}

	void UiWidgetBridge::setTabIndexAllocator(Function<Int()> allocator)
	{
		allocateTabIndex = std::move(allocator);
	}

	Bool UiWidgetBridge::handles(const UiMarkupNode& node) const
	{
		if (node.kind == UiMarkupNodeKind::Text || node.kind == UiMarkupNodeKind::Comment) return false;
		const String name = tagName(node);
		return name == "button" || name == "toggle" || name == "checkbox" || name == "slider" ||
			name == "select" || name == "input" || name == "number-input" || name == "textarea" ||
			name == "progress" || name == "scroll-view" || name == "image" ||
			name == "dom-viewer" || name == "ui-inspector" ||
			UiMarkupSurfaceRegistry::global().isInteractiveSurfaceTag(name);
	}

	Bool UiWidgetBridge::ownsNode(const UiNodeHandle node) const
	{
		if (!node.isValid()) return false;
		for (const auto& [key, record] : records)
			if (record.node == node) return true;
		return false;
	}

	UiWidgetBridge::Record* UiWidgetBridge::findRecord(const String& stableKey)
	{
		const auto found = records.find(stableKey);
		return found == records.end() ? nullptr : &found->second;
	}

	const UiWidgetBridge::Record* UiWidgetBridge::findRecord(const String& stableKey) const
	{
		const auto found = records.find(stableKey);
		return found == records.end() ? nullptr : &found->second;
	}

	UiWidgetHandle UiWidgetBridge::findById(const String& id) const
	{
		const auto found = idToStableKey.find(id);
		if (found == idToStableKey.end()) return {};
		const Record* record = findRecord(found->second);
		return record ? record->handle : UiWidgetHandle{};
	}

	UiNodeHandle UiWidgetBridge::nodeForId(const String& id) const
	{
		const UiWidgetHandle handle = findById(id);
		return handle.isValid() ? factory.node(handle) : UiNodeHandle{};
	}

	void UiWidgetBridge::writeProperty(const String& path, UiValue value)
	{
		if (path.empty()) return;
		(void)properties.set(path, std::move(value));
		if (onPropertyChange)
		{
			UiValue current;
			(void)properties.read(path, current);
			onPropertyChange(path, current);
		}
	}

	UiValue UiWidgetBridge::readProperty(const String& path, const UiPropertyRegistry& scope) const
	{
		if (path.empty()) return UiValue::null();
		UiValue value;
		if (scope.read(path, value)) return value;
		if (properties.read(path, value)) return value;
		return UiValue::null();
	}

	Vector<UiSelectOption> UiWidgetBridge::readSelectOptions(const UiValue& value) const
	{
		Vector<UiSelectOption> options;
		if (value.type != UiValueType::Array) return options;
		for (const UiValue& entry : value.array)
		{
			if (entry.type != UiValueType::Object) continue;
			UiSelectOption option;
			const auto valueField = entry.object.find("value");
			const auto labelField = entry.object.find("label");
			if (valueField != entry.object.end()) option.value = valueField->second.toString();
			if (labelField != entry.object.end()) option.label = labelField->second.toString();
			if (option.label.empty()) option.label = option.value;
			if (option.value.empty()) continue;
			const auto enabledField = entry.object.find("enabled");
			if (enabledField != entry.object.end()) option.enabled = enabledField->second.truthy();
			options.push_back(std::move(option));
		}
		return options;
	}

	void UiWidgetBridge::bindPropertyPaths(
		const UiCompiledBundle& bundle,
		const UInt blueprintIndex,
		Record& record)
	{
		record.valuePath = bindingPath(bundle, blueprintIndex, "value");
		if (record.valuePath.empty()) record.valuePath = bindingPath(bundle, blueprintIndex, "checked");
		if (record.valuePath.empty()) record.valuePath = bindingPath(bundle, blueprintIndex, "selected");
		record.textPath = bindingPath(bundle, blueprintIndex, "text");
		record.optionsPath = bindingPath(bundle, blueprintIndex, "options");
	}

	void UiWidgetBridge::attachChangeHandlers(Record& record)
	{
		if (!record.handle.isValid()) return;
		auto* slot = factory.get(record.handle);
		if (!slot) return;
		switch (record.kind)
		{
		case UiWidgetKind::Toggle:
		case UiWidgetKind::Checkbox:
			if (auto* state = std::get_if<UiToggleState>(&slot->state))
				state->onChange = [this, path = record.valuePath](UiWidgetHandle, const Bool value)
					{ writeProperty(path, UiValue::fromBool(value)); };
			break;
		case UiWidgetKind::Slider:
		case UiWidgetKind::NumberField:
			if (auto* state = std::get_if<UiSliderState>(&slot->state))
				state->onChange = [this, path = record.valuePath](UiWidgetHandle, const Double value)
					{ writeProperty(path, UiValue::fromNumber(value)); };
			if (auto* state = std::get_if<UiNumberFieldState>(&slot->state))
				state->onChange = [this, path = record.valuePath](UiWidgetHandle, const Double value)
					{ writeProperty(path, UiValue::fromNumber(value)); };
			break;
		case UiWidgetKind::Select:
			if (auto* state = std::get_if<UiSelectState>(&slot->state))
				state->onChange = [this, path = record.valuePath](UiWidgetHandle, const String& value)
					{ writeProperty(path, UiValue::fromString(value)); };
			break;
		case UiWidgetKind::TextField:
		case UiWidgetKind::TextArea:
			if (auto* state = std::get_if<UiTextInputState>(&slot->state))
				state->onChange = [this, path = record.valuePath](UiWidgetHandle, const String& value)
					{ writeProperty(path, UiValue::fromString(value)); };
			break;
		default:
			break;
		}
	}

	void UiWidgetBridge::syncRecordIdMapping(Record& record)
	{
		for (auto iterator = idToStableKey.begin(); iterator != idToStableKey.end();)
		{
			if (iterator->second == record.stableKey) iterator = idToStableKey.erase(iterator);
			else ++iterator;
		}
		if (!record.node.isValid()) return;
		const UiNode* node = factory.document.get(record.node);
		if (!node || node->authored.id.empty()) return;
		idToStableKey[node->authored.id] = record.stableKey;
	}

	void UiWidgetBridge::syncRecordFromScope(Record& record, const UiPropertyRegistry& scope)
	{
		if (!record.handle.isValid()) return;
		if (record.domViewer) return;
		if (!record.valuePath.empty())
		{
			const UiValue value = readProperty(record.valuePath, scope);
			switch (record.kind)
			{
			case UiWidgetKind::Toggle:
			case UiWidgetKind::Checkbox:
				if (value.type == UiValueType::Bool)
					(void)factory.setBoolValue(record.handle, value.boolean, false);
				break;
			case UiWidgetKind::Slider:
			case UiWidgetKind::NumberField:
			case UiWidgetKind::Progress:
				if (value.type == UiValueType::Number)
					(void)factory.setNumberValue(record.handle, value.number, false);
				break;
			case UiWidgetKind::Select:
				if (value.type == UiValueType::String)
					(void)factory.setSelectedValue(record.handle, value.string, false);
				break;
			case UiWidgetKind::TextField:
			case UiWidgetKind::TextArea:
				if (value.type == UiValueType::String)
					(void)factory.setTextValue(record.handle, value.string, false);
				break;
			default:
				break;
			}
		}
		if (!record.textPath.empty())
		{
			const UiValue value = readProperty(record.textPath, scope);
			if (value.type == UiValueType::String)
			{
				if (record.kind == UiWidgetKind::Button)
				{
					if (auto* slot = factory.get(record.handle))
					{
						if (UiNode* root = factory.document.get(slot->root))
						{
							root->authored.text = value.string;
							for (const UiNodeHandle child : root->childHandles())
							{
								if (UiNode* text = factory.document.get(child))
								{
									if (text->authored.typeName != "Text") continue;
									text->authored.text = value.string;
									factory.document.markDirty(child, DirtyFlag::Measure);
								}
							}
							factory.document.markDirty(slot->root, DirtyFlag::Paint);
						}
					}
				}
				else
					(void)factory.setTextValue(record.handle, value.string, false);
			}
		}
		if (!record.optionsPath.empty() && record.kind == UiWidgetKind::Select)
		{
			const UiValue value = readProperty(record.optionsPath, scope);
			if (auto* slot = factory.get(record.handle))
			{
				if (auto* state = std::get_if<UiSelectState>(&slot->state))
				{
					state->options = readSelectOptions(value);
					(void)factory.setSelectedValue(record.handle, state->value, false);
				}
			}
		}
	}

	Bool UiWidgetBridge::ensureInstance(
		const UiCompiledBundle& bundle,
		const UInt blueprintIndex,
		const UiNodeHandle parent,
		const String& stableKey,
		const UiPropertyRegistry& scope,
		UiNodeHandle& outNode,
		const U64 reconcileGeneration)
	{
		if (blueprintIndex >= bundle.blueprint.nodes.size()) return false;
		const UiMarkupNode& blueprint = bundle.blueprint.nodes[blueprintIndex];
		if (!handles(blueprint)) return false;

		Record& record = records[stableKey];
		record.stableKey = stableKey;
		record.seenGeneration = reconcileGeneration;
		if (!blueprint.id.empty()) idToStableKey[blueprint.id] = stableKey;

		const String name = tagName(blueprint);
		const UiWidgetCommon common = commonFromMarkup(blueprint, scope, allocateTabIndex);
		const Bool existing = record.handle.isValid() && factory.isValid(record.handle);

		if (!existing)
		{
			if (name == "button")
			{
				UiButtonConfig config;
				config.parent = parent;
				config.id = common.id;
				config.label = common.label;
				config.classes = common.classes;
				config.enabled = common.enabled;
				config.tabIndex = common.tabIndex;
				config.text = blueprint.text;
				const String textLiteral = literalAttribute(blueprint, "text");
				if (!textLiteral.empty()) config.text = textLiteral;
				record.handle = factory.button(std::move(config));
				record.kind = UiWidgetKind::Button;
			}
			else if (name == "toggle")
			{
				UiToggleConfig config;
				config.parent = parent;
				config.id = common.id;
				config.label = common.label;
				config.classes = common.classes;
				config.enabled = common.enabled;
				config.tabIndex = common.tabIndex;
				config.text = literalAttribute(blueprint, "text");
				if (config.text.empty()) config.text = common.label;
				record.handle = factory.toggle(std::move(config));
				record.kind = UiWidgetKind::Toggle;
			}
			else if (name == "checkbox")
			{
				UiToggleConfig config;
				config.parent = parent;
				config.id = common.id;
				config.label = common.label;
				config.classes = common.classes;
				config.enabled = common.enabled;
				config.tabIndex = common.tabIndex;
				config.text = literalAttribute(blueprint, "text");
				if (config.text.empty()) config.text = common.label;
				record.handle = factory.checkbox(std::move(config));
				record.kind = UiWidgetKind::Checkbox;
			}
			else if (name == "slider")
			{
				UiSliderConfig config;
				config.parent = parent;
				config.id = common.id;
				config.label = common.label;
				config.classes = common.classes;
				config.enabled = common.enabled;
				config.tabIndex = common.tabIndex;
				config.minimum = parseNumber(literalAttribute(blueprint, "min"), 0);
				config.maximum = parseNumber(literalAttribute(blueprint, "max"), 1);
				config.step = parseNumber(literalAttribute(blueprint, "step"), 0);
				record.handle = factory.slider(std::move(config));
				record.kind = UiWidgetKind::Slider;
			}
			else if (name == "select")
			{
				UiSelectConfig config;
				config.parent = parent;
				config.id = common.id;
				config.label = common.label;
				config.classes = common.classes;
				config.enabled = common.enabled;
				config.tabIndex = common.tabIndex;
				record.handle = factory.select(std::move(config));
				record.kind = UiWidgetKind::Select;
			}
			else if (name == "input" || name == "number-input")
			{
				const String inputType = literalAttribute(blueprint, "type");
				const Bool numeric = name == "number-input" || inputType == "number";
				if (numeric)
				{
					UiNumberFieldConfig config;
					config.parent = parent;
					config.id = common.id;
					config.label = common.label;
					config.classes = common.classes;
					config.enabled = common.enabled;
					config.tabIndex = common.tabIndex;
					config.placeholder = literalAttribute(blueprint, "placeholder");
					config.bounded = hasLiteralAttribute(blueprint, "min") ||
						hasLiteralAttribute(blueprint, "max");
					config.integer = parseBool(literalAttribute(blueprint, "integer"), false);
					config.showProgress = parseBool(literalAttribute(blueprint, "progress"), config.bounded);
					config.unit = literalAttribute(blueprint, "unit");
					config.minimum = parseNumber(literalAttribute(blueprint, "min"), 0);
					config.maximum = parseNumber(literalAttribute(blueprint, "max"), 1);
					config.step = parseNumber(literalAttribute(blueprint, "step"), 0);
					record.handle = factory.numberField(std::move(config));
					record.kind = UiWidgetKind::NumberField;
				}
				else
				{
					UiTextInputConfig config;
					config.parent = parent;
					config.id = common.id;
					config.label = common.label;
					config.classes = common.classes;
					config.enabled = common.enabled;
					config.tabIndex = common.tabIndex;
					config.placeholder = literalAttribute(blueprint, "placeholder");
					record.handle = factory.textField(std::move(config));
					record.kind = UiWidgetKind::TextField;
				}
			}
			else if (name == "textarea")
			{
				UiTextInputConfig config;
				config.parent = parent;
				config.id = common.id;
				config.label = common.label;
				config.classes = common.classes;
				config.enabled = common.enabled;
				config.tabIndex = common.tabIndex;
				config.placeholder = literalAttribute(blueprint, "placeholder");
				config.multiline = true;
				record.handle = factory.textArea(std::move(config));
				record.kind = UiWidgetKind::TextArea;
			}
			else if (name == "progress")
			{
				UiProgressConfig config;
				config.parent = parent;
				config.id = common.id;
				config.label = common.label;
				config.classes = common.classes;
				config.enabled = common.enabled;
				config.minimum = parseNumber(literalAttribute(blueprint, "min"), 0);
				config.maximum = parseNumber(literalAttribute(blueprint, "max"), 1);
				config.indeterminate = parseBool(literalAttribute(blueprint, "indeterminate"), false);
				record.handle = factory.progress(std::move(config));
				record.kind = UiWidgetKind::Progress;
			}
			else if (name == "scroll-view")
			{
				UiScrollViewConfig config;
				config.parent = parent;
				config.id = common.id;
				config.label = common.label;
				config.classes = common.classes;
				config.enabled = common.enabled;
				record.handle = factory.scrollView(std::move(config));
				record.kind = UiWidgetKind::ScrollView;
			}
			else if (name == "image")
			{
				UiImageConfig config;
				config.parent = parent;
				config.id = common.id;
				config.classes = common.classes;
				config.enabled = common.enabled;
				const String icon = literalAttribute(blueprint, "icon");
				const String resource = literalAttribute(blueprint, "resource");
				config.resource = !icon.empty() ? icon : resource;
				config.icon = !icon.empty() || parseBool(literalAttribute(blueprint, "icon"), false);
				config.alternateText = common.label;
				record.handle = factory.image(std::move(config));
				record.kind = UiWidgetKind::Image;
			}
			else if (UiMarkupSurfaceRegistry::global().isInteractiveSurfaceTag(name))
			{
				UiInteractiveSurfaceConfig config;
				config.parent = parent;
				config.id = common.id;
				config.label = common.label;
				config.classes = common.classes;
				config.enabled = common.enabled;
				config.tabIndex = common.tabIndex;
				record.handle = factory.interactiveSurface(std::move(config));
				record.kind = UiWidgetKind::InteractiveSurface;
			}
			else if (name == "dom-viewer" || name == "ui-inspector")
			{
				UiTextInputConfig config;
				config.parent = parent;
				config.id = common.id;
				config.label = common.label.empty() ? "DOM viewer" : common.label;
				config.classes = common.classes;
				config.enabled = common.enabled;
				config.readOnly = true;
				config.multiline = true;
				config.placeholder = "Live UI tree…";
				record.handle = factory.textArea(std::move(config));
				record.kind = UiWidgetKind::TextArea;
				record.domViewer = true;
			}

			if (!record.handle.isValid()) return false;
			record.node = factory.node(record.handle);
			bindPropertyPaths(bundle, blueprintIndex, record);
			attachChangeHandlers(record);
			if (record.domViewer) attachDomViewerPicker(record);
		}
		else if (record.node.isValid())
		{
			// Reparent on keyed loop moves.
			// Widget factory nodes are reparented through the document API by the host if needed.
			(void)parent;
		}

		bindPropertyPaths(bundle, blueprintIndex, record);
		syncRecordFromScope(record, scope);
		outNode = record.node;
		return outNode.isValid();
	}

	void UiWidgetBridge::applyInstanceBindings(
		const UiCompiledBundle& bundle,
		const UInt blueprintIndex,
		const String& stableKey,
		const UiPropertyRegistry& scope)
	{
		if (Record* record = findRecord(stableKey))
		{
			bindPropertyPaths(bundle, blueprintIndex, *record);
			syncRecordFromScope(*record, scope);
			syncRecordIdMapping(*record);
		}
	}

	void UiWidgetBridge::destroyRecord(const String& stableKey)
	{
		const auto found = records.find(stableKey);
		if (found == records.end()) return;
		for (auto iterator = idToStableKey.begin(); iterator != idToStableKey.end();)
		{
			if (iterator->second == stableKey) iterator = idToStableKey.erase(iterator);
			else ++iterator;
		}
		if (found->second.handle.isValid()) (void)factory.destroy(found->second.handle);
		records.erase(found);
	}

	void UiWidgetBridge::destroyStale(const U64 reconcileGeneration)
	{
		Vector<String> stale;
		for (const auto& [key, record] : records)
			if (record.seenGeneration != reconcileGeneration) stale.push_back(key);
		for (const String& key : stale) destroyRecord(key);
	}

	void UiWidgetBridge::clear()
	{
		for (auto iterator = records.begin(); iterator != records.end();)
		{
			destroyRecord(iterator->first);
			iterator = records.begin();
		}
		idToStableKey.clear();
	}

	void UiWidgetBridge::syncControlLayouts()
	{
		factory.syncControlLayouts();
	}

	void UiWidgetBridge::setDomViewerPickHandler(Function<void(UiNodeHandle)> handler)
	{
		onDomViewerPick = std::move(handler);
	}

	void UiWidgetBridge::attachDomViewerPicker(Record& record)
	{
		if (!record.domViewer || !record.handle.isValid() || !record.node.isValid()) return;
		const String stableKey = record.stableKey;
		factory.listen(record.handle, record.node, UiRoutedEventType::PointerUp,
			[this, stableKey](UiRoutedEvent& event)
			{
				Record* pickedRecord = findRecord(stableKey);
				if (!pickedRecord || !onDomViewerPick) return;
				const UiTextInputState* state = factory.state<UiTextInputState>(pickedRecord->handle);
				if (!state) return;
				const String& text = state->editor.text();
				const SizeT line = Inspector::domViewerLineIndexAtCaret(text, state->editor.caret());
				if (line >= pickedRecord->domViewerLineHandles.size()) return;
				const UiNodeHandle node = pickedRecord->domViewerLineHandles[line];
				if (node.isValid()) onDomViewerPick(node);
				event.handled = true;
			});
	}

	void UiWidgetBridge::syncDomViewers(
		const UiDocument& document, const UiNodeHandle focused, const UiNodeHandle picked)
	{
		for (auto& [key, record] : records)
		{
			(void)key;
			if (!record.domViewer || !record.handle.isValid()) continue;
			const Inspector::UiDomViewerDump dump = Inspector::buildDomViewerDump(
				document, record.node, focused, picked);
			record.domViewerLineHandles = dump.lineHandles;
			const UiTextInputState* current = factory.state<UiTextInputState>(record.handle);
			if (!current || current->editor.text() != dump.text)
				(void)factory.setTextValue(record.handle, dump.text, false);
		}
	}
}
