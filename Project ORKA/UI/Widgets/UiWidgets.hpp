#pragma once

#include "../Core/UiDocument.hpp"
#include "../Input/UiEventRouter.hpp"
#include "../Input/UiTextEditModel.hpp"
#include "../Layout/UiLayoutEngine.hpp"
#include "../Layout/UiScroll.hpp"
#include "../Render/UiPaintBuilder.hpp"
#include "../Text/UiTextPainter.hpp"

#include <variant>

struct Window;

namespace Ui
{
	class UiWidgetBridge;

	struct UiWidgetHandle
	{
		static constexpr UInt invalidIndex = maxUInt;
		UInt index = invalidIndex;
		UInt generation = 0;

		[[nodiscard]] constexpr Bool isValid() const
		{
			return index != invalidIndex && generation != 0;
		}

		constexpr auto operator<=>(const UiWidgetHandle&) const = default;
	};

	enum class UiWidgetKind : Byte
	{
		Box,
		Text,
		Image,
		Icon,
		ScrollView,
		VirtualList,
		FocusScope,
		Portal,
		Overlay,
		WorldSurface,
		GameViewport,
		Button,
		Toggle,
		Checkbox,
		RadioGroup,
		Slider,
		Range,
		Progress,
		TextField,
		NumberField,
		TextArea,
		Select,
		Combo,
	};

	struct UiWidgetClasses
	{
		static String root(UiWidgetKind kind);
		static constexpr const Char* control = "ui-control";
		static constexpr const Char* content = "ui-content";
		static constexpr const Char* label = "ui-label";
		static constexpr const Char* indicator = "ui-indicator";
		static constexpr const Char* track = "ui-track";
		static constexpr const Char* thumb = "ui-thumb";
		static constexpr const Char* selected = "is-selected";
		static constexpr const Char* checked = "is-checked";
		static constexpr const Char* open = "is-open";
		static constexpr const Char* readOnly = "is-read-only";
		static constexpr const Char* chromeless = "ui-chromeless";
	};

	// Resolves visuals by authored style class. Themes install painters; widgets
	// only emit semantic class names and never select colors, shaders or geometry.
	class UiWidgetPainterRegistry
	{
	public:
		void setPainter(String styleClass, UiNodePainter painter);
		[[nodiscard]] Bool removePainter(const String& styleClass);
		[[nodiscard]] Bool paint(const UiNodePaintContext& context, UiDisplayList& output) const;
		[[nodiscard]] UiNodePainter nodePainter() const;

	private:
		Map<String, UiNodePainter> painters;
	};

	struct UiWidgetCommon
	{
		UiNodeHandle parent;
		String id;
		String label;
		Vector<String> classes;
		Bool enabled = true;
		Bool readOnly = false;
		Int tabIndex = 0;
	};

	struct UiBoxConfig : UiWidgetCommon {};
	struct UiTextConfig : UiWidgetCommon { String text; };
	struct UiImageConfig : UiWidgetCommon
	{
		String resource;
		String alternateText;
		Bool icon = false;
	};

	struct UiScrollViewConfig : UiWidgetCommon
	{
		UiOverscrollPolicy horizontalPolicy = UiOverscrollPolicy::Contain;
		UiOverscrollPolicy verticalPolicy = UiOverscrollPolicy::Contain;
	};

	using UiVirtualItemBuilder = Function<UiNodeHandle(UInt, U64, UiNodeHandle)>;
	struct UiVirtualListConfig : UiScrollViewConfig
	{
		Vector<U64> keys;
		Float estimatedItemHeight = 24;
		Float overscanExtent = 48;
		UiVirtualItemBuilder buildItem;
	};

	struct UiFocusScopeConfig : UiWidgetCommon { Bool modal = false; };
	struct UiPortalConfig : UiWidgetCommon
	{
		UiNodeHandle logicalOwner;
		UiNodeHandle target;
		Bool modal = false;
	};

	struct UiWorldSurfaceConfig : UiWidgetCommon
	{
		UiWorldSurfaceMetadata metadata;
		UiWorldSurfaceMapper rayMapper;
	};

	struct UiGameViewportContext
	{
		UiWidgetHandle widget;
		UiNodeHandle node;
		Vec2 position = Vec2(0);
		Vec2 size = Vec2(0);
	};
	using UiGameViewportCallback = Function<void(const UiGameViewportContext&)>;
	struct UiGameViewportConfig : UiWidgetCommon { UiGameViewportCallback render; };

	using UiActivateCallback = Function<void(UiWidgetHandle)>;
	using UiBoolChangeCallback = Function<void(UiWidgetHandle, Bool)>;
	using UiNumberChangeCallback = Function<void(UiWidgetHandle, Double)>;
	using UiRangeChangeCallback = Function<void(UiWidgetHandle, Double, Double)>;
	using UiTextChangeCallback = Function<void(UiWidgetHandle, const String&)>;
	using UiStringChangeCallback = Function<void(UiWidgetHandle, const String&)>;

	struct UiButtonConfig : UiWidgetCommon
	{
		String text;
		UiActivateCallback onActivate;
	};

	struct UiToggleConfig : UiButtonConfig
	{
		Bool value = false;
		UiBoolChangeCallback onChange;
	};

	struct UiRadioOption
	{
		String value;
		String label;
		Bool enabled = true;
	};
	struct UiRadioGroupConfig : UiWidgetCommon
	{
		Vector<UiRadioOption> options;
		String value;
		UiStringChangeCallback onChange;
	};

	struct UiSliderConfig : UiWidgetCommon
	{
		Double minimum = 0;
		Double maximum = 1;
		Double step = 0;
		Double value = 0;
		UiNumberChangeCallback onChange;
	};

	struct UiRangeConfig : UiSliderConfig
	{
		Double upperValue = 1;
		UiRangeChangeCallback onRangeChange;
	};

	struct UiProgressConfig : UiWidgetCommon
	{
		Double minimum = 0;
		Double maximum = 1;
		Double value = 0;
		Bool indeterminate = false;
	};

	struct UiTextInputConfig : UiWidgetCommon
	{
		String value;
		String placeholder;
		Bool password = false;
		Bool multiline = false;
		UiTextEditLimits limits;
		UiTextValidationCallback validate;
		UiTextChangeCallback onChange;
		UiActivateCallback onSubmit;
	};

	enum class UiNumberFieldMode : Byte
	{
		Display,
		Edit,
	};

	struct UiNumberFieldConfig : UiWidgetCommon
	{
		Bool bounded = true;
		Bool integer = false;
		Bool showProgress = true;
		Double minimum = 0;
		Double maximum = 1;
		Double step = 0;
		Double value = 0;
		String unit;
		String placeholder;
		UiNumberChangeCallback onChange;
	};

	struct UiSelectOption
	{
		String value;
		String label;
		Bool enabled = true;
	};
	struct UiSelectConfig : UiWidgetCommon
	{
		Vector<UiSelectOption> options;
		String value;
		Bool editable = false;
		UiStringChangeCallback onChange;
	};

	struct UiBoxState {};
	struct UiTextState { String text; };
	struct UiImageState { String resource; String alternateText; Bool icon = false; };
	struct UiScrollViewState { UiScrollState scroll; UiScrollbarDrag scrollbar; };
	struct UiVirtualListState
	{
		UiScrollState scroll;
		UiScrollbarDrag scrollbar;
		UiVirtualList list;
		Float overscanExtent = 48;
		UiVirtualItemBuilder buildItem;
		Vector<UiNodeHandle> materialized;
	};
	struct UiFocusScopeState { Bool modal = false; };
	struct UiPortalState { UiNodeHandle logicalOwner; UiNodeHandle target; Bool modal = false; };
	struct UiWorldSurfaceState
	{
		UiWorldSurfaceMetadata metadata;
		UiWorldSurfaceMapper rayMapper;
	};
	struct UiGameViewportState { UiGameViewportCallback render; };
	struct UiButtonState { Bool pressed = false; UiActivateCallback onActivate; };
	struct UiToggleState { Bool value = false; Bool pressed = false; UiBoolChangeCallback onChange; };
	struct UiRadioGroupState
	{
		Vector<UiRadioOption> options;
		Vector<UiNodeHandle> optionNodes;
		String value;
		UiStringChangeCallback onChange;
	};
	struct UiSliderState
	{
		Double minimum = 0;
		Double maximum = 1;
		Double step = 0;
		Double value = 0;
		Double upperValue = 1;
		Bool range = false;
		Bool draggingUpper = false;
		UiNumberChangeCallback onChange;
		UiRangeChangeCallback onRangeChange;
	};
	struct UiProgressState { Double minimum = 0; Double maximum = 1; Double value = 0; Bool indeterminate = false; };
	struct UiTextInputState
	{
		explicit UiTextInputState(UiTextEditLimits limits = {}) : editor(limits) {}
		UiTextEditModel editor;
		String placeholder;
		Bool draggingSelection = false;
		SizeT selectionAnchor = 0;
		UiTextChangeCallback onChange;
		UiActivateCallback onSubmit;
	};
	struct UiNumberFieldState
	{
		Bool bounded = true;
		Bool integer = false;
		Bool showProgress = true;
		Double minimum = 0;
		Double maximum = 1;
		Double step = 0;
		Double value = 0;
		Double internalValue = 0;
		String unit;
		UiTextEditModel editor;
		String placeholder;
		UiNumberFieldMode mode = UiNumberFieldMode::Display;
		Bool focused = false;
		Bool scrubbing = false;
		Bool pendingDisplayClick = false;
		Double scrubStartValue = 0;
		Double scrubAccumulatedPixels = 0;
		Bool draggingSelection = false;
		SizeT selectionAnchor = 0;
		UiNumberChangeCallback onChange;
	};
	struct UiSelectState
	{
		Vector<UiSelectOption> options;
		Vector<UiNodeHandle> optionNodes;
		String value;
		Bool open = false;
		Bool editable = false;
		UiStringChangeCallback onChange;
	};

	using UiWidgetState = std::variant<
		UiBoxState, UiTextState, UiImageState, UiScrollViewState, UiVirtualListState,
		UiFocusScopeState, UiPortalState, UiWorldSurfaceState, UiGameViewportState,
		UiButtonState, UiToggleState, UiRadioGroupState, UiSliderState,
		UiProgressState, UiTextInputState, UiNumberFieldState, UiSelectState>;

	class UiWidgetFactory
	{
		friend class UiWidgetBridge;

	public:
		static constexpr SizeT maximumWidgets = 32768;

		UiWidgetFactory(UiDocument& document, UiEventRouter& router);
		~UiWidgetFactory();
		UiWidgetFactory(const UiWidgetFactory&) = delete;
		UiWidgetFactory& operator=(const UiWidgetFactory&) = delete;

		[[nodiscard]] Bool isValid(UiWidgetHandle handle) const;
		[[nodiscard]] UiNodeHandle node(UiWidgetHandle handle) const;
		[[nodiscard]] UiWidgetKind kind(UiWidgetHandle handle) const;
		[[nodiscard]] Bool destroy(UiWidgetHandle handle);
		void collectInvalid();

		[[nodiscard]] UiWidgetHandle box(UiBoxConfig config = {});
		[[nodiscard]] UiWidgetHandle text(UiTextConfig config);
		[[nodiscard]] UiWidgetHandle image(UiImageConfig config);
		[[nodiscard]] UiWidgetHandle scrollView(UiScrollViewConfig config = {});
		[[nodiscard]] UiWidgetHandle virtualList(UiVirtualListConfig config);
		[[nodiscard]] UiWidgetHandle focusScope(UiFocusScopeConfig config = {});
		[[nodiscard]] UiWidgetHandle portal(UiPortalConfig config);
		[[nodiscard]] UiWidgetHandle overlay(UiPortalConfig config);
		[[nodiscard]] UiWidgetHandle worldSurface(UiWorldSurfaceConfig config);
		[[nodiscard]] UiWidgetHandle gameViewport(UiGameViewportConfig config);

		[[nodiscard]] UiWidgetHandle button(UiButtonConfig config);
		[[nodiscard]] UiWidgetHandle toggle(UiToggleConfig config);
		[[nodiscard]] UiWidgetHandle checkbox(UiToggleConfig config);
		[[nodiscard]] UiWidgetHandle radioGroup(UiRadioGroupConfig config);
		[[nodiscard]] UiWidgetHandle slider(UiSliderConfig config);
		[[nodiscard]] UiWidgetHandle range(UiRangeConfig config);
		[[nodiscard]] UiWidgetHandle progress(UiProgressConfig config);
		[[nodiscard]] UiWidgetHandle textField(UiTextInputConfig config);
		[[nodiscard]] UiWidgetHandle numberField(UiNumberFieldConfig config);
		[[nodiscard]] UiWidgetHandle textArea(UiTextInputConfig config);
		[[nodiscard]] UiWidgetHandle select(UiSelectConfig config);
		[[nodiscard]] UiWidgetHandle combo(UiSelectConfig config);

		[[nodiscard]] Bool setEnabled(UiWidgetHandle handle, Bool enabled);
		[[nodiscard]] Bool setReadOnly(UiWidgetHandle handle, Bool readOnly);
		[[nodiscard]] Bool setBoolValue(UiWidgetHandle handle, Bool value, Bool notify = false);
		[[nodiscard]] Bool setNumberValue(UiWidgetHandle handle, Double value, Bool notify = false);
		[[nodiscard]] Bool setRangeValue(UiWidgetHandle handle, Double lower, Double upper, Bool notify = false);
		[[nodiscard]] Bool setTextValue(UiWidgetHandle handle, String value, Bool notify = false);
		[[nodiscard]] Bool setSelectedValue(UiWidgetHandle handle, String value, Bool notify = false);
		[[nodiscard]] Bool setProgress(UiWidgetHandle handle, Double value, Bool indeterminate = false);

		[[nodiscard]] Bool updateScrollMetrics(UiWidgetHandle handle, Vec2 extent, Vec2 viewport);
		[[nodiscard]] Bool materializeVirtualList(UiWidgetHandle handle);
		void renderGameViewports() const;
		void attachLayoutEngine(UiLayoutEngine& layoutEngine);
		void attachTextMetrics(UiTextShapeProvider shapeProvider, UiTextStyleProvider styleProvider);
		void syncControlLayouts();
		void setInputWindow(Window* window) { inputWindow = window; }
		// Window-thread scrub driver: uses Window::mouseDelta while LMB is held.
		void driveNumberScrubFrame(Window& window);

		[[nodiscard]] SizeT hitTestEditByte(UiNodeHandle node, Float viewportX) const;

		template <typename State>
		[[nodiscard]] State* state(UiWidgetHandle handle)
		{
			Slot* slot = get(handle);
			return slot ? std::get_if<State>(&slot->state) : nullptr;
		}

		template <typename State>
		[[nodiscard]] const State* state(UiWidgetHandle handle) const
		{
			const Slot* slot = get(handle);
			return slot ? std::get_if<State>(&slot->state) : nullptr;
		}

	private:
		struct Slot
		{
			UiWidgetKind kind = UiWidgetKind::Box;
			UiNodeHandle root;
			UiWidgetState state = UiBoxState{};
			Vector<UiListenerToken> listeners;
			UInt generation = 1;
			Bool occupied = false;
			Bool readOnly = false;
		};

		// Both scrolling widgets share one scroll state and one scrollbar.
		struct ScrollAccess
		{
			UiScrollState* scroll = nullptr;
			UiScrollbarDrag* scrollbar = nullptr;
			Bool virtualList = false;
		};

		UiDocument& document;
		UiEventRouter& router;
		UiLayoutEngine* layout = nullptr;
		UiTextShapeProvider textShapeProvider;
		UiTextStyleProvider textStyleProvider;
		Vector<Slot> slots;
		Vector<UInt> freeSlots;
		Window* inputWindow = nullptr;
		struct NumberScrubSession
		{
			UiWidgetHandle handle;
			Window* window = nullptr;
			Double scrubStartValue = 0;
			Double scrubAccumulatedPixels = 0;
			Bool moved = false;
		};
		Bool numberScrubActive = false;
		NumberScrubSession numberScrub{};

		void beginNumberScrub(UiWidgetHandle handle);
		void finishNumberScrub(Bool clickToEditIfUntouched);

		[[nodiscard]] Slot* get(UiWidgetHandle handle);
		[[nodiscard]] const Slot* get(UiWidgetHandle handle) const;
		[[nodiscard]] UiWidgetHandle create(UiWidgetKind kind, const UiWidgetCommon& common,
			String typeName, UiSemanticRole role, UiWidgetState state);
		[[nodiscard]] UiNodeHandle child(UiNodeHandle parent, String type, String styleClass,
			String text = {});
		void listen(UiWidgetHandle widget, UiNodeHandle node, UiRoutedEventType type,
			UiEventCallback callback);
		void configureInteraction(UiNodeHandle node, const UiWidgetCommon& common,
			Bool focusable, Bool scope = false, Bool modal = false);
		void markChanged(Slot& slot);
		[[nodiscard]] static ScrollAccess scrollAccess(Slot& slot);
		void syncScroll(Slot& slot);
		void attachScrollbar(UiWidgetHandle handle);
		void beginScrollbarDrag(UiWidgetHandle handle, UiRoutedEvent& event);
		void continueScrollbarDrag(UiWidgetHandle handle, UiRoutedEvent& event);
		void syncToggle(Slot& slot);
		void syncSlider(Slot& slot);
		void syncText(Slot& slot);
		void syncNumberField(Slot& slot);
		Bool commitNumberField(Slot& slot, UiWidgetHandle handle, Bool notify);
		void applyNumberScrubPixels(Slot& slot, UiNumberFieldState& state, Double deltaPixels);
		void syncSelect(Slot& slot);
		void layoutSelectChildren(Slot& slot);
		void layoutToggleChildren(Slot& slot);
		[[nodiscard]] Double pointerValue(const Slot& slot, const UiRoutedEvent& event) const;
		[[nodiscard]] static Double normalized(Double value, Double minimum, Double maximum);
		[[nodiscard]] static Double quantize(Double value, Double minimum, Double maximum, Double step);
	};
}

// Composed, theme-neutral controllers and docking data model.
#include "UiComposedWidgets.hpp"
