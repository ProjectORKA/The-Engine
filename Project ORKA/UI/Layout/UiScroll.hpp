#pragma once

#include "UiLayoutTypes.hpp"

namespace Ui
{
	enum class UiOverscrollPolicy : Byte
	{
		Chain,
		Contain,
		Disabled,
	};

	struct UiScrollAxisState
	{
		Float extent = 0;
		Float viewport = 0;
		Float offset = 0;
		UiOverscrollPolicy overscrollPolicy = UiOverscrollPolicy::Chain;

		[[nodiscard]] Float maximumOffset() const;
	};

	struct UiScrollConsumption
	{
		Vec2 consumed = Vec2(0);
		Vec2 unconsumed = Vec2(0);
	};

	struct UiScrollAnchor
	{
		U64 key = 0;
		Float offsetWithinItem = 0;
		Bool valid = false;
	};

	class UiScrollState
	{
	public:
		void setViewport(Vec2 size);
		void setExtent(Vec2 size);
		void setMetrics(Vec2 extent, Vec2 viewport, Vec2 anchorAdjustment = Vec2(0));
		void setOffset(Vec2 value);
		void scrollBy(Vec2 delta);
		[[nodiscard]] UiScrollConsumption consumeScroll(Vec2 delta);
		void preserveAnchor(Vec2 anchorAdjustment);
		void setOverscrollPolicy(UiOverscrollPolicy horizontal, UiOverscrollPolicy vertical);

		[[nodiscard]] Vec2 offset() const { return currentOffset; }
		[[nodiscard]] Vec2 viewport() const { return viewportSize; }
		[[nodiscard]] Vec2 extent() const { return contentExtent; }
		[[nodiscard]] Vec2 maximumOffset() const;
		[[nodiscard]] UiScrollAxisState horizontal() const;
		[[nodiscard]] UiScrollAxisState vertical() const;

		// Anchor restoration is explicit so input/interaction remains outside layout.
		void restoreVerticalAnchor(Float itemStart, Float offsetWithinItem);

	private:
		Vec2 currentOffset = Vec2(0);
		Vec2 viewportSize = Vec2(0);
		Vec2 contentExtent = Vec2(0);
		UiOverscrollPolicy horizontalPolicy = UiOverscrollPolicy::Chain;
		UiOverscrollPolicy verticalPolicy = UiOverscrollPolicy::Chain;

		void clamp();
	};

	struct UiScrollbarStyle
	{
		Float thickness = 6;
		Float margin = 3;
		Float minimumThumbLength = 24;
	};

	struct UiScrollGutterReservation
	{
		Float vertical = 0;
		Float horizontal = 0;
		Vec2 viewport = Vec2(0);
		Bool showVertical = false;
		Bool showHorizontal = false;
	};

	[[nodiscard]] Float scrollbarCrossAxisReserve(const UiScrollbarStyle& style = {});

	[[nodiscard]] UiScrollGutterReservation reserveScrollGutters(
		UiOverflow overflowX, UiOverflow overflowY,
		Vec2 contentExtent, Vec2 contentSize,
		const UiScrollbarStyle& style = {});

	struct UiScrollbarGeometry
	{
		Bool visible = false;
		UiRect track;
		UiRect thumb;

		[[nodiscard]] Bool containsTrack(Vec2 point) const;
		[[nodiscard]] Bool containsThumb(Vec2 point) const;
	};

	struct UiScrollbarDrag
	{
		Bool active = false;
		Bool vertical = true;
		// Distance from the thumb start to the pointer when the drag began.
		Float grab = 0;
	};

	[[nodiscard]] UiRect scrollportRect(
		const UiRect& contentBox, Float verticalGutter, Float horizontalGutter);

	[[nodiscard]] UiScrollbarGeometry verticalScrollbarGeometry(
		const UiRect& scrollportBox,
		const UiScrollAxisState& axis, const UiScrollbarStyle& style = {});

	[[nodiscard]] UiScrollbarGeometry horizontalScrollbarGeometry(
		const UiRect& scrollportBox,
		const UiScrollAxisState& axis, const UiScrollbarStyle& style = {});

	// Inverse of the thumb placement above, used while dragging the thumb.
	[[nodiscard]] Float scrollOffsetForThumbStart(
		Float trackStart, Float trackLength, Float thumbLength,
		Float thumbStart, Float maximumOffset);

	struct UiVirtualRange
	{
		UInt first = 0;
		UInt last = 0; // Exclusive.
		Float leadingOffset = 0;
		Float totalExtent = 0;

		[[nodiscard]] Bool empty() const { return first == last; }
	};

	struct UiVirtualMeasurementResult
	{
		Bool accepted = false;
		Bool changed = false;
		Float correctedScrollOffset = 0;
		Float scrollAdjustment = 0;
	};

	class UiVirtualList
	{
	public:
		static constexpr UInt maximumItemCount = 1'000'000;
		static constexpr Float minimumItemHeight = 0.01f;
		static constexpr Float maximumItemHeight = 1'000'000.0f;

		explicit UiVirtualList(Float estimatedItemHeight = 24);

		// Keys are stable identities. Existing measurements survive reordering.
		[[nodiscard]] Bool initialize(UInt count, Float estimatedItemHeight = 24);
		[[nodiscard]] Bool reset(Vector<U64> keys, Float estimatedItemHeight = 24);
		[[nodiscard]] Bool setKeys(Vector<U64> keys);
		[[nodiscard]] Bool setEstimatedItemHeight(Float height);
		[[nodiscard]] Bool setMeasuredHeight(UInt index, Float height);
		[[nodiscard]] Bool setMeasuredHeight(U64 key, Float height);
		[[nodiscard]] Bool clearMeasuredHeight(U64 key);
		[[nodiscard]] UiVirtualMeasurementResult setMeasuredHeightPreservingAnchor(
			U64 key, Float height, const UiScrollAnchor& anchor, Float scrollOffset);

		[[nodiscard]] UInt size() const { return static_cast<UInt>(itemKeys.size()); }
		[[nodiscard]] Float estimatedItemHeight() const { return defaultHeight; }
		[[nodiscard]] U64 keyAt(UInt index) const;
		[[nodiscard]] Float totalExtent() const;
		[[nodiscard]] Float heightAt(UInt index) const;
		[[nodiscard]] Float offsetForIndex(UInt index) const;
		[[nodiscard]] UInt indexAtOffset(Float offset) const;
		[[nodiscard]] UiVirtualRange visibleRange(
			Float scrollOffset, Float viewportExtent, Float overscanExtent) const;

		[[nodiscard]] UiScrollAnchor captureAnchor(Float scrollOffset) const;
		[[nodiscard]] Float offsetForAnchor(
			const UiScrollAnchor& anchor, Float fallbackOffset) const;
		[[nodiscard]] UInt indexOf(U64 key) const;

	private:
		Vector<U64> itemKeys;
		Vector<Float> heights;
		Vector<Double> fenwick;
		std::map<U64, Float> measuredByKey;
		std::map<U64, UInt> indexByKey;
		Float defaultHeight = 24;

		[[nodiscard]] static Bool validHeight(Float height);
		[[nodiscard]] Bool validateKeys(const Vector<U64>& keys) const;
		void rebuild();
		void add(UInt index, Float delta);
		[[nodiscard]] Double prefix(UInt count) const;
	};

	// Compatibility name retained for clients of the original range-estimator API.
	using UiVirtualRangeEstimator = UiVirtualList;
}
