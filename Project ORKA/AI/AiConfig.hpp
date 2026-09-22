#pragma once

#include "../OrkaTypes.hpp"

namespace AiConfig
{
	inline constexpr U16 defaultServerPort = 8765;
	inline constexpr Int consentVersion = 1;

	inline constexpr const char* runtimeProviderName = "llama.cpp (llama-server)";
	inline constexpr const char* llamaCppReleaseTag = "b11100";

	inline constexpr const char* defaultModelId = "qwen3.5-9b";
	inline constexpr const char* defaultModelDisplayName = "Qwen3.5 9B Instruct (GGUF)";

	inline constexpr const char* defaultModelHuggingFaceRepo = "unsloth/Qwen3.5-9B-GGUF";
	inline constexpr const char* defaultModelHuggingFaceQuant = "UD-Q4_K_XL";

	// llama.cpp Windows asset name cores (without "llama-{tag}-" prefix / .zip suffix).
	inline constexpr const char* winCpuAssetCore = "bin-win-cpu-x64";
	inline constexpr const char* winCuda13AssetCore = "bin-win-cuda-13.4-x64";
	inline constexpr const char* winCuda12AssetCore = "bin-win-cuda-12.4-x64";
	inline constexpr const char* winCuda13CudartAsset = "cudart-llama-bin-win-cuda-13.4-x64";
	inline constexpr const char* winCuda12CudartAsset = "cudart-llama-bin-win-cuda-12.4-x64";

	inline constexpr Int defaultGpuLayers = 999;

	[[nodiscard]] String llamaCppReleaseAssetZipUrl(const char* assetCore);
	[[nodiscard]] String llamaCppReleaseAssetZipFileName(const char* assetCore);
	[[nodiscard]] String llamaCppReleaseCudartZipUrl(const char* cudartAssetName);
	[[nodiscard]] String defaultModelHuggingFaceSpec();
}
