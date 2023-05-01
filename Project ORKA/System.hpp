#pragma once

#include "Basics.hpp"
#include "Debug.hpp"

UInt availablePhysicalMemory() {
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    if (!GlobalMemoryStatusEx(&status)) {
        logError("Failed to get memory status");
    }
    return status.ullAvailPhys;
}

UInt totalPhysicalMemory() {
	MEMORYSTATUSEX status;
	status.dwLength = sizeof(status);
    if (!GlobalMemoryStatusEx(&status)) {
		logError("Failed to get memory status");
	}
	return status.ullTotalPhys;
}