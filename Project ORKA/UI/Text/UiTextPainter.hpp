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

	struct UiTextPaintStyle
	{
		Float fontSize = 0; // Zero selects a bounded box-relative default.
		Float lineHeight = 0;
		UiLinearColor color =
			UiLinearColor::fromStraightSrgb(Vec4(0.82f, 0.86f, 0.92f, 0.92f));
		Vec2 padding = Vec2(6, 3);
		Bool wrap = true;
		Bool clip = true;
		UiTextWrap wrapPolicy = UiTextWrap::Wrap;
		UiTextFitOptions fit;
		SizeT maximumCodepoints = 16384;
	};

	using UiTextStyleProvider = Function<void(
		const UiNode& node, UiTextPaintStyle& style)>;

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
