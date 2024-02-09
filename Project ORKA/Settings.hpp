#pragma once

// debugging
constexpr bool debugLoggingIsEnabled   = true;
constexpr bool errorLoggingIsEnabled   = true;
constexpr bool warningLoggingIsEnabled = true;
constexpr bool debugLaunchParameters   = true;

constexpr bool printDeviceInfo            = false;
constexpr bool printULLWithBits           = false;
constexpr bool debugWindowIsEnabled       = false;
constexpr bool debugJobSystemIsEnabled    = false;
constexpr bool debugImageLoadingIsEnabled = false;
constexpr bool debugFramebuffersIsEnabled = false;

// planet renderer
#define MAX_CHUNK_LEVEL 63			// 52 is millimeter precision
#define TERRAIN_TEXTURE_SIZE 64
#define TERRAIN_MAP_SIZE (TERRAIN_TEXTURE_SIZE + 1)
#define ONE_METER_CHUNK_LEVEL 39	// at this level 1 unit = 1 meter
#define CHUNK_USAGE_TICK_COUNT 180	// amount of ticks that a chunk will stay active before it will be deleted (needs to be updated regularly)
constexpr float drawDistance = 4.0f;

// networking
// constexpr UInt port = 12297;
constexpr unsigned int port = 65084;

struct Settings
{
	int   defaultWindowWidth  = 1920;
	int   defaultWindowHeight = 1080;
	float targetFrameRate     = 144.0f;
};

inline Settings settings;
