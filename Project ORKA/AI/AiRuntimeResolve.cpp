#include "AiRuntimeResolve.hpp"

#include "../Agent/AgentValue.hpp"
#include "../Debug.hpp"
#include "../File.hpp"
#include "AiConfig.hpp"
#include "AiDownload.hpp"

namespace
{
	Bool findAssetInRelease(const AgentValue& release, const String& nameFragment, String& fileName, String& downloadUrl)
	{
		const AgentValue* assets = release.find("assets");
		if (!assets || !assets->isArray()) return false;

		for (const AgentValue& asset : assets->asArray())
		{
			const AgentValue* name = asset.find("name");
			const AgentValue* url = asset.find("browser_download_url");
			if (!name || !url || !name->isString() || !url->isString()) continue;
			if (name->asString().find(nameFragment) == String::npos) continue;
			fileName = name->asString();
			downloadUrl = url->asString();
			return !downloadUrl.empty();
		}
		return false;
	}

	Bool loadRecentReleases(AgentValue::Array& releases, String& error)
	{
		const Path apiCache = std::filesystem::temp_directory_path() / "orka-llama-releases.json";
		const String apiUrl = "https://api.github.com/repos/ggml-org/llama.cpp/releases?per_page=8";
		if (!downloadUrlToFile(apiUrl, apiCache))
		{
			error = "Could not query GitHub releases API.";
			return false;
		}

		InFile file(apiCache);
		if (!file.isOpen)
		{
			error = "Could not read GitHub releases response.";
			return false;
		}

		String jsonText;
		String line;
		while (file.readLine(line)) jsonText += line;

		const AgentJsonParseResult parsed = parseAgentJson(jsonText);
		if (!parsed.succeeded() || !parsed.value.isArray())
		{
			error = "Invalid GitHub releases JSON.";
			return false;
		}

		releases = parsed.value.asArray();
		return true;
	}

	Bool resolvePinnedPackage(
		const char* assetCore,
		const char* variantId,
		const char* cudartAsset,
		const Int gpuLayers,
		const String& backendLabel,
		LlamaCppRuntimePackage& package)
	{
		package.releaseTag = AiConfig::llamaCppReleaseTag;
		package.variantId = variantId;
		package.zipFileName = AiConfig::llamaCppReleaseAssetZipFileName(assetCore);
		package.downloadUrl = AiConfig::llamaCppReleaseAssetZipUrl(assetCore);
		package.gpuLayers = gpuLayers;
		package.backendLabel = backendLabel;
		package.companionZipFileName.clear();
		package.companionDownloadUrl.clear();

		if (cudartAsset)
		{
			package.companionZipFileName = String(cudartAsset) + ".zip";
			package.companionDownloadUrl = AiConfig::llamaCppReleaseCudartZipUrl(cudartAsset);
		}

		return remoteUrlAvailable(package.downloadUrl);
	}

	Bool resolveFromGitHubApi(const String& assetFragment, LlamaCppRuntimePackage& package, String& error)
	{
		AgentValue::Array releases;
		if (!loadRecentReleases(releases, error)) return false;

		for (const AgentValue& release : releases)
		{
			const AgentValue* tagValue = release.find("tag_name");
			if (!tagValue || !tagValue->isString()) continue;

			String fileName;
			String downloadUrl;
			if (!findAssetInRelease(release, assetFragment, fileName, downloadUrl)) continue;

			package.releaseTag = tagValue->asString();
			package.zipFileName = fileName;
			package.downloadUrl = downloadUrl;

			String companionName;
			String companionUrl;
			if (assetFragment.find("cuda-13.4") != String::npos
				&& findAssetInRelease(release, "cudart-llama-bin-win-cuda-13.4-x64", companionName, companionUrl))
			{
				package.companionZipFileName = companionName;
				package.companionDownloadUrl = companionUrl;
			}
			else if (assetFragment.find("cuda-12.4") != String::npos
				&& findAssetInRelease(release, "cudart-llama-bin-win-cuda-12.4-x64", companionName, companionUrl))
			{
				package.companionZipFileName = companionName;
				package.companionDownloadUrl = companionUrl;
			}
			return true;
		}

		error = "No matching llama.cpp Windows asset in recent releases (" + assetFragment + ").";
		return false;
	}

	Bool tryResolveVariant(
		const char* assetCore,
		const char* variantId,
		const char* cudartAsset,
		const Int gpuLayers,
		const String& backendLabel,
		LlamaCppRuntimePackage& package,
		String& error)
	{
		if (resolvePinnedPackage(assetCore, variantId, cudartAsset, gpuLayers, backendLabel, package)) return true;

		logWarning("AiRuntimeResolve: pinned asset missing for " + String(variantId) + ", querying GitHub…");
		package.variantId = variantId;
		package.gpuLayers = gpuLayers;
		package.backendLabel = backendLabel;
		if (resolveFromGitHubApi(String(assetCore) + ".zip", package, error))
		{
			if (cudartAsset && package.companionDownloadUrl.empty())
			{
				package.companionZipFileName = String(cudartAsset) + ".zip";
				package.companionDownloadUrl = "https://github.com/ggml-org/llama.cpp/releases/download/"
					+ package.releaseTag + "/" + package.companionZipFileName;
			}
			return true;
		}
		return false;
	}
}

Bool resolveLlamaCppWindowsPackage(const AiGpuInfo& gpu, LlamaCppRuntimePackage& package, String& error)
{
	if (gpu.backend == AiGpuBackend::Cuda)
	{
		if (tryResolveVariant(
			AiConfig::winCuda13AssetCore,
			"cuda-13.4",
			AiConfig::winCuda13CudartAsset,
			AiConfig::defaultGpuLayers,
			"NVIDIA CUDA",
			package,
			error))
			return true;

		logWarning("AiRuntimeResolve: CUDA 13.4 build unavailable, trying CUDA 12.4…");
		if (tryResolveVariant(
			AiConfig::winCuda12AssetCore,
			"cuda-12.4",
			AiConfig::winCuda12CudartAsset,
			AiConfig::defaultGpuLayers,
			"NVIDIA CUDA (12.4)",
			package,
			error))
			return true;

		logWarning("AiRuntimeResolve: CUDA builds unavailable, falling back to CPU.");
	}

	return tryResolveVariant(
		AiConfig::winCpuAssetCore,
		"cpu",
		nullptr,
		0,
		"CPU",
		package,
		error);
}
