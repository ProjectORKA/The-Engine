#include "UiTextPainter.hpp"

#include <algorithm>
#include <chrono>
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

		[[nodiscard]] Bool hasClass(const UiNode& node, const String& name)
		{
			for (const String& styleClass : node.authored.classes)
				if (styleClass == name) return true;
			return false;
		}

		[[nodiscard]] Bool resolveContentBox(
			const UiNode& node, const UiTextPaintStyle& style,
			Vec2& boxPosition, Vec2& boxSize, Vec2& innerPosition, Vec2& innerSize)
		{
			boxPosition = node.computed.contentPosition;
			boxSize = node.computed.contentSize;
			if (boxSize.x <= 0 || boxSize.y <= 0)
			{
				boxPosition = node.computed.position;
				boxSize = node.computed.size;
			}
			if (boxSize.x <= 0 || boxSize.y <= 0) return false;
			innerPosition = boxPosition + glm::max(style.padding, Vec2(0));
			innerSize = glm::max(boxSize - glm::max(style.padding, Vec2(0)) * 2.0f, Vec2(0));
			return innerSize.x > 0 && innerSize.y > 0;
		}

		[[nodiscard]] Float resolvePixelSize(
			const UiTextPaintStyle& style, const Vec2 innerSize)
		{
			const Float boundedDefault = std::clamp(innerSize.y * 0.45f, 8.0f, 16.0f);
			return std::clamp(
				style.fontSize > 0 ? style.fontSize : boundedDefault, 4.0f, 256.0f);
		}

		void buildClusterBytes(const String& text, Vector<SizeT>& clusterBytes)
		{
			clusterBytes.clear();
			clusterBytes.push_back(0);
			SizeT cursor = 0;
			while (cursor < text.size())
			{
				(void)decodeOne(text, cursor);
				clusterBytes.push_back(cursor);
			}
		}

		[[nodiscard]] Float glyphAdvanceFor(
			const UiTextGlyphProvider& glyphProvider, const U32 codepoint, const Float pixelSize)
		{
			if (codepoint == '\t') return UiTextPainter::fallbackGlyph(' ', pixelSize).advance * 4.0f;
			UiTextGlyphRenderInfo glyph;
			const Bool advanced = glyphProvider && glyphProvider(codepoint, pixelSize, glyph) &&
				glyph.resource.id != 0 && glyph.advance > 0;
			if (!advanced) return UiTextPainter::fallbackGlyph(codepoint, pixelSize).advance;
			return glyph.advance;
		}

		void fillMetricsFromShaped(
			UiTextEditMetrics& metrics, const UiTextPaintLayout& shaped,
			const String& text, const Float left, const Float innerTop)
		{
			buildClusterBytes(text, metrics.clusterBytes);
			metrics.clusterX.assign(metrics.clusterBytes.size(), left);
			metrics.left = left;
			if (!shaped.layout.lines.empty())
			{
				const UiTextLine& line = shaped.layout.lines.front();
				metrics.baseline = innerTop + line.y + line.baseline;
				metrics.lineHeight = line.height > 0 ? line.height : shaped.layout.fontSize;
				metrics.ascent = line.baseline > 0 ? line.baseline : metrics.lineHeight * 0.8f;
			}
			else
			{
				metrics.baseline = innerTop + shaped.layout.fontSize * 0.8f;
				metrics.lineHeight = shaped.layout.fontSize;
				metrics.ascent = shaped.layout.fontSize * 0.8f;
			}

			Vector<Float> advances(metrics.clusterBytes.size() > 0
				? metrics.clusterBytes.size() - 1 : 0, 0.0f);
			if (!shaped.layout.lines.empty())
			{
				const UiTextLine& line = shaped.layout.lines.front();
				const UInt lineEnd = line.firstCodepoint + line.codepointCount;
				for (const UiShapedGlyphRenderInfo& shapedGlyph : shaped.glyphs)
				{
					if (shapedGlyph.codepointIndex < line.firstCodepoint ||
						shapedGlyph.codepointIndex >= lineEnd) continue;
					if (shapedGlyph.codepointIndex < advances.size())
						advances[shapedGlyph.codepointIndex] += shapedGlyph.glyph.advance;
				}
			}
			Float pen = left;
			metrics.clusterX[0] = pen;
			for (SizeT index = 0; index < advances.size(); ++index)
			{
				pen += advances[index];
				metrics.clusterX[index + 1] = pen;
			}
		}

		void fillMetricsFromFallback(
			UiTextEditMetrics& metrics, const UiTextGlyphProvider& glyphProvider,
			const String& text, const Float left, const Float innerTop,
			const Float pixelSize, const Float lineHeight, const Float ascent)
		{
			buildClusterBytes(text, metrics.clusterBytes);
			metrics.clusterX.clear();
			metrics.clusterX.reserve(metrics.clusterBytes.size());
			metrics.left = left;
			metrics.baseline = innerTop + ascent;
			metrics.ascent = ascent;
			metrics.lineHeight = lineHeight > 0 ? lineHeight : ascent + 2.0f;
			Float pen = left;
			metrics.clusterX.push_back(pen);
			SizeT cursor = 0;
			while (cursor < text.size())
			{
				const U32 codepoint = decodeOne(text, cursor);
				if (codepoint != '\r' && codepoint != '\n')
					pen += glyphAdvanceFor(glyphProvider, codepoint, pixelSize);
				metrics.clusterX.push_back(pen);
			}
		}

		[[nodiscard]] Bool caretBlinkVisible()
		{
			using namespace std::chrono;
			const U64 ms = static_cast<U64>(
				duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count());
			return ((ms / 530) % 2) == 0;
		}

		[[nodiscard]] Bool paintTextEditChrome(
			const UiNode& node, UiDisplayList& output, const UiTextPaintStyle& style,
			const UiTextEditMetrics& metrics, const Float innerHeight)
		{
			if (!node.state.focused || !node.state.semantics.showTextCaret) return true;
			const Bool placeholder = hasClass(node, "is-placeholder-visible");
			const SizeT textEnd = placeholder || metrics.clusterBytes.empty()
				? 0
				: metrics.clusterBytes.back();
			const SizeT caretByte = placeholder
				? 0
				: std::min(node.state.semantics.caretByte, textEnd);
			const SizeT selectionStart = placeholder
				? 0
				: std::min(node.state.semantics.selectionStart, textEnd);
			const SizeT selectionEnd = placeholder
				? 0
				: std::min(node.state.semantics.selectionEnd, textEnd);
			const SizeT rangeStart = std::min(selectionStart, selectionEnd);
			const SizeT rangeEnd = std::max(selectionStart, selectionEnd);

			const Float top = metrics.baseline - metrics.ascent;
			// Match glyph ink height (ascent + small descent), not the larger line box.
			const Float height = std::min(metrics.ascent + std::max(2.0f, metrics.ascent * 0.2f),
				std::max(innerHeight, metrics.ascent));

			if (rangeStart < rangeEnd)
			{
				const Float startX = metrics.xAtByte(rangeStart);
				const Float endX = metrics.xAtByte(rangeEnd);
				const UiQuadGeometry selection{
					Vec2(startX, top),
					Vec2(std::max(endX - startX, 1.0f), height),
					Vec4(0) };
				if (!output.addSolidQuad(selection, style.selectionColor)) return false;
			}

			if (!caretBlinkVisible()) return true;
			const Float caretX = metrics.xAtByte(caretByte);
			const UiQuadGeometry caret{
				Vec2(std::floor(caretX), top),
				Vec2(2.0f, height),
				Vec4(0) };
			return output.addSolidQuad(caret, style.caretColor);
		}
	}

	Float UiTextEditMetrics::xAtByte(const SizeT byteOffset) const
	{
		if (clusterBytes.empty() || clusterX.empty()) return left;
		SizeT best = 0;
		for (SizeT index = 0; index < clusterBytes.size(); ++index)
		{
			if (clusterBytes[index] <= byteOffset) best = index;
			else break;
		}
		return clusterX[std::min(best, clusterX.size() - 1)];
	}

	SizeT UiTextEditMetrics::byteAtX(const Float x) const
	{
		if (clusterBytes.empty()) return 0;
		for (SizeT index = 0; index + 1 < clusterX.size(); ++index)
		{
			const Float mid = (clusterX[index] + clusterX[index + 1]) * 0.5f;
			if (x < mid) return clusterBytes[index];
		}
		return clusterBytes.back();
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

	Bool UiTextPainter::buildEditMetrics(
		const UiNode& node, UiTextPaintStyle style, UiTextEditMetrics& metrics) const
	{
		if (styleProvider) styleProvider(node, style);
		Vec2 boxPosition, boxSize, innerPosition, innerSize;
		if (!resolveContentBox(node, style, boxPosition, boxSize, innerPosition, innerSize))
			return false;

		const Float pixelSize = resolvePixelSize(style, innerSize);
		const UiTextGlyphRenderInfo fallbackMetrics = fallbackGlyph('M', pixelSize);
		const Float lineHeight = style.lineHeight > 0
			? style.lineHeight : fallbackMetrics.lineHeight;
		const Float left = innerPosition.x;
		const Bool placeholder = hasClass(node, "is-placeholder-visible");
		const String text = placeholder ? String{} : node.authored.text;

		UiTextPaintLayout shapedLayout;
		if (!placeholder && shapeProvider &&
			shapeProvider(node, style, innerSize, shapedLayout) &&
			!shapedLayout.layout.lines.empty())
		{
			fillMetricsFromShaped(metrics, shapedLayout, text, left, innerPosition.y);
			return true;
		}

		fillMetricsFromFallback(metrics, glyphProvider, text, left, innerPosition.y,
			pixelSize, lineHeight, fallbackMetrics.ascent);
		return true;
	}

	Bool UiTextPainter::paint(
		const UiNode& node, UiDisplayList& output, UiTextPaintStyle style) const
	{
		const Bool paintCaretOnly = node.authored.text.empty() &&
			node.state.focused && node.state.semantics.showTextCaret;
		if (node.authored.text.empty() && !paintCaretOnly) return true;
		if (styleProvider) styleProvider(node, style);

		Vec2 boxPosition, boxSize, innerPosition, innerSize;
		if (!resolveContentBox(node, style, boxPosition, boxSize, innerPosition, innerSize))
			return true;

		const Float pixelSize = resolvePixelSize(style, innerSize);
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
		const Bool shaped = !paintCaretOnly && shapeProvider &&
			shapeProvider(node, style, innerSize, shapedLayout);

		UiTextEditMetrics editMetrics;
		if (shaped)
			fillMetricsFromShaped(editMetrics, shapedLayout,
				hasClass(node, "is-placeholder-visible") ? String{} : node.authored.text,
				left, innerPosition.y);
		else
			fillMetricsFromFallback(editMetrics, glyphProvider,
				hasClass(node, "is-placeholder-visible") ? String{} : node.authored.text,
				left, innerPosition.y, pixelSize, lineHeight, fallbackMetrics.ascent);

		if (!paintTextEditChrome(node, output, style, editMetrics, innerSize.y))
			return finish(false);
		if (paintCaretOnly) return finish(true);

		Float horizontalOffset = 0;
		if (style.horizontalAlign == UiTextHorizontalAlign::Center &&
			!editMetrics.clusterX.empty())
		{
			const Float textWidth = editMetrics.clusterX.back() - editMetrics.left;
			horizontalOffset = std::max(0.0f, (innerSize.x - textWidth) * 0.5f);
		}

		if (shaped)
		{
			for (const UiTextLine& line : shapedLayout.layout.lines)
			{
				penX = left + horizontalOffset;
				baseline = innerPosition.y + line.y + line.baseline;
				const UInt lineEnd =
					line.firstCodepoint + line.codepointCount;
				for (const UiShapedGlyphRenderInfo& shapedGlyph : shapedLayout.glyphs)
				{
					if (shapedGlyph.codepointIndex < line.firstCodepoint ||
						shapedGlyph.codepointIndex >= lineEnd) continue;
					const UiTextGlyphRenderInfo& glyph = shapedGlyph.glyph;
					if (shapedGlyph.codepoint != ' ' && shapedGlyph.codepoint != '\t' &&
						shapedGlyph.codepoint != '\r' && shapedGlyph.codepoint != '\n')
					{
						UInt resourceIndex = invalidUiResourceIndex;
						if (!output.addResource(glyph.resource, resourceIndex))
							return finish(false);
						const UiQuadGeometry geometry{
							glyphOrigin(glyph,
								Vec2(penX, baseline) + glyph.bearing + shapedGlyph.offset),
							glyph.bitmapSize, Vec4(0) };
						if (!addGlyph(output, glyph, geometry, resourceIndex, style.color))
							return finish(false);
					}
					penX += glyph.advance;
				}
			}
			return finish(true);
		}

		penX = left + horizontalOffset;
		for (SizeT cursor = 0;
			cursor < node.authored.text.size() && decoded < style.maximumCodepoints;
			++decoded)
		{
			const U32 codepoint = decodeOne(node.authored.text, cursor);
			if (codepoint == '\r') continue;
			if (codepoint == '\n')
			{
				penX = left + horizontalOffset;
				baseline += lineHeight;
				if (baseline - fallbackMetrics.ascent >= bottom) break;
				continue;
			}

			if (codepoint == '\t')
			{
				const Float tabAdvance = fallbackGlyph(' ', pixelSize).advance * 4.0f;
				if (style.wrap && penX > left + horizontalOffset && penX + tabAdvance > right)
					{ penX = left + horizontalOffset; baseline += lineHeight; }
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

			if (style.wrap && penX > left + horizontalOffset && penX + glyph.advance > right)
			{
				penX = left + horizontalOffset;
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
