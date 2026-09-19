#include "UiFontService.hpp"

#include "../Render/UiRenderer.hpp"

#include <atomic>

namespace Ui
{
	namespace
	{
		std::atomic<U32> nextFontServiceId{ 1 };

		[[nodiscard]] UiPipeline pipelineFor(const UiGlyphPixelFormat format)
		{
			if (format == UiGlyphPixelFormat::RGB8 ||
				format == UiGlyphPixelFormat::MtsdfRGBA8)
				return UiPipeline::MsdfGlyph;
			if (format == UiGlyphPixelFormat::RGBA8) return UiPipeline::ColorGlyph;
			return UiPipeline::GrayscaleGlyph;
		}
	}

	UiFontService::UiFontService(UiFontServiceConfig supplied)
		: config(std::move(supplied)),
		  shaper(config.shaper),
		  glyphCache(config.glyphCache),
		  id(nextFontServiceId.fetch_add(1))
	{
		if (!id) id = nextFontServiceId.fetch_add(1);
	}

	UiFontService::~UiFontService()
	{
		glyphCache.stopWorkers();
	}

	UiFontFaceId UiFontService::registerFontPath(
		const String& resourceName, const Path& resolvedPath,
		const String& family, const String& style, const Int faceIndex)
	{
		if (resourceName.empty() || resolvedPath.empty() || family.empty())
			return invalidUiFontFaceId;
		UiFontFaceDescriptor descriptor{
			resourceName, resolvedPath, family, style, faceIndex };
		const UiFontFaceId face = shaper.registerFace(descriptor);
		descriptors[face] = descriptor;
		UiFontFamilyDescriptor& registered = families[family];
		registered.name = family;
		registered.faces.push_back(face);
		shaper.registerFamily(registered);
		++metricsRevision;
		asciiWarm = false;
		return face;
	}

	void UiFontService::registerFallbackFamily(
		const String& family, Vector<String> fallbackFamilies)
	{
		if (family.empty()) return;
		UiFontFamilyDescriptor& registered = families[family];
		registered.name = family;
		registered.fallbackFamilies = std::move(fallbackFamilies);
		shaper.registerFamily(registered);
		++metricsRevision;
		asciiWarm = false;
	}

	Bool UiFontService::setDefaultFamily(const String& family)
	{
		if (!families.contains(family)) return false;
		selectedFamily = family;
		++metricsRevision;
		asciiWarm = false;
		return true;
	}

	Bool UiFontService::available() const
	{
		if (selectedFamily.empty()) return false;
		if (!config.allowFallbackShaper && !shaper.advancedBackendAvailable())
			return false;
		const auto found = families.find(selectedFamily);
		if (found == families.end() || found->second.faces.empty()) return false;
		if (config.allowFallbackShaper) return true;
		for (const UiFontFaceId face : found->second.faces)
			if (shaper.faceMetadata(face).loaded) return true;
		return false;
	}

	void UiFontService::start()
	{
		glyphCache.startWorkers();
	}

	UiGlyphRasterRequest UiFontService::rasterRequest(
		const UiTextGlyph& glyph, const Float pixelSize) const
	{
		UiGlyphRasterRequest request;
		request.key.faceId = glyph.faceId;
		request.key.glyphId = glyph.glyphId;
		request.key.pixelSize26_6 = static_cast<UInt>(
			std::max(1.0f, std::round(pixelSize * 64.0f)));
		request.key.mode = UiGlyphRenderMode::HintedGrayscale;
		const UiFontFaceMetadata metadata = shaper.faceMetadata(glyph.faceId);
		if (metadata.colorFormats != static_cast<UInt>(UiGlyphColorFormat::None))
			request.key.mode = UiGlyphRenderMode::ColorRgba;
		if (const auto found = descriptors.find(glyph.faceId);
			found != descriptors.end())
		{
			request.fontPath = found->second.resourcePath;
			request.faceIndex = found->second.faceIndex;
		}
		return request;
	}

	UInt UiFontService::warmCommonAscii(Renderer& renderer)
	{
		if (asciiWarm || !available()) return 0;
		String ascii;
		for (U32 codepoint = 32; codepoint <= 126; ++codepoint)
			ascii.push_back(static_cast<Char>(codepoint));
		UiTextShapeRequest shapeRequest;
		shapeRequest.utf8 = ascii;
		shapeRequest.family = selectedFamily;
		const SharedPointer<const UiShapedText> shaped = shaper.shape(shapeRequest);
		Vector<UiGlyphRasterRequest> requests;
		requests.reserve(shaped->glyphs.size() + 1);
		UiGlyphRasterRequest missing;
		missing.key = config.glyphCache.missingGlyph;
		requests.push_back(missing);
		for (const UiTextGlyph& glyph : shaped->glyphs)
			requests.push_back(rasterRequest(glyph, 16));
		auto upload = [&](const UiGlyphPixelFormat format, const UInt page,
			const UInt generation, const UiGlyphRect& rect,
			const Byte* pixels, const SizeT byteCount)
			{
				uploadUiGlyphAtlas(renderer,
					makeUiGlyphResource(id, format, page, generation),
					format, config.glyphCache.pageWidth,
					config.glyphCache.pageHeight, rect, pixels, byteCount);
			};
		const UInt warmed = glyphCache.warmUp(requests, upload);
		asciiWarm = true;
		return warmed;
	}

	Bool UiFontService::prepareRender(Renderer& renderer)
	{
		if (!available()) return false;
		const UInt warmed = warmCommonAscii(renderer);
		auto upload = [&](const UiGlyphPixelFormat format, const UInt page,
			const UInt generation, const UiGlyphRect& rect,
			const Byte* pixels, const SizeT byteCount)
			{
				uploadUiGlyphAtlas(renderer,
					makeUiGlyphResource(id, format, page, generation),
					format, config.glyphCache.pageWidth,
					config.glyphCache.pageHeight, rect, pixels, byteCount);
			};
		const UInt drained =
			glyphCache.drainUploads(upload, config.uploadBudget);
		return warmed != 0 || drained != 0;
	}

	void UiFontService::shutdown(Renderer& renderer)
	{
		glyphCache.stopWorkers();
		glyphCache.clear();
		releaseUiGlyphAtlases(renderer, id);
		asciiWarm = false;
	}

	Bool UiFontService::shapeNode(
		const Float pixelSize,
		const SharedPointer<const UiShapedText>& shaped,
		Vector<UiShapedGlyphRenderInfo>& output)
	{
		output.clear();
		if (!available() || !shaped || shaped->glyphs.empty()) return false;

		output.reserve(shaped->glyphs.size());
		for (const UiTextRun& run : shaped->runs)
		{
			const Float scale = pixelSize / std::max(run.unitsPerEm, 1.0f);
			for (UInt index = 0; index < run.glyphCount; ++index)
			{
				const UiTextGlyph& source = shaped->glyphs[run.firstGlyph + index];
				const UiGlyphRasterRequest glyphRequest =
					rasterRequest(source, pixelSize);
				UiGlyphAtlasHandle handle;
				UiGlyphRasterResult metrics;
				Bool resident = glyphCache.lookup(
					glyphRequest.key, handle, &metrics);
				if (!resident)
				{
					(void)glyphCache.request(glyphRequest);
					if (!glyphCache.lookup(
						config.glyphCache.missingGlyph, handle, &metrics))
					{
						return false;
					}
				}
				UiShapedGlyphRenderInfo destination;
				destination.codepointIndex = source.cluster;
				destination.codepoint = source.cluster < shaped->codepoints.size() ?
					shaped->codepoints[source.cluster].value : 0xfffd;
				destination.glyph.resource =
					makeUiGlyphResource(id, handle.format, handle.page,
						handle.pageGeneration);
				destination.glyph.pipeline = pipelineFor(handle.format);
				destination.glyph.uvRect = Vec4(
					static_cast<Float>(handle.rect.x) / config.glyphCache.pageWidth,
					static_cast<Float>(handle.rect.y) / config.glyphCache.pageHeight,
					static_cast<Float>(handle.rect.x + handle.rect.width) /
						config.glyphCache.pageWidth,
					static_cast<Float>(handle.rect.y + handle.rect.height) /
						config.glyphCache.pageHeight);
				destination.glyph.bitmapSize =
					Vec2(static_cast<Float>(metrics.width),
						static_cast<Float>(metrics.height));
				destination.glyph.bearing = Vec2(
					metrics.bearingX26_6 / 64.0f,
					-metrics.bearingY26_6 / 64.0f);
				destination.glyph.advance = source.advanceX * scale;
				destination.glyph.ascent = run.ascent * scale;
				destination.glyph.lineHeight =
					(run.ascent + run.descent + run.lineGap) * scale;
				destination.glyph.distanceRange =
					metrics.distanceField.pixelRange;
				destination.offset = Vec2(
					source.offsetX * scale, -source.offsetY * scale);
				output.push_back(std::move(destination));
			}
		}
		return !output.empty();
	}

	UiPreparedText UiFontService::prepareNode(
		const UiNode& node, SharedPointer<const UiShapedText>& shaped)
	{
		if (available())
		{
			UiTextShapeRequest request;
			request.utf8 = node.authored.text;
			request.family = selectedFamily;
			request.contentGeneration = node.computed.styleRevision;
			shaped = shaper.shape(request);
			if (shaped) return shaped->prepareForLayout();
		}

		UiTextPrepareOptions fallback;
		fallback.designUnitsPerEm = 8;
		fallback.ascent = 7;
		fallback.descent = 1;
		fallback.glyphAdvance = [](const U32 codepoint)
			{ return codepoint == '\n' || codepoint == '\r' ? 0.0f : 6.0f; };
		return UiPreparedText::prepareUtf8(node.authored.text, fallback);
	}

	namespace
	{
		UiTextLayoutOptions layoutOptionsFor(const UiTextPaintStyle& style)
		{
			UiTextLayoutOptions options;
			options.fontSize = style.fontSize > 0 ? style.fontSize : 16.0f;
			options.lineHeight = style.lineHeight;
			options.wrap = style.wrap ? style.wrapPolicy : UiTextWrap::KeepAll;
			return options;
		}

		UiTextFitOptions fitOptionsFor(
			const UiTextPaintStyle& style, const UiTextLayoutOptions& options)
		{
			UiTextFitOptions fit = style.fit;
			if (fit.mode != UiTextFit::None && style.fontSize > 0)
				fit.preferredSize = options.fontSize;
			return fit;
		}
	}

	UiIntrinsicMeasure UiFontService::measureIntrinsic(
		const UiNode& node, const UiConstraints& constraints,
		UiTextPaintStyle style, UiTextLayoutResult* layoutResult)
	{
		const Vec2 padding = glm::max(style.padding, Vec2(0)) * 2.0f;
		UiConstraints inner;
		inner.minimum = glm::max(constraints.minimum - padding, Vec2(0));
		inner.maximum.x = constraints.hasBoundedWidth() ?
			std::max(0.0f, constraints.maximum.x - padding.x) : uiLayoutInfinity;
		inner.maximum.y = constraints.hasBoundedHeight() ?
			std::max(0.0f, constraints.maximum.y - padding.y) : uiLayoutInfinity;

		SharedPointer<const UiShapedText> shaped;
		const UiPreparedText prepared = prepareNode(node, shaped);
		const UiTextLayoutOptions options = layoutOptionsFor(style);
		const UiTextFitOptions fit = fitOptionsFor(style, options);
		const UiTextLineLayout lineLayout(prepared);
		UiTextLayoutResult arranged;
		if (fit.mode != UiTextFit::None &&
			inner.hasBoundedWidth() && inner.hasBoundedHeight())
			arranged = lineLayout.fit(inner.maximum, options, fit);
		else
			arranged = lineLayout.layout(
				inner.hasBoundedWidth() ? inner.maximum.x : uiLayoutInfinity,
				options);
		const UiTextIntrinsicWidths widths =
			lineLayout.intrinsicWidths(arranged.fontSize, options.wrap);
		const UiIntrinsicMeasure measured{
			Vec2(widths.minimum, arranged.size.y),
			inner.constrain(arranged.size), arranged.baseline };
		if (layoutResult) *layoutResult = arranged;
		return {
			constraints.constrain(measured.minimum + padding),
			constraints.constrain(measured.preferred + padding),
			measured.baseline < 0 ? -1 : measured.baseline + style.padding.y
		};
	}

	Bool UiFontService::preparePaintLayout(
		const UiNode& node, const UiTextPaintStyle& style,
		const Vec2 definiteBox, UiTextPaintLayout& output)
	{
		output = {};
		if (node.authored.text.empty()) return true;
		SharedPointer<const UiShapedText> shaped;
		const UiPreparedText prepared = prepareNode(node, shaped);
		const UiTextLayoutOptions options = layoutOptionsFor(style);
		const UiTextFitOptions fit = fitOptionsFor(style, options);
		const Vec2 safeBox = glm::max(definiteBox, Vec2(0));
		const UiTextLineLayout lineLayout(prepared);
		output.layout = lineLayout.fit(safeBox, options, fit);

		if (shapeNode(output.layout.fontSize, shaped, output.glyphs))
			return true;

		output.glyphs.reserve(prepared.codepoints().size());
		for (UInt index = 0; index < prepared.codepoints().size(); ++index)
		{
			UiShapedGlyphRenderInfo glyph;
			glyph.codepoint = prepared.codepoints()[index].value;
			glyph.codepointIndex = index;
			glyph.glyph = UiTextPainter::fallbackGlyph(
				glyph.codepoint, output.layout.fontSize);
			output.glyphs.push_back(std::move(glyph));
		}
		return true;
	}

	U64 UiFontService::metricsGeneration() const
	{
		return shaper.registryGeneration() ^ (metricsRevision << 32);
	}

	UiTextShapeProvider UiFontService::shapeProvider()
	{
		return [this](const UiNode& node, const UiTextPaintStyle& style,
			const Vec2 definiteBox, UiTextPaintLayout& output)
			{ return preparePaintLayout(node, style, definiteBox, output); };
	}
}
