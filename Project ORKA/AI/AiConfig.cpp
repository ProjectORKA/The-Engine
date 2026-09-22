#include "AiConfig.hpp"

String AiConfig::llamaCppReleaseAssetZipUrl(const char* assetCore)
{
	return "https://github.com/ggml-org/llama.cpp/releases/download/"
		+ String(llamaCppReleaseTag) + "/llama-" + llamaCppReleaseTag + "-" + assetCore + ".zip";
}

String AiConfig::llamaCppReleaseAssetZipFileName(const char* assetCore)
{
	return "llama-" + String(llamaCppReleaseTag) + "-" + assetCore + ".zip";
}

String AiConfig::llamaCppReleaseCudartZipUrl(const char* cudartAssetName)
{
	return "https://github.com/ggml-org/llama.cpp/releases/download/"
		+ String(llamaCppReleaseTag) + "/" + cudartAssetName + ".zip";
}

String AiConfig::defaultModelHuggingFaceSpec()
{
	return String(defaultModelHuggingFaceRepo) + ":" + defaultModelHuggingFaceQuant;
}
