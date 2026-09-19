#include "UiSvgIconService.hpp"

#include "UiSvgRasterizer.hpp"

#include "../../Debug.hpp"
#include "../../Renderer.hpp"
#include "../../ResourceManager.hpp"
#include "../Render/UiRenderer.hpp"

#include <atomic>

namespace Ui
{
	namespace
	{
		constexpr U64 iconResourceMagic = 0x5549535647ULL;
		std::atomic<U32> nextIconServiceId{1};

		[[nodiscard]] U64 hashIconName(const String& name)
		{
			U64 hash = 1469598103934665603ULL;
			for (const Char character : name)
			{
				hash ^= static_cast<U64>(static_cast<unsigned char>(character));
				hash *= 1099511628211ULL;
			}
			return hash;
		}
	}

	UiSvgIconService::UiSvgIconService()
	{
		serviceId = nextIconServiceId.fetch_add(1);
		if (!serviceId) serviceId = nextIconServiceId.fetch_add(1);
	}

	void UiSvgIconService::setIconRoot(const Path& rootDirectory)
	{
		iconRoot = rootDirectory;
	}

	void UiSvgIconService::ensureIconRoot()
	{
		if (!iconRoot.empty()) return;
		iconRoot = resourceManager.getOrkaDataPath() / "Icons" / "tabler";
	}

	void UiSvgIconService::shutdown(Renderer& renderer)
	{
		releaseUiImageTextures(renderer, serviceId);
		cache.clear();
		resourceLookup.clear();
		++generation;
	}

	UiResourceRef UiSvgIconService::makeResource(const String& name, const UInt pixelSize) const
	{
		const U64 hash = hashIconName(name) ^ (static_cast<U64>(pixelSize) << 32);
		return { iconResourceMagic ^ hash, UiTexturePool::Images, generation };
	}

	Bool UiSvgIconService::resolve(const String& name, const UInt pixelSize)
	{
		CachedIcon cached;
		return ensureRasterized(name, pixelSize, cached);
	}

	Bool UiSvgIconService::ensureRasterized(
		const String& name, const UInt pixelSize, CachedIcon& output)
	{
		const IconKey key{name, pixelSize};
		if (const auto found = cache.find(key); found != cache.end())
		{
			output = found->second;
			return !output.rgba.empty();
		}

		ensureIconRoot();
		if (iconRoot.empty()) return false;

		const Path iconPath = iconRoot / (name + ".svg");
		UiSvgDocument document;
		if (!UiSvgRasterizer::loadDocument(iconPath, document))
		{
			logWarning("UiSvgIconService: failed to load icon '" + name +
				"' from " + iconPath.string());
			return false;
		}

		UiSvgRasterResult raster;
		if (!UiSvgRasterizer::rasterize(document, pixelSize, raster)) return false;

		CachedIcon cached;
		cached.resource = makeResource(name, pixelSize);
		cached.width = raster.width;
		cached.height = raster.height;
		cached.rgba = std::move(raster.rgba);
		cache.emplace(key, cached);
		resourceLookup.emplace(cached.resource.id, key);
		output = cached;
		return true;
	}

	Bool UiSvgIconService::uploadPendingTextures(Renderer& renderer)
	{
		Bool uploadedAny = false;
		for (auto& [key, icon] : cache)
		{
			(void)key;
			if (icon.gpuUploaded || icon.rgba.empty()) continue;
			uploadUiImageTexture(
				renderer, icon.resource, serviceId,
				icon.width, icon.height, icon.rgba.data(), icon.rgba.size());
			icon.gpuUploaded = true;
			uploadedAny = true;
		}
		return uploadedAny;
	}

	Bool UiSvgIconService::paint(
		UiDisplayList& output, const String& iconName,
		const UiQuadGeometry& geometry, const UiLinearColor& tint,
		const UInt pixelSize)
	{
		if (iconName.empty() || geometry.size.x <= 0 || geometry.size.y <= 0) return true;

		CachedIcon cached;
		if (!ensureRasterized(iconName, pixelSize, cached)) return true;
		const IconKey key{iconName, pixelSize};
		const auto found = cache.find(key);
		if (found == cache.end() || found->second.rgba.empty()) return true;

		UInt resourceIndex = invalidUiResourceIndex;
		if (!output.addResource(found->second.resource, resourceIndex)) return false;

		const Float iconSize = min(geometry.size.x, geometry.size.y);
		const Vec2 position(
			geometry.position.x + (geometry.size.x - iconSize) * 0.5f,
			geometry.position.y + (geometry.size.y - iconSize) * 0.5f);
		return output.addImageQuad(
			UiQuadGeometry{position, Vec2(iconSize, iconSize), Vec4(0)},
			UiImagePaint{resourceIndex, Vec4(0, 0, 1, 1), tint});
	}
} // namespace Ui
