#pragma once

#include "UiTextShaper.hpp"

#include <condition_variable>
#include <unordered_map>

namespace Ui
{
	enum class UiGlyphRenderMode : Byte
	{
		Automatic,
		HintedGrayscale,
		Msdf,
		Mtsdf,
		ColorRgba,
	};

	enum class UiGlyphPixelFormat : Byte { R8, RGB8, RGBA8, MtsdfRGBA8 };

	enum class UiGlyphRasterFailure : Byte
	{
		None,
		InvalidRequest,
		BackendUnavailable,
		FontOpenFailed,
		VariationUnsupported,
		VariationAxisUnknown,
		VariationApplicationFailed,
		GlyphLoadFailed,
		DeterministicLimitExceeded,
		RasterizationFailed,
	};

	struct UiGlyphDistanceFieldMetadata
	{
		Bool enabled = false;
		Bool trueDistanceAlpha = false;
		Float pixelRange = 0;
		Float emScale = 0;
		Vec2 translation = Vec2(0);
	};

	struct UiGlyphRenderContext
	{
		Float pixelSize = 16;
		Bool screenSpace = true;
		Bool pixelAligned = true;
		Bool transformed = false;
		Bool worldSpace = false;
		Bool colorGlyph = false;
		Float grayscaleMaximumSize = 24;
	};

	[[nodiscard]] UiGlyphRenderMode selectUiGlyphRenderMode(
		const UiGlyphRenderContext& context);
	[[nodiscard]] UiGlyphPixelFormat uiGlyphPixelFormat(UiGlyphRenderMode mode);
	[[nodiscard]] UInt uiGlyphBytesPerPixel(UiGlyphPixelFormat format);
	[[nodiscard]] UiGlyphDistanceFieldMetadata uiGlyphDistanceFieldMetadata(
		UiGlyphRenderMode mode, Float pixelRange = 4);

	struct UiGlyphKey
	{
		UiFontFaceId faceId = invalidUiFontFaceId;
		UInt glyphId = 0;
		UInt pixelSize26_6 = 16 * 64;
		UiGlyphRenderMode mode = UiGlyphRenderMode::HintedGrayscale;
		U64 variation = 0;
		Int outline26_6 = 0;

		constexpr auto operator<=>(const UiGlyphKey&) const = default;
	};

	struct UiGlyphKeyHash
	{
		[[nodiscard]] SizeT operator()(const UiGlyphKey& key) const noexcept;
	};

	struct UiGlyphRect
	{
		UInt x = 0;
		UInt y = 0;
		UInt width = 0;
		UInt height = 0;
		constexpr auto operator<=>(const UiGlyphRect&) const = default;
	};

	struct UiGlyphAtlasHandle
	{
		UiGlyphPixelFormat format = UiGlyphPixelFormat::R8;
		UInt page = maxUInt;
		UInt pageGeneration = 0;
		UiGlyphRect rect;

		[[nodiscard]] Bool isValid() const
		{
			return page != maxUInt && pageGeneration != 0;
		}
	};

	// Deterministic bottom-left skyline allocator. Reset is intentionally public
	// so allocation policy can be tested without a renderer or cache.
	class UiGlyphSkylineAllocator
	{
	public:
		UiGlyphSkylineAllocator(UInt width = 0, UInt height = 0);
		void reset(UInt width, UInt height);
		[[nodiscard]] Bool allocate(UInt width, UInt height, UiGlyphRect& output);
		[[nodiscard]] UInt width() const { return atlasWidth; }
		[[nodiscard]] UInt height() const { return atlasHeight; }

	private:
		struct Node { UInt x = 0; UInt y = 0; UInt width = 0; };
		UInt atlasWidth = 0;
		UInt atlasHeight = 0;
		Vector<Node> skyline;
	};

	struct UiGlyphColorMetadata
	{
		Bool color = false;
		Bool premultipliedAlpha = false;
		UiGlyphColorFormat source = UiGlyphColorFormat::None;
		UInt palette = 0;
	};

	struct UiGlyphRasterRequest
	{
		UiGlyphKey key;
		Path fontPath;
		Int faceIndex = 0;
		Vector<UiVariableAxis> axes;
		UInt padding = 0; // Zero uses the cache's mode-specific padding.
	};

	struct UiGlyphRasterResult
	{
		UiGlyphKey key;
		UiGlyphPixelFormat format = UiGlyphPixelFormat::R8;
		UInt width = 0;
		UInt height = 0;
		Int bearingX26_6 = 0;
		Int bearingY26_6 = 0;
		Int advanceX26_6 = 0;
		UInt padding = 0;
		Vector<Byte> pixels;
		UiGlyphColorMetadata color;
		UiGlyphDistanceFieldMetadata distanceField;
		UiGlyphRasterFailure failure = UiGlyphRasterFailure::None;
		Bool succeeded = false;
	};

	using UiGlyphRasterizer = Function<Bool(
		const UiGlyphRasterRequest&, UiGlyphRasterResult&)>;
	using UiGlyphUploadCallback = Function<void(
		UiGlyphPixelFormat, UInt page, UInt pageGeneration,
		const UiGlyphRect&, const Byte* pixels, SizeT byteCount)>;

	struct UiGlyphCacheConfig
	{
		UInt pageWidth = 1024;
		UInt pageHeight = 1024;
		UInt maximumR8Pages = 4;
		UInt maximumRGB8Pages = 4;
		UInt maximumRGBA8Pages = 2;
		UInt workerCount = 1;
		SizeT requestQueueCapacity = 256;
		SizeT resultQueueCapacity = 256;
		UInt grayscalePadding = 1;
		UInt distanceFieldPadding = 4;
		UInt colorPadding = 1;
		UiGlyphRasterizer rasterizer;
		UiGlyphKey missingGlyph;
	};

	struct UiGlyphUploadBudget
	{
		SizeT maximumBytes = 4 * 1024 * 1024;
		UInt maximumGlyphs = 256;
	};

	struct UiGlyphCacheStats
	{
		SizeT residentGlyphs = 0;
		SizeT pendingGlyphs = 0;
		SizeT queuedRequests = 0;
		SizeT queuedResults = 0;
		U64 pageEvictions = 0;
	};

	class UiGlyphCache
	{
	public:
		explicit UiGlyphCache(UiGlyphCacheConfig config = {});
		~UiGlyphCache();
		UiGlyphCache(const UiGlyphCache&) = delete;
		UiGlyphCache& operator=(const UiGlyphCache&) = delete;

		void startWorkers();
		void stopWorkers();
		[[nodiscard]] Bool workersRunning() const;

		// Thread-safe. Repeated calls for an in-flight key enqueue only once.
		[[nodiscard]] Bool request(const UiGlyphRasterRequest& request);
		[[nodiscard]] Bool lookup(
			const UiGlyphKey& key, UiGlyphAtlasHandle& handle,
			UiGlyphRasterResult* metrics = nullptr);
		[[nodiscard]] UiGlyphAtlasHandle lookupOrMissing(const UiGlyphKey& key);

		// Must be called on the render thread. The callback owns the graphics API.
		[[nodiscard]] UInt drainUploads(
			const UiGlyphUploadCallback& upload,
			const UiGlyphUploadBudget& budget = {});

		// Synchronous render-thread warm-up, useful for common ASCII/UI glyphs.
		[[nodiscard]] UInt warmUp(
			const Vector<UiGlyphRasterRequest>& requests,
			const UiGlyphUploadCallback& upload);

		void clear();
		[[nodiscard]] UiGlyphCacheStats stats() const;
		[[nodiscard]] Bool isHandleCurrent(const UiGlyphAtlasHandle& handle) const;

	private:
		struct Impl;
		UniquePointer<Impl> impl;
	};

	// Built-in raster entry point. The default build emits a deterministic tofu
	// glyph for glyph zero. ORKA_UI_ENABLE_ADVANCED_TEXT adds FreeType loading.
	[[nodiscard]] Bool rasterUiGlyph(
		const UiGlyphRasterRequest& request, UiGlyphRasterResult& result);
}
