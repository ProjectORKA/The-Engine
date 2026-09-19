#include "UiDisplayList.hpp"

namespace Ui
{
	namespace
	{
		[[nodiscard]] Float saturate(const Float value)
		{
			return std::clamp(value, 0.0f, 1.0f);
		}

		[[nodiscard]] Float srgbToLinear(const Float value)
		{
			const Float clamped = saturate(value);
			return clamped <= 0.04045f
				? clamped / 12.92f
				: std::pow((clamped + 0.055f) / 1.055f, 2.4f);
		}

		[[nodiscard]] Bool addWouldOverflow(const SizeT current, const SizeT added, const SizeT maximum)
		{
			return added > maximum || current > maximum - added;
		}

		[[nodiscard]] UiClipRect intersectRects(const UiClipRect& first, const UiClipRect& second)
		{
			const Vec2 start = glm::max(first.position, second.position);
			const Vec2 end = glm::min(first.position + glm::max(first.size, Vec2(0)),
				second.position + glm::max(second.size, Vec2(0)));
			return { start, glm::max(end - start, Vec2(0)) };
		}

		[[nodiscard]] Bool isGlyphPipeline(const UiPipeline pipeline)
		{
			return pipeline == UiPipeline::GrayscaleGlyph ||
				pipeline == UiPipeline::MsdfGlyph ||
				pipeline == UiPipeline::ColorGlyph;
		}

		[[nodiscard]] UiTexturePool glyphPool(const UiPipeline pipeline)
		{
			switch (pipeline)
			{
			case UiPipeline::GrayscaleGlyph: return UiTexturePool::GrayscaleGlyphs;
			case UiPipeline::MsdfGlyph: return UiTexturePool::DistanceFieldGlyphs;
			case UiPipeline::ColorGlyph: return UiTexturePool::ColorGlyphs;
			default: return UiTexturePool::None;
			}
		}
	}

	UiLinearColor UiLinearColor::transparent()
	{
		return {};
	}

	UiLinearColor UiLinearColor::fromStraightLinear(Vec4 color)
	{
		color = glm::clamp(color, Vec4(0), Vec4(1));
		return { Vec4(Vec3(color) * color.a, color.a) };
	}

	UiLinearColor UiLinearColor::fromPremultipliedLinear(Vec4 color)
	{
		color.a = saturate(color.a);
		color.r = std::clamp(color.r, 0.0f, color.a);
		color.g = std::clamp(color.g, 0.0f, color.a);
		color.b = std::clamp(color.b, 0.0f, color.a);
		return { color };
	}

	UiLinearColor UiLinearColor::fromStraightSrgb(Vec4 color)
	{
		color = glm::clamp(color, Vec4(0), Vec4(1));
		const Vec4 linear(srgbToLinear(color.r), srgbToLinear(color.g),
			srgbToLinear(color.b), color.a);
		return fromStraightLinear(linear);
	}

	UiDisplayList::UiDisplayList(const UiRenderBudgets budgets)
		: limits(budgets)
	{
	}

	Bool UiDisplayList::addResource(const UiResourceRef resource, UInt& resourceIndex)
	{
		if (finalized || resource.id == 0 || resource.pool == UiTexturePool::None) return false;
		const auto existing = std::find(resourceData.begin(), resourceData.end(), resource);
		if (existing != resourceData.end())
		{
			resourceIndex = static_cast<UInt>(existing - resourceData.begin());
			return true;
		}
		if (resourceData.size() >= limits.maximumResources ||
			resourceData.size() >= static_cast<SizeT>(maxUInt)) return false;
		resourceIndex = static_cast<UInt>(resourceData.size());
		resourceData.push_back(resource);
		return true;
	}

	Bool UiDisplayList::appendCommand(UiDisplayCommand command)
	{
		if (finalized || commandData.size() >= limits.maximumCommands) return false;
		commandData.push_back(std::move(command));
		return true;
	}

	Bool UiDisplayList::appendInstanceDraw(const UiPipeline pipeline, const UiTexturePool pool,
		const UInt resourceIndex, UiQuadInstance instance, const UiBlendMode blend)
	{
		if (finalized || instanceData.size() >= limits.maximumInstances ||
			instanceData.size() >= static_cast<SizeT>(maxUInt) ||
			commandData.size() >= limits.maximumCommands) return false;

		const UInt first = static_cast<UInt>(instanceData.size());
		instanceData.push_back(std::move(instance));
		UiDrawCommand draw;
		draw.pipeline = pipeline;
		draw.blend = blend;
		draw.texturePool = pool;
		draw.resourceIndex = resourceIndex;
		draw.instances = { first, 1 };
		draw.paintOrder = nextPaintOrder++;
		commandData.emplace_back(draw);
		return true;
	}

	Bool UiDisplayList::addSolidQuad(const UiQuadGeometry& geometry, const UiLinearColor color,
		const UiBlendMode blend)
	{
		UiQuadInstance instance;
		instance.geometry = geometry;
		for (UiLinearColor& corner : instance.colors) corner = color;
		return appendInstanceDraw(UiPipeline::SolidQuad, UiTexturePool::None,
			invalidUiResourceIndex, instance, blend);
	}

	Bool UiDisplayList::addInsetBorder(const UiQuadGeometry& geometry, Float width,
		const UiLinearColor color, const UiBlendMode blend)
	{
		if (width <= 0 || geometry.size.x <= 0 || geometry.size.y <= 0) return true;
		const Float horizontal = std::min(width, geometry.size.y);
		const Float vertical = std::min(width, geometry.size.x);
		const Vec2& position = geometry.position;
		const Vec2& size = geometry.size;
		const Float sideHeight = std::max(0.0f, size.y - horizontal * 2);
		Bool succeeded = true;
		succeeded = addSolidQuad(
			UiQuadGeometry{position, Vec2(size.x, horizontal), geometry.cornerRadii}, color, blend) &&
			succeeded;
		succeeded = addSolidQuad(
			UiQuadGeometry{position + Vec2(0, size.y - horizontal), Vec2(size.x, horizontal),
				geometry.cornerRadii}, color, blend) && succeeded;
		if (sideHeight > 0)
		{
			succeeded = addSolidQuad(
				UiQuadGeometry{position + Vec2(0, horizontal), Vec2(vertical, sideHeight),
					geometry.cornerRadii}, color, blend) && succeeded;
			succeeded = addSolidQuad(
				UiQuadGeometry{position + Vec2(size.x - vertical, horizontal),
					Vec2(vertical, sideHeight), geometry.cornerRadii}, color, blend) && succeeded;
		}
		return succeeded;
	}

	Bool UiDisplayList::addGradientQuad(const UiQuadGeometry& geometry, const UiGradient& gradient,
		const UiBlendMode blend)
	{
		UiQuadInstance instance;
		instance.geometry = geometry;
		instance.colors[0] = gradient.startColor;
		instance.colors[1] = gradient.endColor;
		instance.colors[2] = gradient.endColor;
		instance.colors[3] = gradient.startColor;
		instance.gradientStart = gradient.start;
		instance.gradientEnd = gradient.end;
		return appendInstanceDraw(UiPipeline::GradientQuad, UiTexturePool::None,
			invalidUiResourceIndex, instance, blend);
	}

	Bool UiDisplayList::addImageQuad(const UiQuadGeometry& geometry, const UiImagePaint& paint,
		const UiBlendMode blend)
	{
		UiQuadInstance instance;
		instance.geometry = geometry;
		instance.uvRect = paint.uvRect;
		instance.resourceIndex = paint.resourceIndex;
		for (UiLinearColor& corner : instance.colors) corner = paint.tint;
		return appendInstanceDraw(UiPipeline::ImageQuad, UiTexturePool::Images,
			paint.resourceIndex, instance, blend);
	}

	Bool UiDisplayList::addGlyph(const UiPipeline glyphPipeline, const UiQuadGeometry& geometry,
		const UiGlyphPaint& paint, const UiBlendMode blend)
	{
		if (!isGlyphPipeline(glyphPipeline)) return false;
		UiQuadInstance instance;
		instance.geometry = geometry;
		instance.uvRect = paint.uvRect;
		instance.resourceIndex = paint.resourceIndex;
		instance.distanceRange = paint.distanceRange;
		for (UiLinearColor& corner : instance.colors) corner = paint.color;
		return appendInstanceDraw(glyphPipeline, glyphPool(glyphPipeline),
			paint.resourceIndex, instance, blend);
	}

	Bool UiDisplayList::addGrayscaleGlyph(const UiQuadGeometry& geometry,
		const UiGlyphPaint& paint, const UiBlendMode blend)
	{
		return addGlyph(UiPipeline::GrayscaleGlyph, geometry, paint, blend);
	}

	Bool UiDisplayList::addMsdfGlyph(const UiQuadGeometry& geometry,
		const UiGlyphPaint& paint, const UiBlendMode blend)
	{
		return addGlyph(UiPipeline::MsdfGlyph, geometry, paint, blend);
	}

	Bool UiDisplayList::addColorGlyph(const UiQuadGeometry& geometry,
		const UiGlyphPaint& paint, const UiBlendMode blend)
	{
		return addGlyph(UiPipeline::ColorGlyph, geometry, paint, blend);
	}

	Bool UiDisplayList::addPathMesh(const std::span<const UiMeshVertex> vertices,
		const std::span<const UInt> indices, const UiBlendMode blend)
	{
		if (finalized || vertices.empty() || indices.empty() ||
			addWouldOverflow(vertexData.size(), vertices.size(), limits.maximumMeshVertices) ||
			addWouldOverflow(indexData.size(), indices.size(), limits.maximumMeshIndices) ||
			commandData.size() >= limits.maximumCommands ||
			vertexData.size() + vertices.size() > static_cast<SizeT>(maxUInt) ||
			indexData.size() + indices.size() > static_cast<SizeT>(maxUInt)) return false;

		UiDrawCommand draw;
		draw.pipeline = UiPipeline::PathMesh;
		draw.blend = blend;
		draw.mesh = {
			static_cast<UInt>(vertexData.size()), static_cast<UInt>(vertices.size()),
			static_cast<UInt>(indexData.size()), static_cast<UInt>(indices.size())
		};
		draw.paintOrder = nextPaintOrder++;
		vertexData.insert(vertexData.end(), vertices.begin(), vertices.end());
		indexData.insert(indexData.end(), indices.begin(), indices.end());
		commandData.emplace_back(draw);
		return true;
	}

	Bool UiDisplayList::pushClip(UiClipShape shape)
	{
		return appendCommand(UiPushClipCommand{ std::move(shape) });
	}

	Bool UiDisplayList::popClip()
	{
		return appendCommand(UiPopClipCommand{});
	}

	Bool UiDisplayList::pushTransform(const glm::mat3 transform)
	{
		return appendCommand(UiPushTransformCommand{ transform });
	}

	Bool UiDisplayList::popTransform()
	{
		return appendCommand(UiPopTransformCommand{});
	}

	Bool UiDisplayList::beginLayer(UiBeginLayerCommand layer)
	{
		layer.opacity = saturate(layer.opacity);
		return appendCommand(layer);
	}

	Bool UiDisplayList::endLayer()
	{
		return appendCommand(UiEndLayerCommand{});
	}

	Bool UiDisplayList::setWorldSurface(UiWorldSurfaceMetadata metadata)
	{
		if (finalized || surface.has_value() || metadata.pixelsPerWorldUnit <= 0 ||
			metadata.logicalSize.x < 0 || metadata.logicalSize.y < 0) return false;
		surface = std::move(metadata);
		return true;
	}

	void UiDisplayList::clear()
	{
		if (finalized) return;
		commandData.clear();
		instanceData.clear();
		vertexData.clear();
		indexData.clear();
		resourceData.clear();
		surface.reset();
		nextPaintOrder = 0;
	}

	Bool UiDisplayList::append(const UiDisplayList& other)
	{
		if (finalized ||
			addWouldOverflow(commandData.size(), other.commandData.size(), limits.maximumCommands) ||
			addWouldOverflow(instanceData.size(), other.instanceData.size(), limits.maximumInstances) ||
			addWouldOverflow(resourceData.size(), other.resourceData.size(), limits.maximumResources) ||
			addWouldOverflow(vertexData.size(), other.vertexData.size(), limits.maximumMeshVertices) ||
			addWouldOverflow(indexData.size(), other.indexData.size(), limits.maximumMeshIndices)) return false;

		Vector<UInt> resourceRemap(other.resourceData.size(), invalidUiResourceIndex);
		for (SizeT i = 0; i < other.resourceData.size(); ++i)
		{
			if (!addResource(other.resourceData[i], resourceRemap[i])) return false;
		}

		const UInt instanceOffset = static_cast<UInt>(instanceData.size());
		const UInt vertexOffset = static_cast<UInt>(vertexData.size());
		const UInt indexOffset = static_cast<UInt>(indexData.size());
		for (UiQuadInstance instance : other.instanceData)
		{
			if (instance.resourceIndex != invalidUiResourceIndex)
			{
				if (instance.resourceIndex >= resourceRemap.size()) return false;
				instance.resourceIndex = resourceRemap[instance.resourceIndex];
			}
			instanceData.push_back(std::move(instance));
		}
		vertexData.insert(vertexData.end(), other.vertexData.begin(), other.vertexData.end());
		indexData.insert(indexData.end(), other.indexData.begin(), other.indexData.end());

		for (UiDisplayCommand command : other.commandData)
		{
			if (UiDrawCommand* draw = std::get_if<UiDrawCommand>(&command))
			{
				draw->paintOrder = nextPaintOrder++;
				if (draw->pipeline == UiPipeline::PathMesh)
				{
					draw->mesh.firstVertex += vertexOffset;
					draw->mesh.firstIndex += indexOffset;
				}
				else
				{
					draw->instances.first += instanceOffset;
					if (draw->resourceIndex != invalidUiResourceIndex)
					{
						if (draw->resourceIndex >= resourceRemap.size()) return false;
						draw->resourceIndex = resourceRemap[draw->resourceIndex];
					}
				}
			}
			commandData.push_back(std::move(command));
		}
		return true;
	}

	Bool UiDisplayListFinalizer::finalize(UiDisplayList& list, UiRenderPacket& packet,
		Vector<UiRenderDiagnostic>& diagnostics)
	{
		diagnostics.clear();
		if (list.finalized)
		{
			diagnostics.push_back({ UiRenderDiagnosticCode::AlreadyFinalized, 0,
				"Display list has already been finalized." });
			return false;
		}

		auto report = [&diagnostics](const UiRenderDiagnosticCode code,
			const SizeT commandIndex, String message)
		{
			diagnostics.push_back({ code, commandIndex, std::move(message) });
		};

		if (list.commandData.size() > list.limits.maximumCommands ||
			list.instanceData.size() > list.limits.maximumInstances ||
			list.resourceData.size() > list.limits.maximumResources ||
			list.vertexData.size() > list.limits.maximumMeshVertices ||
			list.indexData.size() > list.limits.maximumMeshIndices)
		{
			report(UiRenderDiagnosticCode::BudgetExceeded, 0, "Display list exceeds its configured budget.");
		}

		Vector<UInt> clipStack;
		Vector<UInt> layerStack;
		UInt transformDepth = 0;
		UInt nextLayerState = 1;
		Vector<UiEffectiveClip> effectiveClips;
		Vector<UiRenderBatch> batches;
		Bool mayMergeWithPrevious = false;

		for (SizeT commandIndex = 0; commandIndex < list.commandData.size(); ++commandIndex)
		{
			const UiDisplayCommand& command = list.commandData[commandIndex];
			if (const auto* push = std::get_if<UiPushClipCommand>(&command))
			{
				UiEffectiveClip effective{
					push->shape, clipStack.empty() ? 0 : clipStack.back(), false
				};
				if (const auto* rect = std::get_if<UiClipRect>(&effective.shape);
					rect && !clipStack.empty() && transformDepth == 0)
				{
					const UiEffectiveClip& parent = effectiveClips[clipStack.back() - 1];
					if (const auto* parentRect = std::get_if<UiClipRect>(&parent.shape))
					{
						effective.shape = intersectRects(*parentRect, *rect);
						effective.parentClipState = 0;
						effective.axisAlignedIntersection = true;
					}
				}
				effectiveClips.push_back(std::move(effective));
				clipStack.push_back(static_cast<UInt>(effectiveClips.size()));
				if (clipStack.size() > list.limits.maximumStackDepth)
					report(UiRenderDiagnosticCode::BudgetExceeded, commandIndex, "Clip stack depth exceeds budget.");
				mayMergeWithPrevious = false;
				continue;
			}
			if (std::holds_alternative<UiPopClipCommand>(command))
			{
				if (clipStack.empty())
					report(UiRenderDiagnosticCode::UnbalancedClipStack, commandIndex, "Clip stack underflow.");
				else clipStack.pop_back();
				mayMergeWithPrevious = false;
				continue;
			}
			if (std::holds_alternative<UiPushTransformCommand>(command))
			{
				++transformDepth;
				if (transformDepth > list.limits.maximumStackDepth)
					report(UiRenderDiagnosticCode::BudgetExceeded, commandIndex, "Transform stack depth exceeds budget.");
				mayMergeWithPrevious = false;
				continue;
			}
			if (std::holds_alternative<UiPopTransformCommand>(command))
			{
				if (transformDepth == 0)
					report(UiRenderDiagnosticCode::UnbalancedTransformStack, commandIndex, "Transform stack underflow.");
				else --transformDepth;
				mayMergeWithPrevious = false;
				continue;
			}
			if (std::holds_alternative<UiBeginLayerCommand>(command))
			{
				layerStack.push_back(nextLayerState++);
				if (layerStack.size() > list.limits.maximumStackDepth)
					report(UiRenderDiagnosticCode::BudgetExceeded, commandIndex, "Layer stack depth exceeds budget.");
				mayMergeWithPrevious = false;
				continue;
			}
			if (std::holds_alternative<UiEndLayerCommand>(command))
			{
				if (layerStack.empty())
					report(UiRenderDiagnosticCode::UnbalancedLayerStack, commandIndex, "Layer stack underflow.");
				else layerStack.pop_back();
				mayMergeWithPrevious = false;
				continue;
			}

			const auto* draw = std::get_if<UiDrawCommand>(&command);
			if (!draw)
			{
				report(UiRenderDiagnosticCode::InvalidCommand, commandIndex, "Unknown display command.");
				continue;
			}

			if (draw->pipeline == UiPipeline::PathMesh)
			{
				const UiMeshRange& range = draw->mesh;
				if (range.vertexCount == 0 || range.indexCount == 0 ||
					addWouldOverflow(range.firstVertex, range.vertexCount, list.vertexData.size()) ||
					addWouldOverflow(range.firstIndex, range.indexCount, list.indexData.size()))
				{
					report(UiRenderDiagnosticCode::InvalidMeshRange, commandIndex, "Path mesh range is out of bounds.");
				}
				else
				{
					for (UInt i = 0; i < range.indexCount; ++i)
					{
						if (list.indexData[range.firstIndex + i] >= range.vertexCount)
						{
							report(UiRenderDiagnosticCode::InvalidMeshIndex, commandIndex,
								"Path mesh contains an index outside its vertex range.");
							break;
						}
					}
				}
			}
			else
			{
				const UiInstanceRange& range = draw->instances;
				if (range.count == 0 ||
					addWouldOverflow(range.first, range.count, list.instanceData.size()))
					report(UiRenderDiagnosticCode::InvalidInstanceRange, commandIndex,
						"Instance range is out of bounds.");

				const Bool needsResource = draw->texturePool != UiTexturePool::None;
				if (needsResource && (draw->resourceIndex >= list.resourceData.size() ||
					list.resourceData[draw->resourceIndex].pool != draw->texturePool))
					report(UiRenderDiagnosticCode::InvalidResource, commandIndex,
						"Draw references an invalid resource or texture pool.");
			}

			UiRenderBatch candidate;
			candidate.pipeline = draw->pipeline;
			candidate.blend = draw->blend;
			candidate.texturePool = draw->texturePool;
			candidate.resourceIndex = draw->resourceIndex;
			candidate.clipState = clipStack.empty() ? 0 : clipStack.back();
			candidate.layerState = layerStack.empty() ? 0 : layerStack.back();
			candidate.instances = draw->instances;
			candidate.mesh = draw->mesh;
			candidate.firstPaintOrder = draw->paintOrder;
			candidate.firstCommand = static_cast<UInt>(commandIndex);
			candidate.drawCommandCount = 1;

			Bool compatible = mayMergeWithPrevious && !batches.empty();
			if (compatible)
			{
				UiRenderBatch& previous = batches.back();
				compatible = previous.pipeline == candidate.pipeline &&
					previous.blend == candidate.blend &&
					previous.texturePool == candidate.texturePool &&
					previous.clipState == candidate.clipState &&
					previous.layerState == candidate.layerState;
				if (draw->pipeline == UiPipeline::PathMesh)
				{
					compatible = compatible &&
						previous.mesh.firstVertex + previous.mesh.vertexCount == candidate.mesh.firstVertex &&
						previous.mesh.firstIndex + previous.mesh.indexCount == candidate.mesh.firstIndex;
				}
				else
				{
					compatible = compatible &&
						previous.instances.first + previous.instances.count == candidate.instances.first;
				}
				if (compatible)
				{
					previous.resourceIndex = previous.resourceIndex == candidate.resourceIndex
						? previous.resourceIndex : invalidUiResourceIndex;
					previous.instances.count += candidate.instances.count;
					previous.mesh.vertexCount += candidate.mesh.vertexCount;
					previous.mesh.indexCount += candidate.mesh.indexCount;
					++previous.drawCommandCount;
				}
			}
			if (!compatible) batches.push_back(candidate);
			mayMergeWithPrevious = true;
		}

		if (!clipStack.empty())
			report(UiRenderDiagnosticCode::UnbalancedClipStack, list.commandData.size(), "Clip stack is not empty.");
		if (transformDepth != 0)
			report(UiRenderDiagnosticCode::UnbalancedTransformStack, list.commandData.size(), "Transform stack is not empty.");
		if (!layerStack.empty())
			report(UiRenderDiagnosticCode::UnbalancedLayerStack, list.commandData.size(), "Layer stack is not empty.");
		if (!diagnostics.empty()) return false;

		UiRenderPacket result;
		result.commandData = list.commandData;
		result.batchData = std::move(batches);
		result.clipData = std::move(effectiveClips);
		result.instanceData = list.instanceData;
		result.vertexData = list.vertexData;
		result.indexData = list.indexData;
		result.resourceData = list.resourceData;
		result.surface = list.surface;
		packet = std::move(result);
		list.finalized = true;
		return true;
	}
}
