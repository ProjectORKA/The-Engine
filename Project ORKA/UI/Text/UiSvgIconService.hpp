#pragma once

#include "../Render/UiDisplayList.hpp"
#include "UiSvgTypes.hpp"

struct Renderer;

namespace Ui
{
	struct UiSvgIconPaintRequest
	{
		String name;
		UInt pixelSize = 24;
		UiLinearColor tint = UiLinearColor::fromStraightLinear(Vec4(1));
	};

	class UiSvgIconService
	{
	public:
		UiSvgIconService();

		void setIconRoot(const Path& rootDirectory);
		void shutdown(Renderer& renderer);

		// Upload CPU-rasterized icons to the modern UI image atlas. Call after
		// building the display list and before submitUi().
		[[nodiscard]] Bool uploadPendingTextures(Renderer& renderer);

		[[nodiscard]] Bool resolve(const String& iconName, UInt pixelSize = 24);
		[[nodiscard]] Bool paint(
			UiDisplayList& output, const String& iconName,
			const UiQuadGeometry& geometry, const UiLinearColor& tint,
			UInt pixelSize = 24);

	private:
		struct CachedIcon
		{
			UiResourceRef resource;
			UInt width = 0;
			UInt height = 0;
			Vector<Byte> rgba;
			Bool gpuUploaded = false;
		};

		struct IconKey
		{
			String name;
			UInt pixelSize = 0;

			constexpr auto operator<=>(const IconKey&) const = default;
		};

		Path iconRoot;
		Map<IconKey, CachedIcon> cache;
		Map<UiResourceId, IconKey> resourceLookup;
		U32 serviceId = 0;
		UInt generation = 1;

		[[nodiscard]] UiResourceRef makeResource(const String& name, UInt pixelSize) const;
		[[nodiscard]] Bool ensureRasterized(
			const String& name, UInt pixelSize, CachedIcon& output);
		void ensureIconRoot();
	};
} // namespace Ui
