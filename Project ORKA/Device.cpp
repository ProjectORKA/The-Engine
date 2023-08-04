#include "Device.hpp"
#include "Debug.hpp"
#include "Platform.hpp"

ULL totalPhysicalMemory()
{
	MEMORYSTATUSEX status;
	status.dwLength = sizeof status;
	if(!GlobalMemoryStatusEx(&status)) logError("Failed to get memory status");
	return status.ullTotalPhys;
}

ULL availablePhysicalMemory()
{
	MEMORYSTATUSEX status;
	status.dwLength = sizeof status;
	if(!GlobalMemoryStatusEx(&status)) logError("Failed to get memory status");
	return status.ullAvailPhys;
}
