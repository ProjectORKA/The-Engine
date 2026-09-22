#pragma once

#include "../OrkaTypes.hpp"
#include "AiProcess.hpp"

enum class AiRuntimePhase : Int
{
	Idle,
	DownloadingRuntime,
	ExtractingRuntime,
	DownloadingModel,
	StartingServer,
	Ready,
	Error
};

struct AiRuntimeSnapshot
{
	AiRuntimePhase phase = AiRuntimePhase::Idle;
	String statusLine;
	Double downloadProgress = 0.0;
	String error;
	String computeBackend = "CPU";
};

struct AiLocalRuntime
{
	~AiLocalRuntime();

	void shutdown();
	[[nodiscard]] AiRuntimeSnapshot snapshot() const;
	void ensureReadyAsync();

private:
	mutable Mutex mutex;
	AiRuntimeSnapshot state;
	AiChildProcess serverProcess;
	Int serverGpuLayers = 0;
	String serverBackendLabel = "CPU";
	Thread worker;
	Atomic<Bool> workerRunning{false};
	Atomic<Bool> shutdownRequested{false};

	void setState(AiRuntimePhase phase, const String& statusLine, Double progress = 0.0, const String& error = {});
	void runWorker();
	[[nodiscard]] Path locateServerExecutable() const;
	[[nodiscard]] Bool ensureRuntimeInstalled();
	[[nodiscard]] Bool ensureModelInstalled();
	[[nodiscard]] Bool startServerIfNeeded();
};
