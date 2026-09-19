#pragma once

#include "../Core/UiDocument.hpp"
#include "UiEventQueue.hpp"

namespace Ui
{
	enum class UiRoutedEventType : Byte
	{
		PointerDown,
		PointerUp,
		PointerMove,
		PointerEnter,
		PointerLeave,
		PointerCancel,
		Activate,
		DoubleClick,
		DragStart,
		Drag,
		DragEnd,
		Wheel,
		KeyDown,
		KeyUp,
		TextInput,
		CompositionStart,
		CompositionUpdate,
		CompositionCommit,
		CompositionCancel,
		FocusIn,
		FocusOut,
		Cancel,
	};

	enum class UiEventPhase : Byte { Capture, Target, Bubble };
	enum class UiNavigationDirection : Byte { Left, Right, Up, Down };

	struct UiWorldRay
	{
		DVec3 origin = DVec3(0);
		DVec3 direction = DVec3(0, 1, 0);
	};

	struct UiRoutedEvent
	{
		UiRoutedEventType type = UiRoutedEventType::PointerMove;
		UiEventPhase phase = UiEventPhase::Target;
		UiNodeHandle target;
		UiNodeHandle currentTarget;
		UiNodeHandle relatedTarget;
		DVec2 position = DVec2(0);
		DVec2 delta = DVec2(0);
		U64 timestampMicroseconds = 0;
		UInt pointerId = 0;
		UiPointerKind pointerKind = UiPointerKind::Mouse;
		Int button = 0;
		Int key = 0;
		Int modifiers = 0;
		U32 codepoint = 0;
		String text;
		SizeT compositionStart = 0;
		SizeT compositionEnd = 0;
		String commandId;
		Bool handled = false;
		Bool defaultPrevented = false;
		Bool propagationStopped = false;
		Bool immediatePropagationStopped = false;
		Bool capturePointer = false;
		Bool releasePointer = false;
		Bool focusTarget = false;

		void preventDefault() { defaultPrevented = true; }
		void stopPropagation() { propagationStopped = true; }
		void stopImmediatePropagation()
		{
			immediatePropagationStopped = true;
			propagationStopped = true;
		}
	};

	using UiEventCallback = Function<void(UiRoutedEvent&)>;
	using UiCommandDispatcher = Function<void(const String&, UiRoutedEvent&)>;
	using UiWorldSurfaceMapper = Function<Bool(const UiWorldRay&, DVec2&)>;

	struct UiListenerOptions
	{
		Bool capture = false;
		Bool once = false;
	};

	struct UiListenerToken
	{
		U64 value = 0;
		[[nodiscard]] Bool isValid() const { return value != 0; }
		constexpr auto operator<=>(const UiListenerToken&) const = default;
	};

	struct UiInteractionData
	{
		Bool hitTestVisible = true;
		Bool focusable = false;
		Bool focusScope = false;
		Bool modalScope = false;
		Int tabIndex = -1;
		Array<UiNodeHandle, 4> explicitNeighbors{};
	};

	class UiEventRouter
	{
	public:
		static constexpr SizeT maximumListeners = 16384;
		static constexpr SizeT maximumPointerStates = 32;
		static constexpr SizeT maximumDeferredOperations = 4096;

		explicit UiEventRouter(UiDocument& document);
		~UiEventRouter();
		UiEventRouter(const UiEventRouter&) = delete;
		UiEventRouter& operator=(const UiEventRouter&) = delete;

		[[nodiscard]] UiListenerToken addListener(UiNodeHandle node, UiRoutedEventType type,
			UiEventCallback callback, UiListenerOptions options = {});
		[[nodiscard]] UiListenerToken addCommandListener(UiNodeHandle node, UiRoutedEventType type,
			String commandId, UiListenerOptions options = {});
		[[nodiscard]] Bool removeListener(UiListenerToken token);
		void setCommandDispatcher(UiCommandDispatcher dispatcher);
		void setDefaultAction(UiNodeHandle node, UiRoutedEventType type, UiEventCallback action);
		void setDefaultCommand(UiNodeHandle node, UiRoutedEventType type, String commandId);
		void setCancelAction(UiNodeHandle scope, UiEventCallback action);
		void setCancelCommand(UiNodeHandle scope, String commandId);

		void setInteraction(UiNodeHandle node, UiInteractionData interaction);
		[[nodiscard]] UiInteractionData interaction(UiNodeHandle node) const;
		void clearNode(UiNodeHandle node);

		[[nodiscard]] UiNodeHandle hitTest(DVec2 viewportPosition) const;
		[[nodiscard]] UiNodeHandle hoveredNode(UInt pointerId = 0) const;
		[[nodiscard]] Bool dispatch(const UiEvent& event);
		[[nodiscard]] Bool dispatchWorldEvent(UiEvent event, const UiWorldRay& ray);
		void setWorldSurfaceMapper(UiWorldSurfaceMapper mapper);

		[[nodiscard]] Bool capturePointer(UInt pointerId, UiNodeHandle node);
		void releasePointer(UInt pointerId, UiNodeHandle node = {});
		void cancelPointer(UInt pointerId);
		void cancelAllPointers();

		[[nodiscard]] Bool focus(UiNodeHandle node);
		void clearFocus();
		[[nodiscard]] UiNodeHandle focusedNode() const { return focused; }
		[[nodiscard]] Bool moveFocusTab(Bool backwards = false);
		[[nodiscard]] Bool moveFocusSpatial(UiNavigationDirection direction);
		void pushModalScope(UiNodeHandle scope);
		void popModalScope(UiNodeHandle scope);
		[[nodiscard]] UiNodeHandle activeModalScope() const;

		void applyDeferredOperations();
		void reset();

	private:
		struct Listener;
		struct Action;
		struct PointerState;
		enum class DeferredType : Byte { RemoveListener, ClearNode };
		struct DeferredOperation
		{
			DeferredType type = DeferredType::RemoveListener;
			UiListenerToken listener;
			UiNodeHandle node;
		};

		UiDocument& owner;
		Vector<Listener> listeners;
		Vector<Listener> pendingListeners;
		Vector<Action> defaults;
		Vector<Action> cancels;
		Vector<PointerState> pointers;
		Vector<DeferredOperation> deferred;
		Vector<UiNodeHandle> modalScopes;
		Vector<UiNodeHandle> modalFocusRestoration;
		Map<UiNodeHandle, UiInteractionData> interactions;
		UiCommandDispatcher commandDispatcher;
		UiWorldSurfaceMapper worldMapper;
		UiNodeHandle focused;
		U64 nextListener = 1;
		UInt dispatchDepth = 0;

		[[nodiscard]] Vector<UiNodeHandle> pathTo(UiNodeHandle node) const;
		[[nodiscard]] Bool isDescendantOf(UiNodeHandle node, UiNodeHandle ancestor) const;
		[[nodiscard]] Bool isInteractive(UiNodeHandle node) const;
		[[nodiscard]] PointerState* pointer(UInt pointerId, Bool create);
		void updateHover(PointerState& state, UiNodeHandle target, const UiEvent& source);
		[[nodiscard]] Bool route(UiRoutedEvent& event);
		void invokeAt(UiRoutedEvent& event, UiNodeHandle node, UiEventPhase phase);
		void runDefault(UiRoutedEvent& event);
		void runCancel(UiRoutedEvent& event);
		void cleanInvalidState();
		[[nodiscard]] U64 eventTime(const UiEvent& event) const;
	};
}
