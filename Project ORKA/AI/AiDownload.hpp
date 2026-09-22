#pragma once

#include "../OrkaTypes.hpp"

struct AiDownloadProgress
{
	U64 bytesReceived = 0;
	U64 bytesTotal = 0;
	String phase;
};

using AiDownloadProgressCallback = Function<void(const AiDownloadProgress&)>;

[[nodiscard]] Bool remoteUrlAvailable(const String& url);
[[nodiscard]] Bool downloadUrlToFile(
	const String& url,
	const Path& destination,
	const AiDownloadProgressCallback& onProgress = {});
