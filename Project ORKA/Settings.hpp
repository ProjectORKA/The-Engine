#pragma once

// debugging
#ifdef _DEBUG
constexpr bool debugLoggingIsEnabled   = true;
constexpr bool errorLoggingIsEnabled   = true;
constexpr bool warningLoggingIsEnabled = true;
constexpr bool debugBreakOnWarning     = true;
constexpr bool debugBreakOnError       = true;
constexpr bool debugLaunchParameters   = true;
constexpr bool openglStateTracking     = true;
constexpr bool errorCheckingEnabled    = true;
constexpr bool debugFramebuffersIsEnabled = true;
constexpr bool debugAudioSystem           = true;
constexpr bool printDeviceInfo            = false;
constexpr bool printULLWithBits           = false;
constexpr bool debugWindowIsEnabled       = false;
constexpr bool debugJobSystemIsEnabled    = false;
constexpr bool debugImageLoadingIsEnabled = false;
#define DEBUG_OPENGL
#define TRACE_OPENGL
#define CHECK_OPENGL
#else
constexpr bool debugLoggingIsEnabled   = false;
constexpr bool errorLoggingIsEnabled   = true;
constexpr bool warningLoggingIsEnabled = true;
constexpr bool debugLaunchParameters   = false;
constexpr bool debugBreakOnWarning     = false;
constexpr bool debugBreakOnError       = false;
constexpr bool openglStateTracking	   = false;
constexpr bool errorCheckingEnabled    = true;
constexpr bool debugFramebuffersIsEnabled = false;
constexpr bool debugAudioSystem           = false;
constexpr bool printDeviceInfo            = false;
constexpr bool printULLWithBits           = false;
constexpr bool debugWindowIsEnabled       = false;
constexpr bool debugJobSystemIsEnabled    = false;
constexpr bool debugImageLoadingIsEnabled = false;
#endif

// mesh system
constexpr int supportedMeshVersion = 3;

// planet renderer
#define MAX_RENDER_CHUNKS 256
#define MAX_CHUNK_LEVEL 63			 // 52 is millimeter precision
#define TERRAIN_TEXTURE_SIZE 32		 // 32 is good enough
#define TERRAIN_MAP_SIZE (TERRAIN_TEXTURE_SIZE + 1)
#define ONE_METER_CHUNK_LEVEL 39	 // at this level 1 unit = 1 meter
#define CHUNK_USAGE_TICK_COUNT 180	 // amount of ticks that a chunk will stay active before it will be deleted (needs to be updated regularly)
constexpr float drawDistance = 4.0f; 

// networking
// constexpr UInt port = 12297;
constexpr unsigned int port = 65084;

struct Settings
{
	int   defaultWindowWidth  = 1920;
	int   defaultWindowHeight = 1080;
	float targetFrameRate     = 144.0f;
	bool  vsyncEnabled        = true;
	bool  shareOpenGLContexts = false;
	bool  transparentFramebufferEnabled = false;
};

inline Settings settings;
