#pragma once

#include "UiDisplayList.hpp"
#include "../Text/UiGlyphCache.hpp"

struct Renderer;

namespace Ui
{
	struct UiRendererCapabilities
	{
		Bool initialized = false;
		Bool persistentMappedStreaming = false;
		Bool orphanedStreamingFallback = true;
		Bool rectangularScissor = true;
		Bool nonRectClipStencil = false;
		Bool worldSurfaceCallback = true;
		Bool pooledLayerCallbacks = true;
	};

	struct UiRendererCallbacks
	{
		// Resolve and bind an image or atlas page to texture unit zero.
		Function<Bool(Renderer&, const UiResourceRef&)> bindTexture;

		// Called before a world-surface packet. Return false to reject the draw.
		// The callback may configure view/projection state; localToWorld is applied
		// by the backend's world-space vertex shader.
		Function<Bool(Renderer&, const UiWorldSurfaceMetadata&)> beginWorldSurface;
		Function<void(Renderer&, const UiWorldSurfaceMetadata&)> endWorldSurface;

		// Isolated layers can be redirected to a pooled render target here.
		Function<Bool(Renderer&, const UiBeginLayerCommand&, UInt)> beginLayer;
		Function<void(Renderer&, const UiBeginLayerCommand&, UInt)> endLayer;
	};

	class UiRenderer
	{
	public:
		UiRenderer();
		~UiRenderer();
		UiRenderer(const UiRenderer&) = delete;
		UiRenderer& operator=(const UiRenderer&) = delete;

		void create(Renderer& renderer);
		void destroy(Renderer& renderer);
		void submit(Renderer& renderer, const UiRenderPacket& packet);
		void setCallbacks(UiRendererCallbacks callbacks);
		void uploadGlyphAtlas(
			Renderer& renderer, const UiResourceRef& resource,
			UiGlyphPixelFormat format, UInt width, UInt height,
			const UiGlyphRect& rect, const Byte* pixels, SizeT byteCount);
		void uploadImageTexture(
			Renderer& renderer, const UiResourceRef& resource, U32 ownerId,
			UInt width, UInt height, const Byte* rgbaPixels, SizeT byteCount);
		void releaseGlyphAtlases(U32 serviceId);
		void releaseImageTextures(U32 ownerId);

		[[nodiscard]] const UiRendererCapabilities& capabilities() const;

	private:
		struct Implementation;
		UniquePointer<Implementation> implementation;
	};

	// Renderer owns the backend lifecycle; hosts submit through this narrow
	// boundary so Renderer.hpp does not depend on the retained UI headers.
	void submitUi(Renderer& renderer, const UiRenderPacket& packet);
	void setUiRendererCallbacks(
		Renderer& renderer, UiRendererCallbacks callbacks);

	// Atlas resource IDs carry service, pixel format, and page. The generation
	// remains explicit so stale display-list handles fail resolution after reuse.
	[[nodiscard]] UiResourceRef makeUiGlyphResource(
		U32 serviceId, UiGlyphPixelFormat format, UInt page, UInt generation);
	[[nodiscard]] Bool decodeUiGlyphResource(
		const UiResourceRef& resource, U32& serviceId,
		UiGlyphPixelFormat& format, UInt& page);
	void uploadUiGlyphAtlas(
		Renderer& renderer, const UiResourceRef& resource,
		UiGlyphPixelFormat format, UInt width, UInt height,
		const UiGlyphRect& rect, const Byte* pixels, SizeT byteCount);
	void releaseUiGlyphAtlases(Renderer& renderer, U32 serviceId);
	void uploadUiImageTexture(
		Renderer& renderer, const UiResourceRef& resource, U32 ownerId,
		UInt width, UInt height, const Byte* rgbaPixels, SizeT byteCount);
	void releaseUiImageTextures(Renderer& renderer, U32 ownerId);
}
