#pragma once

#include "../Core/UiTypes.hpp"

namespace Ui
{
	constexpr Float uiLayoutInfinity = std::numeric_limits<Float>::infinity();

	struct UiConstraints
	{
		Vec2 minimum = Vec2(0);
		Vec2 maximum = Vec2(uiLayoutInfinity);

		[[nodiscard]] static UiConstraints tight(Vec2 size);
		[[nodiscard]] static UiConstraints loose(Vec2 maximum);
		[[nodiscard]] UiConstraints normalized() const;
		[[nodiscard]] Vec2 constrain(Vec2 size) const;
		[[nodiscard]] Bool hasBoundedWidth() const;
		[[nodiscard]] Bool hasBoundedHeight() const;
		[[nodiscard]] Bool operator==(const UiConstraints& other) const;
	};

	struct UiEdges
	{
		Float left = 0;
		Float top = 0;
		Float right = 0;
		Float bottom = 0;

		[[nodiscard]] Float horizontal() const { return left + right; }
		[[nodiscard]] Float vertical() const { return top + bottom; }
		[[nodiscard]] Vec2 topLeft() const { return Vec2(left, top); }
		[[nodiscard]] Vec2 total() const { return Vec2(horizontal(), vertical()); }
		constexpr auto operator<=>(const UiEdges&) const = default;
	};

	struct UiRect
	{
		Vec2 position = Vec2(0);
		Vec2 size = Vec2(0);

		[[nodiscard]] Vec2 end() const { return position + size; }
		[[nodiscard]] Bool isEmpty() const { return size.x <= 0 || size.y <= 0; }
		[[nodiscard]] UiRect intersected(const UiRect& other) const;
	};

	struct UiIntrinsicMeasure
	{
		Vec2 minimum = Vec2(0);
		Vec2 preferred = Vec2(0);
		Float baseline = -1;
	};

	enum class UiSizeKind : Byte
	{
		Auto,
		Fixed,
		Percent,
		Content,
		Fr,
	};

	struct UiSizeSpec
	{
		UiSizeKind kind = UiSizeKind::Auto;
		Float value = 0;

		[[nodiscard]] static UiSizeSpec fixed(Float value) { return { UiSizeKind::Fixed, value }; }
		[[nodiscard]] static UiSizeSpec percent(Float value) { return { UiSizeKind::Percent, value }; }
		[[nodiscard]] static UiSizeSpec content() { return { UiSizeKind::Content, 0 }; }
		[[nodiscard]] static UiSizeSpec fr(Float value = 1) { return { UiSizeKind::Fr, value }; }
		constexpr auto operator<=>(const UiSizeSpec&) const = default;
	};

	enum class UiDisplay : Byte { Flex, Grid, Overlay, None };
	enum class UiOverflow : Byte { Visible, Hidden, Scroll, Auto };
	enum class UiAxis : Byte { Row, Column };
	enum class UiWrap : Byte { NoWrap, Wrap, Balance };
	enum class UiJustify : Byte { Start, Center, End, SpaceBetween, SpaceAround, SpaceEvenly };
	enum class UiAlign : Byte { Auto, Start, Center, End, Stretch, Baseline };
	enum class UiPositionType : Byte { Flow, Absolute };

	struct UiGridTrack
	{
		UiSizeSpec size = UiSizeSpec::fr();
		UiSizeSpec minimum;
		UiSizeSpec maximum;
	};

	struct UiLayoutStyle
	{
		UiDisplay display = UiDisplay::Flex;
		UiSizeSpec width;
		UiSizeSpec height;
		UiSizeSpec minimumWidth;
		UiSizeSpec minimumHeight;
		UiSizeSpec maximumWidth;
		UiSizeSpec maximumHeight;
		Float aspectRatio = 0;
		UiEdges padding;
		UiEdges margin;
		Vec2 gap = Vec2(0);
		UiOverflow overflowX = UiOverflow::Visible;
		UiOverflow overflowY = UiOverflow::Visible;

		UiAxis flexDirection = UiAxis::Column;
		UiWrap flexWrap = UiWrap::NoWrap;
		Float flexGrow = 0;
		Float flexShrink = 1;
		UiSizeSpec flexBasis;
		UiJustify justifyContent = UiJustify::Start;
		UiAlign alignItems = UiAlign::Stretch;
		UiAlign alignSelf = UiAlign::Auto;
		UiAlign alignContent = UiAlign::Start;

		Vector<UiGridTrack> gridColumns;
		Vector<UiGridTrack> gridRows;
		Int gridColumn = -1;
		Int gridRow = -1;
		UInt gridColumnSpan = 1;
		UInt gridRowSpan = 1;

		UiPositionType position = UiPositionType::Flow;
		UiSizeSpec left;
		UiSizeSpec top;
		UiSizeSpec right;
		UiSizeSpec bottom;
	};

	enum class UiLayoutDiagnosticCode : Byte
	{
		PercentageCycle,
		IntrinsicCycle,
		DepthLimit,
		InvalidValue,
		GridPlacementOverflow,
	};

	struct UiLayoutDiagnostic
	{
		UiLayoutDiagnosticCode code = UiLayoutDiagnosticCode::InvalidValue;
		UiNodeHandle node;
		String message;
	};
}
