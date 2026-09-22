#pragma once

#include "../OrkaTypes.hpp"

enum class AiGpuBackend : Int
{
	Cpu = 0,
	Cuda = 1
};

struct AiGpuInfo
{
	AiGpuBackend backend = AiGpuBackend::Cpu;
	Bool nvidiaDriverPresent = false;
	String deviceLabel;
};

[[nodiscard]] AiGpuInfo detectAiGpu();
