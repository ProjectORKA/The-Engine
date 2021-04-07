
#pragma once

#include "Basics.hpp"
#include "Settings.hpp"

#define HEIGHTMAP_FOR_NORMALS_SIZE (HEIGHTMAP_SIZE + 2)

using Heightmap				= Float[HEIGHTMAP_SIZE][HEIGHTMAP_SIZE];
using HeightmapForNormals	= Float[HEIGHTMAP_FOR_NORMALS_SIZE][HEIGHTMAP_FOR_NORMALS_SIZE];