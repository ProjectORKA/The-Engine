
#include "Platform.hpp"
#include "Windows.hpp"

ULong totalPhysicalMemoryInBytes()
{
	MEMORYSTATUSEX status;
	status.dwLength = sizeof status;
	if(!GlobalMemoryStatusEx(&status)) logError("Failed to get memory status");
	return status.ullTotalPhys;
}

ULong availablePhysicalMemoryInBytes()
{
	MEMORYSTATUSEX status;
	status.dwLength = sizeof status;
	if(!GlobalMemoryStatusEx(&status)) logError("Failed to get memory status");
	return status.ullAvailPhys;
}
