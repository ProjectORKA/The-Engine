#pragma once

#include "UiLayoutEngine.hpp"
#include "UiScroll.hpp"
#include "../Input/UiEventRouter.hpp"

namespace Ui
{
	[[nodiscard]] Bool overflowAllowsScrolling(UiOverflow policy);
	[[nodiscard]] Bool overflowClips(
		UiOverflow policy, const Float contentExtent, const Float viewportExtent);
	[[nodiscard]] Bool overflowShowsScrollbar(
		UiOverflow policy, const Float contentExtent, const Float viewportExtent);
	[[nodiscard]] UiOverflow parseOverflowKeyword(const String& name, Bool& supported);

	// Applies CSS overflow (visible/hidden/scroll/auto) without manual scroll binding.
	class UiOverflowController
	{
	public:
		UiOverflowController(UiDocument& document, UiLayoutEngine& layout, UiEventRouter& router);

		void reset();
		void synchronizeAfterLayout();
		[[nodiscard]] Bool consumePendingRelayout();

		[[nodiscard]] const UiScrollState* scrollState(UiNodeHandle node) const;
		[[nodiscard]] UiScrollbarDrag* scrollbarDrag(UiNodeHandle node);

	private:
		struct Entry
		{
			UiNodeHandle handle;
			UiScrollState scroll;
			UiScrollbarDrag scrollbar;
			UiListenerToken wheelToken;
			UiListenerToken pointerDownToken;
			UiListenerToken dragToken;
			UiListenerToken pointerUpToken;
			Bool listenersAttached = false;
		};

		UiDocument& document;
		UiLayoutEngine& layoutEngine;
		UiEventRouter& router;
		Vector<Entry> entries;
		UiListenerToken rootWheelToken;
		Bool pendingRelayout = false;

		[[nodiscard]] Entry* findEntry(UiNodeHandle node);
		[[nodiscard]] const Entry* findEntry(UiNodeHandle node) const;
		[[nodiscard]] Bool isWidgetManagedScroller(const UiNode& node) const;
		[[nodiscard]] Bool isScrollableContainer(UiNodeHandle node) const;
		[[nodiscard]] UiNodeHandle findScrollTarget(UiNodeHandle start) const;
		void ensureEntry(UiNodeHandle node);
		void pruneEntries();
		void attachListeners(Entry& entry);
		void detachListeners(Entry& entry);
		void publishPresentation(Entry& entry, const UiScrollGutterReservation& gutters);
		void handleWheel(Entry& entry, UiRoutedEvent& event);
		void beginScrollbarDrag(Entry& entry, UiRoutedEvent& event);
		void continueScrollbarDrag(Entry& entry, UiRoutedEvent& event);
		void markScrollDirty(UiNodeHandle node);
	};
}
