#include "UiLayoutEngine.hpp"

#include "UiOverflowController.hpp"
#include "UiScroll.hpp"
#include "../Theme/UiAppFlex.hpp"

#include <numeric>

namespace Ui
{
	namespace
	{
		Float nonNegative(const Float value)
		{
			return std::isfinite(value) ? std::max(value, 0.0f) : 0.0f;
		}

		Float mainValue(const Vec2 value, const UiAxis axis) { return axis == UiAxis::Row ? value.x : value.y; }
		Float crossValue(const Vec2 value, const UiAxis axis) { return axis == UiAxis::Row ? value.y : value.x; }
		void setMain(Vec2& value, const UiAxis axis, const Float amount) { (axis == UiAxis::Row ? value.x : value.y) = amount; }
		void setCross(Vec2& value, const UiAxis axis, const Float amount) { (axis == UiAxis::Row ? value.y : value.x) = amount; }
		// CSS: align stretch only fills an axis whose size is auto; explicit sizes win.
		Bool stretchesWidth(const UiLayoutStyle& style) { return style.width.kind == UiSizeKind::Auto; }
		Bool stretchesHeight(const UiLayoutStyle& style) { return style.height.kind == UiSizeKind::Auto; }
		Bool stretchesCross(const UiLayoutStyle& style, const UiAxis axis)
		{
			return axis == UiAxis::Row ? stretchesHeight(style) : stretchesWidth(style);
		}
		Float mainStart(const UiEdges& edges, const UiAxis axis) { return axis == UiAxis::Row ? edges.left : edges.top; }
		Float mainEnd(const UiEdges& edges, const UiAxis axis) { return axis == UiAxis::Row ? edges.right : edges.bottom; }
		Float crossStart(const UiEdges& edges, const UiAxis axis) { return axis == UiAxis::Row ? edges.top : edges.left; }
		Float crossEnd(const UiEdges& edges, const UiAxis axis) { return axis == UiAxis::Row ? edges.bottom : edges.right; }
		Bool equalRect(const UiRect& left, const UiRect& right)
		{
			return left.position.x == right.position.x && left.position.y == right.position.y &&
				left.size.x == right.size.x && left.size.y == right.size.y;
		}

		Bool equalVec2(const Vec2& left, const Vec2& right)
		{
			return left.x == right.x && left.y == right.y;
		}

		// CSS: `visible` paired with a non-visible axis computes to `auto`, so both axes clip
		// (children must not paint over the other axis's scrollbar gutter).
		[[nodiscard]] UiOverflow scrollClipPolicy(const UiOverflow overflow, const UiOverflow otherAxis)
		{
			if (overflowAllowsScrolling(overflow)) return UiOverflow::Hidden;
			if (overflow == UiOverflow::Visible && otherAxis != UiOverflow::Visible) return UiOverflow::Hidden;
			return overflow;
		}

		void intersectClipAxis(
			UiRect& clip, const UiRect& contentRect, const UiAxis axis, const UiOverflow overflow,
			const Float contentExtent, const Float viewportExtent)
		{
			if (!overflowClips(overflow, contentExtent, viewportExtent)) return;
			if (axis == UiAxis::Row)
			{
				const Float start = std::max(clip.position.x, contentRect.position.x);
				const Float finish = std::min(clip.end().x, contentRect.end().x);
				clip.position.x = start;
				clip.size.x = std::max(0.0f, finish - start);
			}
			else
			{
				const Float start = std::max(clip.position.y, contentRect.position.y);
				const Float finish = std::min(clip.end().y, contentRect.end().y);
				clip.position.y = start;
				clip.size.y = std::max(0.0f, finish - start);
			}
		}

		[[nodiscard]] Bool scrollsMainAxis(const UiLayoutStyle& parentStyle, const UiAxis axis)
		{
			return (axis == UiAxis::Column && overflowAllowsScrolling(parentStyle.overflowY)) ||
				(axis == UiAxis::Row && overflowAllowsScrolling(parentStyle.overflowX));
		}

		// Scroll containers clip on one axis but must still measure children at full content size.
		[[nodiscard]] UiConstraints measureConstraintsForFlexChild(
			const UiLayoutStyle& parentStyle, const Vec2 referenceMaximum, const UiAxis axis)
		{
			Vec2 maximum = referenceMaximum;
			if (axis == UiAxis::Column && overflowAllowsScrolling(parentStyle.overflowY))
				maximum.y = uiLayoutInfinity;
			if (axis == UiAxis::Row && overflowAllowsScrolling(parentStyle.overflowX))
				maximum.x = uiLayoutInfinity;
			return UiConstraints::loose(maximum);
		}

		[[nodiscard]] Float flexAutomaticMinimumMain(
			const UiIntrinsicMeasure& measured, const UiAxis axis)
		{
			return mainValue(measured.minimum, axis);
		}

		[[nodiscard]] Float flexItemMinimumMainAuto(
			const UiLayoutStyle& childStyle, const UiIntrinsicMeasure& measured, const UiAxis axis)
		{
			const Bool childClipsMainAxis =
				(axis == UiAxis::Column && childStyle.overflowY != UiOverflow::Visible) ||
				(axis == UiAxis::Row && childStyle.overflowX != UiOverflow::Visible);
			return childClipsMainAxis ? 0.0f : flexAutomaticMinimumMain(measured, axis);
		}

		[[nodiscard]] Float hypotheticalFlexMain(
			const Float basis, const Float minimumMain, const Float maximumMain)
		{
			return std::min(std::max(basis, minimumMain), maximumMain);
		}

		UiConstraints insetConstraints(const UiConstraints& constraints, const Vec2 amount)
		{
			UiConstraints result;
			result.minimum = glm::max(constraints.minimum - amount, Vec2(0));
			result.maximum.x = std::isfinite(constraints.maximum.x) ? std::max(0.0f, constraints.maximum.x - amount.x) : uiLayoutInfinity;
			result.maximum.y = std::isfinite(constraints.maximum.y) ? std::max(0.0f, constraints.maximum.y - amount.y) : uiLayoutInfinity;
			return result.normalized();
		}

		struct FlexItem
		{
			UiNodeHandle node;
			Vec2 measured = Vec2(0);
			UiEdges margin;
			Float basis = 0;
			Float main = 0;
			Float cross = 0;
			Float grow = 0;
			Float shrink = 1;
			Float minimumMain = 0;
			Float maximumMain = uiLayoutInfinity;
			Float baseline = -1;
		};

		struct FlexLine
		{
			Vector<FlexItem> items;
			Float main = 0;
			Float cross = 0;
			Float baseline = 0;
		};

		struct FlexWrapBreakItem
		{
			Float outerMain = 0;
			Float minimumOuterMain = 0;
			Float cross = 0;
			Float minimumCross = 0;
			Float baseline = -1;
			Float basis = 0;
			Float grow = 0;
		};

		constexpr Float flexWrapBalanceCostEpsilon = 1e-5f;

		// When several break plans share the same cost (common with equal-size grow items),
		// prefer packing more items on the current line so line breaks stay stable.
		[[nodiscard]] Bool preferFlexWrapBreakPlan(
			const Float candidateCost, const SizeT candidateEnd,
			const Float bestCost, const SizeT bestEnd, const SizeT unsetEnd)
		{
			if (candidateCost < bestCost - flexWrapBalanceCostEpsilon) return true;
			if (candidateCost > bestCost + flexWrapBalanceCostEpsilon) return false;
			if (bestEnd == unsetEnd) return true;
			return candidateEnd > bestEnd;
		}

		[[nodiscard]] Float flexItemOuterMain(const FlexItem& item, const UiAxis axis)
		{
			const Float hypotheticalMain = hypotheticalFlexMain(
				item.basis, item.minimumMain, item.maximumMain);
			return hypotheticalMain + mainStart(item.margin, axis) + mainEnd(item.margin, axis);
		}

		[[nodiscard]] Float flexItemFlexBaseOuterMain(const FlexItem& item, const UiAxis axis)
		{
			return item.basis + mainStart(item.margin, axis) + mainEnd(item.margin, axis);
		}

		[[nodiscard]] Float flexLineFlexBasePackedMain(
			const Vector<FlexItem>& items, const SizeT start, const SizeT end,
			const Float itemGap, const UiAxis axis)
		{
			Float total = 0;
			for (SizeT index = start; index < end; ++index)
			{
				if (index > start) total += itemGap;
				total += flexItemFlexBaseOuterMain(items[index], axis);
			}
			return total;
		}

		void appendFlexItemToLine(FlexLine& line, FlexItem&& item, const Float itemGap, const UiAxis axis)
		{
			const Float outerMain = flexItemOuterMain(item, axis);
			if (!line.items.empty()) line.main += itemGap;
			line.main += outerMain;
			line.cross = std::max(line.cross,
				item.cross + crossStart(item.margin, axis) + crossEnd(item.margin, axis));
			line.baseline = std::max(line.baseline, item.baseline + crossStart(item.margin, axis));
			line.items.push_back(std::move(item));
		}

		[[nodiscard]] Float flexLinePackedMain(
			const Vector<FlexWrapBreakItem>& items, const SizeT start, const SizeT end,
			const Float itemGap)
		{
			Float total = 0;
			for (SizeT index = start; index < end; ++index)
			{
				if (index > start) total += itemGap;
				total += items[index].outerMain;
			}
			return total;
		}

		[[nodiscard]] Float flexLinePackedMain(
			const Vector<FlexItem>& items, const SizeT start, const SizeT end,
			const Float itemGap, const UiAxis axis)
		{
			Float total = 0;
			for (SizeT index = start; index < end; ++index)
			{
				if (index > start) total += itemGap;
				total += flexItemOuterMain(items[index], axis);
			}
			return total;
		}

		// Knuth-Plass-inspired line cost: penalize flex-grow stretch, especially orphaned last lines.
		[[nodiscard]] Float flexLineBalanceBadness(
			const Vector<FlexWrapBreakItem>& items, const SizeT start, const SizeT end,
			const Float availableMain, const Float itemGap, const SizeT totalItems)
		{
			const SizeT count = end - start;
			Float growTotal = 0;
			Float basisPackedMain = 0;
			for (SizeT index = start; index < end; ++index)
			{
				growTotal += items[index].grow;
				if (index > start) basisPackedMain += itemGap;
				basisPackedMain += items[index].basis;
			}

			Float badness = 0;
			if (growTotal > 0 && basisPackedMain <= availableMain)
			{
				const Float flexibleSpace = availableMain - basisPackedMain;
				Float worstStretchRatio = 1;
				for (SizeT index = start; index < end; ++index)
				{
					const Float grownMain = items[index].basis +
						flexibleSpace * items[index].grow / growTotal;
					if (items[index].basis > 0)
						worstStretchRatio = std::max(worstStretchRatio, grownMain / items[index].basis);
				}
				const Float stretchExcess = worstStretchRatio - 1;
				badness += stretchExcess * stretchExcess * stretchExcess;
			}

			if (count == 1 && end == totalItems && totalItems > 1)
				badness += 1000;

			return badness;
		}

		[[nodiscard]] Vector<SizeT> greedyFlexLineBreaks(
			const Vector<FlexWrapBreakItem>& items, const Float availableMain, const Float itemGap)
		{
			const SizeT itemCount = items.size();
			Vector<SizeT> breaks;
			breaks.push_back(0);
			SizeT lineStart = 0;
			Float lineMain = 0;
			for (SizeT index = 0; index < itemCount; ++index)
			{
				const Float addition = (index > lineStart ? itemGap : 0) + items[index].outerMain;
				if (index > lineStart && lineMain + addition > availableMain)
				{
					breaks.push_back(index);
					lineStart = index;
					lineMain = items[index].outerMain;
				}
				else
				{
					if (index > lineStart) lineMain += itemGap;
					lineMain += items[index].outerMain;
				}
			}
			breaks.push_back(itemCount);
			return breaks;
		}

		[[nodiscard]] Vector<SizeT> balancedFlexLineBreaks(
			const Vector<FlexWrapBreakItem>& items, const Float availableMain, const Float itemGap)
		{
			const SizeT itemCount = items.size();
			if (itemCount == 0 || !std::isfinite(availableMain))
				return greedyFlexLineBreaks(items, availableMain, itemGap);

			Vector<Float> cost(itemCount + 1, uiLayoutInfinity);
			Vector<SizeT> nextBreak(itemCount + 1, itemCount);
			cost[itemCount] = 0;

			for (SizeT start = itemCount; start-- > 0;)
			{
				for (SizeT end = start + 1; end <= itemCount; ++end)
				{
					const SizeT lineCount = end - start;
					const Float packedMain = flexLinePackedMain(items, start, end, itemGap);
					if (lineCount > 1 && packedMain > availableMain + 0.01f) break;

					const Float lineCost = flexLineBalanceBadness(
						items, start, end, availableMain, itemGap, itemCount);
					const Float totalCost = lineCost + cost[end];
					if (preferFlexWrapBreakPlan(
						totalCost, end, cost[start], nextBreak[start], itemCount))
					{
						cost[start] = totalCost;
						nextBreak[start] = end;
					}
				}
			}

			if (!std::isfinite(cost[0]))
				return greedyFlexLineBreaks(items, availableMain, itemGap);

			Vector<SizeT> breaks;
			for (SizeT start = 0; start < itemCount; start = nextBreak[start])
				breaks.push_back(start);
			breaks.push_back(itemCount);
			return breaks;
		}

		[[nodiscard]] Vector<SizeT> flexLineBreaks(
			const Vector<FlexWrapBreakItem>& items, const Float availableMain,
			const Float itemGap, const UiWrap wrap)
		{
			if (wrap == UiWrap::Wrap && std::isfinite(availableMain))
				return balancedFlexLineBreaks(items, availableMain, itemGap);
			Vector<SizeT> breaks;
			breaks.push_back(0);
			breaks.push_back(items.size());
			return breaks;
		}

		[[nodiscard]] Vector<FlexLine> buildFlexLinesFromBreaks(
			Vector<FlexItem>&& items, const Vector<SizeT>& breaks,
			const Float itemGap, const UiAxis axis)
		{
			Vector<FlexLine> lines;
			for (SizeT lineIndex = 0; lineIndex + 1 < breaks.size(); ++lineIndex)
			{
				FlexLine line;
				for (SizeT index = breaks[lineIndex]; index < breaks[lineIndex + 1]; ++index)
					appendFlexItemToLine(line, std::move(items[index]), itemGap, axis);
				lines.push_back(std::move(line));
			}
			return lines;
		}

		void justifyOffsets(const UiJustify justify, const Float freeSpace, const SizeT count,
			Float& leading, Float& between)
		{
			leading = 0;
			between = 0;
			const Float free = std::max(0.0f, freeSpace);
			switch (justify)
			{
			case UiJustify::Center: leading = free * 0.5f; break;
			case UiJustify::End: leading = free; break;
			case UiJustify::SpaceBetween: if (count > 1) between = free / static_cast<Float>(count - 1); break;
			case UiJustify::SpaceAround:
				if (count) { between = free / static_cast<Float>(count); leading = between * 0.5f; }
				break;
			case UiJustify::SpaceEvenly:
				if (count) { between = free / static_cast<Float>(count + 1); leading = between; }
				break;
			default: break;
			}
		}

		UiJustify alignmentDistribution(const UiAlign alignment)
		{
			switch (alignment)
			{
			case UiAlign::Center: return UiJustify::Center;
			case UiAlign::End: return UiJustify::End;
			default: return UiJustify::Start;
			}
		}
	}

	UiConstraints UiConstraints::tight(const Vec2 size)
	{
		const Vec2 safe = glm::max(size, Vec2(0));
		return { safe, safe };
	}

	UiConstraints UiConstraints::loose(const Vec2 maximum)
	{
		return UiConstraints{ Vec2(0), maximum }.normalized();
	}

	UiConstraints UiConstraints::normalized() const
	{
		UiConstraints result = *this;
		for (Int axis = 0; axis < 2; ++axis)
		{
			result.minimum[axis] = nonNegative(result.minimum[axis]);
			if (std::isnan(result.maximum[axis])) result.maximum[axis] = result.minimum[axis];
			else if (std::isfinite(result.maximum[axis])) result.maximum[axis] = std::max(result.minimum[axis], result.maximum[axis]);
			else result.maximum[axis] = uiLayoutInfinity;
		}
		return result;
	}

	Vec2 UiConstraints::constrain(const Vec2 size) const
	{
		const UiConstraints safe = normalized();
		Vec2 result = size;
		for (Int axis = 0; axis < 2; ++axis)
		{
			if (!std::isfinite(result[axis])) result[axis] = safe.minimum[axis];
			result[axis] = std::max(safe.minimum[axis], result[axis]);
			if (std::isfinite(safe.maximum[axis])) result[axis] = std::min(safe.maximum[axis], result[axis]);
		}
		return result;
	}

	Bool UiConstraints::hasBoundedWidth() const { return std::isfinite(maximum.x); }
	Bool UiConstraints::hasBoundedHeight() const { return std::isfinite(maximum.y); }
	Bool UiConstraints::operator==(const UiConstraints& other) const
	{
		return minimum.x == other.minimum.x && minimum.y == other.minimum.y &&
			maximum.x == other.maximum.x && maximum.y == other.maximum.y;
	}

	UiRect UiRect::intersected(const UiRect& other) const
	{
		const Vec2 start = glm::max(position, other.position);
		const Vec2 finish = glm::min(end(), other.end());
		return { start, glm::max(finish - start, Vec2(0)) };
	}

	UiLayoutEngine::UiLayoutEngine(UiDocument& newDocument)
		: document(newDocument)
	{
		states.reserve(UiDocument::maxNodes);
	}

	UiLayoutEngine::NodeState& UiLayoutEngine::stateFor(const UiNodeHandle node)
	{
		for (NodeState& state : states) if (state.handle == node) return state;
		states.push_back({});
		states.back().handle = node;
		return states.back();
	}

	const UiLayoutEngine::NodeState* UiLayoutEngine::findState(const UiNodeHandle node) const
	{
		for (const NodeState& state : states) if (state.handle == node) return &state;
		return nullptr;
	}

	void UiLayoutEngine::setStyle(const UiNodeHandle node, const UiLayoutStyle& newStyle)
	{
		if (!document.isValid(node)) return;
		NodeState& state = stateFor(node);
		UiLayoutStyle style = newStyle;
		if (const UiNode* uiNode = document.get(node))
			applyFlexClasses(uiNode->authored.classes, style);
		state.style = style;
		++state.styleRevision;
		state.hasCachedMeasure = false;
		state.hasCachedArrange = false;
		document.markDirty(node, DirtyFlag::Measure);
	}

	const UiLayoutStyle& UiLayoutEngine::style(const UiNodeHandle node) const
	{
		const NodeState* state = findState(node);
		return state ? state->style : defaultStyle;
	}

	UiLayoutStyle UiLayoutEngine::resolvedStyle(const UiNodeHandle node) const
	{
		UiLayoutStyle resolved = findState(node) ? findState(node)->style : defaultStyle;
		if (const UiNode* uiNode = document.get(node))
		{
			applyFlexClasses(uiNode->authored.classes, resolved);
			applyControlLayoutDefaults(*uiNode, resolved);
		}
		return resolved;
	}

	void UiLayoutEngine::clearStyle(const UiNodeHandle node)
	{
		std::erase_if(states, [&](const NodeState& state) { return state.handle == node; });
		if (document.isValid(node)) document.markDirty(node, DirtyFlag::Measure);
	}

	void UiLayoutEngine::setIntrinsicMeasurer(const UiNodeHandle node, UiIntrinsicMeasurer measurer)
	{
		setOwnedIntrinsicMeasurer(node, std::move(measurer), 0);
	}

	void UiLayoutEngine::setOwnedIntrinsicMeasurer(
		const UiNodeHandle node, UiIntrinsicMeasurer measurer, const U64 owner)
	{
		if (!document.isValid(node)) return;
		NodeState& state = stateFor(node);
		state.intrinsicMeasurer = std::move(measurer);
		state.intrinsicMeasurerOwner = owner;
		++state.contentRevision;
		state.hasCachedMeasure = false;
			state.hasCachedArrange = false;
		document.markDirty(node, DirtyFlag::Measure);
	}

	void UiLayoutEngine::clearIntrinsicMeasurer(const UiNodeHandle node)
	{
		if (NodeState* state = const_cast<NodeState*>(findState(node)))
		{
			state->intrinsicMeasurer = {};
			++state->contentRevision;
			state->hasCachedMeasure = false;
			state->hasCachedArrange = false;
		}
		if (document.isValid(node)) document.markDirty(node, DirtyFlag::Measure);
	}

	Bool UiLayoutEngine::hasIntrinsicMeasurer(const UiNodeHandle node) const
	{
		const NodeState* state = findState(node);
		return state && static_cast<Bool>(state->intrinsicMeasurer);
	}

	U64 UiLayoutEngine::intrinsicMeasurerOwner(const UiNodeHandle node) const
	{
		const NodeState* state = findState(node);
		return state && state->intrinsicMeasurer ? state->intrinsicMeasurerOwner : 0;
	}

	void UiLayoutEngine::invalidateContent(const UiNodeHandle node)
	{
		if (!document.isValid(node)) return;
		NodeState& state = stateFor(node);
		++state.contentRevision;
		state.hasCachedMeasure = false;
		state.hasCachedArrange = false;
		document.markDirty(node, DirtyFlag::Measure);
	}

	void UiLayoutEngine::diagnose(const UiLayoutDiagnosticCode code, const UiNodeHandle node, String message)
	{
		if (layoutDiagnostics.size() >= maximumDiagnostics) return;
		layoutDiagnostics.push_back({ code, node, std::move(message) });
	}

	Float UiLayoutEngine::resolve(const UiSizeSpec& spec, const Float reference, const Float intrinsic,
		const UiNodeHandle node, const Bool percentageReferenceIsBounded)
	{
		switch (spec.kind)
		{
		case UiSizeKind::Fixed: return nonNegative(spec.value);
		case UiSizeKind::Percent:
			if (percentageReferenceIsBounded && std::isfinite(reference))
				return nonNegative(reference * spec.value);
			diagnose(UiLayoutDiagnosticCode::PercentageCycle, node,
				"Percentage size has no bounded containing size; intrinsic fallback was used.");
			return nonNegative(intrinsic);
		case UiSizeKind::Content:
		case UiSizeKind::Auto: return nonNegative(intrinsic);
		case UiSizeKind::Fr: return nonNegative(intrinsic);
		}
		return nonNegative(intrinsic);
	}

	void UiLayoutEngine::clearLayoutDirty(const UiNodeHandle node, const DirtyFlag flag)
	{
		if (UiNode* item = document.get(node)) item->dirtyFlags &= ~dirty(flag);
	}

	void UiLayoutEngine::clearHiddenSubtree(const UiNodeHandle node)
	{
		UiNode* item = document.get(node);
		if (!item) return;
		item->computed.measuredSize = Vec2(0);
		item->computed.position = Vec2(0);
		item->computed.size = Vec2(0);
		item->computed.contentSize = Vec2(0);
		item->computed.scrollExtent = Vec2(0);
		item->computed.clipPosition = Vec2(0);
		item->computed.clipSize = Vec2(0);
		item->computed.baseline = -1;
		item->computed.layoutRevision = layoutRevision;
		item->dirtyFlags &= ~(dirty(DirtyFlag::Measure) | dirty(DirtyFlag::Arrange));
		if (NodeState* state = const_cast<NodeState*>(findState(node))) state->hasCachedArrange = false;
		for (const UiNodeHandle child : item->childHandles()) clearHiddenSubtree(child);
	}

	UiIntrinsicMeasure UiLayoutEngine::measureNode(
		const UiNodeHandle node, const UiConstraints& rawConstraints, const SizeT depth)
	{
		UiNode* item = document.get(node);
		if (!item) return {};
		if (depth > maximumLayoutDepth)
		{
			diagnose(UiLayoutDiagnosticCode::DepthLimit, node, "Layout depth limit reached; zero intrinsic size was used.");
			return {};
		}
		const UiLayoutStyle nodeStyle = resolvedStyle(node);
		if (nodeStyle.display == UiDisplay::None)
		{
			clearHiddenSubtree(node);
			return {};
		}
		if (std::ranges::find(measureStack, node) != measureStack.end())
		{
			diagnose(UiLayoutDiagnosticCode::IntrinsicCycle, node,
				"Recursive intrinsic measurement detected; last stable size was used.");
			return { Vec2(0), glm::max(item->computed.measuredSize, Vec2(0)), item->computed.baseline };
		}

		const UiConstraints constraints = rawConstraints.normalized();
		NodeState& state = stateFor(node);
		if (!hasDirty(item->dirtyFlags, DirtyFlag::Measure) && state.hasCachedMeasure &&
			state.cachedConstraints == constraints && state.cachedStyleRevision == state.styleRevision &&
			state.cachedContentRevision == state.contentRevision)
			return state.cachedMeasure;

		measureStack.push_back(node);
		const UiConstraints innerConstraints = insetConstraints(constraints, nodeStyle.padding.total());
		UiIntrinsicMeasure content;
		if (state.intrinsicMeasurer) content = state.intrinsicMeasurer(*item, innerConstraints);

		UiIntrinsicMeasure children;
		switch (nodeStyle.display)
		{
		case UiDisplay::Flex: children = measureFlex(node, innerConstraints, depth + 1); break;
		case UiDisplay::Grid: children = measureGrid(node, innerConstraints, depth + 1); break;
		case UiDisplay::Overlay: children = measureOverlay(node, innerConstraints, depth + 1); break;
		default: break;
		}
		content.minimum = glm::max(content.minimum, children.minimum);
		content.preferred = glm::max(content.preferred, children.preferred);
		if (content.baseline < 0) content.baseline = children.baseline;

		// CSS content-box: width/height apply to the content area; padding is additive.
		const UiEdges& padding = nodeStyle.padding;
		const Float maxContentWidth = constraints.hasBoundedWidth() ?
			glm::max(constraints.maximum.x - padding.horizontal(), 0.0f) : constraints.maximum.x;
		const Float maxContentHeight = constraints.hasBoundedHeight() ?
			glm::max(constraints.maximum.y - padding.vertical(), 0.0f) : constraints.maximum.y;

		Vec2 contentPreferred = content.preferred;
		contentPreferred.x = resolve(nodeStyle.width, maxContentWidth, contentPreferred.x, node, constraints.hasBoundedWidth());
		contentPreferred.y = resolve(nodeStyle.height, maxContentHeight, contentPreferred.y, node, constraints.hasBoundedHeight());

		if (nodeStyle.aspectRatio > 0 && std::isfinite(nodeStyle.aspectRatio))
		{
			const Bool widthExplicit = nodeStyle.width.kind == UiSizeKind::Fixed || nodeStyle.width.kind == UiSizeKind::Percent;
			const Bool heightExplicit = nodeStyle.height.kind == UiSizeKind::Fixed || nodeStyle.height.kind == UiSizeKind::Percent;
			if (widthExplicit && !heightExplicit) contentPreferred.y = contentPreferred.x / nodeStyle.aspectRatio;
			else if (heightExplicit && !widthExplicit) contentPreferred.x = contentPreferred.y * nodeStyle.aspectRatio;
		}

		const Float minimumContentWidth = nodeStyle.minimumWidth.kind == UiSizeKind::Auto ? content.minimum.x :
			resolve(nodeStyle.minimumWidth, maxContentWidth, content.minimum.x, node, constraints.hasBoundedWidth());
		const Float minimumContentHeight = nodeStyle.minimumHeight.kind == UiSizeKind::Auto ? content.minimum.y :
			resolve(nodeStyle.minimumHeight, maxContentHeight, content.minimum.y, node, constraints.hasBoundedHeight());
		const Float maximumContentWidth = nodeStyle.maximumWidth.kind == UiSizeKind::Auto ? uiLayoutInfinity :
			resolve(nodeStyle.maximumWidth, maxContentWidth, contentPreferred.x, node, constraints.hasBoundedWidth());
		const Float maximumContentHeight = nodeStyle.maximumHeight.kind == UiSizeKind::Auto ? uiLayoutInfinity :
			resolve(nodeStyle.maximumHeight, maxContentHeight, contentPreferred.y, node, constraints.hasBoundedHeight());
		contentPreferred = glm::max(contentPreferred, Vec2(minimumContentWidth, minimumContentHeight));
		contentPreferred = glm::min(contentPreferred, Vec2(maximumContentWidth, maximumContentHeight));

		Vec2 preferred = contentPreferred + padding.total();
		preferred = constraints.constrain(preferred);

		UiIntrinsicMeasure result;
		result.minimum = constraints.constrain(content.minimum + nodeStyle.padding.total());
		result.preferred = preferred;
		result.baseline = content.baseline < 0 ? -1 : content.baseline + nodeStyle.padding.top;
		item->computed.measuredSize = result.preferred;
		item->computed.baseline = result.baseline;
		state.cachedConstraints = constraints;
		state.cachedMeasure = result;
		state.cachedStyleRevision = state.styleRevision;
		state.cachedContentRevision = state.contentRevision;
		state.hasCachedMeasure = true;
		measureStack.pop_back();
		clearLayoutDirty(node, DirtyFlag::Measure);
		return result;
	}

	UiIntrinsicMeasure UiLayoutEngine::measureFlex(
		const UiNodeHandle node, const UiConstraints& constraints, const SizeT depth)
	{
		const UiNode* parent = document.get(node);
		if (!parent) return {};
		const UiLayoutStyle parentStyle = resolvedStyle(node);
		const UiAxis axis = parentStyle.flexDirection;
		const Float availableMain = mainValue(constraints.maximum, axis);
		const Float itemGap = axis == UiAxis::Row ? parentStyle.gap.x : parentStyle.gap.y;
		const Float lineGap = axis == UiAxis::Row ? parentStyle.gap.y : parentStyle.gap.x;
		Float baseline = -1;

		const UiConstraints childMeasureConstraints =
			measureConstraintsForFlexChild(parentStyle, constraints.maximum, axis);
		Vector<FlexWrapBreakItem> breakItems;
		for (const UiNodeHandle child : parent->childHandles())
		{
			const UiLayoutStyle childStyle = resolvedStyle(child);
			if (childStyle.display == UiDisplay::None || childStyle.position == UiPositionType::Absolute) continue;
			const UiIntrinsicMeasure measured = measureNode(child, childMeasureConstraints, depth);
			const Float childBasis = childStyle.flexBasis.kind == UiSizeKind::Auto ?
				mainValue(measured.preferred, axis) :
				resolve(childStyle.flexBasis, availableMain, mainValue(measured.preferred, axis), child,
					std::isfinite(availableMain));
			const UiSizeSpec& minimumMainSpec = axis == UiAxis::Row ? childStyle.minimumWidth : childStyle.minimumHeight;
			const UiSizeSpec& maximumMainSpec = axis == UiAxis::Row ? childStyle.maximumWidth : childStyle.maximumHeight;
			Float minimumMain = 0;
			if (minimumMainSpec.kind == UiSizeKind::Fixed && minimumMainSpec.value == 0)
				minimumMain = 0;
			else if (minimumMainSpec.kind != UiSizeKind::Auto)
				minimumMain = resolve(minimumMainSpec, availableMain, 0, child, std::isfinite(availableMain));
			else
				minimumMain = flexItemMinimumMainAuto(childStyle, measured, axis);
			Float maximumMain = uiLayoutInfinity;
			if (maximumMainSpec.kind != UiSizeKind::Auto)
				maximumMain = resolve(maximumMainSpec, availableMain, childBasis, child, std::isfinite(availableMain));
			const Float hypotheticalMain = hypotheticalFlexMain(childBasis, minimumMain, maximumMain);
			FlexWrapBreakItem breakItem;
			breakItem.outerMain = hypotheticalMain + mainStart(childStyle.margin, axis) +
				mainEnd(childStyle.margin, axis);
			breakItem.minimumOuterMain = minimumMain + mainStart(childStyle.margin, axis) +
				mainEnd(childStyle.margin, axis);
			breakItem.cross = crossValue(measured.preferred, axis) + crossStart(childStyle.margin, axis) +
				crossEnd(childStyle.margin, axis);
			breakItem.minimumCross = crossValue(measured.minimum, axis) + crossStart(childStyle.margin, axis) +
				crossEnd(childStyle.margin, axis);
			breakItem.basis = childBasis;
			breakItem.grow = nonNegative(childStyle.flexGrow);
			breakItems.push_back(breakItem);
			if (baseline < 0 && measured.baseline >= 0) baseline = measured.baseline + childStyle.margin.top;
		}

		const Vector<SizeT> breaks = flexLineBreaks(
			breakItems, availableMain, itemGap, parentStyle.flexWrap);
		Float widest = 0;
		Float totalCross = 0;
		Float minimumMain = 0;
		Float minimumCross = 0;
		for (SizeT lineIndex = 0; lineIndex + 1 < breaks.size(); ++lineIndex)
		{
			Float lineMain = 0;
			Float lineCross = 0;
			Float lineMinimumMain = 0;
			Float lineMinimumCross = 0;
			for (SizeT index = breaks[lineIndex]; index < breaks[lineIndex + 1]; ++index)
			{
				if (index > breaks[lineIndex])
				{
					lineMain += itemGap;
					lineMinimumMain += itemGap;
				}
				lineMain += breakItems[index].outerMain;
				lineMinimumMain += breakItems[index].minimumOuterMain;
				lineCross = std::max(lineCross, breakItems[index].cross);
				lineMinimumCross = std::max(lineMinimumCross, breakItems[index].minimumCross);
			}
			widest = std::max(widest, lineMain);
			minimumMain = std::max(minimumMain, lineMinimumMain);
			totalCross += lineCross;
			minimumCross += lineMinimumCross;
			if (lineIndex > 0)
			{
				totalCross += lineGap;
				minimumCross += lineGap;
			}
		}
		Vec2 preferred(0);
		setMain(preferred, axis, widest);
		setCross(preferred, axis, totalCross);
		Vec2 minimum(0);
		setMain(minimum, axis, minimumMain);
		setCross(minimum, axis, minimumCross);
		return { minimum, preferred, baseline };
	}

	Vector<UiLayoutEngine::GridPlacement> UiLayoutEngine::placeGridItems(
		const UiNodeHandle node, UInt& columns, UInt& rows)
	{
		const UiNode* parent = document.get(node);
		const UiLayoutStyle parentStyle = resolvedStyle(node);
		columns = std::max<UInt>(1, static_cast<UInt>(parentStyle.gridColumns.size()));
		rows = std::max<UInt>(1, static_cast<UInt>(parentStyle.gridRows.size()));
		columns = std::min<UInt>(columns, static_cast<UInt>(maximumGridTracks));
		rows = std::min<UInt>(rows, static_cast<UInt>(maximumGridTracks));
		Vector<GridPlacement> result;
		Vector<Bool> occupied(columns * rows, false);

		auto ensureRows = [&](const UInt wanted)
			{
				const UInt safe = std::min<UInt>(wanted, static_cast<UInt>(maximumGridTracks));
				if (safe > rows) { occupied.resize(columns * safe, false); rows = safe; }
			};
		auto fits = [&](const UInt column, const UInt row, const UInt columnSpan, const UInt rowSpan)
			{
				if (column + columnSpan > columns) return false;
				ensureRows(row + rowSpan);
				if (row + rowSpan > rows) return false;
				for (UInt y = row; y < row + rowSpan; ++y)
					for (UInt x = column; x < column + columnSpan; ++x)
						if (occupied[y * columns + x]) return false;
				return true;
			};
		auto occupy = [&](const GridPlacement& placement)
			{
				for (UInt y = placement.row; y < placement.row + placement.rowSpan; ++y)
					for (UInt x = placement.column; x < placement.column + placement.columnSpan; ++x)
						occupied[y * columns + x] = true;
			};

		if (!parent) return result;
		for (const UiNodeHandle child : parent->childHandles())
		{
			const UiLayoutStyle childStyle = resolvedStyle(child);
			if (childStyle.display == UiDisplay::None || childStyle.position == UiPositionType::Absolute) continue;
			GridPlacement placement{ child, 0, 0,
				std::max<UInt>(1, childStyle.gridColumnSpan), std::max<UInt>(1, childStyle.gridRowSpan) };
			placement.columnSpan = std::min(placement.columnSpan, columns);
			if (childStyle.gridColumn >= 0 && childStyle.gridRow >= 0)
			{
				placement.column = std::min<UInt>(static_cast<UInt>(childStyle.gridColumn), columns - 1);
				placement.row = static_cast<UInt>(childStyle.gridRow);
				if (!fits(placement.column, placement.row, placement.columnSpan, placement.rowSpan))
					diagnose(UiLayoutDiagnosticCode::GridPlacementOverflow, child,
						"Explicit grid placement overlapped or exceeded the track budget; deterministic auto placement was used.");
				else { occupy(placement); result.push_back(placement); continue; }
			}
			Bool placed = false;
			for (UInt row = 0; row < maximumGridTracks && !placed; ++row)
			{
				ensureRows(row + placement.rowSpan);
				for (UInt column = 0; column < columns; ++column)
					if (fits(column, row, placement.columnSpan, placement.rowSpan))
					{
						placement.column = column;
						placement.row = row;
						occupy(placement);
						result.push_back(placement);
						placed = true;
						break;
					}
			}
			if (!placed) diagnose(UiLayoutDiagnosticCode::GridPlacementOverflow, child,
				"Grid auto-placement track budget was exhausted; item was omitted.");
		}
		return result;
	}

	UiIntrinsicMeasure UiLayoutEngine::measureGrid(
		const UiNodeHandle node, const UiConstraints& constraints, const SizeT depth)
	{
		UInt columns = 1, rows = 1;
		const Vector<GridPlacement> placements = placeGridItems(node, columns, rows);
		Vector<Float> columnSizes(columns, 0), rowSizes(rows, 0);
		for (const GridPlacement& placement : placements)
		{
			const UiIntrinsicMeasure measured = measureNode(placement.node, UiConstraints::loose(constraints.maximum), depth);
			const UiLayoutStyle childStyle = resolvedStyle(placement.node);
			const Float width = measured.preferred.x + childStyle.margin.horizontal();
			const Float height = measured.preferred.y + childStyle.margin.vertical();
			const Float columnShare = width / static_cast<Float>(placement.columnSpan);
			const Float rowShare = height / static_cast<Float>(placement.rowSpan);
			for (UInt x = placement.column; x < placement.column + placement.columnSpan; ++x) columnSizes[x] = std::max(columnSizes[x], columnShare);
			for (UInt y = placement.row; y < placement.row + placement.rowSpan; ++y) rowSizes[y] = std::max(rowSizes[y], rowShare);
		}
		const UiLayoutStyle parentStyle = resolvedStyle(node);
		auto applyIntrinsicTracks = [&](Vector<Float>& sizes, const Vector<UiGridTrack>& tracks,
			const Float reference, const Bool bounded)
			{
				for (UInt i = 0; i < sizes.size() && i < tracks.size(); ++i)
				{
					const UiGridTrack& track = tracks[i];
					if (track.size.kind == UiSizeKind::Fixed) sizes[i] = nonNegative(track.size.value);
					else if (track.size.kind == UiSizeKind::Percent && bounded) sizes[i] = nonNegative(reference * track.size.value);
					const Float minimum = track.minimum.kind == UiSizeKind::Auto ? 0 :
						resolve(track.minimum, reference, 0, node, bounded);
					const Float maximum = track.maximum.kind == UiSizeKind::Auto ? uiLayoutInfinity :
						resolve(track.maximum, reference, sizes[i], node, bounded);
					sizes[i] = std::min(std::max(sizes[i], minimum), maximum);
				}
			};
		applyIntrinsicTracks(columnSizes, parentStyle.gridColumns, constraints.maximum.x, constraints.hasBoundedWidth());
		applyIntrinsicTracks(rowSizes, parentStyle.gridRows, constraints.maximum.y, constraints.hasBoundedHeight());
		const Float width = std::accumulate(columnSizes.begin(), columnSizes.end(), 0.0f) + parentStyle.gap.x * static_cast<Float>(columns - 1);
		const Float height = std::accumulate(rowSizes.begin(), rowSizes.end(), 0.0f) + parentStyle.gap.y * static_cast<Float>(rows - 1);
		return { Vec2(0), Vec2(width, height), -1 };
	}

	UiIntrinsicMeasure UiLayoutEngine::measureOverlay(
		const UiNodeHandle node, const UiConstraints& constraints, const SizeT depth)
	{
		const UiNode* parent = document.get(node);
		UiIntrinsicMeasure result;
		if (!parent) return result;
		for (const UiNodeHandle child : parent->childHandles())
		{
			const UiLayoutStyle childStyle = resolvedStyle(child);
			if (childStyle.display == UiDisplay::None || childStyle.position == UiPositionType::Absolute) continue;
			const UiIntrinsicMeasure measured = measureNode(child, UiConstraints::loose(constraints.maximum), depth);
			result.preferred = glm::max(result.preferred, measured.preferred + childStyle.margin.total());
			result.minimum = glm::max(result.minimum, measured.minimum + childStyle.margin.total());
			if (result.baseline < 0) result.baseline = measured.baseline;
		}
		return result;
	}

	void UiLayoutEngine::layout()
	{
		const Vec2 viewportSize = glm::max(Vec2(document.viewport.logicalSize), Vec2(0));
		layout(UiConstraints::tight(viewportSize));
	}

	void UiLayoutEngine::layout(const UiConstraints& rootConstraints)
	{
		layoutDiagnostics.clear();
		measureStack.clear();
		std::erase_if(states, [&](const NodeState& state) { return !document.isValid(state.handle); });
		++layoutRevision;
		if (layoutRevision == 0) ++layoutRevision;
		const UiIntrinsicMeasure rootMeasure = measureNode(document.root(), rootConstraints, 0);
		const UiRect rootRect{ Vec2(0), rootConstraints.constrain(rootMeasure.preferred) };
		arrangeNode(document.root(), rootRect, rootRect, 0);
	}

	void UiLayoutEngine::arrangeNode(
		const UiNodeHandle node, const UiRect& rawRect, const UiRect& inheritedClip, const SizeT depth)
	{
		UiNode* item = document.get(node);
		if (!item) return;
		if (depth > maximumLayoutDepth)
		{
			diagnose(UiLayoutDiagnosticCode::DepthLimit, node, "Arrange depth limit reached; descendants were omitted.");
			return;
		}
		const UiLayoutStyle nodeStyle = resolvedStyle(node);
		if (nodeStyle.display == UiDisplay::None) { clearHiddenSubtree(node); return; }
		const UiRect rect{ rawRect.position, glm::max(rawRect.size, Vec2(0)) };
		const UiRect contentRect{
			rect.position + nodeStyle.padding.topLeft(),
			glm::max(rect.size - nodeStyle.padding.total(), Vec2(0))
		};
		UiRect clip = inheritedClip;
		NodeState& nodeState = stateFor(node);
		if (!hasDirty(item->dirtyFlags, DirtyFlag::Arrange) && nodeState.hasCachedArrange &&
			equalRect(nodeState.cachedRect, rect) &&
			equalRect(nodeState.cachedInheritedClip, inheritedClip) &&
			nodeState.arrangedStyleRevision == nodeState.styleRevision &&
			nodeState.arrangedContentRevision == nodeState.contentRevision &&
			equalVec2(nodeState.cachedScrollOffset, item->computed.scroll.offset) &&
			nodeState.cachedVerticalGutter == item->computed.scroll.verticalGutter &&
			nodeState.cachedHorizontalGutter == item->computed.scroll.horizontalGutter)
			return;

		const Bool changed = item->computed.position != rect.position || item->computed.size != rect.size;
		item->computed.position = rect.position;
		item->computed.size = rect.size;
		item->computed.contentPosition = contentRect.position;
		item->computed.contentSize = contentRect.size;
		item->computed.layoutRevision = layoutRevision;
		if (changed) item->dirtyFlags |= dirty(DirtyFlag::Paint) | dirty(DirtyFlag::HitTest);

		const Vec2 scrollOffset = item->computed.scroll.offset;
		const UiRect scrollport = scrollportRect(
			contentRect,
			item->computed.scroll.verticalGutter,
			item->computed.scroll.horizontalGutter);
		const UiRect flowRect{
			scrollport.position - scrollOffset,
			scrollport.size
		};

		// Clip descendants to this scrollport before laying out children. Extent is
		// not known yet, so assume unbounded content for auto overflow.
		UiRect childClip = clip;
		intersectClipAxis(childClip, scrollport, UiAxis::Row,
			scrollClipPolicy(nodeStyle.overflowX, nodeStyle.overflowY), uiLayoutInfinity, scrollport.size.x);
		intersectClipAxis(childClip, scrollport, UiAxis::Column,
			scrollClipPolicy(nodeStyle.overflowY, nodeStyle.overflowX), uiLayoutInfinity, scrollport.size.y);

		switch (nodeStyle.display)
		{
		case UiDisplay::Flex: arrangeFlex(node, flowRect, childClip, depth + 1); break;
		case UiDisplay::Grid: arrangeGrid(node, flowRect, childClip, depth + 1); break;
		case UiDisplay::Overlay: arrangeOverlay(node, flowRect, childClip, depth + 1); break;
		default: break;
		}
		for (const UiNodeHandle child : item->childHandles())
			if (resolvedStyle(child).display != UiDisplay::None &&
				resolvedStyle(child).position == UiPositionType::Absolute)
				arrangeAbsolute(child, flowRect, childClip, depth + 1);

		Vec2 extent = scrollport.size;
		for (const UiNodeHandle child : item->childHandles())
			if (const UiNode* childNode = document.get(child))
				extent = glm::max(extent,
					childNode->computed.position + childNode->computed.size -
					scrollport.position + scrollOffset);
		item->computed.scrollExtent = glm::max(extent, Vec2(0));

		intersectClipAxis(clip, scrollport, UiAxis::Row,
			scrollClipPolicy(nodeStyle.overflowX, nodeStyle.overflowY),
			item->computed.scrollExtent.x, scrollport.size.x);
		intersectClipAxis(clip, scrollport, UiAxis::Column,
			scrollClipPolicy(nodeStyle.overflowY, nodeStyle.overflowX),
			item->computed.scrollExtent.y, scrollport.size.y);
		const Bool clipChanged = !equalRect(nodeState.cachedClip, clip);
		item->computed.clipPosition = clip.position;
		item->computed.clipSize = clip.size;
		if (clipChanged) item->dirtyFlags |= dirty(DirtyFlag::Paint);

		nodeState.cachedRect = rect;
		nodeState.cachedClip = clip;
		nodeState.cachedInheritedClip = inheritedClip;
		nodeState.cachedScrollOffset = scrollOffset;
		nodeState.cachedVerticalGutter = item->computed.scroll.verticalGutter;
		nodeState.cachedHorizontalGutter = item->computed.scroll.horizontalGutter;
		nodeState.arrangedStyleRevision = nodeState.styleRevision;
		nodeState.arrangedContentRevision = nodeState.contentRevision;
		nodeState.hasCachedArrange = true;
		clearLayoutDirty(node, DirtyFlag::Arrange);
	}

	void UiLayoutEngine::arrangeFlex(
		const UiNodeHandle node, const UiRect& contentRect, const UiRect& clip, const SizeT depth)
	{
		const UiNode* parent = document.get(node);
		if (!parent) return;
		const UiLayoutStyle parentStyle = resolvedStyle(node);
		const UiAxis axis = parentStyle.flexDirection;
		const Float availableMain = mainValue(contentRect.size, axis);
		const Float availableCross = crossValue(contentRect.size, axis);
		const Bool scrollMainAxis = scrollsMainAxis(parentStyle, axis);
		const Float itemGap = axis == UiAxis::Row ? parentStyle.gap.x : parentStyle.gap.y;
		const Float lineGap = axis == UiAxis::Row ? parentStyle.gap.y : parentStyle.gap.x;
		const UiConstraints childMeasureConstraints =
			measureConstraintsForFlexChild(parentStyle, contentRect.size, axis);
		Vector<FlexItem> flexItems;
		Vector<FlexWrapBreakItem> breakItems;
		for (const UiNodeHandle child : parent->childHandles())
		{
			const UiLayoutStyle childStyle = resolvedStyle(child);
			if (childStyle.display == UiDisplay::None || childStyle.position == UiPositionType::Absolute) continue;
			const UiIntrinsicMeasure measured = measureNode(child, childMeasureConstraints, depth);
			FlexItem flexItem;
			flexItem.node = child;
			flexItem.measured = measured.preferred;
			flexItem.margin = childStyle.margin;
			flexItem.basis = childStyle.flexBasis.kind == UiSizeKind::Auto ?
				mainValue(measured.preferred, axis) :
				resolve(childStyle.flexBasis, availableMain, mainValue(measured.preferred, axis), child, true);
			flexItem.main = flexItem.basis;
			flexItem.cross = crossValue(measured.preferred, axis);
			flexItem.grow = nonNegative(childStyle.flexGrow);
			flexItem.shrink = scrollMainAxis ? 0 : nonNegative(childStyle.flexShrink);
			const UiSizeSpec& minimumMainSpec = axis == UiAxis::Row ? childStyle.minimumWidth : childStyle.minimumHeight;
			const UiSizeSpec& maximumMainSpec = axis == UiAxis::Row ? childStyle.maximumWidth : childStyle.maximumHeight;
			if (minimumMainSpec.kind == UiSizeKind::Fixed && minimumMainSpec.value == 0)
				flexItem.minimumMain = 0;
			else if (minimumMainSpec.kind != UiSizeKind::Auto)
				flexItem.minimumMain = resolve(minimumMainSpec, availableMain, 0, child, true);
			else
				flexItem.minimumMain = flexItemMinimumMainAuto(childStyle, measured, axis);
			if (maximumMainSpec.kind != UiSizeKind::Auto)
				flexItem.maximumMain = resolve(maximumMainSpec, availableMain, flexItem.main, child, true);
			flexItem.baseline = measured.baseline;
			FlexWrapBreakItem breakItem;
			breakItem.outerMain = flexItemOuterMain(flexItem, axis);
			breakItem.cross = flexItem.cross + crossStart(flexItem.margin, axis) + crossEnd(flexItem.margin, axis);
			breakItem.baseline = flexItem.baseline + crossStart(flexItem.margin, axis);
			breakItem.basis = flexItem.basis;
			breakItem.grow = flexItem.grow;
			breakItems.push_back(breakItem);
			flexItems.push_back(flexItem);
		}

		const Vector<SizeT> breaks = flexLineBreaks(
			breakItems, availableMain, itemGap, parentStyle.flexWrap);
		Vector<FlexLine> lines = buildFlexLinesFromBreaks(std::move(flexItems), breaks, itemGap, axis);

		// Column flex defaults to align-items: stretch. Lines must span the container cross
		// size so row-direction children (token grids) receive the full available width.
		if (axis == UiAxis::Column && parentStyle.alignItems == UiAlign::Stretch &&
			std::isfinite(availableCross) && availableCross > 0)
		{
			for (FlexLine& line : lines)
				line.cross = std::max(line.cross, availableCross);
		}

		Float totalCross = 0;
		for (const FlexLine& line : lines) totalCross += line.cross;
		if (lines.size() > 1) totalCross += lineGap * static_cast<Float>(lines.size() - 1);
		if (parentStyle.alignContent == UiAlign::Stretch && !lines.empty() && availableCross > totalCross)
		{
			const Float addition = (availableCross - totalCross) / static_cast<Float>(lines.size());
			for (FlexLine& line : lines) line.cross += addition;
			totalCross = availableCross;
		}
		Float crossLeading = 0, crossBetween = lineGap;
		Float extraBetween = 0;
		justifyOffsets(alignmentDistribution(parentStyle.alignContent),
			availableCross - totalCross, lines.size(), crossLeading, extraBetween);
		crossBetween += extraBetween;
		Float lineOffset = crossLeading;

		for (FlexLine& line : lines)
		{
			Float growTotal = 0, shrinkTotal = 0;
			for (const FlexItem& flexItem : line.items)
			{
				growTotal += flexItem.grow;
				shrinkTotal += flexItem.shrink * flexItem.basis;
			}
			const Float lineFlexBaseMain = flexLineFlexBasePackedMain(
				line.items, 0, line.items.size(), itemGap, axis);
			const Float flexibleSpace = availableMain - lineFlexBaseMain;
			for (FlexItem& flexItem : line.items)
			{
				if (flexibleSpace > 0 && growTotal > 0)
					flexItem.main += flexibleSpace * flexItem.grow / growTotal;
				else if (flexibleSpace < 0 && shrinkTotal > 0 && !scrollMainAxis)
				{
					flexItem.main += flexibleSpace * (flexItem.shrink * flexItem.basis) / shrinkTotal;
					flexItem.main = std::max(flexItem.minimumMain, flexItem.main);
				}
				flexItem.main = std::min(std::max(flexItem.main, flexItem.minimumMain), flexItem.maximumMain);
			}
			Float usedMain = itemGap * static_cast<Float>(line.items.empty() ? 0 : line.items.size() - 1);
			for (const FlexItem& flexItem : line.items)
				usedMain += flexItem.main + mainStart(flexItem.margin, axis) + mainEnd(flexItem.margin, axis);
			Float leading = 0, between = itemGap;
			Float distributed = 0;
			justifyOffsets(parentStyle.justifyContent, availableMain - usedMain, line.items.size(), leading, distributed);
			between += distributed;
			Float cursor = leading;
			for (FlexItem& flexItem : line.items)
			{
				const UiLayoutStyle childStyle = resolvedStyle(flexItem.node);
				const UiAlign alignment = childStyle.alignSelf == UiAlign::Auto ? parentStyle.alignItems : childStyle.alignSelf;
				const Float outerCross = flexItem.cross + crossStart(flexItem.margin, axis) + crossEnd(flexItem.margin, axis);
				Float crossOffset = crossStart(flexItem.margin, axis);
				Float childCross = flexItem.cross;
				if (alignment == UiAlign::Center) crossOffset += (line.cross - outerCross) * 0.5f;
				else if (alignment == UiAlign::End) crossOffset += line.cross - outerCross;
				else if (alignment == UiAlign::Stretch && stretchesCross(childStyle, axis))
					childCross = std::max(0.0f, line.cross - crossStart(flexItem.margin, axis) - crossEnd(flexItem.margin, axis));
				else if (alignment == UiAlign::Baseline && flexItem.baseline >= 0)
					crossOffset = line.baseline - flexItem.baseline;
				Vec2 position = contentRect.position;
				Vec2 size(0);
				setMain(position, axis, mainValue(contentRect.position, axis) + cursor + mainStart(flexItem.margin, axis));
				setCross(position, axis, crossValue(contentRect.position, axis) + lineOffset + crossOffset);
				setMain(size, axis, flexItem.main);
				setCross(size, axis, childCross);
				arrangeNode(flexItem.node, { position, size }, clip, depth);
				cursor += flexItem.main + mainStart(flexItem.margin, axis) + mainEnd(flexItem.margin, axis) + between;
			}
			lineOffset += line.cross + crossBetween;
		}
	}

	void UiLayoutEngine::arrangeGrid(
		const UiNodeHandle node, const UiRect& contentRect, const UiRect& clip, const SizeT depth)
	{
		UInt columns = 1, rows = 1;
		const Vector<GridPlacement> placements = placeGridItems(node, columns, rows);
		const UiLayoutStyle parentStyle = resolvedStyle(node);
		Vector<Float> columnSizes(columns, 0), rowSizes(rows, 0);
		Vector<Float> columnFractions(columns, 0), rowFractions(rows, 0);

		auto initializeTracks = [&](Vector<Float>& sizes, Vector<Float>& fractions,
			const Vector<UiGridTrack>& tracks, const Float reference)
			{
				for (UInt i = 0; i < sizes.size(); ++i)
				{
					const UiGridTrack track = i < tracks.size() ? tracks[i] : UiGridTrack{};
					if (track.size.kind == UiSizeKind::Fixed) sizes[i] = nonNegative(track.size.value);
					else if (track.size.kind == UiSizeKind::Percent) sizes[i] = nonNegative(reference * track.size.value);
					else if (track.size.kind == UiSizeKind::Fr) fractions[i] = std::max(track.size.value, 0.0f);
					if (track.minimum.kind != UiSizeKind::Auto)
						sizes[i] = std::max(sizes[i], resolve(track.minimum, reference, 0, node, true));
				}
			};
		initializeTracks(columnSizes, columnFractions, parentStyle.gridColumns, contentRect.size.x);
		initializeTracks(rowSizes, rowFractions, parentStyle.gridRows, contentRect.size.y);

		for (const GridPlacement& placement : placements)
		{
			const UiIntrinsicMeasure measured = measureNode(placement.node, UiConstraints::loose(contentRect.size), depth);
			const UiLayoutStyle childStyle = resolvedStyle(placement.node);
			const Float widthShare = (measured.preferred.x + childStyle.margin.horizontal()) / static_cast<Float>(placement.columnSpan);
			const Float heightShare = (measured.preferred.y + childStyle.margin.vertical()) / static_cast<Float>(placement.rowSpan);
			for (UInt x = placement.column; x < placement.column + placement.columnSpan; ++x)
				if (columnFractions[x] == 0) columnSizes[x] = std::max(columnSizes[x], widthShare);
			for (UInt y = placement.row; y < placement.row + placement.rowSpan; ++y)
				if (rowFractions[y] == 0) rowSizes[y] = std::max(rowSizes[y], heightShare);
		}
		auto distributeFractions = [](Vector<Float>& sizes, const Vector<Float>& fractions, const Float available, const Float gaps)
			{
				const Float occupied = std::accumulate(sizes.begin(), sizes.end(), gaps);
				const Float fractionTotal = std::accumulate(fractions.begin(), fractions.end(), 0.0f);
				if (fractionTotal <= 0) return;
				const Float unit = std::max(0.0f, available - occupied) / fractionTotal;
				for (UInt i = 0; i < sizes.size(); ++i) sizes[i] += fractions[i] * unit;
			};
		distributeFractions(columnSizes, columnFractions, contentRect.size.x, parentStyle.gap.x * static_cast<Float>(columns - 1));
		distributeFractions(rowSizes, rowFractions, contentRect.size.y, parentStyle.gap.y * static_cast<Float>(rows - 1));
		auto applyTrackMaximums = [&](Vector<Float>& sizes, const Vector<UiGridTrack>& tracks, const Float reference)
			{
				for (UInt i = 0; i < sizes.size() && i < tracks.size(); ++i)
					if (tracks[i].maximum.kind != UiSizeKind::Auto)
						sizes[i] = std::min(sizes[i], resolve(tracks[i].maximum, reference, sizes[i], node, true));
			};
		applyTrackMaximums(columnSizes, parentStyle.gridColumns, contentRect.size.x);
		applyTrackMaximums(rowSizes, parentStyle.gridRows, contentRect.size.y);

		Vector<Float> columnOffsets(columns, 0), rowOffsets(rows, 0);
		for (UInt i = 1; i < columns; ++i) columnOffsets[i] = columnOffsets[i - 1] + columnSizes[i - 1] + parentStyle.gap.x;
		for (UInt i = 1; i < rows; ++i) rowOffsets[i] = rowOffsets[i - 1] + rowSizes[i - 1] + parentStyle.gap.y;
		for (const GridPlacement& placement : placements)
		{
			const UiLayoutStyle childStyle = resolvedStyle(placement.node);
			const UiIntrinsicMeasure measured = measureNode(placement.node, UiConstraints::loose(contentRect.size), depth);
			Float width = -parentStyle.gap.x;
			for (UInt x = placement.column; x < placement.column + placement.columnSpan; ++x) width += columnSizes[x] + parentStyle.gap.x;
			Float height = -parentStyle.gap.y;
			for (UInt y = placement.row; y < placement.row + placement.rowSpan; ++y) height += rowSizes[y] + parentStyle.gap.y;
			const Vec2 cellSize = glm::max(Vec2(width - childStyle.margin.horizontal(), height - childStyle.margin.vertical()), Vec2(0));
			const UiAlign alignment = childStyle.alignSelf == UiAlign::Auto ? parentStyle.alignItems : childStyle.alignSelf;
			Vec2 size = glm::min(measured.preferred, cellSize);
			if (alignment == UiAlign::Stretch)
			{
				if (stretchesWidth(childStyle)) size.x = cellSize.x;
				if (stretchesHeight(childStyle)) size.y = cellSize.y;
			}
			Vec2 alignmentOffset(0);
			if (alignment == UiAlign::Center) alignmentOffset = (cellSize - size) * 0.5f;
			else if (alignment == UiAlign::End) alignmentOffset = cellSize - size;
			const Vec2 position = contentRect.position + Vec2(columnOffsets[placement.column] + childStyle.margin.left,
				rowOffsets[placement.row] + childStyle.margin.top) + alignmentOffset;
			arrangeNode(placement.node, { position, size }, clip, depth);
		}
	}

	void UiLayoutEngine::arrangeOverlay(
		const UiNodeHandle node, const UiRect& contentRect, const UiRect& clip, const SizeT depth)
	{
		const UiNode* parent = document.get(node);
		if (!parent) return;
		const UiLayoutStyle parentStyle = resolvedStyle(node);
		for (const UiNodeHandle child : parent->childHandles())
		{
			const UiLayoutStyle childStyle = resolvedStyle(child);
			if (childStyle.display == UiDisplay::None || childStyle.position == UiPositionType::Absolute) continue;
			const UiIntrinsicMeasure measured = measureNode(child, UiConstraints::loose(contentRect.size), depth);
			const UiAlign alignment = childStyle.alignSelf == UiAlign::Auto ? parentStyle.alignItems : childStyle.alignSelf;
			Vec2 size = measured.preferred;
			const Vec2 available = glm::max(contentRect.size - childStyle.margin.total(), Vec2(0));
			if (alignment == UiAlign::Stretch)
			{
				if (stretchesWidth(childStyle)) size.x = available.x;
				if (stretchesHeight(childStyle)) size.y = available.y;
			}
			Vec2 offset(0);
			if (alignment == UiAlign::Center) offset = (available - glm::min(size, available)) * 0.5f;
			else if (alignment == UiAlign::End) offset = available - glm::min(size, available);
			const Vec2 position = contentRect.position + childStyle.margin.topLeft() + offset;
			arrangeNode(child, { position, size }, clip, depth);
		}
	}

	void UiLayoutEngine::arrangeAbsolute(
		const UiNodeHandle node, const UiRect& containingRect, const UiRect& clip, const SizeT depth)
	{
		const UiLayoutStyle nodeStyle = resolvedStyle(node);
		const UiIntrinsicMeasure measured = measureNode(node, UiConstraints::loose(containingRect.size), depth);
		const Bool hasLeft = nodeStyle.left.kind != UiSizeKind::Auto;
		const Bool hasRight = nodeStyle.right.kind != UiSizeKind::Auto;
		const Bool hasTop = nodeStyle.top.kind != UiSizeKind::Auto;
		const Bool hasBottom = nodeStyle.bottom.kind != UiSizeKind::Auto;
		const Float left = hasLeft ? resolve(nodeStyle.left, containingRect.size.x, 0, node, true) : 0;
		const Float right = hasRight ? resolve(nodeStyle.right, containingRect.size.x, 0, node, true) : 0;
		const Float top = hasTop ? resolve(nodeStyle.top, containingRect.size.y, 0, node, true) : 0;
		const Float bottom = hasBottom ? resolve(nodeStyle.bottom, containingRect.size.y, 0, node, true) : 0;
		Vec2 size = measured.preferred;
		if (hasLeft && hasRight) size.x = std::max(0.0f, containingRect.size.x - left - right - nodeStyle.margin.horizontal());
		if (hasTop && hasBottom) size.y = std::max(0.0f, containingRect.size.y - top - bottom - nodeStyle.margin.vertical());
		Vec2 position = containingRect.position + nodeStyle.margin.topLeft();
		position.x += hasLeft ? left : (hasRight ? containingRect.size.x - right - size.x - nodeStyle.margin.right : 0);
		position.y += hasTop ? top : (hasBottom ? containingRect.size.y - bottom - size.y - nodeStyle.margin.bottom : 0);
		arrangeNode(node, { position, size }, clip, depth);
	}
}
