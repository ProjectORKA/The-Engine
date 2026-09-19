#include "UiTextPainter.hpp"

#include <algorithm>
#include <cmath>

namespace Ui
{
	namespace
	{
		constexpr UInt atlasWidth = 128;
		constexpr UInt atlasHeight = 48;
		constexpr UInt atlasColumns = 16;
		constexpr UInt cellSize = 8;

		// Five column bits per printable ASCII glyph, least-significant bit at
		// the top. This is expanded into the renderer-owned R8 atlas at create.
		constexpr U8 glyphColumns[][5] = {
			{0x00,0x00,0x00,0x00,0x00},{0x00,0x00,0x5f,0x00,0x00},
			{0x00,0x07,0x00,0x07,0x00},{0x14,0x7f,0x14,0x7f,0x14},
			{0x24,0x2a,0x7f,0x2a,0x12},{0x23,0x13,0x08,0x64,0x62},
			{0x36,0x49,0x55,0x22,0x50},{0x00,0x05,0x03,0x00,0x00},
			{0x00,0x1c,0x22,0x41,0x00},{0x00,0x41,0x22,0x1c,0x00},
			{0x14,0x08,0x3e,0x08,0x14},{0x08,0x08,0x3e,0x08,0x08},
			{0x00,0x50,0x30,0x00,0x00},{0x08,0x08,0x08,0x08,0x08},
			{0x00,0x60,0x60,0x00,0x00},{0x20,0x10,0x08,0x04,0x02},
			{0x3e,0x51,0x49,0x45,0x3e},{0x00,0x42,0x7f,0x40,0x00},
			{0x42,0x61,0x51,0x49,0x46},{0x21,0x41,0x45,0x4b,0x31},
			{0x18,0x14,0x12,0x7f,0x10},{0x27,0x45,0x45,0x45,0x39},
			{0x3c,0x4a,0x49,0x49,0x30},{0x01,0x71,0x09,0x05,0x03},
			{0x36,0x49,0x49,0x49,0x36},{0x06,0x49,0x49,0x29,0x1e},
			{0x00,0x36,0x36,0x00,0x00},{0x00,0x56,0x36,0x00,0x00},
			{0x08,0x14,0x22,0x41,0x00},{0x14,0x14,0x14,0x14,0x14},
			{0x00,0x41,0x22,0x14,0x08},{0x02,0x01,0x51,0x09,0x06},
			{0x32,0x49,0x79,0x41,0x3e},{0x7e,0x11,0x11,0x11,0x7e},
			{0x7f,0x49,0x49,0x49,0x36},{0x3e,0x41,0x41,0x41,0x22},
			{0x7f,0x41,0x41,0x22,0x1c},{0x7f,0x49,0x49,0x49,0x41},
			{0x7f,0x09,0x09,0x09,0x01},{0x3e,0x41,0x49,0x49,0x7a},
			{0x7f,0x08,0x08,0x08,0x7f},{0x00,0x41,0x7f,0x41,0x00},
			{0x20,0x40,0x41,0x3f,0x01},{0x7f,0x08,0x14,0x22,0x41},
			{0x7f,0x40,0x40,0x40,0x40},{0x7f,0x02,0x0c,0x02,0x7f},
			{0x7f,0x04,0x08,0x10,0x7f},{0x3e,0x41,0x41,0x41,0x3e},
			{0x7f,0x09,0x09,0x09,0x06},{0x3e,0x41,0x51,0x21,0x5e},
			{0x7f,0x09,0x19,0x29,0x46},{0x46,0x49,0x49,0x49,0x31},
			{0x01,0x01,0x7f,0x01,0x01},{0x3f,0x40,0x40,0x40,0x3f},
			{0x1f,0x20,0x40,0x20,0x1f},{0x3f,0x40,0x38,0x40,0x3f},
			{0x63,0x14,0x08,0x14,0x63},{0x07,0x08,0x70,0x08,0x07},
			{0x61,0x51,0x49,0x45,0x43},{0x00,0x7f,0x41,0x41,0x00},
			{0x02,0x04,0x08,0x10,0x20},{0x00,0x41,0x41,0x7f,0x00},
			{0x04,0x02,0x01,0x02,0x04},{0x40,0x40,0x40,0x40,0x40},
			{0x00,0x01,0x02,0x04,0x00},{0x20,0x54,0x54,0x54,0x78},
			{0x7f,0x48,0x44,0x44,0x38},{0x38,0x44,0x44,0x44,0x20},
			{0x38,0x44,0x44,0x48,0x7f},{0x38,0x54,0x54,0x54,0x18},
			{0x08,0x7e,0x09,0x01,0x02},{0x0c,0x52,0x52,0x52,0x3e},
			{0x7f,0x08,0x04,0x04,0x78},{0x00,0x44,0x7d,0x40,0x00},
			{0x20,0x40,0x44,0x3d,0x00},{0x7f,0x10,0x28,0x44,0x00},
			{0x00,0x41,0x7f,0x40,0x00},{0x7c,0x04,0x18,0x04,0x78},
			{0x7c,0x08,0x04,0x04,0x78},{0x38,0x44,0x44,0x44,0x38},
			{0x7c,0x14,0x14,0x14,0x08},{0x08,0x14,0x14,0x18,0x7c},
			{0x7c,0x08,0x04,0x04,0x08},{0x48,0x54,0x54,0x54,0x20},
			{0x04,0x3f,0x44,0x40,0x20},{0x3c,0x40,0x40,0x20,0x7c},
			{0x1c,0x20,0x40,0x20,0x1c},{0x3c,0x40,0x30,0x40,0x3c},
			{0x44,0x28,0x10,0x28,0x44},{0x0c,0x50,0x50,0x50,0x3c},
			{0x44,0x64,0x54,0x4c,0x44},{0x00,0x08,0x36,0x41,0x00},
			{0x00,0x00,0x7f,0x00,0x00},{0x00,0x41,0x36,0x08,0x00},
			{0x02,0x01,0x02,0x04,0x02}
		};
		static_assert(std::size(glyphColumns) == 95);

		[[nodiscard]] Bool continuation(const U8 value)
		{
			return (value & 0xc0) == 0x80;
		}

		[[nodiscard]] U32 decodeOne(const String& text, SizeT& cursor)
		{
			const U8 first = static_cast<U8>(text[cursor++]);
			if (first < 0x80) return first;

			UInt count = 0;
			U32 value = 0;
			U32 minimum = 0;
			if (first >= 0xc2 && first <= 0xdf)
				{ count = 1; value = first & 0x1f; minimum = 0x80; }
			else if (first >= 0xe0 && first <= 0xef)
				{ count = 2; value = first & 0x0f; minimum = 0x800; }
			else if (first >= 0xf0 && first <= 0xf4)
				{ count = 3; value = first & 0x07; minimum = 0x10000; }
			else return 0xfffd;

			if (cursor + count > text.size()) return 0xfffd;
			const SizeT continuationStart = cursor;
			for (UInt index = 0; index < count; ++index)
			{
				const U8 next = static_cast<U8>(text[cursor + index]);
				if (!continuation(next)) return 0xfffd;
				value = (value << 6) | (next & 0x3f);
			}
			cursor = continuationStart + count;
			if (value < minimum || value > 0x10ffff ||
				(value >= 0xd800 && value <= 0xdfff)) return 0xfffd;
			return value;
		}

		[[nodiscard]] UiTexturePool poolFor(const UiPipeline pipeline)
		{
			switch (pipeline)
			{
			case UiPipeline::GrayscaleGlyph: return UiTexturePool::GrayscaleGlyphs;
			case UiPipeline::MsdfGlyph: return UiTexturePool::DistanceFieldGlyphs;
			case UiPipeline::ColorGlyph: return UiTexturePool::ColorGlyphs;
			default: return UiTexturePool::None;
			}
		}

		// The built-in bitmap font has no antialiasing, so fractional placement
		// would smear stems across texel borders. Atlas glyphs keep their exact
		// subpixel position.
		[[nodiscard]] Vec2 glyphOrigin(
			const UiTextGlyphRenderInfo& glyph, const Vec2 position)
		{
			if (glyph.resource != builtinFallbackFontResource()) return position;
			return Vec2(std::round(position.x), std::round(position.y));
		}

		[[nodiscard]] Bool addGlyph(UiDisplayList& output,
			const UiTextGlyphRenderInfo& glyph, const UiQuadGeometry& geometry,
			const UInt resourceIndex, const UiLinearColor color)
		{
			const UiGlyphPaint paint{
				resourceIndex, glyph.uvRect, color, glyph.distanceRange };
			return output.addGlyph(glyph.pipeline, geometry, paint);
		}

		[[nodiscard]] UiClipRect paintClipRect(
			const UiNode& node, const Vec2 boxPosition, const Vec2 boxSize)
		{
			UiClipRect clip{ boxPosition, boxSize };
			if (node.computed.clipSize.x <= 0 || node.computed.clipSize.y <= 0) return clip;
			const Vec2 layoutEnd = node.computed.clipPosition + node.computed.clipSize;
			const Vec2 boxEnd = boxPosition + boxSize;
			const Vec2 start = glm::max(boxPosition, node.computed.clipPosition);
			const Vec2 end = glm::min(boxEnd, layoutEnd);
			clip.position = start;
			clip.size = glm::max(end - start, Vec2(0));
			return clip;
		}
	}

	UiTextPainter::UiTextPainter(
		UiTextGlyphProvider suppliedGlyphProvider,
		UiTextStyleProvider suppliedStyleProvider,
		UiTextShapeProvider suppliedShapeProvider)
		: glyphProvider(std::move(suppliedGlyphProvider)),
		  styleProvider(std::move(suppliedStyleProvider)),
		  shapeProvider(std::move(suppliedShapeProvider))
	{
	}

	UiTextGlyphRenderInfo UiTextPainter::fallbackGlyph(
		const U32 requestedCodepoint, const Float pixelSize)
	{
		const U32 codepoint = requestedCodepoint >= 32 && requestedCodepoint <= 126
			? requestedCodepoint : static_cast<U32>('?');
		const UInt glyph = codepoint - 32;
		const UInt cellX = (glyph % atlasColumns) * cellSize;
		const UInt cellY = (glyph / atlasColumns) * cellSize;
		const Float scale = std::max(1.0f, pixelSize) / 8.0f;
		// Whole-texel expansion keeps stems a uniform width, but only when the
		// size is already close to a texel multiple. Forcing it otherwise would
		// widen glyphs past the advance the layout model reserved for them.
		const Float nearestScale = std::round(scale);
		const Float bitmapScale = std::abs(scale - nearestScale) <= 0.05f
			? std::max(1.0f, nearestScale) : scale;

		UiTextGlyphRenderInfo result;
		result.resource = builtinFallbackFontResource();
		result.pipeline = UiPipeline::GrayscaleGlyph;
		result.uvRect = Vec4(
			(static_cast<Float>(cellX) + 1.5f) / atlasWidth,
			(static_cast<Float>(cellY) + 0.5f) / atlasHeight,
			(static_cast<Float>(cellX) + 5.5f) / atlasWidth,
			(static_cast<Float>(cellY) + 6.5f) / atlasHeight);
		result.bitmapSize = Vec2(5, 7) * bitmapScale;
		result.bearing = Vec2(0, -7.0f * bitmapScale);
		result.advance = 6.0f * scale;
		result.ascent = 7.0f * scale;
		result.lineHeight = 8.0f * scale;
		return result;
	}

	void UiTextPainter::buildFallbackAtlas(
		Vector<Byte>& pixels, UInt& width, UInt& height)
	{
		width = atlasWidth;
		height = atlasHeight;
		pixels.assign(static_cast<SizeT>(width) * height, 0);
		for (UInt glyph = 0; glyph < std::size(glyphColumns); ++glyph)
		{
			const UInt originX = (glyph % atlasColumns) * cellSize + 1;
			const UInt originY = (glyph / atlasColumns) * cellSize;
			for (UInt x = 0; x < 5; ++x)
			{
				for (UInt y = 0; y < 7; ++y)
				{
					if ((glyphColumns[glyph][x] & (1u << y)) != 0)
						pixels[static_cast<SizeT>(originY + y) * width + originX + x] = 255;
				}
			}
		}
	}

	Bool UiTextPainter::paint(
		const UiNode& node, UiDisplayList& output, UiTextPaintStyle style) const
	{
		if (node.authored.text.empty()) return true;
		if (styleProvider) styleProvider(node, style);

		Vec2 boxPosition = node.computed.contentPosition;
		Vec2 boxSize = node.computed.contentSize;
		if (boxSize.x <= 0 || boxSize.y <= 0)
		{
			boxPosition = node.computed.position;
			boxSize = node.computed.size;
		}
		if (boxSize.x <= 0 || boxSize.y <= 0) return true;

		const Vec2 innerPosition = boxPosition + glm::max(style.padding, Vec2(0));
		const Vec2 innerSize = glm::max(
			boxSize - glm::max(style.padding, Vec2(0)) * 2.0f, Vec2(0));
		if (innerSize.x <= 0 || innerSize.y <= 0) return true;

		const Float boundedDefault = std::clamp(innerSize.y * 0.45f, 8.0f, 16.0f);
		const Float pixelSize = std::clamp(
			style.fontSize > 0 ? style.fontSize : boundedDefault, 4.0f, 256.0f);
		const UiTextGlyphRenderInfo fallbackMetrics = fallbackGlyph('M', pixelSize);
		const Float lineHeight = style.lineHeight > 0
			? style.lineHeight : fallbackMetrics.lineHeight;
		const Float left = innerPosition.x;
		const Float right = innerPosition.x + innerSize.x;
		const Float bottom = innerPosition.y + innerSize.y;
		Float penX = left;
		Float baseline = innerPosition.y + fallbackMetrics.ascent;
		const Float computedBaseline =
			node.computed.position.y + node.computed.baseline;
		if (node.computed.baseline >= 0 &&
			computedBaseline >= innerPosition.y &&
			computedBaseline < bottom)
			baseline = computedBaseline;
		SizeT decoded = 0;
		Bool clipPushed = false;
		if (style.clip)
		{
			const UiClipRect clip = paintClipRect(node, boxPosition, boxSize);
			if (clip.size.x <= 0 || clip.size.y <= 0) return true;
			if (!output.pushClip(clip)) return false;
			clipPushed = true;
		}

		auto finish = [&](const Bool result)
		{
			if (clipPushed && !output.popClip()) return false;
			return result;
		};

		UiTextPaintLayout shapedLayout;
		if (shapeProvider && shapeProvider(node, style, innerSize, shapedLayout))
		{
			for (const UiTextLine& line : shapedLayout.layout.lines)
			{
				penX = left;
				baseline = innerPosition.y + line.y + line.baseline;
				const UInt lineEnd =
					line.firstCodepoint + line.codepointCount;
				for (const UiShapedGlyphRenderInfo& shaped : shapedLayout.glyphs)
				{
					if (shaped.codepointIndex < line.firstCodepoint ||
						shaped.codepointIndex >= lineEnd) continue;
					const UiTextGlyphRenderInfo& glyph = shaped.glyph;
					if (shaped.codepoint != ' ' && shaped.codepoint != '\t' &&
						shaped.codepoint != '\r' && shaped.codepoint != '\n')
					{
						UInt resourceIndex = invalidUiResourceIndex;
						if (!output.addResource(glyph.resource, resourceIndex))
							return finish(false);
						const UiQuadGeometry geometry{
							glyphOrigin(glyph,
								Vec2(penX, baseline) + glyph.bearing + shaped.offset),
							glyph.bitmapSize, Vec4(0) };
						if (!addGlyph(output, glyph, geometry, resourceIndex, style.color))
							return finish(false);
					}
					penX += glyph.advance;
				}
			}
			return finish(true);
		}

		for (SizeT cursor = 0;
			cursor < node.authored.text.size() && decoded < style.maximumCodepoints;
			++decoded)
		{
			const U32 codepoint = decodeOne(node.authored.text, cursor);
			if (codepoint == '\r') continue;
			if (codepoint == '\n')
			{
				penX = left;
				baseline += lineHeight;
				if (baseline - fallbackMetrics.ascent >= bottom) break;
				continue;
			}

			if (codepoint == '\t')
			{
				const Float tabAdvance = fallbackGlyph(' ', pixelSize).advance * 4.0f;
				if (style.wrap && penX > left && penX + tabAdvance > right)
					{ penX = left; baseline += lineHeight; }
				else penX += tabAdvance;
				if (baseline - fallbackMetrics.ascent >= bottom) break;
				continue;
			}

			UiTextGlyphRenderInfo glyph;
			Bool advanced = glyphProvider && glyphProvider(codepoint, pixelSize, glyph);
			advanced = advanced && glyph.resource.id != 0 &&
				glyph.resource.pool == poolFor(glyph.pipeline) &&
				glyph.advance > 0 && glyph.lineHeight > 0;
			if (!advanced) glyph = fallbackGlyph(codepoint, pixelSize);

			if (style.wrap && penX > left && penX + glyph.advance > right)
			{
				penX = left;
				baseline += lineHeight;
			}
			if (baseline - fallbackMetrics.ascent >= bottom) break;

			if (codepoint != ' ')
			{
				UInt resourceIndex = invalidUiResourceIndex;
				if (!output.addResource(glyph.resource, resourceIndex))
					return finish(false);
				const UiQuadGeometry geometry{
					glyphOrigin(glyph, Vec2(penX, baseline) + glyph.bearing),
					glyph.bitmapSize, Vec4(0) };
				if (!addGlyph(output, glyph, geometry, resourceIndex, style.color))
					return finish(false);
			}
			penX += glyph.advance;
		}
		return finish(true);
	}
}
