#pragma once

#include "Basics.hpp"

#define TARGETFRAMERATE 144

//planet system
#define MAX_CHUNK_LEVEL 63			//52 is millimeter precision
#define ONE_METER_CHUNK_LEVEL 39	//at this level 1 unit = 1 meter
#define CHUNK_USAGE_TICK_COUNT 180	//amount of ticks that a chunk will stay active before it will be deleted (needs to be updated regularly)
extern Float drawDistance;

//terrain rendering
#define TERRAIN_TEXTURE_SIZE 65
#define TERRAIN_MAP_SIZE (TERRAIN_TEXTURE_SIZE + 1)

//terrain generation
#define TERRAIN_ROUGHNESS 1.7
#define SEALEVEL ULLONG_MAX / 2 //1677721600000
#define MAX_TERRAIN_TILE_HEIGHT LLONG_MAX / 300
#define TERRAIN_GENERATION_SEED 645123587412588622