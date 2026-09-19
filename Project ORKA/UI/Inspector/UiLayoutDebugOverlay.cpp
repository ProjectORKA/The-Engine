#include "UiLayoutDebugOverlay.hpp"

#include <algorithm>
#include <cmath>

namespace Ui::Inspector
{
	namespace
	{
		Bool finiteRect(const Vec2 position, const Vec2 size)
		{
			return std::isfinite(position.x) && std::isfinite(position.y) &&
				std::isfinite(size.x) && std::isfinite(size.y) &&
				size.x >= 0 && size.y >= 0;
		}

		UiEdges nonNegativeEdges(const UiEdges& edges)
		{
			return UiEdges{
				std::max(0.0f, edges.left),
				std::max(0.0f, edges.top),
				std::max(0.0f, edges.right),
				std::max(0.0f, edges.bottom)
			};
		}

		constexpr Float maximumOverlaySpan = 4096.0f;

		Bool saneSpan(const Float value)
		{
			return std::isfinite(value) && value > 0.0f && value <= maximumOverlaySpan;
		}

		Bool addFill(UiDisplayList& output, const Vec2 position, const Vec2 size,
			const UiLinearColor color)
		{
			if (!finiteRect(position, size) || !saneSpan(size.x) || !saneSpan(size.y)) return true;
			(void)output.addSolidQuad({ position, size }, color);
			return true;
		}

		void addOutline(UiDisplayList& output, const Vec2 position, const Vec2 size,
			const Float width, const UiLinearColor color)
		{
			if (!finiteRect(position, size) || width <= 0) return;
			(void)output.addInsetBorder({ position, size }, width, color);
		}

		Vec2 marginOrigin(const Vec2 borderPosition, const UiEdges& margin)
		{
			return borderPosition - Vec2(margin.left, margin.top);
		}

		Vec2 marginSize(const Vec2 borderSize, const UiEdges& margin)
		{
			return borderSize + margin.total();
		}

		void paintMarginStrips(UiDisplayList& output, const Vec2 borderPosition,
			const Vec2 borderSize, const UiEdges& margin, const UiLinearColor color)
		{
			const UiEdges safeMargin = nonNegativeEdges(margin);
			if (safeMargin.left <= 0 && safeMargin.right <= 0 &&
				safeMargin.top <= 0 && safeMargin.bottom <= 0) return;

			const Vec2 marginPosition = marginOrigin(borderPosition, safeMargin);
			const Vec2 marginBoxSize = marginSize(borderSize, safeMargin);

			if (safeMargin.top > 0)
				(void)addFill(output, marginPosition, Vec2(marginBoxSize.x, safeMargin.top), color);
			if (safeMargin.bottom > 0)
				(void)addFill(output,
					Vec2(marginPosition.x, marginPosition.y + marginBoxSize.y - safeMargin.bottom),
					Vec2(marginBoxSize.x, safeMargin.bottom), color);
			if (safeMargin.left > 0)
				(void)addFill(output,
					Vec2(marginPosition.x, borderPosition.y),
					Vec2(safeMargin.left, borderSize.y), color);
			if (safeMargin.right > 0)
				(void)addFill(output,
					Vec2(borderPosition.x + borderSize.x, borderPosition.y),
					Vec2(safeMargin.right, borderSize.y), color);
		}

		Vector<UiNodeHandle> flowChildren(const UiDocument& document,
			const UiLayoutEngine& layoutEngine, const UiNodeHandle parent)
		{
			Vector<UiNodeHandle> result;
			const UiNode* parentNode = document.get(parent);
			if (!parentNode) return result;
			for (const UiNodeHandle child : parentNode->childHandles())
			{
				const UiLayoutStyle childStyle = layoutEngine.resolvedStyle(child);
				if (childStyle.display == UiDisplay::None ||
					childStyle.position == UiPositionType::Absolute) continue;
				result.push_back(child);
			}
			return result;
		}

		Bool rangesOverlap(const Float leftStart, const Float leftEnd,
			const Float rightStart, const Float rightEnd, const Float tolerance)
		{
			return leftStart < rightEnd - tolerance && rightStart < leftEnd - tolerance;
		}

		Bool siblingsShareFlexLine(
			const Vec2 leftOrigin, const Vec2 leftSize,
			const Vec2 rightOrigin, const Vec2 rightSize,
			const UiAxis axis)
		{
			constexpr Float lineTolerance = 2.0f;
			if (axis == UiAxis::Row)
			{
				const Float leftMainEnd = leftOrigin.x + leftSize.x;
				if (leftMainEnd > rightOrigin.x + lineTolerance) return false;
				return rangesOverlap(leftOrigin.y, leftOrigin.y + leftSize.y,
					rightOrigin.y, rightOrigin.y + rightSize.y, lineTolerance);
			}
			const Float leftMainEnd = leftOrigin.y + leftSize.y;
			if (leftMainEnd > rightOrigin.y + lineTolerance) return false;
			return rangesOverlap(leftOrigin.x, leftOrigin.x + leftSize.x,
				rightOrigin.x, rightOrigin.x + rightSize.x, lineTolerance);
		}

		void paintFlexGapBetween(UiDisplayList& output, const UiDocument& document,
			const UiLayoutEngine& layoutEngine, const UiNodeHandle left,
			const UiNodeHandle right, const UiAxis axis, const UiLinearColor color)
		{
			const UiNode* leftNode = document.get(left);
			const UiNode* rightNode = document.get(right);
			if (!leftNode || !rightNode) return;

			const UiLayoutStyle leftStyle = layoutEngine.resolvedStyle(left);
			const UiLayoutStyle rightStyle = layoutEngine.resolvedStyle(right);
			const UiEdges leftMargin = nonNegativeEdges(leftStyle.margin);
			const UiEdges rightMargin = nonNegativeEdges(rightStyle.margin);
			const Vec2 leftMarginOrigin = marginOrigin(leftNode->computed.position, leftMargin);
			const Vec2 leftMarginSize = marginSize(leftNode->computed.size, leftMargin);
			const Vec2 rightMarginOrigin = marginOrigin(rightNode->computed.position, rightMargin);
			const Vec2 rightMarginSize = marginSize(rightNode->computed.size, rightMargin);
			if (!siblingsShareFlexLine(
				leftMarginOrigin, leftMarginSize, rightMarginOrigin, rightMarginSize, axis))
				return;

			Vec2 gapPosition(0);
			Vec2 gapSize(0);
			if (axis == UiAxis::Row)
			{
				const Float gapStart = leftMarginOrigin.x + leftMarginSize.x;
				const Float gapEnd = rightMarginOrigin.x;
				gapSize.x = gapEnd - gapStart;
				if (gapSize.x <= 0) return;
				gapPosition.x = gapStart;
				gapPosition.y = std::min(leftMarginOrigin.y, rightMarginOrigin.y);
				gapSize.y = std::max(leftMarginOrigin.y + leftMarginSize.y,
					rightMarginOrigin.y + rightMarginSize.y) - gapPosition.y;
			}
			else
			{
				const Float gapStart = leftMarginOrigin.y + leftMarginSize.y;
				const Float gapEnd = rightMarginOrigin.y;
				gapSize.y = gapEnd - gapStart;
				if (gapSize.y <= 0) return;
				gapPosition.y = gapStart;
				gapPosition.x = std::min(leftMarginOrigin.x, rightMarginOrigin.x);
				gapSize.x = std::max(leftMarginOrigin.x + leftMarginSize.x,
					rightMarginOrigin.x + rightMarginSize.x) - gapPosition.x;
			}
			(void)addFill(output, gapPosition, gapSize, color);
		}

		void paintInternalFlexGaps(UiDisplayList& output, const UiDocument& document,
			const UiLayoutEngine& layoutEngine, const UiNodeHandle hovered,
			const UiLinearColor color)
		{
			const UiLayoutStyle nodeStyle = layoutEngine.resolvedStyle(hovered);
			if (nodeStyle.display != UiDisplay::Flex) return;

			const Vector<UiNodeHandle> children = flowChildren(document, layoutEngine, hovered);
			if (children.size() < 2) return;

			const UiAxis axis = nodeStyle.flexDirection;
			for (SizeT index = 1; index < children.size(); ++index)
			{
				paintFlexGapBetween(output, document, layoutEngine,
					children[index - 1], children[index], axis, color);
			}
		}
	}

	UiLayoutDebugOverlayResult UiLayoutDebugOverlay::paintHovered(
		const UiDocument& document,
		const UiLayoutEngine& layoutEngine,
		const UiNodeHandle hovered,
		UiDisplayList& output,
		const UiLayoutDebugOverlayStyle& style)
	{
		UiLayoutDebugOverlayResult result;
		if (!document.isValid(hovered))
			return result;
		if (output.isFinalized())
		{
			result.valid = false;
			return result;
		}

		const UiNode* node = document.get(hovered);
		if (!node) return result;

		const UiLayoutStyle nodeStyle = layoutEngine.resolvedStyle(hovered);
		const Vec2 borderPosition = node->computed.position;
		const Vec2 borderSize = node->computed.size;
		const Vec2 contentPosition = node->computed.contentPosition;
		const Vec2 contentSize = node->computed.contentSize;

		if (!finiteRect(borderPosition, borderSize))
			return result;

		paintMarginStrips(output, borderPosition, borderSize, nodeStyle.margin, style.marginColor);
		(void)addFill(output, borderPosition, borderSize, style.paddingColor);
		if (finiteRect(contentPosition, contentSize))
			(void)addFill(output, contentPosition, contentSize, style.contentColor);
		paintInternalFlexGaps(output, document, layoutEngine, hovered, style.gapColor);
		addOutline(output, borderPosition, borderSize, style.outlineWidth, style.borderOutlineColor);

		result.painted = true;
		return result;
	}
}
