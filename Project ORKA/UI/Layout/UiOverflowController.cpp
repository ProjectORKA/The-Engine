#include "UiOverflowController.hpp"

namespace Ui
{
	namespace
	{
		constexpr Float overflowEpsilon = 0.5f;

		[[nodiscard]] Bool finiteNonNegative(const Float value)
		{
			return std::isfinite(value) && value >= 0.0f;
		}
	}

	Bool overflowAllowsScrolling(const UiOverflow policy)
	{
		return policy == UiOverflow::Scroll || policy == UiOverflow::Auto;
	}

	Bool overflowClips(
		const UiOverflow policy, const Float contentExtent, const Float viewportExtent)
	{
		switch (policy)
		{
		case UiOverflow::Visible: return false;
		case UiOverflow::Hidden: return true;
		case UiOverflow::Scroll: return true;
		case UiOverflow::Auto:
			return finiteNonNegative(contentExtent) && finiteNonNegative(viewportExtent) &&
				contentExtent > viewportExtent + overflowEpsilon;
		}
		return false;
	}

	Bool overflowShowsScrollbar(
		const UiOverflow policy, const Float contentExtent, const Float viewportExtent)
	{
		if (!overflowAllowsScrolling(policy)) return false;
		if (policy == UiOverflow::Scroll)
			return finiteNonNegative(viewportExtent) && viewportExtent > 0.0f;
		return overflowClips(UiOverflow::Auto, contentExtent, viewportExtent);
	}

	UiOverflow parseOverflowKeyword(const String& name, Bool& supported)
	{
		supported = true;
		if (name == "hidden") return UiOverflow::Hidden;
		if (name == "scroll") return UiOverflow::Scroll;
		if (name == "auto") return UiOverflow::Auto;
		if (name == "visible") return UiOverflow::Visible;
		supported = false;
		return UiOverflow::Visible;
	}

	UiOverflowController::UiOverflowController(
		UiDocument& documentRef, UiLayoutEngine& layoutRef, UiEventRouter& routerRef)
		: document(documentRef), layoutEngine(layoutRef), router(routerRef)
	{
		rootWheelToken = router.addListener(document.root(), UiRoutedEventType::Wheel,
			[this](UiRoutedEvent& event)
			{
				if (event.handled) return;
				const UiNodeHandle target = findScrollTarget(event.target);
				Entry* entry = findEntry(target);
				if (!entry) return;
				handleWheel(*entry, event);
			}, { .capture = true });
	}

	void UiOverflowController::reset()
	{
		for (Entry& entry : entries) detachListeners(entry);
		entries.clear();
		pendingRelayout = false;
	}

	Bool UiOverflowController::consumePendingRelayout()
	{
		const Bool relayout = pendingRelayout;
		pendingRelayout = false;
		return relayout;
	}

	const UiScrollState* UiOverflowController::scrollState(const UiNodeHandle node) const
	{
		const Entry* entry = findEntry(node);
		return entry ? &entry->scroll : nullptr;
	}

	UiScrollbarDrag* UiOverflowController::scrollbarDrag(const UiNodeHandle node)
	{
		Entry* entry = findEntry(node);
		return entry ? &entry->scrollbar : nullptr;
	}

	UiOverflowController::Entry* UiOverflowController::findEntry(const UiNodeHandle node)
	{
		const auto found = std::find_if(entries.begin(), entries.end(),
			[&](const Entry& entry) { return entry.handle == node; });
		return found != entries.end() ? &*found : nullptr;
	}

	const UiOverflowController::Entry* UiOverflowController::findEntry(const UiNodeHandle node) const
	{
		return const_cast<UiOverflowController*>(this)->findEntry(node);
	}

	Bool UiOverflowController::isWidgetManagedScroller(const UiNode& node) const
	{
		return node.authored.typeName == "ScrollView" || node.authored.typeName == "VirtualList";
	}

	Bool UiOverflowController::isScrollableContainer(const UiNodeHandle node) const
	{
		if (!document.isValid(node)) return false;
		const UiNode* item = document.get(node);
		if (!item || !item->state.visible || !item->state.enabled) return false;
		if (isWidgetManagedScroller(*item)) return false;
		const UiLayoutStyle nodeStyle = layoutEngine.resolvedStyle(node);
		return overflowAllowsScrolling(nodeStyle.overflowX) ||
			overflowAllowsScrolling(nodeStyle.overflowY);
	}

	UiNodeHandle UiOverflowController::findScrollTarget(const UiNodeHandle start) const
	{
		UiNodeHandle current = start;
		while (document.isValid(current))
		{
			const UiNode* item = document.get(current);
			if (!item) break;
			const UiLayoutStyle nodeStyle = layoutEngine.resolvedStyle(current);
			if (!isWidgetManagedScroller(*item) &&
				(overflowAllowsScrolling(nodeStyle.overflowX) ||
					overflowAllowsScrolling(nodeStyle.overflowY)))
			{
				const Float extentX = item->computed.scrollExtent.x;
				const Float extentY = item->computed.scrollExtent.y;
				const Float viewportX = item->computed.scroll.viewport.x > 0
					? item->computed.scroll.viewport.x
					: item->computed.contentSize.x;
				const Float viewportY = item->computed.scroll.viewport.y > 0
					? item->computed.scroll.viewport.y
					: item->computed.contentSize.y;
				const Bool scrollableX = overflowAllowsScrolling(nodeStyle.overflowX) &&
					overflowClips(UiOverflow::Auto, extentX, viewportX);
				const Bool scrollableY = overflowAllowsScrolling(nodeStyle.overflowY) &&
					overflowClips(UiOverflow::Auto, extentY, viewportY);
				if (scrollableX || scrollableY) return current;
			}
			current = item->parentHandle();
		}
		return {};
	}

	void UiOverflowController::ensureEntry(const UiNodeHandle node)
	{
		if (findEntry(node)) return;
		Entry entry;
		entry.handle = node;
		entries.push_back(std::move(entry));
	}

	void UiOverflowController::pruneEntries()
	{
		std::erase_if(entries, [&](Entry& entry)
			{
				if (document.isValid(entry.handle) && isScrollableContainer(entry.handle))
					return false;
				detachListeners(entry);
				return true;
			});
	}

	void UiOverflowController::attachListeners(Entry& entry)
	{
		if (entry.listenersAttached) return;
		entry.wheelToken = router.addListener(entry.handle, UiRoutedEventType::Wheel,
			[this, handle = entry.handle](UiRoutedEvent& event)
			{
				Entry* slot = findEntry(handle);
				if (!slot) return;
				handleWheel(*slot, event);
			});
		entry.pointerDownToken = router.addListener(entry.handle, UiRoutedEventType::PointerDown,
			[this, handle = entry.handle](UiRoutedEvent& event)
			{
				Entry* slot = findEntry(handle);
				if (!slot) return;
				beginScrollbarDrag(*slot, event);
			});
		entry.dragToken = router.addListener(entry.handle, UiRoutedEventType::Drag,
			[this, handle = entry.handle](UiRoutedEvent& event)
			{
				Entry* slot = findEntry(handle);
				if (!slot) return;
				continueScrollbarDrag(*slot, event);
			});
		entry.pointerUpToken = router.addListener(entry.handle, UiRoutedEventType::PointerUp,
			[this, handle = entry.handle](UiRoutedEvent& event)
			{
				Entry* slot = findEntry(handle);
				if (!slot || !slot->scrollbar.active) return;
				slot->scrollbar.active = false;
				event.releasePointer = true;
				event.handled = true;
			});
		entry.listenersAttached = true;
	}

	void UiOverflowController::detachListeners(Entry& entry)
	{
		if (!entry.listenersAttached) return;
		if (entry.wheelToken.isValid()) (void)router.removeListener(entry.wheelToken);
		if (entry.pointerDownToken.isValid()) (void)router.removeListener(entry.pointerDownToken);
		if (entry.dragToken.isValid()) (void)router.removeListener(entry.dragToken);
		if (entry.pointerUpToken.isValid()) (void)router.removeListener(entry.pointerUpToken);
		entry.wheelToken = {};
		entry.pointerDownToken = {};
		entry.dragToken = {};
		entry.pointerUpToken = {};
		entry.listenersAttached = false;
	}

	void UiOverflowController::markScrollDirty(const UiNodeHandle node)
	{
		document.markDirty(node, DirtyFlag::Arrange);
		document.markDirty(node, DirtyFlag::Paint);
		pendingRelayout = true;
	}

	void UiOverflowController::publishPresentation(
		Entry& entry, const UiScrollGutterReservation& gutters)
	{
		UiNode* root = document.get(entry.handle);
		if (!root) return;
		root->computed.scroll.offset = entry.scroll.offset();
		root->computed.scroll.viewport = entry.scroll.viewport();
		root->computed.scroll.content = entry.scroll.extent();
		root->computed.scroll.verticalGutter = gutters.vertical;
		root->computed.scroll.horizontalGutter = gutters.horizontal;
		root->computed.scroll.showVerticalScrollbar = gutters.showVertical;
		root->computed.scroll.showHorizontalScrollbar = gutters.showHorizontal;
	}

	void UiOverflowController::synchronizeAfterLayout()
	{
		pruneEntries();
		document.traverse([&](const UiNodeHandle handle, UiNode& node)
			{
				if (!isScrollableContainer(handle)) return;
				const UiLayoutStyle nodeStyle = layoutEngine.resolvedStyle(handle);
				if (!overflowAllowsScrolling(nodeStyle.overflowX) &&
					!overflowAllowsScrolling(nodeStyle.overflowY)) return;

				ensureEntry(handle);
				Entry* entry = findEntry(handle);
				if (!entry) return;

				const Vec2 extent = node.computed.scrollExtent;
				const Vec2 contentSize = node.computed.contentSize;
				const UiScrollGutterReservation gutters = reserveScrollGutters(
					nodeStyle.overflowX, nodeStyle.overflowY, extent, contentSize);
				const Float previousVerticalGutter = node.computed.scroll.verticalGutter;
				const Float previousHorizontalGutter = node.computed.scroll.horizontalGutter;
				const Vec2 previousOffset = entry->scroll.offset();
				entry->scroll.setMetrics(extent, gutters.viewport);
				publishPresentation(*entry, gutters);
				attachListeners(*entry);

				if (entry->scroll.offset() != previousOffset ||
					gutters.vertical != previousVerticalGutter ||
					gutters.horizontal != previousHorizontalGutter)
					markScrollDirty(handle);
			});
	}

	void UiOverflowController::handleWheel(Entry& entry, UiRoutedEvent& event)
	{
		UiNode* root = document.get(entry.handle);
		if (!root || !root->state.enabled) return;
		const UiLayoutStyle nodeStyle = layoutEngine.resolvedStyle(entry.handle);
		Vec2 delta(static_cast<Float>(event.delta.x), static_cast<Float>(event.delta.y));
		if (!overflowAllowsScrolling(nodeStyle.overflowX)) delta.x = 0;
		if (!overflowAllowsScrolling(nodeStyle.overflowY)) delta.y = 0;
		if (delta.x == 0 && delta.y == 0) return;

		const UiScrollConsumption consumed = entry.scroll.consumeScroll(delta);
		if (consumed.consumed.x == 0 && consumed.consumed.y == 0) return;
		const UiScrollGutterReservation gutters{
			root->computed.scroll.verticalGutter,
			root->computed.scroll.horizontalGutter,
			entry.scroll.viewport(),
			root->computed.scroll.showVerticalScrollbar,
			root->computed.scroll.showHorizontalScrollbar
		};
		publishPresentation(entry, gutters);
		markScrollDirty(entry.handle);
		event.handled = true;
		event.stopPropagation();
	}

	void UiOverflowController::beginScrollbarDrag(Entry& entry, UiRoutedEvent& event)
	{
		UiNode* root = document.get(entry.handle);
		if (!root || !root->state.enabled) return;

		const Vec2 pointer(static_cast<Float>(event.position.x), static_cast<Float>(event.position.y));
		const UiRect contentBox{root->computed.contentPosition, root->computed.contentSize};
		const UiRect scrollport = scrollportRect(
			contentBox,
			root->computed.scroll.verticalGutter,
			root->computed.scroll.horizontalGutter);
		const UiScrollbarGeometry vertical = root->computed.scroll.showVerticalScrollbar
			? verticalScrollbarGeometry(scrollport, entry.scroll.vertical()) : UiScrollbarGeometry{};
		const UiScrollbarGeometry horizontal = root->computed.scroll.showHorizontalScrollbar
			? horizontalScrollbarGeometry(scrollport, entry.scroll.horizontal()) : UiScrollbarGeometry{};

		const Bool onVertical = vertical.containsTrack(pointer);
		if (!onVertical && !horizontal.containsTrack(pointer)) return;
		const UiScrollbarGeometry& bar = onVertical ? vertical : horizontal;
		const Float thumbLength = onVertical ? bar.thumb.size.y : bar.thumb.size.x;
		const Float pointerAlong = onVertical ? pointer.y : pointer.x;
		const Float thumbStart = onVertical ? bar.thumb.position.y : bar.thumb.position.x;

		entry.scrollbar.active = true;
		entry.scrollbar.vertical = onVertical;
		entry.scrollbar.grab = bar.containsThumb(pointer)
			? pointerAlong - thumbStart
			: thumbLength * 0.5f;

		event.capturePointer = true;
		event.handled = true;
		event.stopPropagation();
		continueScrollbarDrag(entry, event);
	}

	void UiOverflowController::continueScrollbarDrag(Entry& entry, UiRoutedEvent& event)
	{
		UiNode* root = document.get(entry.handle);
		if (!entry.scrollbar.active || !root) return;

		const Vec2 pointer(static_cast<Float>(event.position.x), static_cast<Float>(event.position.y));
		const UiRect contentBox{root->computed.contentPosition, root->computed.contentSize};
		const UiRect scrollport = scrollportRect(
			contentBox,
			root->computed.scroll.verticalGutter,
			root->computed.scroll.horizontalGutter);
		const Bool onVertical = entry.scrollbar.vertical;
		const UiScrollAxisState axis =
			onVertical ? entry.scroll.vertical() : entry.scroll.horizontal();
		const UiScrollbarGeometry bar = onVertical
			? verticalScrollbarGeometry(scrollport, axis)
			: horizontalScrollbarGeometry(scrollport, axis);
		if (!bar.visible)
		{
			entry.scrollbar.active = false;
			return;
		}

		const Float pointerAlong = onVertical ? pointer.y : pointer.x;
		const Float offset = scrollOffsetForThumbStart(
			onVertical ? bar.track.position.y : bar.track.position.x,
			onVertical ? bar.track.size.y : bar.track.size.x,
			onVertical ? bar.thumb.size.y : bar.thumb.size.x,
			pointerAlong - entry.scrollbar.grab,
			axis.maximumOffset());

		Vec2 next = entry.scroll.offset();
		if (onVertical) next.y = offset;
		else next.x = offset;
		entry.scroll.setOffset(next);
		const UiScrollGutterReservation gutters{
			root->computed.scroll.verticalGutter,
			root->computed.scroll.horizontalGutter,
			entry.scroll.viewport(),
			root->computed.scroll.showVerticalScrollbar,
			root->computed.scroll.showHorizontalScrollbar
		};
		publishPresentation(entry, gutters);
		markScrollDirty(entry.handle);
		event.handled = true;
	}
}
