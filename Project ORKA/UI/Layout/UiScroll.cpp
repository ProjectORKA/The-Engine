#include "UiScroll.hpp"

#include "UiOverflowController.hpp"

namespace Ui
{
	namespace
	{
		Float finiteNonNegative(const Float value)
		{
			return std::isfinite(value) ? std::max(0.0f, value) : 0.0f;
		}

		Float finiteValue(const Float value)
		{
			return std::isfinite(value) ? value : 0.0f;
		}

		Float finiteFloat(const Double value)
		{
			const Double limit = static_cast<Double>(std::numeric_limits<Float>::max());
			return static_cast<Float>(std::clamp(value, -limit, limit));
		}

		void consumeAxis(
			const Float delta,
			Float& offset,
			const Float maximum,
			const UiOverscrollPolicy policy,
			Float& consumed,
			Float& unconsumed)
		{
			const Float safeDelta = finiteValue(delta);
			if (policy == UiOverscrollPolicy::Disabled)
			{
				consumed = 0;
				unconsumed = safeDelta;
				return;
			}

			const Float next = std::clamp(offset + safeDelta, 0.0f, maximum);
			const Float applied = next - offset;
			offset = next;

			if (policy == UiOverscrollPolicy::Contain)
			{
				consumed = safeDelta;
				unconsumed = 0;
			}
			else
			{
				consumed = applied;
				unconsumed = safeDelta - applied;
			}
		}
	}

	Float UiScrollAxisState::maximumOffset() const
	{
		return std::max(0.0f, finiteNonNegative(extent) - finiteNonNegative(viewport));
	}

	void UiScrollState::setViewport(const Vec2 size)
	{
		viewportSize = Vec2(finiteNonNegative(size.x), finiteNonNegative(size.y));
		clamp();
	}

	void UiScrollState::setExtent(const Vec2 size)
	{
		contentExtent = Vec2(finiteNonNegative(size.x), finiteNonNegative(size.y));
		clamp();
	}

	void UiScrollState::setMetrics(
		const Vec2 newExtent,
		const Vec2 newViewport,
		const Vec2 anchorAdjustment)
	{
		contentExtent = Vec2(finiteNonNegative(newExtent.x), finiteNonNegative(newExtent.y));
		viewportSize = Vec2(finiteNonNegative(newViewport.x), finiteNonNegative(newViewport.y));
		currentOffset += Vec2(finiteValue(anchorAdjustment.x), finiteValue(anchorAdjustment.y));
		clamp();
	}

	void UiScrollState::setOffset(const Vec2 value)
	{
		currentOffset = Vec2(finiteValue(value.x), finiteValue(value.y));
		clamp();
	}

	void UiScrollState::scrollBy(const Vec2 delta)
	{
		(void)consumeScroll(delta);
	}

	UiScrollConsumption UiScrollState::consumeScroll(const Vec2 delta)
	{
		UiScrollConsumption result;
		const Vec2 maximum = maximumOffset();
		consumeAxis(delta.x, currentOffset.x, maximum.x, horizontalPolicy,
			result.consumed.x, result.unconsumed.x);
		consumeAxis(delta.y, currentOffset.y, maximum.y, verticalPolicy,
			result.consumed.y, result.unconsumed.y);
		return result;
	}

	void UiScrollState::preserveAnchor(const Vec2 anchorAdjustment)
	{
		currentOffset += Vec2(finiteValue(anchorAdjustment.x), finiteValue(anchorAdjustment.y));
		clamp();
	}

	void UiScrollState::setOverscrollPolicy(
		const UiOverscrollPolicy horizontal,
		const UiOverscrollPolicy vertical)
	{
		horizontalPolicy = horizontal;
		verticalPolicy = vertical;
	}

	Vec2 UiScrollState::maximumOffset() const
	{
		return Vec2(
			std::max(0.0f, contentExtent.x - viewportSize.x),
			std::max(0.0f, contentExtent.y - viewportSize.y));
	}

	UiScrollAxisState UiScrollState::horizontal() const
	{
		return { contentExtent.x, viewportSize.x, currentOffset.x, horizontalPolicy };
	}

	UiScrollAxisState UiScrollState::vertical() const
	{
		return { contentExtent.y, viewportSize.y, currentOffset.y, verticalPolicy };
	}

	void UiScrollState::restoreVerticalAnchor(
		const Float itemStart,
		const Float offsetWithinItem)
	{
		currentOffset.y = finiteValue(itemStart) + finiteValue(offsetWithinItem);
		clamp();
	}

	void UiScrollState::clamp()
	{
		const Vec2 maximum = maximumOffset();
		currentOffset.x = std::clamp(finiteValue(currentOffset.x), 0.0f, maximum.x);
		currentOffset.y = std::clamp(finiteValue(currentOffset.y), 0.0f, maximum.y);
	}

	namespace
	{
		// Shared by both axes: the track runs from trackStart for trackLength,
		// and is crossThickness wide starting at crossStart.
		UiScrollbarGeometry axisScrollbar(
			const Float trackStart,
			const Float trackLength,
			const Float crossStart,
			const UiScrollAxisState& axis,
			const UiScrollbarStyle& style,
			const Bool vertical)
		{
			UiScrollbarGeometry geometry;
			const Float length = finiteNonNegative(trackLength);
			const Float thickness = finiteNonNegative(style.thickness);
			const Float extent = finiteNonNegative(axis.extent);
			const Float viewport = finiteNonNegative(axis.viewport);
			const Float maximum = axis.maximumOffset();
			if (length <= 0 || thickness <= 0 || viewport <= 0 || extent <= 0 || maximum <= 0)
				return geometry;

			const Float shortest = std::min(length, finiteNonNegative(style.minimumThumbLength));
			const Float thumbLength = std::clamp(length * (viewport / extent), shortest, length);
			const Float travel = length - thumbLength;
			const Float progress = std::clamp(finiteNonNegative(axis.offset) / maximum, 0.0f, 1.0f);
			const Float thumbStart = trackStart + travel * progress;

			geometry.visible = true;
			if (vertical)
			{
				geometry.track = {Vec2(crossStart, trackStart), Vec2(thickness, length)};
				geometry.thumb = {Vec2(crossStart, thumbStart), Vec2(thickness, thumbLength)};
			}
			else
			{
				geometry.track = {Vec2(trackStart, crossStart), Vec2(length, thickness)};
				geometry.thumb = {Vec2(thumbStart, crossStart), Vec2(thumbLength, thickness)};
			}
			return geometry;
		}

		Bool contains(const UiRect& rect, const Vec2 point)
		{
			return point.x >= rect.position.x && point.x <= rect.position.x + rect.size.x &&
				point.y >= rect.position.y && point.y <= rect.position.y + rect.size.y;
		}
	}

	Bool UiScrollbarGeometry::containsTrack(const Vec2 point) const
	{
		return visible && contains(track, point);
	}

	Bool UiScrollbarGeometry::containsThumb(const Vec2 point) const
	{
		return visible && contains(thumb, point);
	}

	Float scrollbarCrossAxisReserve(const UiScrollbarStyle& style)
	{
		return finiteNonNegative(style.thickness) + finiteNonNegative(style.margin);
	}

	UiScrollGutterReservation reserveScrollGutters(
		const UiOverflow overflowX,
		const UiOverflow overflowY,
		const Vec2 contentExtent,
		const Vec2 contentSize,
		const UiScrollbarStyle& style)
	{
		UiScrollGutterReservation reservation;
		Vec2 viewport = Vec2(
			finiteNonNegative(contentSize.x), finiteNonNegative(contentSize.y));
		const Float reserve = scrollbarCrossAxisReserve(style);

		reservation.showVertical = overflowShowsScrollbar(
			overflowY, contentExtent.y, viewport.y);
		if (reservation.showVertical)
		{
			reservation.vertical = reserve;
			viewport.x = std::max(0.0f, viewport.x - reserve);
		}

		reservation.showHorizontal = overflowShowsScrollbar(
			overflowX, contentExtent.x, viewport.x);
		if (reservation.showHorizontal)
		{
			reservation.horizontal = reserve;
			viewport.y = std::max(0.0f, viewport.y - reserve);
		}

		if (reservation.showVertical && reservation.showHorizontal)
		{
			const Bool stillVertical = overflowShowsScrollbar(
				overflowY, contentExtent.y, viewport.y);
			if (!stillVertical)
			{
				reservation.showVertical = false;
				reservation.vertical = 0;
				viewport = Vec2(
					finiteNonNegative(contentSize.x),
					std::max(0.0f, finiteNonNegative(contentSize.y) - reserve));
			}
		}

		reservation.viewport = viewport;
		return reservation;
	}

	UiRect scrollportRect(
		const UiRect& contentBox,
		const Float verticalGutter,
		const Float horizontalGutter)
	{
		return {
			contentBox.position,
			glm::max(
				contentBox.size - Vec2(finiteNonNegative(verticalGutter), finiteNonNegative(horizontalGutter)),
				Vec2(0))
		};
	}

	UiScrollbarGeometry verticalScrollbarGeometry(
		const UiRect& scrollportBox,
		const UiScrollAxisState& axis,
		const UiScrollbarStyle& style)
	{
		const Float trackX = scrollportBox.end().x + finiteNonNegative(style.margin);
		return axisScrollbar(
			scrollportBox.position.y, scrollportBox.size.y, trackX, axis, style, true);
	}

	UiScrollbarGeometry horizontalScrollbarGeometry(
		const UiRect& scrollportBox,
		const UiScrollAxisState& axis,
		const UiScrollbarStyle& style)
	{
		const Float trackY = scrollportBox.end().y + finiteNonNegative(style.margin);
		return axisScrollbar(
			scrollportBox.position.x, scrollportBox.size.x, trackY, axis, style, false);
	}

	Float scrollOffsetForThumbStart(
		const Float trackStart,
		const Float trackLength,
		const Float thumbLength,
		const Float thumbStart,
		const Float maximumOffset)
	{
		const Float travel = finiteNonNegative(trackLength) - finiteNonNegative(thumbLength);
		const Float maximum = finiteNonNegative(maximumOffset);
		if (travel <= 0 || maximum <= 0) return 0;
		const Float progress = std::clamp(
			(finiteValue(thumbStart) - finiteValue(trackStart)) / travel, 0.0f, 1.0f);
		return progress * maximum;
	}

	UiVirtualList::UiVirtualList(const Float estimatedItemHeight)
	{
		if (validHeight(estimatedItemHeight))
			defaultHeight = estimatedItemHeight;
	}

	Bool UiVirtualList::initialize(const UInt count, const Float estimatedItemHeight)
	{
		if (count > maximumItemCount || !validHeight(estimatedItemHeight))
			return false;

		Vector<U64> keys;
		keys.reserve(count);
		for (UInt index = 0; index < count; ++index)
			keys.push_back(static_cast<U64>(index));

		measuredByKey.clear();
		defaultHeight = estimatedItemHeight;
		itemKeys = std::move(keys);
		rebuild();
		return true;
	}

	Bool UiVirtualList::reset(Vector<U64> keys, const Float estimatedItemHeight)
	{
		if (!validHeight(estimatedItemHeight) || !validateKeys(keys))
			return false;

		measuredByKey.clear();
		defaultHeight = estimatedItemHeight;
		itemKeys = std::move(keys);
		rebuild();
		return true;
	}

	Bool UiVirtualList::setKeys(Vector<U64> keys)
	{
		if (!validateKeys(keys))
			return false;

		itemKeys = std::move(keys);
		rebuild();
		for (auto measurement = measuredByKey.begin(); measurement != measuredByKey.end();)
		{
			if (indexByKey.find(measurement->first) == indexByKey.end())
				measurement = measuredByKey.erase(measurement);
			else
				++measurement;
		}
		return true;
	}

	Bool UiVirtualList::setEstimatedItemHeight(const Float height)
	{
		if (!validHeight(height))
			return false;
		if (defaultHeight == height)
			return true;

		defaultHeight = height;
		rebuild();
		return true;
	}

	Bool UiVirtualList::setMeasuredHeight(const UInt index, const Float height)
	{
		return index < size() && setMeasuredHeight(itemKeys[index], height);
	}

	Bool UiVirtualList::setMeasuredHeight(const U64 key, const Float height)
	{
		if (!validHeight(height))
			return false;

		const auto index = indexByKey.find(key);
		if (index == indexByKey.end())
			return false;

		const UInt itemIndex = index->second;
		const Float previous = heights[itemIndex];
		measuredByKey[key] = height;
		if (previous == height)
			return false;

		heights[itemIndex] = height;
		add(itemIndex, height - previous);
		return true;
	}

	Bool UiVirtualList::clearMeasuredHeight(const U64 key)
	{
		const auto measurement = measuredByKey.find(key);
		if (measurement == measuredByKey.end())
			return false;

		measuredByKey.erase(measurement);
		const auto index = indexByKey.find(key);
		if (index == indexByKey.end())
			return true;

		const UInt itemIndex = index->second;
		const Float previous = heights[itemIndex];
		heights[itemIndex] = defaultHeight;
		add(itemIndex, defaultHeight - previous);
		return true;
	}

	UiVirtualMeasurementResult UiVirtualList::setMeasuredHeightPreservingAnchor(
		const U64 key,
		const Float height,
		const UiScrollAnchor& anchor,
		const Float scrollOffset)
	{
		UiVirtualMeasurementResult result;
		result.correctedScrollOffset = finiteNonNegative(scrollOffset);
		if (!validHeight(height) || indexByKey.find(key) == indexByKey.end())
			return result;

		result.accepted = true;
		const Float oldAnchorOffset = offsetForAnchor(anchor, result.correctedScrollOffset);
		result.changed = setMeasuredHeight(key, height);
		if (!result.changed)
			return result;

		const Float newAnchorOffset = offsetForAnchor(anchor, oldAnchorOffset);
		result.scrollAdjustment = newAnchorOffset - oldAnchorOffset;
		result.correctedScrollOffset =
			std::clamp(result.correctedScrollOffset + result.scrollAdjustment, 0.0f, totalExtent());
		return result;
	}

	U64 UiVirtualList::keyAt(const UInt index) const
	{
		return index < size() ? itemKeys[index] : 0;
	}

	Float UiVirtualList::totalExtent() const
	{
		return finiteFloat(prefix(size()));
	}

	Float UiVirtualList::heightAt(const UInt index) const
	{
		return index < size() ? heights[index] : 0.0f;
	}

	Float UiVirtualList::offsetForIndex(const UInt index) const
	{
		return finiteFloat(prefix(std::min(index, size())));
	}

	UInt UiVirtualList::indexAtOffset(const Float offset) const
	{
		if (itemKeys.empty())
			return 0;

		const Double target = static_cast<Double>(finiteNonNegative(offset));
		if (target >= prefix(size()))
			return size() - 1;

		UInt index = 0;
		Double accumulated = 0;
		UInt bit = 1;
		while (bit <= size() / 2)
			bit <<= 1;

		for (; bit != 0; bit >>= 1)
		{
			const UInt next = index + bit;
			if (next <= size() && accumulated + fenwick[next] <= target)
			{
				index = next;
				accumulated += fenwick[next];
			}
		}
		return std::min(index, size() - 1);
	}

	UiVirtualRange UiVirtualList::visibleRange(
		const Float scrollOffset,
		const Float viewportExtent,
		const Float overscanExtent) const
	{
		UiVirtualRange result;
		result.totalExtent = totalExtent();
		if (itemKeys.empty() || !std::isfinite(viewportExtent) || viewportExtent <= 0)
			return result;

		const Double total = prefix(size());
		const Double scroll = std::clamp(
			static_cast<Double>(finiteNonNegative(scrollOffset)), 0.0, total);
		const Double overscan = static_cast<Double>(finiteNonNegative(overscanExtent));
		const Float start = finiteFloat(std::max(0.0, scroll - overscan));
		const Float end = finiteFloat(std::min(total,
			scroll + static_cast<Double>(viewportExtent) + overscan));

		result.first = indexAtOffset(start);
		result.leadingOffset = offsetForIndex(result.first);
		if (end <= static_cast<Double>(result.leadingOffset))
		{
			result.last = result.first;
			return result;
		}

		const UInt endIndex = indexAtOffset(end);
		result.last = endIndex;
		if (end >= total || static_cast<Double>(offsetForIndex(endIndex)) < static_cast<Double>(end))
			++result.last;
		result.last = std::min(result.last, size());
		return result;
	}

	UiScrollAnchor UiVirtualList::captureAnchor(const Float scrollOffset) const
	{
		if (itemKeys.empty())
			return {};

		const Float clampedOffset = std::clamp(
			finiteNonNegative(scrollOffset), 0.0f, totalExtent());
		const UInt index = indexAtOffset(clampedOffset);
		return { itemKeys[index], clampedOffset - offsetForIndex(index), true };
	}

	Float UiVirtualList::offsetForAnchor(
		const UiScrollAnchor& anchor,
		const Float fallbackOffset) const
	{
		const Float fallback = finiteNonNegative(fallbackOffset);
		if (!anchor.valid)
			return fallback;

		const auto index = indexByKey.find(anchor.key);
		if (index == indexByKey.end())
			return fallback;
		return finiteNonNegative(
			offsetForIndex(index->second) + finiteValue(anchor.offsetWithinItem));
	}

	UInt UiVirtualList::indexOf(const U64 key) const
	{
		const auto index = indexByKey.find(key);
		return index == indexByKey.end() ? maxUInt : index->second;
	}

	Bool UiVirtualList::validHeight(const Float height)
	{
		return std::isfinite(height) &&
			height >= minimumItemHeight &&
			height <= maximumItemHeight;
	}

	Bool UiVirtualList::validateKeys(const Vector<U64>& keys) const
	{
		if (keys.size() > maximumItemCount)
			return false;

		std::map<U64, Bool> unique;
		for (const U64 key : keys)
		{
			if (!unique.emplace(key, true).second)
				return false;
		}
		return true;
	}

	void UiVirtualList::rebuild()
	{
		indexByKey.clear();
		heights.assign(itemKeys.size(), defaultHeight);
		fenwick.assign(itemKeys.size() + 1, 0.0);

		for (UInt index = 0; index < size(); ++index)
		{
			const U64 key = itemKeys[index];
			indexByKey.emplace(key, index);
			const auto measurement = measuredByKey.find(key);
			if (measurement != measuredByKey.end())
				heights[index] = measurement->second;

			const UInt fenwickIndex = index + 1;
			fenwick[fenwickIndex] += static_cast<Double>(heights[index]);
			const UInt parent = fenwickIndex + (fenwickIndex & (~fenwickIndex + 1));
			if (parent < fenwick.size())
				fenwick[parent] += fenwick[fenwickIndex];
		}
	}

	void UiVirtualList::add(const UInt index, const Float delta)
	{
		for (UInt fenwickIndex = index + 1;
			fenwickIndex < fenwick.size();
			fenwickIndex += fenwickIndex & (~fenwickIndex + 1))
		{
			fenwick[fenwickIndex] += static_cast<Double>(delta);
		}
	}

	Double UiVirtualList::prefix(const UInt count) const
	{
		Double result = 0;
		for (UInt fenwickIndex = std::min(count, size());
			fenwickIndex != 0;
			fenwickIndex -= fenwickIndex & (~fenwickIndex + 1))
		{
			result += fenwick[fenwickIndex];
		}
		return result;
	}
}
