#include "UiWidgets.hpp"

#include "../Layout/UiOverflowController.hpp"
#include "../Theme/UiAppFlex.hpp"
#include "../Theme/UiAppHelpers.hpp"
#include "../Theme/UiAppTheme.hpp"

namespace Ui
{
	namespace
	{
		constexpr Int keySpace = 32;
		constexpr Int keyEnter = 257;
		constexpr Int keyTab = 258;
		constexpr Int keyBackspace = 259;
		constexpr Int keyDelete = 261;
		constexpr Int keyRight = 262;
		constexpr Int keyLeft = 263;
		constexpr Int keyDown = 264;
		constexpr Int keyUp = 265;
		constexpr Int keyHome = 268;
		constexpr Int keyEnd = 269;
		constexpr Int modifierShift = 0x0001;
		constexpr Int modifierControl = 0x0002;

		void addClass(UiNode& node, const String& value)
		{
			if (std::find(node.authored.classes.begin(), node.authored.classes.end(), value) ==
				node.authored.classes.end())
				node.authored.classes.push_back(value);
		}

		void removeClass(UiNode& node, const String& value)
		{
			std::erase(node.authored.classes, value);
		}

		String numberText(const Double value)
		{
			String result = std::to_string(value);
			while (result.size() > 1 && result.back() == '0') result.pop_back();
			if (!result.empty() && result.back() == '.') result.pop_back();
			return result;
		}
	}

	String UiWidgetClasses::root(const UiWidgetKind kind)
	{
		switch (kind)
		{
		case UiWidgetKind::Box: return "ui-box";
		case UiWidgetKind::Text: return "ui-text";
		case UiWidgetKind::Image: return "ui-image";
		case UiWidgetKind::Icon: return "ui-icon";
		case UiWidgetKind::ScrollView: return "ui-scroll-view";
		case UiWidgetKind::VirtualList: return "ui-virtual-list";
		case UiWidgetKind::FocusScope: return "ui-focus-scope";
		case UiWidgetKind::Portal: return "ui-portal";
		case UiWidgetKind::Overlay: return "ui-overlay";
		case UiWidgetKind::WorldSurface: return "ui-world-surface";
		case UiWidgetKind::GameViewport: return "ui-game-viewport";
		case UiWidgetKind::Button: return "ui-button";
		case UiWidgetKind::Toggle: return "ui-toggle";
		case UiWidgetKind::Checkbox: return "ui-checkbox";
		case UiWidgetKind::RadioGroup: return "ui-radio-group";
		case UiWidgetKind::Slider: return "ui-slider";
		case UiWidgetKind::Range: return "ui-range";
		case UiWidgetKind::Progress: return "ui-progress";
		case UiWidgetKind::TextField: return "ui-text-field";
		case UiWidgetKind::TextArea: return "ui-text-area";
		case UiWidgetKind::Select: return "ui-select";
		case UiWidgetKind::Combo: return "ui-combo";
		}
		return "ui-widget";
	}

	void UiWidgetPainterRegistry::setPainter(String styleClass, UiNodePainter painter)
	{
		if (styleClass.empty()) return;
		if (painter) painters[std::move(styleClass)] = std::move(painter);
		else painters.erase(styleClass);
	}

	Bool UiWidgetPainterRegistry::removePainter(const String& styleClass)
	{
		return painters.erase(styleClass) != 0;
	}

	Bool UiWidgetPainterRegistry::paint(
		const UiNodePaintContext& context, UiDisplayList& output) const
	{
		for (auto iterator = context.node.authored.classes.rbegin();
			iterator != context.node.authored.classes.rend(); ++iterator)
		{
			const auto painter = painters.find(*iterator);
			if (painter != painters.end()) return painter->second(context, output);
		}
		// Unstyled nodes intentionally emit no visuals.
		return true;
	}

	UiNodePainter UiWidgetPainterRegistry::nodePainter() const
	{
		return [this](const UiNodePaintContext& context, UiDisplayList& output)
		{
			return paint(context, output);
		};
	}

	UiWidgetFactory::UiWidgetFactory(UiDocument& document, UiEventRouter& router)
		: document(document), router(router)
	{
	}

	UiWidgetFactory::~UiWidgetFactory()
	{
		for (Slot& slot : slots)
		{
			if (!slot.occupied) continue;
			if (const auto* scope = std::get_if<UiFocusScopeState>(&slot.state);
				scope && scope->modal) router.popModalScope(slot.root);
			if (const auto* portal = std::get_if<UiPortalState>(&slot.state);
				portal && portal->modal) router.popModalScope(slot.root);
			for (const UiListenerToken token : slot.listeners) (void)router.removeListener(token);
			router.clearNode(slot.root);
		}
	}

	Bool UiWidgetFactory::isValid(const UiWidgetHandle handle) const
	{
		return get(handle) != nullptr;
	}

	UiNodeHandle UiWidgetFactory::node(const UiWidgetHandle handle) const
	{
		const Slot* slot = get(handle);
		return slot ? slot->root : UiNodeHandle{};
	}

	UiWidgetKind UiWidgetFactory::kind(const UiWidgetHandle handle) const
	{
		const Slot* slot = get(handle);
		return slot ? slot->kind : UiWidgetKind::Box;
	}

	UiWidgetFactory::Slot* UiWidgetFactory::get(const UiWidgetHandle handle)
	{
		if (!handle.isValid() || handle.index >= slots.size()) return nullptr;
		Slot& slot = slots[handle.index];
		return slot.occupied && slot.generation == handle.generation ? &slot : nullptr;
	}

	const UiWidgetFactory::Slot* UiWidgetFactory::get(const UiWidgetHandle handle) const
	{
		if (!handle.isValid() || handle.index >= slots.size()) return nullptr;
		const Slot& slot = slots[handle.index];
		return slot.occupied && slot.generation == handle.generation ? &slot : nullptr;
	}

	UiWidgetHandle UiWidgetFactory::create(const UiWidgetKind kind, const UiWidgetCommon& common,
		String typeName, const UiSemanticRole role, UiWidgetState state)
	{
		if (slots.size() >= maximumWidgets && freeSlots.empty()) return {};
		const UiNodeHandle parent = document.isValid(common.parent) ? common.parent : document.root();
		UiAuthoredData authored;
		authored.typeName = std::move(typeName);
		authored.id = common.id;
		authored.classes = common.classes;
		authored.classes.insert(authored.classes.begin(), UiWidgetClasses::root(kind));
		const UiNodeHandle root = document.createNode(parent, std::move(authored));
		if (!root.isValid()) return {};

		UInt index = 0;
		if (freeSlots.empty())
		{
			index = static_cast<UInt>(slots.size());
			slots.emplace_back();
		}
		else
		{
			index = freeSlots.back();
			freeSlots.pop_back();
		}

		Slot& slot = slots[index];
		slot.kind = kind;
		slot.root = root;
		slot.state = std::move(state);
		slot.listeners.clear();
		slot.occupied = true;
		slot.readOnly = common.readOnly;
		const UiWidgetHandle handle{index, slot.generation};

		if (UiNode* node = document.get(root))
		{
			node->state.enabled = common.enabled;
			node->state.semantics.role = role;
			node->state.semantics.label = common.label;
			if (common.readOnly) addClass(*node, UiWidgetClasses::readOnly);
		}
		return handle;
	}

	UiNodeHandle UiWidgetFactory::child(
		const UiNodeHandle parent, String type, String styleClass, String text)
	{
		UiAuthoredData authored;
		authored.typeName = std::move(type);
		authored.text = std::move(text);
		authored.classes.push_back(std::move(styleClass));
		return document.createNode(parent, std::move(authored));
	}

	void UiWidgetFactory::listen(const UiWidgetHandle widget, const UiNodeHandle node,
		const UiRoutedEventType type, UiEventCallback callback)
	{
		Slot* slot = get(widget);
		if (!slot) return;
		const UiListenerToken token = router.addListener(node, type, std::move(callback));
		if (token.isValid()) slot->listeners.push_back(token);
	}

	void UiWidgetFactory::configureInteraction(const UiNodeHandle node,
		const UiWidgetCommon& common, const Bool focusable, const Bool scope, const Bool modal)
	{
		UiInteractionData interaction;
		interaction.hitTestVisible = common.enabled;
		interaction.focusable = focusable && common.enabled;
		interaction.focusScope = scope;
		interaction.modalScope = modal;
		interaction.tabIndex = focusable ? common.tabIndex : -1;
		router.setInteraction(node, interaction);
	}

	Bool UiWidgetFactory::destroy(const UiWidgetHandle handle)
	{
		Slot* slot = get(handle);
		if (!slot) return false;
		if (const auto* scope = std::get_if<UiFocusScopeState>(&slot->state);
			scope && scope->modal) router.popModalScope(slot->root);
		if (const auto* portal = std::get_if<UiPortalState>(&slot->state);
			portal && portal->modal) router.popModalScope(slot->root);
		for (const UiListenerToken token : slot->listeners) (void)router.removeListener(token);
		router.clearNode(slot->root);
		if (document.isValid(slot->root) && !document.destroyNode(slot->root)) return false;
		slot->occupied = false;
		slot->listeners.clear();
		slot->root = {};
		++slot->generation;
		if (slot->generation == 0) slot->generation = 1;
		freeSlots.push_back(handle.index);
		return true;
	}

	void UiWidgetFactory::collectInvalid()
	{
		for (UInt index = 0; index < slots.size(); ++index)
		{
			Slot& slot = slots[index];
			if (!slot.occupied || document.isValid(slot.root)) continue;
			for (const UiListenerToken token : slot.listeners) (void)router.removeListener(token);
			slot.occupied = false;
			slot.listeners.clear();
			++slot.generation;
			if (slot.generation == 0) slot.generation = 1;
			freeSlots.push_back(index);
		}
	}

	UiWidgetHandle UiWidgetFactory::box(UiBoxConfig config)
	{
		return create(UiWidgetKind::Box, config, "Box", UiSemanticRole::None, UiBoxState{});
	}

	UiWidgetHandle UiWidgetFactory::text(UiTextConfig config)
	{
		const String value = config.text;
		const UiWidgetHandle handle = create(
			UiWidgetKind::Text, config, "Text", UiSemanticRole::Text, UiTextState{value});
		if (UiNode* root = document.get(node(handle)))
		{
			root->authored.text = value;
			root->state.semantics.value = value;
		}
		return handle;
	}

	UiWidgetHandle UiWidgetFactory::image(UiImageConfig config)
	{
		const UiWidgetKind kind = config.icon ? UiWidgetKind::Icon : UiWidgetKind::Image;
		const UiWidgetHandle handle = create(kind, config, config.icon ? "Icon" : "Image",
			UiSemanticRole::Image, UiImageState{config.resource, config.alternateText, config.icon});
		if (UiNode* root = document.get(node(handle)))
		{
			root->authored.text.clear();
			root->state.semantics.value = config.resource;
			root->state.semantics.label =
				config.alternateText.empty() ? config.label : config.alternateText;
		}
		return handle;
	}

	UiWidgetHandle UiWidgetFactory::scrollView(UiScrollViewConfig config)
	{
		UiScrollViewState state;
		state.scroll.setOverscrollPolicy(config.horizontalPolicy, config.verticalPolicy);
		const UiWidgetHandle handle = create(UiWidgetKind::ScrollView, config,
			"ScrollView", UiSemanticRole::None, std::move(state));
		configureInteraction(node(handle), config, false);
		listen(handle, node(handle), UiRoutedEventType::Wheel,
			[this, handle](UiRoutedEvent& event)
			{
				Slot* slot = get(handle);
				auto* state = slot ? std::get_if<UiScrollViewState>(&slot->state) : nullptr;
				if (!slot || !state || !document.get(slot->root)->state.enabled) return;
				const UiScrollConsumption consumed = state->scroll.consumeScroll(
					Vec2(static_cast<Float>(event.delta.x), static_cast<Float>(event.delta.y)));
				if (consumed.consumed.x != 0 || consumed.consumed.y != 0)
				{
					syncScroll(*slot);
					event.handled = true;
				}
			});
		attachScrollbar(handle);
		return handle;
	}

	UiWidgetHandle UiWidgetFactory::virtualList(UiVirtualListConfig config)
	{
		UiVirtualListState state;
		state.list = UiVirtualList(config.estimatedItemHeight);
		(void)state.list.setKeys(std::move(config.keys));
		state.scroll.setOverscrollPolicy(config.horizontalPolicy, config.verticalPolicy);
		state.overscanExtent = config.overscanExtent;
		state.buildItem = std::move(config.buildItem);
		const UiWidgetHandle handle = create(UiWidgetKind::VirtualList, config,
			"VirtualList", UiSemanticRole::List, std::move(state));
		configureInteraction(node(handle), config, false);
		listen(handle, node(handle), UiRoutedEventType::Wheel,
			[this, handle](UiRoutedEvent& event)
			{
				Slot* slot = get(handle);
				auto* state = slot ? std::get_if<UiVirtualListState>(&slot->state) : nullptr;
				if (!slot || !state || !document.get(slot->root)->state.enabled) return;
				const UiScrollConsumption consumed = state->scroll.consumeScroll(
					Vec2(static_cast<Float>(event.delta.x), static_cast<Float>(event.delta.y)));
				if (consumed.consumed.x != 0 || consumed.consumed.y != 0)
				{
					// Item builders may create widgets, so the slot is refetched.
					(void)materializeVirtualList(handle);
					if (Slot* refreshed = get(handle)) syncScroll(*refreshed);
					event.handled = true;
				}
			});
		attachScrollbar(handle);
		return handle;
	}

	UiWidgetHandle UiWidgetFactory::focusScope(UiFocusScopeConfig config)
	{
		const UiWidgetHandle handle = create(UiWidgetKind::FocusScope, config,
			"FocusScope", UiSemanticRole::None, UiFocusScopeState{config.modal});
		configureInteraction(node(handle), config, false, true, config.modal);
		if (config.modal) router.pushModalScope(node(handle));
		return handle;
	}

	UiWidgetHandle UiWidgetFactory::portal(UiPortalConfig config)
	{
		const UiNodeHandle target = document.isValid(config.target) ? config.target : document.root();
		config.parent = target;
		const UiWidgetHandle handle = create(UiWidgetKind::Portal, config, "Portal",
			UiSemanticRole::None, UiPortalState{config.logicalOwner, target, config.modal});
		configureInteraction(node(handle), config, false, config.modal, config.modal);
		if (config.modal) router.pushModalScope(node(handle));
		return handle;
	}

	UiWidgetHandle UiWidgetFactory::overlay(UiPortalConfig config)
	{
		const UiNodeHandle target = document.isValid(config.target) ? config.target : document.root();
		config.parent = target;
		const UiWidgetHandle handle = create(UiWidgetKind::Overlay, config, "Overlay",
			UiSemanticRole::Dialog, UiPortalState{config.logicalOwner, target, config.modal});
		configureInteraction(node(handle), config, false, true, config.modal);
		if (config.modal) router.pushModalScope(node(handle));
		return handle;
	}

	UiWidgetHandle UiWidgetFactory::worldSurface(UiWorldSurfaceConfig config)
	{
		const UiWidgetHandle handle = create(UiWidgetKind::WorldSurface, config, "WorldSurface",
			UiSemanticRole::Application,
			UiWorldSurfaceState{std::move(config.metadata), std::move(config.rayMapper)});
		configureInteraction(node(handle), config, false, true);
		return handle;
	}

	UiWidgetHandle UiWidgetFactory::gameViewport(UiGameViewportConfig config)
	{
		const UiWidgetHandle handle = create(UiWidgetKind::GameViewport, config, "GameViewport",
			UiSemanticRole::Application, UiGameViewportState{std::move(config.render)});
		configureInteraction(node(handle), config, true);
		return handle;
	}

	UiWidgetHandle UiWidgetFactory::button(UiButtonConfig config)
	{
		const UiWidgetHandle handle = create(UiWidgetKind::Button, config, "Button",
			UiSemanticRole::Button, UiButtonState{false, std::move(config.onActivate)});
		const UiNodeHandle root = node(handle);
		if (!config.text.empty())
			(void)child(root, "Text", UiWidgetClasses::label, config.text);
		configureInteraction(root, config, true);
		listen(handle, root, UiRoutedEventType::PointerDown,
			[this, handle](UiRoutedEvent& event)
			{
				Slot* slot = get(handle);
				auto* state = slot ? std::get_if<UiButtonState>(&slot->state) : nullptr;
				UiNode* root = slot ? document.get(slot->root) : nullptr;
				if (!state || !root || !root->state.enabled || slot->readOnly) return;
				state->pressed = true;
				root->state.active = true;
				event.capturePointer = true;
				event.focusTarget = true;
				markChanged(*slot);
			});
		listen(handle, root, UiRoutedEventType::PointerUp,
			[this, handle](UiRoutedEvent& event)
			{
				Slot* slot = get(handle);
				auto* state = slot ? std::get_if<UiButtonState>(&slot->state) : nullptr;
				UiNode* root = slot ? document.get(slot->root) : nullptr;
				if (!state || !root) return;
				state->pressed = false;
				root->state.active = false;
				event.releasePointer = true;
				markChanged(*slot);
			});
		listen(handle, root, UiRoutedEventType::Activate,
			[this, handle](UiRoutedEvent& event)
			{
				Slot* slot = get(handle);
				auto* state = slot ? std::get_if<UiButtonState>(&slot->state) : nullptr;
				const UiNode* root = slot ? document.get(slot->root) : nullptr;
				if (!state || !root || !root->state.enabled || slot->readOnly) return;
				if (state->onActivate) state->onActivate(handle);
				event.handled = true;
			});
		return handle;
	}

	UiWidgetHandle UiWidgetFactory::toggle(UiToggleConfig config)
	{
		const UiWidgetHandle handle = create(UiWidgetKind::Toggle, config, "Toggle",
			UiSemanticRole::Button, UiToggleState{config.value, false, std::move(config.onChange)});
		const UiNodeHandle root = node(handle);
		(void)child(root, "Indicator", UiWidgetClasses::indicator);
		(void)child(root, "Text", UiWidgetClasses::label, config.text);
		configureInteraction(root, config, true);
		listen(handle, root, UiRoutedEventType::Activate,
			[this, handle](UiRoutedEvent& event)
			{
				Slot* slot = get(handle);
				auto* state = slot ? std::get_if<UiToggleState>(&slot->state) : nullptr;
				const UiNode* root = slot ? document.get(slot->root) : nullptr;
				if (!state || !root || !root->state.enabled || slot->readOnly) return;
				state->value = !state->value;
				syncToggle(*slot);
				if (state->onChange) state->onChange(handle, state->value);
				event.handled = true;
			});
		if (Slot* slot = get(handle)) syncToggle(*slot);
		return handle;
	}

	UiWidgetHandle UiWidgetFactory::checkbox(UiToggleConfig config)
	{
		const UiWidgetHandle handle = toggle(std::move(config));
		Slot* slot = get(handle);
		if (!slot) return {};
		slot->kind = UiWidgetKind::Checkbox;
		if (UiNode* root = document.get(slot->root))
		{
			root->authored.typeName = "Checkbox";
			root->authored.classes.front() = UiWidgetClasses::root(UiWidgetKind::Checkbox);
			root->state.semantics.role = UiSemanticRole::CheckBox;
		}
		return handle;
	}

	UiWidgetHandle UiWidgetFactory::radioGroup(UiRadioGroupConfig config)
	{
		const UiWidgetHandle handle = create(UiWidgetKind::RadioGroup, config, "RadioGroup",
			UiSemanticRole::None, UiRadioGroupState{config.options, {}, config.value, std::move(config.onChange)});
		const UiNodeHandle root = node(handle);
		configureInteraction(root, config, false, true);
		Slot* slot = get(handle);
		auto* state = slot ? std::get_if<UiRadioGroupState>(&slot->state) : nullptr;
		if (!slot || !state) return {};
		for (UInt index = 0; index < state->options.size(); ++index)
		{
			const UiRadioOption option = state->options[index];
			const UiNodeHandle optionNode = child(root, "Radio", "ui-radio");
			state->optionNodes.push_back(optionNode);
			(void)child(optionNode, "Indicator", UiWidgetClasses::indicator);
			(void)child(optionNode, "Text", UiWidgetClasses::label, option.label);
			if (UiNode* node = document.get(optionNode))
			{
				node->state.enabled = config.enabled && option.enabled;
				node->state.semantics.role = UiSemanticRole::Radio;
				node->state.semantics.label = option.label;
				node->state.semantics.value = option.value;
				node->state.semantics.checked = option.value == state->value;
				if (node->state.semantics.checked) addClass(*node, UiWidgetClasses::checked);
			}
			UiWidgetCommon optionCommon = config;
			optionCommon.enabled = config.enabled && option.enabled;
			optionCommon.tabIndex = option.value == state->value ? config.tabIndex : -1;
			configureInteraction(optionNode, optionCommon, true);
			listen(handle, optionNode, UiRoutedEventType::Activate,
				[this, handle, index](UiRoutedEvent& event)
				{
					Slot* slot = get(handle);
					auto* state = slot ? std::get_if<UiRadioGroupState>(&slot->state) : nullptr;
					if (!slot || !state || slot->readOnly || index >= state->options.size() ||
						!state->options[index].enabled) return;
					(void)setSelectedValue(handle, state->options[index].value, true);
					event.handled = true;
				});
		}
		return handle;
	}

	UiWidgetHandle UiWidgetFactory::slider(UiSliderConfig config)
	{
		UiSliderState state;
		state.minimum = config.minimum;
		state.maximum = std::max(config.minimum, config.maximum);
		state.step = std::max(0.0, config.step);
		state.value = quantize(config.value, state.minimum, state.maximum, state.step);
		state.onChange = std::move(config.onChange);
		const UiWidgetHandle handle = create(UiWidgetKind::Slider, config, "Slider",
			UiSemanticRole::Slider, std::move(state));
		const UiNodeHandle root = node(handle);
		(void)child(root, "Track", UiWidgetClasses::track);
		(void)child(root, "Thumb", UiWidgetClasses::thumb);
		configureInteraction(root, config, true);
		const auto updateFromPointer = [this, handle](UiRoutedEvent& event)
		{
			Slot* slot = get(handle);
			auto* state = slot ? std::get_if<UiSliderState>(&slot->state) : nullptr;
			const UiNode* root = slot ? document.get(slot->root) : nullptr;
			if (!state || !root || !root->state.enabled || slot->readOnly) return;
			state->value = pointerValue(*slot, event);
			syncSlider(*slot);
			if (state->onChange) state->onChange(handle, state->value);
			event.handled = true;
		};
		listen(handle, root, UiRoutedEventType::PointerDown,
			[updateFromPointer](UiRoutedEvent& event) mutable
			{
				updateFromPointer(event);
				event.capturePointer = true;
				event.focusTarget = true;
			});
		listen(handle, root, UiRoutedEventType::Drag, updateFromPointer);
		listen(handle, root, UiRoutedEventType::PointerUp,
			[](UiRoutedEvent& event) { event.releasePointer = true; });
		listen(handle, root, UiRoutedEventType::KeyDown,
			[this, handle](UiRoutedEvent& event)
			{
				if (event.key != keyLeft && event.key != keyDown &&
					event.key != keyRight && event.key != keyUp) return;
				Slot* slot = get(handle);
				auto* state = slot ? std::get_if<UiSliderState>(&slot->state) : nullptr;
				const UiNode* root = slot ? document.get(slot->root) : nullptr;
				if (!state || !root || !root->state.enabled || slot->readOnly) return;
				const Double increment = state->step > 0 ? state->step :
					std::max((state->maximum - state->minimum) / 100.0, 0.01);
				const Double direction =
					event.key == keyLeft || event.key == keyDown ? -1.0 : 1.0;
				state->value = quantize(state->value + increment * direction,
					state->minimum, state->maximum, state->step);
				syncSlider(*slot);
				if (state->onChange) state->onChange(handle, state->value);
				event.handled = true;
			});
		if (Slot* slot = get(handle)) syncSlider(*slot);
		return handle;
	}

	UiWidgetHandle UiWidgetFactory::range(UiRangeConfig config)
	{
		UiSliderState state;
		state.minimum = config.minimum;
		state.maximum = std::max(config.minimum, config.maximum);
		state.step = std::max(0.0, config.step);
		state.value = quantize(config.value, state.minimum, state.maximum, state.step);
		state.upperValue = quantize(config.upperValue, state.minimum, state.maximum, state.step);
		if (state.value > state.upperValue) std::swap(state.value, state.upperValue);
		state.range = true;
		state.onRangeChange = std::move(config.onRangeChange);
		const UiWidgetHandle handle = create(UiWidgetKind::Range, config, "Range",
			UiSemanticRole::Slider, std::move(state));
		const UiNodeHandle root = node(handle);
		(void)child(root, "Track", UiWidgetClasses::track);
		(void)child(root, "Thumb", "ui-thumb-lower");
		(void)child(root, "Thumb", "ui-thumb-upper");
		configureInteraction(root, config, true);
		listen(handle, root, UiRoutedEventType::PointerDown,
			[this, handle](UiRoutedEvent& event)
			{
				Slot* slot = get(handle);
				auto* state = slot ? std::get_if<UiSliderState>(&slot->state) : nullptr;
				if (!slot || !state || slot->readOnly) return;
				const Double value = pointerValue(*slot, event);
				state->draggingUpper =
					std::abs(value - state->upperValue) < std::abs(value - state->value);
				if (state->draggingUpper) state->upperValue = std::max(value, state->value);
				else state->value = std::min(value, state->upperValue);
				syncSlider(*slot);
				if (state->onRangeChange) state->onRangeChange(handle, state->value, state->upperValue);
				event.capturePointer = true;
				event.focusTarget = true;
				event.handled = true;
			});
		listen(handle, root, UiRoutedEventType::Drag,
			[this, handle](UiRoutedEvent& event)
			{
				Slot* slot = get(handle);
				auto* state = slot ? std::get_if<UiSliderState>(&slot->state) : nullptr;
				if (!slot || !state || slot->readOnly) return;
				const Double value = pointerValue(*slot, event);
				if (state->draggingUpper) state->upperValue = std::max(value, state->value);
				else state->value = std::min(value, state->upperValue);
				syncSlider(*slot);
				if (state->onRangeChange) state->onRangeChange(handle, state->value, state->upperValue);
				event.handled = true;
			});
		listen(handle, root, UiRoutedEventType::PointerUp,
			[](UiRoutedEvent& event) { event.releasePointer = true; });
		listen(handle, root, UiRoutedEventType::KeyDown,
			[this, handle](UiRoutedEvent& event)
			{
				if (event.key != keyLeft && event.key != keyDown &&
					event.key != keyRight && event.key != keyUp) return;
				Slot* slot = get(handle);
				auto* state = slot ? std::get_if<UiSliderState>(&slot->state) : nullptr;
				const UiNode* root = slot ? document.get(slot->root) : nullptr;
				if (!state || !root || !root->state.enabled || slot->readOnly) return;
				const Double increment = state->step > 0 ? state->step :
					std::max((state->maximum - state->minimum) / 100.0, 0.01);
				const Double direction =
					event.key == keyLeft || event.key == keyDown ? -1.0 : 1.0;
				const Bool upper = (event.modifiers & modifierShift) != 0;
				if (upper)
					state->upperValue = std::max(state->value, quantize(
						state->upperValue + increment * direction,
						state->minimum, state->maximum, state->step));
				else
					state->value = std::min(state->upperValue, quantize(
						state->value + increment * direction,
						state->minimum, state->maximum, state->step));
				syncSlider(*slot);
				if (state->onRangeChange)
					state->onRangeChange(handle, state->value, state->upperValue);
				event.handled = true;
				event.preventDefault();
			});
		if (Slot* slot = get(handle)) syncSlider(*slot);
		return handle;
	}

	UiWidgetHandle UiWidgetFactory::progress(UiProgressConfig config)
	{
		UiProgressState state{config.minimum, std::max(config.minimum, config.maximum),
			config.value, config.indeterminate};
		state.value = std::clamp(state.value, state.minimum, state.maximum);
		const UiWidgetHandle handle = create(UiWidgetKind::Progress, config, "Progress",
			UiSemanticRole::ProgressBar, state);
		(void)child(node(handle), "Track", UiWidgetClasses::track);
		(void)child(node(handle), "Indicator", UiWidgetClasses::indicator);
		(void)setProgress(handle, state.value, state.indeterminate);
		return handle;
	}

	UiWidgetHandle UiWidgetFactory::textField(UiTextInputConfig config)
	{
		UiTextInputState state(config.limits);
		UiTextEditMetadata metadata;
		metadata.readOnly = config.readOnly;
		metadata.password = config.password;
		metadata.multiline = false;
		state.editor.setMetadata(metadata);
		state.editor.setValidationCallback(std::move(config.validate));
		(void)state.editor.setText(std::move(config.value));
		state.placeholder = std::move(config.placeholder);
		state.onChange = std::move(config.onChange);
		state.onSubmit = std::move(config.onSubmit);
		const UiWidgetHandle handle = create(UiWidgetKind::TextField, config, "TextField",
			UiSemanticRole::TextBox, std::move(state));
		configureInteraction(node(handle), config, true);
		if (Slot* slot = get(handle)) syncText(*slot);

		const auto changed = [this, handle]()
		{
			Slot* slot = get(handle);
			auto* state = slot ? std::get_if<UiTextInputState>(&slot->state) : nullptr;
			if (!slot || !state) return;
			syncText(*slot);
			if (state->onChange) state->onChange(handle, state->editor.text());
		};
		listen(handle, node(handle), UiRoutedEventType::TextInput,
			[this, handle, changed](UiRoutedEvent& event)
			{
				Slot* slot = get(handle);
				auto* state = slot ? std::get_if<UiTextInputState>(&slot->state) : nullptr;
				if (!slot || !state || slot->readOnly || !document.get(slot->root)->state.enabled) return;
				if (state->editor.insert(event.text, event.timestampMicroseconds)) changed();
				event.handled = true;
			});
		listen(handle, node(handle), UiRoutedEventType::CompositionStart,
			[this, handle](UiRoutedEvent&)
			{
				if (auto* state = this->state<UiTextInputState>(handle)) state->editor.compositionStart();
			});
		listen(handle, node(handle), UiRoutedEventType::CompositionUpdate,
			[this, handle](UiRoutedEvent& event)
			{
				if (auto* state = this->state<UiTextInputState>(handle))
					(void)state->editor.compositionUpdate(event.text);
			});
		listen(handle, node(handle), UiRoutedEventType::CompositionCommit,
			[this, handle, changed](UiRoutedEvent& event)
			{
				if (auto* state = this->state<UiTextInputState>(handle))
					if (state->editor.compositionCommit(event.text, event.timestampMicroseconds)) changed();
			});
		listen(handle, node(handle), UiRoutedEventType::CompositionCancel,
			[this, handle](UiRoutedEvent&)
			{
				if (auto* state = this->state<UiTextInputState>(handle)) state->editor.compositionCancel();
			});
		listen(handle, node(handle), UiRoutedEventType::KeyDown,
			[this, handle, changed](UiRoutedEvent& event)
			{
				Slot* slot = get(handle);
				auto* state = slot ? std::get_if<UiTextInputState>(&slot->state) : nullptr;
				if (!slot || !state || !document.get(slot->root)->state.enabled) return;
				const Bool shift = (event.modifiers & modifierShift) != 0;
				const Bool control = (event.modifiers & modifierControl) != 0;
				Bool didEdit = false;
				switch (event.key)
				{
				case keyBackspace: didEdit = state->editor.deleteBackward(event.timestampMicroseconds); break;
				case keyDelete: didEdit = state->editor.deleteForward(event.timestampMicroseconds); break;
				case keyLeft:
					if (control) state->editor.moveByWord(-1, shift);
					else state->editor.moveByCluster(-1, shift);
					break;
				case keyRight:
					if (control) state->editor.moveByWord(1, shift);
					else state->editor.moveByCluster(1, shift);
					break;
				case keyUp: state->editor.moveVertically(-1, shift); break;
				case keyDown: state->editor.moveVertically(1, shift); break;
				case keyHome: state->editor.moveToLineBoundary(false, shift); break;
				case keyEnd: state->editor.moveToLineBoundary(true, shift); break;
				case keyEnter:
					if (state->editor.metadata().multiline && !slot->readOnly)
						didEdit = state->editor.insert("\n", event.timestampMicroseconds);
					else if (state->onSubmit) state->onSubmit(handle);
					break;
				case 'A':
					if (control) (void)state->editor.execute(UiTextEditCommand::SelectAll);
					else return;
					break;
				case 'Z':
					if (control && !slot->readOnly)
						didEdit = shift ? state->editor.redo() : state->editor.undo();
					else return;
					break;
				case 'Y':
					if (control && !slot->readOnly) didEdit = state->editor.redo();
					else return;
					break;
				case keyTab: return;
				default: return;
				}
				if (didEdit) changed();
				else syncText(*slot);
				event.handled = true;
				event.preventDefault();
			});
		return handle;
	}

	UiWidgetHandle UiWidgetFactory::textArea(UiTextInputConfig config)
	{
		config.multiline = true;
		const UiWidgetHandle handle = textField(std::move(config));
		Slot* slot = get(handle);
		if (!slot) return {};
		slot->kind = UiWidgetKind::TextArea;
		if (auto* state = std::get_if<UiTextInputState>(&slot->state))
		{
			UiTextEditMetadata metadata = state->editor.metadata();
			metadata.multiline = true;
			state->editor.setMetadata(metadata);
		}
		if (UiNode* root = document.get(slot->root))
		{
			root->authored.typeName = "TextArea";
			root->authored.classes.front() = UiWidgetClasses::root(UiWidgetKind::TextArea);
		}
		return handle;
	}

	UiWidgetHandle UiWidgetFactory::select(UiSelectConfig config)
	{
		const UiWidgetHandle handle = create(UiWidgetKind::Select, config, "Select",
			UiSemanticRole::Menu, UiSelectState{config.options, {}, config.value, false,
				false, std::move(config.onChange)});
		const UiNodeHandle root = node(handle);
		(void)child(root, "Text", UiWidgetClasses::label);
		(void)child(root, "Indicator", UiWidgetClasses::indicator);
		const UiNodeHandle menu = child(root, "Menu", "ui-select-menu");
		configureInteraction(root, config, true);
		Slot* slot = get(handle);
		auto* state = slot ? std::get_if<UiSelectState>(&slot->state) : nullptr;
		if (!slot || !state) return {};
		for (UInt index = 0; index < state->options.size(); ++index)
		{
			const UiSelectOption option = state->options[index];
			const UiNodeHandle optionNode = child(menu, "MenuItem", "ui-select-option", option.label);
			state->optionNodes.push_back(optionNode);
			if (UiNode* node = document.get(optionNode))
			{
				node->state.enabled = option.enabled;
				node->state.visible = false;
				node->state.semantics.role = UiSemanticRole::MenuItem;
				node->state.semantics.label = option.label;
				node->state.semantics.value = option.value;
			}
			UiWidgetCommon optionCommon = config;
			optionCommon.enabled = option.enabled;
			configureInteraction(optionNode, optionCommon, true);
			listen(handle, optionNode, UiRoutedEventType::Activate,
				[this, handle, index](UiRoutedEvent& event)
				{
					Slot* slot = get(handle);
					auto* state = slot ? std::get_if<UiSelectState>(&slot->state) : nullptr;
					if (!slot || !state || slot->readOnly || index >= state->options.size() ||
						!state->options[index].enabled) return;
					(void)setSelectedValue(handle, state->options[index].value, true);
					state->open = false;
					syncSelect(*slot);
					event.handled = true;
					event.stopPropagation();
				});
		}
		listen(handle, root, UiRoutedEventType::Activate,
			[this, handle](UiRoutedEvent& event)
			{
				Slot* slot = get(handle);
				auto* state = slot ? std::get_if<UiSelectState>(&slot->state) : nullptr;
				if (!slot || !state || slot->readOnly || !document.get(slot->root)->state.enabled) return;
				state->open = !state->open;
				syncSelect(*slot);
				event.handled = true;
			});
		listen(handle, root, UiRoutedEventType::KeyDown,
			[this, handle](UiRoutedEvent& event)
			{
				Slot* slot = get(handle);
				auto* state = slot ? std::get_if<UiSelectState>(&slot->state) : nullptr;
				if (!slot || !state || slot->readOnly || state->options.empty()) return;
				if (event.key != keyUp && event.key != keyDown) return;
				SizeT selected = 0;
				for (SizeT index = 0; index < state->options.size(); ++index)
					if (state->options[index].value == state->value) selected = index;
				const Int direction = event.key == keyUp ? -1 : 1;
				for (SizeT count = 0; count < state->options.size(); ++count)
				{
					selected = static_cast<SizeT>(
						(static_cast<Int>(selected) + direction +
							static_cast<Int>(state->options.size())) %
						static_cast<Int>(state->options.size()));
					if (state->options[selected].enabled)
					{
						(void)setSelectedValue(handle, state->options[selected].value, true);
						break;
					}
				}
				event.handled = true;
				event.preventDefault();
			});
		syncSelect(*slot);
		return handle;
	}

	UiWidgetHandle UiWidgetFactory::combo(UiSelectConfig config)
	{
		config.editable = true;
		const UiWidgetHandle handle = select(std::move(config));
		Slot* slot = get(handle);
		if (!slot) return {};
		slot->kind = UiWidgetKind::Combo;
		if (auto* state = std::get_if<UiSelectState>(&slot->state)) state->editable = true;
		if (UiNode* root = document.get(slot->root))
		{
			root->authored.typeName = "Combo";
			root->authored.classes.front() = UiWidgetClasses::root(UiWidgetKind::Combo);
			root->state.semantics.role = UiSemanticRole::TextBox;
		}
		listen(handle, slot->root, UiRoutedEventType::TextInput,
			[this, handle](UiRoutedEvent& event)
			{
				Slot* slot = get(handle);
				auto* state = slot ? std::get_if<UiSelectState>(&slot->state) : nullptr;
				const UiNode* root = slot ? document.get(slot->root) : nullptr;
				if (!slot || !state || !root || !root->state.enabled || slot->readOnly ||
					event.text.empty()) return;
				state->value += event.text;
				state->open = true;
				syncSelect(*slot);
				if (state->onChange) state->onChange(handle, state->value);
				event.handled = true;
			});
		listen(handle, slot->root, UiRoutedEventType::KeyDown,
			[this, handle](UiRoutedEvent& event)
			{
				if (event.key != keyBackspace && event.key != keyDelete) return;
				Slot* slot = get(handle);
				auto* state = slot ? std::get_if<UiSelectState>(&slot->state) : nullptr;
				const UiNode* root = slot ? document.get(slot->root) : nullptr;
				if (!slot || !state || !root || !root->state.enabled || slot->readOnly ||
					state->value.empty()) return;
				const Vector<SizeT> boundaries = UiTextEditModel::clusterBoundaries(state->value);
				if (boundaries.size() > 1) state->value.erase(boundaries[boundaries.size() - 2]);
				syncSelect(*slot);
				if (state->onChange) state->onChange(handle, state->value);
				event.handled = true;
				event.preventDefault();
			});
		return handle;
	}

	Bool UiWidgetFactory::setEnabled(const UiWidgetHandle handle, const Bool enabled)
	{
		Slot* slot = get(handle);
		UiNode* root = slot ? document.get(slot->root) : nullptr;
		if (!slot || !root) return false;
		root->state.enabled = enabled;
		UiInteractionData interaction = router.interaction(slot->root);
		interaction.hitTestVisible = enabled;
		interaction.focusable = enabled && interaction.tabIndex >= 0;
		router.setInteraction(slot->root, interaction);
		markChanged(*slot);
		return true;
	}

	Bool UiWidgetFactory::setReadOnly(const UiWidgetHandle handle, const Bool readOnly)
	{
		Slot* slot = get(handle);
		UiNode* root = slot ? document.get(slot->root) : nullptr;
		if (!slot || !root) return false;
		slot->readOnly = readOnly;
		if (readOnly) addClass(*root, UiWidgetClasses::readOnly);
		else removeClass(*root, UiWidgetClasses::readOnly);
		if (auto* state = std::get_if<UiTextInputState>(&slot->state))
		{
			UiTextEditMetadata metadata = state->editor.metadata();
			metadata.readOnly = readOnly;
			state->editor.setMetadata(metadata);
		}
		markChanged(*slot);
		return true;
	}

	Bool UiWidgetFactory::setBoolValue(
		const UiWidgetHandle handle, const Bool value, const Bool notify)
	{
		Slot* slot = get(handle);
		auto* state = slot ? std::get_if<UiToggleState>(&slot->state) : nullptr;
		if (!slot || !state) return false;
		if (state->value == value) return true;
		state->value = value;
		syncToggle(*slot);
		if (notify && state->onChange) state->onChange(handle, value);
		return true;
	}

	Bool UiWidgetFactory::setNumberValue(
		const UiWidgetHandle handle, const Double value, const Bool notify)
	{
		Slot* slot = get(handle);
		auto* state = slot ? std::get_if<UiSliderState>(&slot->state) : nullptr;
		if (!slot || !state || state->range) return false;
		state->value = quantize(value, state->minimum, state->maximum, state->step);
		syncSlider(*slot);
		if (notify && state->onChange) state->onChange(handle, state->value);
		return true;
	}

	Bool UiWidgetFactory::setRangeValue(const UiWidgetHandle handle,
		Double lower, Double upper, const Bool notify)
	{
		Slot* slot = get(handle);
		auto* state = slot ? std::get_if<UiSliderState>(&slot->state) : nullptr;
		if (!slot || !state || !state->range) return false;
		lower = quantize(lower, state->minimum, state->maximum, state->step);
		upper = quantize(upper, state->minimum, state->maximum, state->step);
		if (lower > upper) std::swap(lower, upper);
		state->value = lower;
		state->upperValue = upper;
		syncSlider(*slot);
		if (notify && state->onRangeChange) state->onRangeChange(handle, lower, upper);
		return true;
	}

	Bool UiWidgetFactory::setTextValue(
		const UiWidgetHandle handle, String value, const Bool notify)
	{
		Slot* slot = get(handle);
		if (!slot) return false;
		if (auto* text = std::get_if<UiTextState>(&slot->state))
		{
			text->text = std::move(value);
			if (UiNode* root = document.get(slot->root))
			{
				root->authored.text = text->text;
				root->state.semantics.value = text->text;
			}
			markChanged(*slot);
			return true;
		}
		auto* input = std::get_if<UiTextInputState>(&slot->state);
		if (!input || !input->editor.setText(std::move(value))) return false;
		syncText(*slot);
		if (notify && input->onChange) input->onChange(handle, input->editor.text());
		return true;
	}

	Bool UiWidgetFactory::setSelectedValue(
		const UiWidgetHandle handle, String value, const Bool notify)
	{
		Slot* slot = get(handle);
		if (!slot) return false;
		if (auto* radio = std::get_if<UiRadioGroupState>(&slot->state))
		{
			const auto found = std::find_if(radio->options.begin(), radio->options.end(),
				[&value](const UiRadioOption& option) { return option.value == value; });
			if (found == radio->options.end()) return false;
			radio->value = std::move(value);
			for (SizeT index = 0; index < radio->optionNodes.size(); ++index)
			{
				if (UiNode* option = document.get(radio->optionNodes[index]))
				{
					option->state.semantics.checked = radio->options[index].value == radio->value;
					if (option->state.semantics.checked) addClass(*option, UiWidgetClasses::checked);
					else removeClass(*option, UiWidgetClasses::checked);
					document.markDirty(radio->optionNodes[index], DirtyFlag::Paint);
				}
			}
			if (notify && radio->onChange) radio->onChange(handle, radio->value);
			return true;
		}
		auto* select = std::get_if<UiSelectState>(&slot->state);
		if (!select) return false;
		const auto found = std::find_if(select->options.begin(), select->options.end(),
			[&value](const UiSelectOption& option) { return option.value == value; });
		if (found == select->options.end() && !select->editable) return false;
		select->value = std::move(value);
		syncSelect(*slot);
		if (notify && select->onChange) select->onChange(handle, select->value);
		return true;
	}

	Bool UiWidgetFactory::setProgress(
		const UiWidgetHandle handle, const Double value, const Bool indeterminate)
	{
		Slot* slot = get(handle);
		auto* state = slot ? std::get_if<UiProgressState>(&slot->state) : nullptr;
		UiNode* root = slot ? document.get(slot->root) : nullptr;
		if (!slot || !state || !root) return false;
		state->value = std::clamp(value, state->minimum, state->maximum);
		state->indeterminate = indeterminate;
		root->state.semantics.value = indeterminate ? String{} : numberText(state->value);
		if (indeterminate) addClass(*root, "is-indeterminate");
		else removeClass(*root, "is-indeterminate");
		markChanged(*slot);
		return true;
	}

	Bool UiWidgetFactory::updateScrollMetrics(
		const UiWidgetHandle handle, const Vec2 extent, const Vec2 viewport)
	{
		Slot* slot = get(handle);
		if (!slot) return false;
		const ScrollAccess access = scrollAccess(*slot);
		if (!access.scroll) return false;
		access.scroll->setMetrics(extent, viewport);
		if (UiNode* root = document.get(slot->root)) root->computed.scrollExtent = extent;
		syncScroll(*slot);
		return true;
	}

	Bool UiWidgetFactory::materializeVirtualList(const UiWidgetHandle handle)
	{
		Slot* slot = get(handle);
		auto* state = slot ? std::get_if<UiVirtualListState>(&slot->state) : nullptr;
		if (!slot || !state || !state->buildItem || document.isTraversing()) return false;
		for (const UiNodeHandle item : state->materialized)
			if (document.isValid(item)) (void)document.destroyNode(item);
		state->materialized.clear();
		const UiVirtualRange range = state->list.visibleRange(
			state->scroll.offset().y, state->scroll.viewport().y, state->overscanExtent);
		for (UInt index = range.first; index < range.last; ++index)
		{
			const UiNodeHandle item = state->buildItem(index, state->list.keyAt(index), slot->root);
			if (document.isValid(item)) state->materialized.push_back(item);
		}
		if (UiNode* root = document.get(slot->root))
			root->computed.scrollExtent.y = state->list.totalExtent();
		markChanged(*slot);
		return true;
	}

	void UiWidgetFactory::attachLayoutEngine(UiLayoutEngine& layoutEngine)
	{
		layout = &layoutEngine;
	}

	void UiWidgetFactory::syncControlLayouts()
	{
		if (!layout) return;
		for (Slot& slot : slots)
		{
			if (!slot.occupied) continue;
			switch (slot.kind)
			{
			case UiWidgetKind::Toggle:
			case UiWidgetKind::Checkbox:
				layoutToggleChildren(slot);
				break;
			case UiWidgetKind::Select:
			case UiWidgetKind::Combo:
				layoutSelectChildren(slot);
				break;
			default:
				break;
			}
		}
	}

	void UiWidgetFactory::renderGameViewports() const
	{
		for (UInt index = 0; index < slots.size(); ++index)
		{
			const Slot& slot = slots[index];
			if (!slot.occupied) continue;
			const auto* state = std::get_if<UiGameViewportState>(&slot.state);
			const UiNode* root = document.get(slot.root);
			if (!state || !state->render || !root || !root->state.visible) continue;
			state->render(UiGameViewportContext{
				UiWidgetHandle{index, slot.generation}, slot.root,
				root->computed.position, root->computed.size});
		}
	}

	void UiWidgetFactory::markChanged(Slot& slot)
	{
		document.markDirty(slot.root, DirtyFlag::Style);
		document.markDirty(slot.root, DirtyFlag::Paint);
	}

	UiWidgetFactory::ScrollAccess UiWidgetFactory::scrollAccess(Slot& slot)
	{
		if (auto* view = std::get_if<UiScrollViewState>(&slot.state))
			return {&view->scroll, &view->scrollbar, false};
		if (auto* list = std::get_if<UiVirtualListState>(&slot.state))
			return {&list->scroll, &list->scrollbar, true};
		return {};
	}

	void UiWidgetFactory::syncScroll(Slot& slot)
	{
		const ScrollAccess access = scrollAccess(slot);
		UiNode* root = document.get(slot.root);
		if (!access.scroll || !root) return;
		const UiLayoutStyle* nodeStyle = layout ? &layout->style(slot.root) : nullptr;
		const UiOverflow overflowX = nodeStyle ? nodeStyle->overflowX : UiOverflow::Auto;
		const UiOverflow overflowY = nodeStyle ? nodeStyle->overflowY : UiOverflow::Auto;
		const UiScrollGutterReservation gutters = reserveScrollGutters(
			overflowX, overflowY, access.scroll->extent(), root->computed.contentSize);
		access.scroll->setMetrics(access.scroll->extent(), gutters.viewport);
		// Painting reads the offset from the node, because the scroll state
		// itself stays private to the widget layer.
		root->computed.scroll.offset = access.scroll->offset();
		root->computed.scroll.viewport = access.scroll->viewport();
		root->computed.scroll.content = access.scroll->extent();
		root->computed.scroll.verticalGutter = gutters.vertical;
		root->computed.scroll.horizontalGutter = gutters.horizontal;
		root->computed.scroll.showVerticalScrollbar = gutters.showVertical;
		root->computed.scroll.showHorizontalScrollbar = gutters.showHorizontal;
		markChanged(slot);
	}

	void UiWidgetFactory::attachScrollbar(const UiWidgetHandle handle)
	{
		const UiNodeHandle root = node(handle);
		listen(handle, root, UiRoutedEventType::PointerDown,
			[this, handle](UiRoutedEvent& event) { beginScrollbarDrag(handle, event); });
		listen(handle, root, UiRoutedEventType::Drag,
			[this, handle](UiRoutedEvent& event) { continueScrollbarDrag(handle, event); });
		listen(handle, root, UiRoutedEventType::PointerUp,
			[this, handle](UiRoutedEvent& event)
			{
				Slot* slot = get(handle);
				const ScrollAccess access = slot ? scrollAccess(*slot) : ScrollAccess{};
				if (!access.scrollbar || !access.scrollbar->active) return;
				access.scrollbar->active = false;
				event.releasePointer = true;
				event.handled = true;
			});
	}

	void UiWidgetFactory::beginScrollbarDrag(
		const UiWidgetHandle handle, UiRoutedEvent& event)
	{
		Slot* slot = get(handle);
		const ScrollAccess access = slot ? scrollAccess(*slot) : ScrollAccess{};
		const UiNode* root = slot ? document.get(slot->root) : nullptr;
		if (!access.scroll || !access.scrollbar || !root || !root->state.enabled) return;

		const Vec2 pointer(static_cast<Float>(event.position.x), static_cast<Float>(event.position.y));
		const UiRect contentBox{root->computed.contentPosition, root->computed.contentSize};
		const UiRect scrollport = scrollportRect(
			contentBox,
			root->computed.scroll.verticalGutter,
			root->computed.scroll.horizontalGutter);
		const UiScrollbarGeometry vertical =
			verticalScrollbarGeometry(scrollport, access.scroll->vertical());
		const UiScrollbarGeometry horizontal =
			horizontalScrollbarGeometry(scrollport, access.scroll->horizontal());

		const Bool onVertical = vertical.containsTrack(pointer);
		if (!onVertical && !horizontal.containsTrack(pointer)) return;
		const UiScrollbarGeometry& bar = onVertical ? vertical : horizontal;
		const Float thumbLength = onVertical ? bar.thumb.size.y : bar.thumb.size.x;
		const Float pointerAlong = onVertical ? pointer.y : pointer.x;
		const Float thumbStart = onVertical ? bar.thumb.position.y : bar.thumb.position.x;

		access.scrollbar->active = true;
		access.scrollbar->vertical = onVertical;
		// Clicking the bare track jumps the thumb under the cursor first.
		access.scrollbar->grab = bar.containsThumb(pointer)
			? pointerAlong - thumbStart
			: thumbLength * 0.5f;

		event.capturePointer = true;
		event.handled = true;
		event.stopPropagation();
		continueScrollbarDrag(handle, event);
	}

	void UiWidgetFactory::continueScrollbarDrag(
		const UiWidgetHandle handle, UiRoutedEvent& event)
	{
		Slot* slot = get(handle);
		const ScrollAccess access = slot ? scrollAccess(*slot) : ScrollAccess{};
		const UiNode* root = slot ? document.get(slot->root) : nullptr;
		if (!access.scroll || !access.scrollbar || !access.scrollbar->active || !root) return;

		const Vec2 pointer(static_cast<Float>(event.position.x), static_cast<Float>(event.position.y));
		const UiRect contentBox{root->computed.contentPosition, root->computed.contentSize};
		const UiRect scrollport = scrollportRect(
			contentBox,
			root->computed.scroll.verticalGutter,
			root->computed.scroll.horizontalGutter);
		const Bool onVertical = access.scrollbar->vertical;
		const UiScrollAxisState axis =
			onVertical ? access.scroll->vertical() : access.scroll->horizontal();
		const UiScrollbarGeometry bar = onVertical
			? verticalScrollbarGeometry(scrollport, axis)
			: horizontalScrollbarGeometry(scrollport, axis);
		if (!bar.visible)
		{
			access.scrollbar->active = false;
			return;
		}

		const Float pointerAlong = onVertical ? pointer.y : pointer.x;
		const Float offset = scrollOffsetForThumbStart(
			onVertical ? bar.track.position.y : bar.track.position.x,
			onVertical ? bar.track.size.y : bar.track.size.x,
			onVertical ? bar.thumb.size.y : bar.thumb.size.x,
			pointerAlong - access.scrollbar->grab,
			axis.maximumOffset());

		Vec2 next = access.scroll->offset();
		if (onVertical) next.y = offset;
		else next.x = offset;
		access.scroll->setOffset(next);

		const Bool virtualList = access.virtualList;
		event.handled = true;
		if (virtualList)
		{
			(void)materializeVirtualList(handle);
			if (Slot* refreshed = get(handle)) syncScroll(*refreshed);
			return;
		}
		syncScroll(*slot);
	}

	void UiWidgetFactory::layoutToggleChildren(Slot& slot)
	{
		UiNode* root = document.get(slot.root);
		if (!layout || !root || root->computed.size.x <= 0) return;

		const Float reserved = slot.kind == UiWidgetKind::Checkbox ? 34.0f : 60.0f;
		for (const UiNodeHandle child : root->childHandles())
		{
			UiNode* childNode = document.get(child);
			if (!childNode) continue;
			UiLayoutStyle childStyle;
			if (childNode->authored.typeName == "Text")
			{
				childStyle.position = UiPositionType::Absolute;
				childStyle.left = UiSizeSpec::fixed(reserved);
				childStyle.right = UiSizeSpec::fixed(8);
				childStyle.top = UiSizeSpec::fixed(0);
				childStyle.height = UiSizeSpec::fixed(root->computed.size.y);
				layout->setStyle(child, childStyle);
				document.markDirty(child, DirtyFlag::Measure);
			}
			else if (childNode->authored.typeName == "Indicator")
			{
				childStyle.display = UiDisplay::None;
				layout->setStyle(child, childStyle);
				document.markDirty(child, DirtyFlag::Measure);
			}
		}
	}

	void UiWidgetFactory::syncToggle(Slot& slot)
	{
		auto* state = std::get_if<UiToggleState>(&slot.state);
		UiNode* root = document.get(slot.root);
		if (!state || !root) return;
		root->state.semantics.checked = state->value;
		root->state.semantics.selected = state->value;
		root->state.semantics.value = state->value ? "true" : "false";
		if (state->value) addClass(*root, UiWidgetClasses::checked);
		else removeClass(*root, UiWidgetClasses::checked);
		layoutToggleChildren(slot);
		markChanged(slot);
	}

	void UiWidgetFactory::syncSlider(Slot& slot)
	{
		auto* state = std::get_if<UiSliderState>(&slot.state);
		UiNode* root = document.get(slot.root);
		if (!state || !root) return;
		root->state.semantics.label = numberText(state->minimum) + ":" + numberText(state->maximum);
		root->state.semantics.value = state->range
			? numberText(state->value) + ":" + numberText(state->upperValue)
			: numberText(state->value);
		markChanged(slot);
	}

	void UiWidgetFactory::layoutSelectChildren(Slot& slot)
	{
		auto* state = std::get_if<UiSelectState>(&slot.state);
		UiNode* root = document.get(slot.root);
		if (!layout || !state || !root || root->computed.size.x <= 0) return;

		UiNodeHandle menuNode;
		for (const UiNodeHandle child : root->childHandles())
		{
			UiNode* childNode = document.get(child);
			if (childNode && childNode->authored.typeName == "Menu") menuNode = child;
		}
		if (!menuNode.isValid()) return;

		const UiNodeHandle flyoutRoot = document.root();
		if (state->open)
		{
			if (const UiNode* menu = document.get(menuNode))
				if (menu->parentHandle() != flyoutRoot)
					(void)document.reparentNode(menuNode, flyoutRoot);
		}
		else if (const UiNode* menu = document.get(menuNode))
		{
			if (menu->parentHandle() != slot.root)
				(void)document.reparentNode(menuNode, slot.root);
		}

		if (!root->childHandles().empty())
		{
			const UiNodeHandle labelNode = root->childHandles().front();
			UiLayoutStyle labelStyle;
			labelStyle.position = UiPositionType::Absolute;
			labelStyle.left = UiSizeSpec::fixed(UiAppFlex::p10);
			labelStyle.right = UiSizeSpec::fixed(28);
			labelStyle.top = UiSizeSpec::fixed(0);
			labelStyle.height = UiSizeSpec::fixed(root->computed.size.y);
			layout->setStyle(labelNode, labelStyle);
			document.markDirty(labelNode, DirtyFlag::Measure);
		}

		const Float optionHeight = UiAppLayout::controlRowHeight - UiAppFlex::p10;
		const Float menuHeight = optionHeight * static_cast<Float>(state->optionNodes.size());
		UiLayoutStyle menuStyle;
		menuStyle.position = UiPositionType::Absolute;
		if (state->open)
		{
			const Vec2 global = nodeGlobalOrigin(document, slot.root);
			menuStyle.left = UiSizeSpec::fixed(global.x);
			menuStyle.top = UiSizeSpec::fixed(global.y + root->computed.size.y + UiAppFlex::g5);
		}
		else
		{
			menuStyle.left = UiSizeSpec::fixed(0);
			menuStyle.top = UiSizeSpec::fixed(root->computed.size.y + UiAppFlex::g5);
		}
		menuStyle.width = UiSizeSpec::fixed(root->computed.size.x);
		menuStyle.height = UiSizeSpec::fixed(menuHeight);
		menuStyle.display = state->open ? UiDisplay::Flex : UiDisplay::None;
		menuStyle.flexDirection = UiAxis::Column;
		menuStyle.padding = uniformPadding(UiAppFlex::p5);
		layout->setStyle(menuNode, menuStyle);
		document.markDirty(menuNode, DirtyFlag::Measure);

		for (SizeT index = 0; index < state->optionNodes.size(); ++index)
		{
			UiLayoutStyle optionStyle;
			optionStyle.width = UiSizeSpec::percent(1);
			optionStyle.height = UiSizeSpec::fixed(optionHeight);
			optionStyle.flexShrink = 0;
			optionStyle.padding = uniformPadding(UiAppFlex::p10);
			layout->setStyle(state->optionNodes[index], optionStyle);
			document.markDirty(state->optionNodes[index], DirtyFlag::Measure);
		}
	}

	void UiWidgetFactory::syncText(Slot& slot)
	{
		auto* state = std::get_if<UiTextInputState>(&slot.state);
		UiNode* root = document.get(slot.root);
		if (!state || !root) return;
		root->authored.text = state->editor.displayText();
		if (root->authored.text.empty())
		{
			root->authored.text = state->placeholder;
			addClass(*root, "is-placeholder-visible");
		}
		else removeClass(*root, "is-placeholder-visible");
		root->state.semantics.value = state->editor.semanticValue();
		markChanged(slot);
	}

	void UiWidgetFactory::syncSelect(Slot& slot)
	{
		auto* state = std::get_if<UiSelectState>(&slot.state);
		UiNode* root = document.get(slot.root);
		if (!state || !root) return;
		root->state.semantics.value = state->value;
		if (state->open) addClass(*root, UiWidgetClasses::open);
		else removeClass(*root, UiWidgetClasses::open);
		String label = state->value;
		for (const UiSelectOption& option : state->options)
			if (option.value == state->value) label = option.label;
		if (!root->childHandles().empty())
			if (UiNode* labelNode = document.get(root->childHandles().front()))
				labelNode->authored.text = label;
		for (SizeT index = 0; index < state->optionNodes.size(); ++index)
		{
			if (UiNode* option = document.get(state->optionNodes[index]))
			{
				option->state.visible = state->open;
				option->state.semantics.selected = state->options[index].value == state->value;
				if (option->state.semantics.selected) addClass(*option, UiWidgetClasses::selected);
				else removeClass(*option, UiWidgetClasses::selected);
			}
			document.markDirty(state->optionNodes[index], DirtyFlag::Paint);
		}
		layoutSelectChildren(slot);
		markChanged(slot);
	}

	Double UiWidgetFactory::pointerValue(
		const Slot& slot, const UiRoutedEvent& event) const
	{
		const auto* state = std::get_if<UiSliderState>(&slot.state);
		const UiNode* root = document.get(slot.root);
		if (!state || !root || root->computed.size.x <= 0) return state ? state->value : 0;
		const Double fraction = std::clamp(
			(event.position.x - root->computed.position.x) / root->computed.size.x, 0.0, 1.0);
		return quantize(state->minimum + fraction * (state->maximum - state->minimum),
			state->minimum, state->maximum, state->step);
	}

	Double UiWidgetFactory::normalized(
		const Double value, const Double minimum, const Double maximum)
	{
		return maximum > minimum ? std::clamp((value - minimum) / (maximum - minimum), 0.0, 1.0) : 0;
	}

	Double UiWidgetFactory::quantize(
		const Double value, const Double minimum, const Double maximum, const Double step)
	{
		Double result = std::clamp(value, minimum, maximum);
		if (step > 0) result = minimum + std::round((result - minimum) / step) * step;
		return std::clamp(result, minimum, maximum);
	}
}
