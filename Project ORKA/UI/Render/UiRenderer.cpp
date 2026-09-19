#include "UiRenderer.hpp"

#include "../../Renderer.hpp"
#include "../../Logger.hpp"
#include "../Text/UiTextPainter.hpp"

#include <cstring>

namespace Ui
{
	namespace
	{
		constexpr UInt streamSegmentCount = 3;
		constexpr ULL minimumStreamBytes = 64 * 1024;
		constexpr U64 glyphResourceMagic = 0x5547ULL;

		struct StreamBuffer
		{
			BufferID id = 0;
			Byte* mapped = nullptr;
			ULL segmentBytes = 0;
			UInt segment = 0;
			Bool persistent = false;
			External::GLsync fences[streamSegmentCount] = {};

			void release()
			{
				for (External::GLsync& fence : fences)
				{
					if (fence) External::glDeleteSync(fence);
					fence = nullptr;
				}
				if (mapped && id) External::glUnmapNamedBuffer(id);
				if (id) External::glDeleteBuffers(1, &id);
				id = 0;
				mapped = nullptr;
				segmentBytes = 0;
			}

			void ensure(const ULL requiredBytes, const Bool usePersistent)
			{
				const ULL requested = std::max(minimumStreamBytes, requiredBytes);
				if (id && persistent == usePersistent && requested <= segmentBytes) return;
				release();

				persistent = usePersistent;
				segmentBytes = 1;
				while (segmentBytes < requested) segmentBytes *= 2;
				External::glCreateBuffers(1, &id);
				if (persistent)
				{
					const External::GLbitfield flags =
						GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
					External::glNamedBufferStorage(id,
						static_cast<External::GLsizeiptr>(segmentBytes * streamSegmentCount),
						nullptr, flags);
					mapped = static_cast<Byte*>(External::glMapNamedBufferRange(id, 0,
						static_cast<External::GLsizeiptr>(segmentBytes * streamSegmentCount), flags));
					if (!mapped)
					{
						release();
						ensure(requiredBytes, false);
					}
				}
				else
				{
					External::glNamedBufferData(id, static_cast<External::GLsizeiptr>(segmentBytes),
						nullptr, GL_STREAM_DRAW);
				}
			}

			[[nodiscard]] ULL beginFrame()
			{
				if (!persistent)
				{
					External::glNamedBufferData(id, static_cast<External::GLsizeiptr>(segmentBytes),
						nullptr, GL_STREAM_DRAW);
					return 0;
				}

				segment = (segment + 1) % streamSegmentCount;
				if (fences[segment])
				{
					while (true)
					{
						const External::GLenum status = External::glClientWaitSync(
							fences[segment], GL_SYNC_FLUSH_COMMANDS_BIT, 1000000);
						if (status == GL_ALREADY_SIGNALED || status == GL_CONDITION_SATISFIED) break;
						if (status == GL_WAIT_FAILED) break;
					}
					External::glDeleteSync(fences[segment]);
					fences[segment] = nullptr;
				}
				return segmentBytes * segment;
			}

			void upload(const ULL offset, const void* source, const ULL bytes) const
			{
				if (bytes == 0) return;
				if (persistent) std::memcpy(mapped + offset, source, static_cast<SizeT>(bytes));
				else External::glNamedBufferSubData(id, static_cast<External::GLintptr>(offset),
					static_cast<External::GLsizeiptr>(bytes), source);
			}

			void finishFrame()
			{
				if (persistent)
					fences[segment] = External::glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
			}
		};

		struct alignas(16) GpuQuad
		{
			Vec4 originAxisX = Vec4(0);
			Vec4 axisYSize = Vec4(0);
			Vec4 cornerRadii = Vec4(0);
			Vec4 colors[4] = {};
			Vec4 uvRect = Vec4(0, 0, 1, 1);
			Vec4 gradient = Vec4(0, 0, 1, 0);
			Vec4 parameters = Vec4(0);
		};

		struct GpuMeshVertex
		{
			Vec2 position = Vec2(0);
			Vec2 uv = Vec2(0);
			Vec4 color = Vec4(0);
		};

		struct BatchState
		{
			glm::mat3 transform = glm::mat3(1);
			Float opacity = 1;
		};

		[[nodiscard]] Vec2 transformPoint(const glm::mat3& transform, const Vec2 point)
		{
			const glm::vec3 result = transform * glm::vec3(point, 1);
			return Vec2(result);
		}

		[[nodiscard]] const Char* shaderName(const UiPipeline pipeline)
		{
			switch (pipeline)
			{
			case UiPipeline::ImageQuad: return "uiModernImage";
			case UiPipeline::GrayscaleGlyph: return "uiModernGrayscaleGlyph";
			case UiPipeline::MsdfGlyph: return "uiModernMsdf";
			case UiPipeline::ColorGlyph: return "uiModernColorGlyph";
			case UiPipeline::PathMesh: return "uiModernPath";
			default: return "uiModernSolid";
			}
		}

		[[nodiscard]] UiClipRect clipBounds(const UiClipShape& shape)
		{
			if (const auto* rect = std::get_if<UiClipRect>(&shape)) return *rect;
			if (const auto* rounded = std::get_if<UiClipRoundedRect>(&shape)) return rounded->rect;
			const UiClipTransformed& transformed = std::get<UiClipTransformed>(shape);
			const Vec2 p0 = transformPoint(transformed.transform, transformed.shape.rect.position);
			const Vec2 p1 = transformPoint(transformed.transform,
				transformed.shape.rect.position + Vec2(transformed.shape.rect.size.x, 0));
			const Vec2 p2 = transformPoint(transformed.transform,
				transformed.shape.rect.position + Vec2(0, transformed.shape.rect.size.y));
			const Vec2 p3 = transformPoint(transformed.transform,
				transformed.shape.rect.position + transformed.shape.rect.size);
			const Vec2 minimum = glm::min(glm::min(p0, p1), glm::min(p2, p3));
			const Vec2 maximum = glm::max(glm::max(p0, p1), glm::max(p2, p3));
			return { minimum, maximum - minimum };
		}

		[[nodiscard]] UiClipRect effectiveClipBounds(const UiRenderPacket& packet, UInt clipState)
		{
			UiClipRect result = clipBounds(packet.clips()[clipState - 1].shape);
			UInt parent = packet.clips()[clipState - 1].parentClipState;
			while (parent != 0)
			{
				const UiClipRect parentBounds = clipBounds(packet.clips()[parent - 1].shape);
				const Vec2 start = glm::max(result.position, parentBounds.position);
				const Vec2 end = glm::min(result.position + result.size,
					parentBounds.position + parentBounds.size);
				result = { start, glm::max(end - start, Vec2(0)) };
				parent = packet.clips()[parent - 1].parentClipState;
			}
			return result;
		}

		[[nodiscard]] Vector<BatchState> buildBatchStates(const UiRenderPacket& packet)
		{
			Vector<BatchState> result(packet.batches().size());
			Vector<glm::mat3> transforms{ glm::mat3(1) };
			Vector<Float> opacity{ 1 };
			SizeT commandCursor = 0;
			for (SizeT batchIndex = 0; batchIndex < packet.batches().size(); ++batchIndex)
			{
				const UiRenderBatch& batch = packet.batches()[batchIndex];
				while (commandCursor < batch.firstCommand)
				{
					const UiDisplayCommand& command = packet.commands()[commandCursor++];
					if (const auto* push = std::get_if<UiPushTransformCommand>(&command))
						transforms.push_back(transforms.back() * push->transform);
					else if (std::holds_alternative<UiPopTransformCommand>(command) && transforms.size() > 1)
						transforms.pop_back();
					else if (const auto* begin = std::get_if<UiBeginLayerCommand>(&command))
						opacity.push_back(opacity.back() * begin->opacity);
					else if (std::holds_alternative<UiEndLayerCommand>(command) && opacity.size() > 1)
						opacity.pop_back();
				}
				result[batchIndex] = { transforms.back(), opacity.back() };
			}
			return result;
		}

		void setBlend(Renderer& renderer, const UiBlendMode mode)
		{
			switch (mode)
			{
			case UiBlendMode::PremultipliedAlpha:
				renderer.setAlphaBlending(true, BlendFunction::One,
					BlendFunction::OneMinusSrcAlpha, BlendEquation::Add);
				break;
			case UiBlendMode::Additive:
				renderer.setAlphaBlending(true, BlendFunction::One,
					BlendFunction::One, BlendEquation::Add);
				break;
			case UiBlendMode::Opaque:
				renderer.setAlphaBlending(false);
				break;
			}
		}
	}

	struct UiRenderer::Implementation
	{
		struct GlyphAtlasTexture
		{
			UInt texture = 0;
			UInt generation = 0;
			U32 serviceId = 0;
			UiGlyphPixelFormat format = UiGlyphPixelFormat::R8;
			UInt width = 0;
			UInt height = 0;
		};

		struct ImageAtlasTexture
		{
			UInt texture = 0;
			UInt generation = 0;
			U32 ownerId = 0;
			UInt width = 0;
			UInt height = 0;
		};

		UiRendererCapabilities capabilities;
		UiRendererCallbacks callbacks;
		StreamBuffer instances;
		StreamBuffer vertices;
		StreamBuffer indices;
		VertexArrayID quadVao = 0;
		VertexArrayID pathVao = 0;
		BufferID quadVertices = 0;
		BufferID quadIndices = 0;
		UInt fallbackFontTexture = 0;
		Map<UiResourceId, GlyphAtlasTexture> glyphAtlases;
		Map<UiResourceId, ImageAtlasTexture> imageAtlases;
		Bool warnedMissingTextureResolver = false;
		Bool warnedNonRectClip = false;

		void create(Renderer& renderer)
		{
			if (capabilities.initialized) return;
			capabilities.persistentMappedStreaming =
				External::__glewNamedBufferStorage != nullptr &&
				External::__glewMapNamedBufferRange != nullptr;

			const Vec2 corners[] = { Vec2(0, 0), Vec2(1, 0), Vec2(1, 1), Vec2(0, 1) };
			const UInt cornerIndices[] = { 0, 1, 2, 0, 2, 3 };
			External::glCreateBuffers(1, &quadVertices);
			External::glNamedBufferData(quadVertices, sizeof(corners), corners, GL_STATIC_DRAW);
			External::glCreateBuffers(1, &quadIndices);
			External::glNamedBufferData(quadIndices, sizeof(cornerIndices), cornerIndices, GL_STATIC_DRAW);
			External::glCreateVertexArrays(1, &quadVao);
			External::glVertexArrayVertexBuffer(quadVao, 0, quadVertices, 0, sizeof(Vec2));
			External::glEnableVertexArrayAttrib(quadVao, 0);
			External::glVertexArrayAttribFormat(quadVao, 0, 2, GL_FLOAT, false, 0);
			External::glVertexArrayAttribBinding(quadVao, 0, 0);
			External::glVertexArrayElementBuffer(quadVao, quadIndices);

			External::glCreateVertexArrays(1, &pathVao);

			Vector<Byte> fallbackPixels;
			UInt fallbackWidth = 0;
			UInt fallbackHeight = 0;
			UiTextPainter::buildFallbackAtlas(
				fallbackPixels, fallbackWidth, fallbackHeight);
			External::glCreateTextures(GL_TEXTURE_2D, 1, &fallbackFontTexture);
			External::glTextureStorage2D(
				fallbackFontTexture, 1, GL_R8, fallbackWidth, fallbackHeight);
			External::glTextureParameteri(
				fallbackFontTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			External::glTextureParameteri(
				fallbackFontTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			External::glTextureParameteri(
				fallbackFontTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			External::glTextureParameteri(
				fallbackFontTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			External::glTextureSubImage2D(fallbackFontTexture, 0, 0, 0,
				fallbackWidth, fallbackHeight, GL_RED, GL_UNSIGNED_BYTE,
				fallbackPixels.data());
			capabilities.initialized = true;
			(void)renderer;
		}

		void destroy()
		{
			instances.release();
			vertices.release();
			indices.release();
			if (quadVao) External::glDeleteVertexArrays(1, &quadVao);
			if (pathVao) External::glDeleteVertexArrays(1, &pathVao);
			if (quadVertices) External::glDeleteBuffers(1, &quadVertices);
			if (quadIndices) External::glDeleteBuffers(1, &quadIndices);
			if (fallbackFontTexture)
				External::glDeleteTextures(1, &fallbackFontTexture);
			for (auto& [id, atlas] : glyphAtlases)
				if (atlas.texture) External::glDeleteTextures(1, &atlas.texture);
			glyphAtlases.clear();
			for (auto& [id, atlas] : imageAtlases)
				if (atlas.texture) External::glDeleteTextures(1, &atlas.texture);
			imageAtlases.clear();
			quadVao = pathVao = quadVertices = quadIndices = 0;
			fallbackFontTexture = 0;
			capabilities.initialized = false;
		}

		[[nodiscard]] Bool bindTexture(
			Renderer& renderer, const UiResourceRef& resource) const
		{
			if (resource == builtinFallbackFontResource() && fallbackFontTexture)
			{
				External::glBindTextureUnit(0, fallbackFontTexture);
				return true;
			}
			if (const auto found = glyphAtlases.find(resource.id);
				found != glyphAtlases.end() &&
				found->second.generation == resource.generation &&
				found->second.texture)
			{
				External::glBindTextureUnit(0, found->second.texture);
				return true;
			}
			if (const auto found = imageAtlases.find(resource.id);
				found != imageAtlases.end() &&
				found->second.generation == resource.generation &&
				found->second.texture)
			{
				External::glBindTextureUnit(0, found->second.texture);
				return true;
			}
			return callbacks.bindTexture && callbacks.bindTexture(renderer, resource);
		}
	};

	UiRenderer::UiRenderer() : implementation(std::make_unique<Implementation>()) {}
	UiRenderer::~UiRenderer() = default;

	void UiRenderer::create(Renderer& renderer)
	{
		implementation->create(renderer);
	}

	void UiRenderer::destroy(Renderer&)
	{
		implementation->destroy();
	}

	void UiRenderer::setCallbacks(UiRendererCallbacks callbacks)
	{
		implementation->callbacks = std::move(callbacks);
	}

	void UiRenderer::uploadGlyphAtlas(
		Renderer&, const UiResourceRef& resource,
		const UiGlyphPixelFormat format, const UInt width, const UInt height,
		const UiGlyphRect& rect, const Byte* pixels, const SizeT byteCount)
	{
		U32 serviceId = 0;
		UiGlyphPixelFormat encodedFormat = UiGlyphPixelFormat::R8;
		UInt page = 0;
		if (!pixels || !width || !height ||
			!decodeUiGlyphResource(resource, serviceId, encodedFormat, page) ||
			encodedFormat != format || rect.x + rect.width > width ||
			rect.y + rect.height > height ||
			byteCount != static_cast<SizeT>(rect.width) * rect.height *
				uiGlyphBytesPerPixel(format))
			return;
		Implementation::GlyphAtlasTexture& atlas =
			implementation->glyphAtlases[resource.id];
		if (!atlas.texture || atlas.generation != resource.generation ||
			atlas.width != width || atlas.height != height)
		{
			if (atlas.texture) External::glDeleteTextures(1, &atlas.texture);
			atlas = {};
			atlas.generation = resource.generation;
			atlas.serviceId = serviceId;
			atlas.format = format;
			atlas.width = width;
			atlas.height = height;
			const External::GLenum internalFormat =
				format == UiGlyphPixelFormat::RGB8 ? GL_RGB8 :
				(format == UiGlyphPixelFormat::RGBA8 ||
					format == UiGlyphPixelFormat::MtsdfRGBA8) ? GL_RGBA8 : GL_R8;
			External::glCreateTextures(GL_TEXTURE_2D, 1, &atlas.texture);
			External::glTextureStorage2D(
				atlas.texture, 1, internalFormat, width, height);
			External::glTextureParameteri(
				atlas.texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			External::glTextureParameteri(
				atlas.texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			External::glTextureParameteri(
				atlas.texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			External::glTextureParameteri(
				atlas.texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}
		const External::GLenum pixelFormat =
			format == UiGlyphPixelFormat::RGB8 ? GL_RGB :
			(format == UiGlyphPixelFormat::RGBA8 ||
				format == UiGlyphPixelFormat::MtsdfRGBA8) ? GL_RGBA : GL_RED;
		Int previousAlignment = 4;
		External::glGetIntegerv(GL_UNPACK_ALIGNMENT, &previousAlignment);
		External::glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		External::glTextureSubImage2D(atlas.texture, 0,
			rect.x, rect.y, rect.width, rect.height,
			pixelFormat, GL_UNSIGNED_BYTE, pixels);
		External::glPixelStorei(GL_UNPACK_ALIGNMENT, previousAlignment);
		(void)page;
	}

	void UiRenderer::releaseGlyphAtlases(const U32 serviceId)
	{
		for (auto iterator = implementation->glyphAtlases.begin();
			iterator != implementation->glyphAtlases.end();)
		{
			if (iterator->second.serviceId != serviceId)
			{
				++iterator;
				continue;
			}
			if (iterator->second.texture)
				External::glDeleteTextures(1, &iterator->second.texture);
			iterator = implementation->glyphAtlases.erase(iterator);
		}
	}

	void UiRenderer::uploadImageTexture(
		Renderer&, const UiResourceRef& resource, const U32 ownerId,
		const UInt width, const UInt height, const Byte* rgbaPixels, const SizeT byteCount)
	{
		if (!rgbaPixels || !width || !height ||
			byteCount != static_cast<SizeT>(width) * height * 4) return;

		Implementation::ImageAtlasTexture& atlas = implementation->imageAtlases[resource.id];
		if (!atlas.texture || atlas.generation != resource.generation ||
			atlas.width != width || atlas.height != height)
		{
			if (atlas.texture) External::glDeleteTextures(1, &atlas.texture);
			atlas = {};
			atlas.generation = resource.generation;
			atlas.ownerId = ownerId;
			atlas.width = width;
			atlas.height = height;
			External::glCreateTextures(GL_TEXTURE_2D, 1, &atlas.texture);
			External::glTextureStorage2D(atlas.texture, 1, GL_RGBA8, width, height);
			External::glTextureParameteri(
				atlas.texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			External::glTextureParameteri(
				atlas.texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			External::glTextureParameteri(
				atlas.texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			External::glTextureParameteri(
				atlas.texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}
		External::glTextureSubImage2D(
			atlas.texture, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, rgbaPixels);
	}

	void UiRenderer::releaseImageTextures(const U32 ownerId)
	{
		for (auto iterator = implementation->imageAtlases.begin();
			iterator != implementation->imageAtlases.end();)
		{
			if (iterator->second.ownerId != ownerId)
			{
				++iterator;
				continue;
			}
			if (iterator->second.texture)
				External::glDeleteTextures(1, &iterator->second.texture);
			iterator = implementation->imageAtlases.erase(iterator);
		}
	}

	const UiRendererCapabilities& UiRenderer::capabilities() const
	{
		return implementation->capabilities;
	}

	void UiRenderer::submit(Renderer& renderer, const UiRenderPacket& packet)
	{
		Implementation& state = *implementation;
		if (!state.capabilities.initialized) state.create(renderer);
		if (packet.batches().empty()) return;

		const Bool previousCulling = renderer.getCulling();
		const Bool previousDepthTest = renderer.openGlContext.getDepthTest();
		const Bool previousBlending = renderer.openGlContext.getBlending();
		const Bool previousScissorTest = renderer.openGlContext.scissorTest;
		const IVec2 previousScissorPosition(renderer.openGlContext.scissorX,
			renderer.openGlContext.scissorY);
		const IVec2 previousScissorSize(renderer.openGlContext.scissorW,
			renderer.openGlContext.scissorH);
		Int previousProgram = 0;
		Int previousVao = 0;
		Int previousBlendSourceRgb = 0;
		Int previousBlendDestinationRgb = 0;
		Int previousBlendSourceAlpha = 0;
		Int previousBlendDestinationAlpha = 0;
		Int previousBlendEquationRgb = 0;
		Int previousBlendEquationAlpha = 0;
		External::glGetIntegerv(GL_CURRENT_PROGRAM, &previousProgram);
		External::glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &previousVao);
		External::glGetIntegerv(GL_BLEND_SRC_RGB, &previousBlendSourceRgb);
		External::glGetIntegerv(GL_BLEND_DST_RGB, &previousBlendDestinationRgb);
		External::glGetIntegerv(GL_BLEND_SRC_ALPHA, &previousBlendSourceAlpha);
		External::glGetIntegerv(GL_BLEND_DST_ALPHA, &previousBlendDestinationAlpha);
		External::glGetIntegerv(GL_BLEND_EQUATION_RGB, &previousBlendEquationRgb);
		External::glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &previousBlendEquationAlpha);
		const Bool worldSurface = packet.worldSurface().has_value();
		if (worldSurface)
		{
			// The current Renderer matrices are a valid default world-space
			// target. Applications only need a callback when they must redirect
			// the surface or install a different camera.
			if (state.callbacks.beginWorldSurface &&
				!state.callbacks.beginWorldSurface(renderer, *packet.worldSurface())) return;
			renderer.uniforms().setMMatrix(packet.worldSurface()->localToWorld);
			renderer.setDepthTest(packet.worldSurface()->depthTest);
			renderer.setCulling(!packet.worldSurface()->doubleSided);
		}
		else
		{
			renderer.setDepthTest(false);
			renderer.setCulling(false);
			renderer.uniforms().setMMatrix(Matrix(1));
		}

		const Vector<BatchState> batchStates = buildBatchStates(packet);
		Vector<GpuQuad> gpuInstances(packet.instances().size());
		for (SizeT batchIndex = 0; batchIndex < packet.batches().size(); ++batchIndex)
		{
			const UiRenderBatch& batch = packet.batches()[batchIndex];
			if (batch.pipeline == UiPipeline::PathMesh) continue;
			const BatchState& batchState = batchStates[batchIndex];
			for (UInt i = 0; i < batch.instances.count; ++i)
			{
				const UInt instanceIndex = batch.instances.first + i;
				const UiQuadInstance& source = packet.instances()[instanceIndex];
				GpuQuad& destination = gpuInstances[instanceIndex];
				const Vec2 origin = transformPoint(batchState.transform, source.geometry.position);
				const Vec2 axisX = transformPoint(batchState.transform,
					source.geometry.position + Vec2(source.geometry.size.x, 0)) - origin;
				const Vec2 axisY = transformPoint(batchState.transform,
					source.geometry.position + Vec2(0, source.geometry.size.y)) - origin;
				destination.originAxisX = Vec4(origin, axisX);
				destination.axisYSize = Vec4(axisY, source.geometry.size);
				destination.cornerRadii = source.geometry.cornerRadii;
				for (UInt color = 0; color < 4; ++color)
					destination.colors[color] = source.colors[color].premultiplied * batchState.opacity;
				destination.uvRect = source.uvRect;
				destination.gradient = Vec4(source.gradientStart, source.gradientEnd);
				destination.parameters = Vec4(source.distanceRange, 0, 0, 0);
			}
		}

		Vector<GpuMeshVertex> gpuVertices(packet.meshVertices().size());
		for (SizeT i = 0; i < packet.meshVertices().size(); ++i)
		{
			gpuVertices[i].position = packet.meshVertices()[i].position;
			gpuVertices[i].uv = packet.meshVertices()[i].uv;
			gpuVertices[i].color = packet.meshVertices()[i].color.premultiplied;
		}
		for (SizeT batchIndex = 0; batchIndex < packet.batches().size(); ++batchIndex)
		{
			const UiRenderBatch& batch = packet.batches()[batchIndex];
			if (batch.pipeline != UiPipeline::PathMesh) continue;
			for (UInt i = 0; i < batch.mesh.vertexCount; ++i)
			{
				GpuMeshVertex& vertex = gpuVertices[batch.mesh.firstVertex + i];
				vertex.position = transformPoint(batchStates[batchIndex].transform, vertex.position);
				vertex.color *= batchStates[batchIndex].opacity;
			}
		}

		const ULL instanceBytes = gpuInstances.size() * sizeof(GpuQuad);
		const ULL vertexBytes = gpuVertices.size() * sizeof(GpuMeshVertex);
		const ULL indexBytes = packet.meshIndices().size() * sizeof(UInt);
		state.instances.ensure(instanceBytes, state.capabilities.persistentMappedStreaming);
		state.vertices.ensure(vertexBytes, state.capabilities.persistentMappedStreaming);
		state.indices.ensure(indexBytes, state.capabilities.persistentMappedStreaming);
		const ULL instanceOffset = state.instances.beginFrame();
		const ULL vertexOffset = state.vertices.beginFrame();
		const ULL indexOffset = state.indices.beginFrame();
		state.instances.upload(instanceOffset, gpuInstances.data(), instanceBytes);
		state.vertices.upload(vertexOffset, gpuVertices.data(), vertexBytes);
		state.indices.upload(indexOffset, packet.meshIndices().data(), indexBytes);

		External::glVertexArrayVertexBuffer(state.pathVao, 0, state.vertices.id,
			static_cast<External::GLintptr>(vertexOffset), sizeof(GpuMeshVertex));
		External::glEnableVertexArrayAttrib(state.pathVao, 0);
		External::glVertexArrayAttribFormat(state.pathVao, 0, 2, GL_FLOAT, false,
			static_cast<UInt>(offsetof(GpuMeshVertex, position)));
		External::glVertexArrayAttribBinding(state.pathVao, 0, 0);
		External::glEnableVertexArrayAttrib(state.pathVao, 1);
		External::glVertexArrayAttribFormat(state.pathVao, 1, 2, GL_FLOAT, false,
			static_cast<UInt>(offsetof(GpuMeshVertex, uv)));
		External::glVertexArrayAttribBinding(state.pathVao, 1, 0);
		External::glEnableVertexArrayAttrib(state.pathVao, 8);
		External::glVertexArrayAttribFormat(state.pathVao, 8, 4, GL_FLOAT, false,
			static_cast<UInt>(offsetof(GpuMeshVertex, color)));
		External::glVertexArrayAttribBinding(state.pathVao, 8, 0);
		External::glVertexArrayElementBuffer(state.pathVao, state.indices.id);

		if (instanceBytes)
			External::glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 3, state.instances.id,
				static_cast<External::GLintptr>(instanceOffset),
				static_cast<External::GLsizeiptr>(instanceBytes));

		Int viewport[4] = {};
		External::glGetIntegerv(GL_VIEWPORT, viewport);
		UInt activeClip = maxUInt;
		struct ActiveLayer
		{
			UiBeginLayerCommand command;
			UInt state = 0;
			Bool redirected = false;
		};
		Vector<ActiveLayer> activeLayers;
		SizeT layerCommandCursor = 0;
		UInt nextLayerState = 1;
		auto processLayerCommands = [&](const SizeT end)
		{
			while (layerCommandCursor < end)
			{
				const UiDisplayCommand& command = packet.commands()[layerCommandCursor++];
				if (const auto* begin = std::get_if<UiBeginLayerCommand>(&command))
				{
					const UInt layerState = nextLayerState++;
					const Bool redirected = begin->isolated && state.callbacks.beginLayer &&
						state.callbacks.beginLayer(renderer, *begin, layerState);
					activeLayers.push_back({ *begin, layerState, redirected });
				}
				else if (std::holds_alternative<UiEndLayerCommand>(command) && !activeLayers.empty())
				{
					const ActiveLayer layer = activeLayers.back();
					activeLayers.pop_back();
					if (layer.redirected && state.callbacks.endLayer)
						state.callbacks.endLayer(renderer, layer.command, layer.state);
				}
			}
		};
		for (const UiRenderBatch& batch : packet.batches())
		{
			processLayerCommands(batch.firstCommand);
			setBlend(renderer, batch.blend);
			if (batch.clipState != activeClip)
			{
				activeClip = batch.clipState;
				if (activeClip == 0) renderer.disableScissor();
				else
				{
					const UiClipShape& shape = packet.clips()[activeClip - 1].shape;
					if (!std::holds_alternative<UiClipRect>(shape) && !state.warnedNonRectClip)
					{
						logWarning("Modern UI: rounded/transformed clips use conservative scissor bounds; stencil clipping is unavailable.");
						state.warnedNonRectClip = true;
					}
					const UiClipRect bounds = effectiveClipBounds(packet, activeClip);
					const Int left = std::max(0, std::min(viewport[2],
						static_cast<Int>(std::floor(bounds.position.x))));
					const Int top = std::max(0, std::min(viewport[3],
						static_cast<Int>(std::floor(bounds.position.y))));
					const Int right = std::max(left, std::min(viewport[2],
						static_cast<Int>(std::ceil(bounds.position.x + bounds.size.x))));
					const Int bottom = std::max(top, std::min(viewport[3],
						static_cast<Int>(std::ceil(bounds.position.y + bounds.size.y))));
					Int width = right - left;
					Int height = bottom - top;
					Int x = viewport[0] + left;
					Int y = viewport[1] + viewport[3] - bottom;
					if (y < 0)
					{
						height += y;
						y = 0;
					}
					if (x < 0)
					{
						width += x;
						x = 0;
					}
					if (width <= 0 || height <= 0) renderer.disableScissor();
					else renderer.setScissorTest(IVec2(x, y), IVec2(width, height));
				}
			}

			renderer.useShader(shaderName(batch.pipeline));
			auto draw = [&](const UInt resourceIndex, const UiInstanceRange instanceRange,
				const UiMeshRange meshRange)
			{
				Bool textureAvailable = batch.texturePool == UiTexturePool::None;
				if (batch.texturePool != UiTexturePool::None &&
					resourceIndex < packet.resources().size())
					textureAvailable = state.bindTexture(
						renderer, packet.resources()[resourceIndex]);
				if (batch.texturePool != UiTexturePool::None &&
					!textureAvailable && !state.callbacks.bindTexture &&
					!state.warnedMissingTextureResolver)
				{
					logWarning("Modern UI: no texture resolver installed; textured draws use the diagnostic fallback.");
					state.warnedMissingTextureResolver = true;
				}
				renderer.uniforms().setCustomInt1((textureAvailable ? 1 : 0) | (worldSurface ? 2 : 0));
				renderer.uniforms().upload(renderer.openGlContext);

				if (batch.pipeline == UiPipeline::PathMesh)
				{
					External::glBindVertexArray(state.pathVao);
					const ULL byteOffset = indexOffset +
						static_cast<ULL>(meshRange.firstIndex) * sizeof(UInt);
					External::glDrawElementsBaseVertex(GL_TRIANGLES,
						static_cast<Int>(meshRange.indexCount), GL_UNSIGNED_INT,
						reinterpret_cast<const void*>(static_cast<uintptr_t>(byteOffset)),
						static_cast<Int>(meshRange.firstVertex));
				}
				else
				{
					External::glBindVertexArray(state.quadVao);
					External::glDrawElementsInstancedBaseInstance(GL_TRIANGLES, 6,
						GL_UNSIGNED_INT, nullptr, static_cast<Int>(instanceRange.count),
						instanceRange.first);
				}
			};

			if (batch.texturePool != UiTexturePool::None &&
				batch.resourceIndex == invalidUiResourceIndex)
			{
				for (UInt i = 0; i < batch.drawCommandCount; ++i)
				{
					const auto* command = std::get_if<UiDrawCommand>(
						&packet.commands()[batch.firstCommand + i]);
					if (command) draw(command->resourceIndex, command->instances, command->mesh);
				}
			}
			else draw(batch.resourceIndex, batch.instances, batch.mesh);
		}
		processLayerCommands(packet.commands().size());

		state.instances.finishFrame();
		state.vertices.finishFrame();
		state.indices.finishFrame();
		if (worldSurface && state.callbacks.endWorldSurface)
			state.callbacks.endWorldSurface(renderer, *packet.worldSurface());
		External::glUseProgram(static_cast<UInt>(previousProgram));
		External::glBindVertexArray(static_cast<UInt>(previousVao));
		renderer.setAlphaBlending(previousBlending);
		External::glBlendFuncSeparate(previousBlendSourceRgb, previousBlendDestinationRgb,
			previousBlendSourceAlpha, previousBlendDestinationAlpha);
		External::glBlendEquationSeparate(previousBlendEquationRgb, previousBlendEquationAlpha);
		renderer.setCulling(previousCulling);
		renderer.setDepthTest(previousDepthTest);
		if (previousScissorTest)
			renderer.setScissorTest(previousScissorPosition, previousScissorSize);
		else renderer.disableScissor();
	}

	UiResourceRef makeUiGlyphResource(
		const U32 serviceId, const UiGlyphPixelFormat format,
		const UInt page, const UInt generation)
	{
		if (!serviceId || serviceId > 0x00ffffff || page > 0xffff ||
			static_cast<UInt>(format) >
				static_cast<UInt>(UiGlyphPixelFormat::MtsdfRGBA8) ||
			!generation)
			return {};
		const UiResourceId id =
			(glyphResourceMagic << 48) |
			(static_cast<U64>(serviceId) << 24) |
			(static_cast<U64>(format) << 16) |
			static_cast<U64>(page);
		const UiTexturePool pool =
			(format == UiGlyphPixelFormat::RGB8 ||
				format == UiGlyphPixelFormat::MtsdfRGBA8) ?
				UiTexturePool::DistanceFieldGlyphs :
			format == UiGlyphPixelFormat::RGBA8 ?
				UiTexturePool::ColorGlyphs :
				UiTexturePool::GrayscaleGlyphs;
		return { id, pool, generation };
	}

	Bool decodeUiGlyphResource(
		const UiResourceRef& resource, U32& serviceId,
		UiGlyphPixelFormat& format, UInt& page)
	{
		if ((resource.id >> 48) != glyphResourceMagic || !resource.generation)
			return false;
		serviceId = static_cast<U32>((resource.id >> 24) & 0x00ffffff);
		format = static_cast<UiGlyphPixelFormat>((resource.id >> 16) & 0xff);
		page = static_cast<UInt>(resource.id & 0xffff);
		if (!serviceId ||
			static_cast<UInt>(format) >
				static_cast<UInt>(UiGlyphPixelFormat::MtsdfRGBA8))
			return false;
		const UiTexturePool expected =
			(format == UiGlyphPixelFormat::RGB8 ||
				format == UiGlyphPixelFormat::MtsdfRGBA8) ?
				UiTexturePool::DistanceFieldGlyphs :
			format == UiGlyphPixelFormat::RGBA8 ?
				UiTexturePool::ColorGlyphs :
				UiTexturePool::GrayscaleGlyphs;
		return resource.pool == expected;
	}
}
