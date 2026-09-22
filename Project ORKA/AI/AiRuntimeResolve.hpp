#pragma once

#include "../OrkaTypes.hpp"
#include "AiGpuDetect.hpp"

struct LlamaCppRuntimePackage
{
	String releaseTag;
	String variantId = "cpu";
	String zipFileName;
	String downloadUrl;
	String companionZipFileName;
	String companionDownloadUrl;
	Int gpuLayers = 0;
	String backendLabel = "CPU";

	[[nodiscard]] String folderTag() const { return releaseTag + "-" + variantId; }
};

// Picks CUDA build when an NVIDIA driver is present, otherwise CPU. Falls back CPU if CUDA zips are missing.
[[nodiscard]] Bool resolveLlamaCppWindowsPackage(const AiGpuInfo& gpu, LlamaCppRuntimePackage& package, String& error);
