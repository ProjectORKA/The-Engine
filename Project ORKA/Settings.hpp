#pragma once

#include "Basics.hpp"

#define MAX_CHUNK_LEVEL 32			//52 is milimeter precision
#define HEIGHTMAP_SIZE 36			//24
#define TARGETFRAMERATE 144
//#define DEFAULT_WINDOW_WIDTH 1920
//#define DEFAULT_WINDOW_HEIGHT 1080
#define ONE_METER_CHUNK_LEVEL 39	//at this level 1 unit = 1 meter
#define CHUNK_USAGE_TICK_COUNT 180	//amount of ticks that a chunk will stay active before it will be deleted (needs to be updated regularly)
extern Float drawDistance;