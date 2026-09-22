#include "AiGpuDetect.hpp"

#include "../Windows.hpp"

AiGpuInfo detectAiGpu()
{
	AiGpuInfo info;
	if (HMODULE module = LoadLibraryA("nvcuda.dll"))
	{
		FreeLibrary(module);
		info.nvidiaDriverPresent = true;
		info.backend = AiGpuBackend::Cuda;
		info.deviceLabel = "NVIDIA CUDA";
		return info;
	}

	info.deviceLabel = "CPU";
	return info;
}
