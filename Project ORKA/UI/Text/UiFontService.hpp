#pragma once

#include "UiGlyphCache.hpp"
#include "UiTextPainter.hpp"

struct Renderer;

namespace Ui
{
	struct UiFontServiceConfig
	{
		UiTextShaperConfig shaper;
		UiGlyphCacheConfig glyphCache;
		UiGlyphUploadBudget uploadBudget;
		// Enables deterministic dependency-free service validation only.
		Bool allowFallbackShaper = false;
	};

	// Per-host text bridge. Font paths are accepted only through this engine API;
	// authored markup can select registered family names but cannot provide paths.
	class UiFontService
	{
	public:
		explicit UiFontService(UiFontServiceConfig config = {});
		~UiFontService();
		UiFontService(const UiFontService&) = delete;
		UiFontService& operator=(const UiFontService&) = delete;

		[[nodiscard]] UiFontFaceId registerFontPath(
			const String& resourceName, const Path& resolvedPath,
			const String& family, const String& style = "Regular",
			Int faceIndex = 0);
		void registerFallbackFamily(
			const String& family, Vector<String> fallbackFamilies);
		[[nodiscard]] Bool setDefaultFamily(const String& family);
		[[nodiscard]] const String& defaultFamily() const { return selectedFamily; }
		[[nodiscard]] Bool available() const;

		// Called by UiHost on the render thread before painting.
		[[nodiscard]] Bool prepareRender(Renderer& renderer);
		void start();
		void shutdown(Renderer& renderer);

		[[nodiscard]] UiTextShapeProvider shapeProvider();
		[[nodiscard]] UiIntrinsicMeasure measureIntrinsic(
			const UiNode& node, const UiConstraints& constraints,
			UiTextPaintStyle style = {},
			UiTextLayoutResult* layoutResult = nullptr);
		[[nodiscard]] Bool preparePaintLayout(
			const UiNode& node, const UiTextPaintStyle& style,
			Vec2 definiteBox, UiTextPaintLayout& output);
		[[nodiscard]] U64 metricsGeneration() const;
		[[nodiscard]] UiGlyphCacheStats stats() const { return glyphCache.stats(); }
		[[nodiscard]] U32 serviceId() const { return id; }

	private:
		UiFontServiceConfig config;
		UiTextShaper shaper;
		UiGlyphCache glyphCache;
		Map<UiFontFaceId, UiFontFaceDescriptor> descriptors;
		Map<String, UiFontFamilyDescriptor> families;
		String selectedFamily;
		U32 id = 0;
		U64 metricsRevision = 1;
		Bool asciiWarm = false;

		[[nodiscard]] Bool shapeNode(
			Float pixelSize, const SharedPointer<const UiShapedText>& shaped,
			Vector<UiShapedGlyphRenderInfo>& output);
		[[nodiscard]] UiPreparedText prepareNode(
			const UiNode& node, SharedPointer<const UiShapedText>& shaped);
		[[nodiscard]] UiGlyphRasterRequest rasterRequest(
			const UiTextGlyph& glyph, Float pixelSize) const;
		[[nodiscard]] UInt warmCommonAscii(Renderer& renderer);
	};
}
