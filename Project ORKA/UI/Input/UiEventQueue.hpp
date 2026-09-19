#pragma once

#include "../Core/UiTypes.hpp"

namespace Ui
{
	enum class UiEventType : Byte
	{
		PointerMove,
		PointerButton,
		PointerCancel,
		Key,
		Text,
		CompositionStart,
		CompositionUpdate,
		CompositionCommit,
		CompositionCancel,
		Scroll,
		Focus,
		Resize,
	};

	enum class UiPointerKind : Byte
	{
		Mouse,
		Touch,
		Pen,
		World,
	};

	enum class UiEventAction : Byte
	{
		None,
		Pressed,
		Released,
		Repeated,
	};

	struct UiEvent
	{
		UiEventType type = UiEventType::PointerMove;
		UiEventAction action = UiEventAction::None;
		U64 sequence = 0;
		U64 timestampMicroseconds = 0;
		DVec2 position = DVec2(0);
		DVec2 delta = DVec2(0);
		IVec2 logicalSize = IVec2(0);
		IVec2 framebufferSize = IVec2(0);
		Int code = 0;
		Int scanCode = 0;
		Int modifiers = 0;
		UInt pointerId = 0;
		UiPointerKind pointerKind = UiPointerKind::Mouse;
		U32 codepoint = 0;
		String text;
		SizeT compositionStart = 0;
		SizeT compositionEnd = 0;
		Bool focused = false;
	};

	class UiEventQueue
	{
	public:
		static constexpr SizeT defaultCapacity = 1024;
		static constexpr SizeT maximumCapacity = 16384;

		explicit UiEventQueue(SizeT capacity = defaultCapacity);

		// Returns false when full. Existing events are never silently discarded.
		[[nodiscard]] Bool push(UiEvent event);
		[[nodiscard]] Bool pop(UiEvent& event);
		[[nodiscard]] Vector<UiEvent> drain(SizeT maximum = maximumCapacity);
		void clear();

		[[nodiscard]] SizeT size() const;
		[[nodiscard]] SizeT capacity() const { return queueCapacity; }
		[[nodiscard]] U64 droppedCount() const;

	private:
		mutable Mutex mutex;
		Deque<UiEvent> events;
		SizeT queueCapacity = defaultCapacity;
		U64 nextSequence = 1;
		U64 droppedEvents = 0;
	};
}
