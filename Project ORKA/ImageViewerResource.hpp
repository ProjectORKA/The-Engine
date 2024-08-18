#pragma once
#include "Basics.hpp"
#include "CPUTexture.hpp"
#include "GPUTexture.hpp"

struct ImageViewerResource
{
	ImageViewerResource() = delete;
	ImageViewerResource(const Path& path, Index index);

	[[nodiscard]] Bool   inRam() const;
	[[nodiscard]] Bool   onGpu() const;
	[[nodiscard]] String getName() const;
	[[nodiscard]] Path   getPath() const;
	[[nodiscard]] String getHash() const;
	[[nodiscard]] Int    getWidth() const;
	[[nodiscard]] Int    getHeight() const;
	[[nodiscard]] Float  getPriority() const;
	[[nodiscard]] Float  getAspectRatio() const;
	[[nodiscard]] Bool   hasLoadAttemptFailed() const;

	void destroy();
	void loadOntoGpu();
	void loadIntoRam();
	void unloadFromGpu();
	void unloadFromRam();
	void use(Int textureSlot) const;
	void calculateRating(Index currentImageIndex, UInt resourceCount);

private:
	String     name;
	String     hash;
	CpuTexture cpuTexture;
	GPUTexture gpuTexture;
	Int        width                = 0;
	Int        height               = 0;
	Float      priority             = 0;
	Index      index                = 0;
	Bool       loadAttemptFailed    = false;
	Bool       isBeingLoadedIntoRam = false;
	Path       path                 = Path("");
};
