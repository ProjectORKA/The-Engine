#pragma once

#include "../Core/UiDocument.hpp"
#include "../Render/UiDisplayList.hpp"
#include "UiTextLayout.hpp"

namespace Ui
{
	struct UiTextGlyphRenderInfo
	{
		UiResourceRef resource;
		UiPipeline pipeline = UiPipeline::GrayscaleGlyph;
		Vec4 uvRect = Vec4(0, 0, 1, 1);
		Vec2 bitmapSize = Vec2(0);
		Vec2 bearing = Vec2(0); // From pen baseline to bitmap top-left.
		Float advance = 0;
		Float ascent = 0;
		Float lineHeight = 0;
		Float distanceRange = 0;
	};

	// Return true when a resident advanced atlas glyph is ready. Returning
	// false transparently selects the built-in ASCII fallback.
	using UiTextGlyphProvider = Function<Bool(
		U32 codepoint, Float pixelSize, UiTextGlyphRenderInfo& output)>;

	struct UiShapedGlyphRenderInfo
	{
		U32 codepoint = 0;
		UInt codepointIndex = 0;
		UiTextGlyphRenderInfo glyph;
		Vec2 offset = Vec2(0);
	};

	struct UiTextPaintLayout
	{
		Vector<UiShapedGlyphRenderInfo> glyphs;
		UiTextLayoutResult layout;
	};

	struct UiTextPaintStyle;

	// Uses the same prepared text and line-layout policy as intrinsic measure.
	using UiTextShapeProvider = Function<Bool(
		const UiNode& node, const UiTextPaintStyle& style,
		Vec2 definiteBox, UiTextPaintLayout& output)>;

	enum class UiTextHorizontalAlign : Byte
	{
		Start,
		Center,
	};

	struct UiTextPaintStyle
	{
		Float fontSize = 0; // Zero selects a bounded box-relative default.
		Float lineHeight = 0;
		UiLinearColor color =
			UiLinearColor::fromStraightSrgb(Vec4(0.82f, 0.86f, 0.92f, 0.92f));
		UiLinearColor caretColor =
			UiLinearColor::fromStraightSrgb(Vec4(0.92f, 0.94f, 0.98f, 1.0f));
		UiLinearColor selectionColor =
			UiLinearColor::fromStraightSrgb(Vec4(0.25f, 0.55f, 0.95f, 0.40f));
		Vec2 padding = Vec2(6, 3);
		UiTextHorizontalAlign horizontalAlign = UiTextHorizontalAlign::Start;
		Bool wrap = true;
		Bool clip = true;
		UiTextWrap wrapPolicy = UiTextWrap::Wrap;
		UiTextFitOptions fit;
		SizeT maximumCodepoints = 16384;
	};

	using UiTextStyleProvider = Function<void(
		const UiNode& node, UiTextPaintStyle& style)>;

	// Shared caret / hit-test geometry for editable text (matches paint advances).
	struct UiTextEditMetrics
	{
		Float left = 0;
		Float baseline = 0;
		Float ascent = 0;
		Float lineHeight = 0;
		Vector<SizeT> clusterBytes; // codepoint boundary byte offsets, ends with text.size()
		Vector<Float> clusterX;     // x at each clusterBytes entry (same length)

		[[nodiscard]] Float xAtByte(SizeT byteOffset) const;
		[[nodiscard]] SizeT byteAtX(Float x) const;
	};

	class UiTextPainter
	{
	public:
		explicit UiTextPainter(
			UiTextGlyphProvider glyphProvider = {},
			UiTextStyleProvider styleProvider = {},
			UiTextShapeProvider shapeProvider = {});

		[[nodiscard]] Bool paint(
			const UiNode& node, UiDisplayList& output,
			UiTextPaintStyle style = {}) const;

		// Builds caret/hit metrics using the same shaping path as paint().
		[[nodiscard]] Bool buildEditMetrics(
			const UiNode& node, UiTextPaintStyle style, UiTextEditMetrics& metrics) const;

		[[nodiscard]] static UiTextGlyphRenderInfo fallbackGlyph(
			U32 codepoint, Float pixelSize);
		static void buildFallbackAtlas(
			Vector<Byte>& pixels, UInt& width, UInt& height);

	private:
		UiTextGlyphProvider glyphProvider;
		UiTextStyleProvider styleProvider;
		UiTextShapeProvider shapeProvider;
	};
}
