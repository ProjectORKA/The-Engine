#pragma once

#include "Basics.hpp"

// debugging
constexpr Bool debugLoggingIsEnabled = true;
constexpr Bool errorLoggingIsEnabled = true;
constexpr Bool warningLoggingIsEnabled = true;
constexpr Bool debugImageLoadingIsEnabled = false;
constexpr Bool debugFramebuffersIsEnabled = false;

// planet renderer
#define MAX_CHUNK_LEVEL 63			// 52 is millimeter precision
#define TERRAIN_TEXTURE_SIZE 32
#define TERRAIN_MAP_SIZE (TERRAIN_TEXTURE_SIZE + 1)
#define ONE_METER_CHUNK_LEVEL 39	// at this level 1 unit = 1 meter
#define CHUNK_USAGE_TICK_COUNT 180	// amount of ticks that a chunk will stay active before it will be deleted (needs to be updated regularly)
constexpr Float drawDistance = 4.0f;

// networking
// constexpr UInt port = 12297;
constexpr UInt port = 65084;

struct Settings
{
	Int   defaultWindowWidth  = 1920;
	Int   defaultWindowHeight = 1080;
	Float targetFrameRate     = 144.0f;
};

inline Settings settings;
