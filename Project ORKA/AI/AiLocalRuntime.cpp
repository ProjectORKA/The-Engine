#include "AiLocalRuntime.hpp"

#include "../Debug.hpp"
#include "AiArchive.hpp"
#include "AiConfig.hpp"
#include "AiDownload.hpp"
#include "AiGpuDetect.hpp"
#include "AiOpenAiClient.hpp"
#include "AiPaths.hpp"
#include "AiRuntimeResolve.hpp"

#include <algorithm>
#include <thread>

namespace
{
	void sleepMs(const Int ms)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(ms));
	}

	Bool downloadAndExtractZip(
		const String& url,
		const Path& zipPath,
		const Path& destination,
		const AiDownloadProgressCallback& onProgress)
	{
		if (!downloadUrlToFile(url, zipPath, onProgress)) return false;
		return extractZipArchive(zipPath, destination);
	}
}

AiLocalRuntime::~AiLocalRuntime()
{
	shutdown();
}

void AiLocalRuntime::shutdown()
{
	shutdownRequested.store(true);
	if (worker.joinable()) worker.join();
	serverProcess.terminate();
}

AiRuntimeSnapshot AiLocalRuntime::snapshot() const
{
	LockGuard lock(mutex);
	return state;
}

void AiLocalRuntime::setState(const AiRuntimePhase phase, const String& statusLine, const Double progress, const String& error)
{
	LockGuard lock(mutex);
	state.phase = phase;
	state.statusLine = statusLine;
	state.downloadProgress = progress;
	state.error = error;
	state.computeBackend = serverBackendLabel;
}

void AiLocalRuntime::ensureReadyAsync()
{
	if (workerRunning.load()) return;
	if (worker.joinable()) worker.join();
	shutdownRequested.store(false);
	workerRunning.store(true);
	worker = Thread([this]()
		{
			runWorker();
			workerRunning.store(false);
		});
}

Path AiLocalRuntime::locateServerExecutable() const
{
	Path exe = findFirstFileNamed(AiPaths::runtimeRoot(), "llama-server.exe");
	if (!exe.empty()) return exe;
	return findFirstFileNamed(AiPaths::runtimeRoot(), "server.exe");
}

Bool AiLocalRuntime::ensureRuntimeInstalled()
{
	const AiGpuInfo gpu = detectAiGpu();
	LlamaCppRuntimePackage package;
	String resolveError;
	if (!resolveLlamaCppWindowsPackage(gpu, package, resolveError))
	{
		setState(AiRuntimePhase::Error, "Runtime resolve failed.", 0.0, resolveError);
		return false;
	}

	serverGpuLayers = package.gpuLayers;
	serverBackendLabel = package.backendLabel;
	AiPaths::setActiveRuntimeTag(package.folderTag());
	AiPaths::ensureLayout();

	const Path server = locateServerExecutable();
	if (!server.empty() && std::filesystem::exists(server)) return true;

	const String downloadLabel = package.gpuLayers > 0
		? "Downloading llama.cpp CUDA runtime…"
		: "Downloading llama.cpp CPU runtime…";
	setState(AiRuntimePhase::DownloadingRuntime, downloadLabel, 0.0);

	const Path zipPath = AiPaths::runtimeZip(package.zipFileName);
	const Path runtimeRoot = AiPaths::runtimeRoot();

	const Bool mainExtracted = downloadAndExtractZip(package.downloadUrl, zipPath, runtimeRoot,
		[this, &downloadLabel](const AiDownloadProgress& progress)
		{
			Double ratio = 0.0;
			if (progress.bytesTotal > 0)
				ratio = static_cast<Double>(progress.bytesReceived) / static_cast<Double>(progress.bytesTotal);
			setState(AiRuntimePhase::DownloadingRuntime, downloadLabel, ratio);
		});
	if (!mainExtracted)
	{
		setState(AiRuntimePhase::Error, "Runtime download failed.", 0.0,
			"Could not download llama.cpp (" + package.downloadUrl + "). Check your network connection.");
		return false;
	}

	if (!package.companionDownloadUrl.empty())
	{
		setState(AiRuntimePhase::ExtractingRuntime, "Installing CUDA runtime libraries…", 1.0);
		const Path companionZip = AiPaths::runtimeZip(package.companionZipFileName);
		if (!downloadAndExtractZip(package.companionDownloadUrl, companionZip, runtimeRoot, {}))
		{
			setState(AiRuntimePhase::Error, "CUDA libraries install failed.", 0.0,
				"Could not download CUDA companion DLLs (" + package.companionDownloadUrl + ").");
			return false;
		}
	}
	else
	{
		setState(AiRuntimePhase::ExtractingRuntime, "Extracting llama.cpp runtime…", 1.0);
	}

	if (locateServerExecutable().empty())
	{
		setState(AiRuntimePhase::Error, "Runtime install incomplete.", 0.0, "llama-server.exe was not found after extraction.");
		return false;
	}
	return true;
}

Bool AiLocalRuntime::ensureModelInstalled()
{
	return true;
}

Bool AiLocalRuntime::startServerIfNeeded()
{
	AiOpenAiClientConfig config;
	if (aiServerResponds(config))
	{
		setState(AiRuntimePhase::Ready, "Connected to local model server (" + serverBackendLabel + ").", 1.0);
		return true;
	}

	const Path serverExe = locateServerExecutable();
	if (serverExe.empty())
	{
		setState(AiRuntimePhase::Error, "Server files missing.", 0.0, "llama-server.exe was not found.");
		return false;
	}

	setState(AiRuntimePhase::DownloadingModel,
		"Downloading " + String(AiConfig::defaultModelDisplayName) + " via llama-server…", 0.0);
	if (serverProcess.isRunning()) serverProcess.terminate();

	const Path workingDirectory = serverExe.parent_path();
	String command =
		"\"" + serverExe.string() + "\""
		" -hf \"" + AiConfig::defaultModelHuggingFaceSpec() + "\""
		" --host 127.0.0.1"
		" --port " + toString(static_cast<Int>(AiConfig::defaultServerPort)) +
		" -c 8192";

	if (serverGpuLayers > 0)
		command += " -ngl " + toString(serverGpuLayers);

	Vector<char> commandBuffer(command.begin(), command.end());
	commandBuffer.push_back('\0');
	serverProcess = startHiddenProcess(commandBuffer.data(), workingDirectory);

	for (Int attempt = 0; attempt < 600 && !shutdownRequested.load(); ++attempt)
	{
		if (aiServerResponds(config))
		{
			setState(AiRuntimePhase::Ready,
				"Local model ready (" + serverBackendLabel + ").", 1.0);
			return true;
		}
		const Double progress = std::min(0.95, static_cast<Double>(attempt) / 600.0);
		setState(AiRuntimePhase::DownloadingModel,
			"Preparing " + String(AiConfig::defaultModelDisplayName)
			+ " on " + serverBackendLabel + " (first launch can take several minutes)…",
			progress);
		sleepMs(1000);
	}

	setState(AiRuntimePhase::Error, "Server did not start.", 0.0,
		"Timed out waiting for llama-server and the Qwen3.5 weights.");
	return false;
}

void AiLocalRuntime::runWorker()
{
	if (!ensureRuntimeInstalled()) return;
	if (shutdownRequested.load()) return;
	if (!ensureModelInstalled()) return;
	if (shutdownRequested.load()) return;
	(void)startServerIfNeeded();
}
