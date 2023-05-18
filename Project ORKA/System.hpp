#pragma once

#include "Basics.hpp"
#include "Debug.hpp"
#include "Platform.hpp"

ULL availablePhysicalMemory() {
	MEMORYSTATUSEX status;
	status.dwLength = sizeof(status);
	if (!GlobalMemoryStatusEx(&status)) { logError("Failed to get memory status"); }
	return status.ullAvailPhys;
}

ULL totalPhysicalMemory() {
	MEMORYSTATUSEX status;
	status.dwLength = sizeof(status);
	if (!GlobalMemoryStatusEx(&status)) { logError("Failed to get memory status"); }
	return status.ullTotalPhys;
}
