#include "TerrainRendering.hpp"
#include "Random.hpp"

CpuMesh createTerrainMesh(const AdvancedHeightMap& heightmap)
{
	if(heightmap.loaded)
	{
		CpuMesh mesh;
		mesh.name          = "terrain";
		mesh.primitiveMode = PrimitiveMode::TriangleStrip;
		mesh.clearGeometry();

		for(Int y = 0; y < TERRAIN_MAP_SIZE; y++)
		{
			for(Int x = 0; x < TERRAIN_MAP_SIZE; x++)
			{
				// create vertices
				Vec3 position;
				position.x = static_cast<Float>(x) / static_cast<Float>(TERRAIN_MAP_SIZE - 1);
				position.y = static_cast<Float>(y) / static_cast<Float>(TERRAIN_MAP_SIZE - 1);
				if(static_cast<ULL>(heightmap.upperLimit) == static_cast<ULL>(heightmap.lowerLimit))
				{
					position.z = 0;
				}
				else
				{
					position.z                                                                                 = static_cast<Float>((heightmap.height[x][y] - heightmap.lowerLimit) / (heightmap.upperLimit - heightmap.lowerLimit));
				}
				mesh.positions.push_back(position);

				// create normals
				mesh.normals.emplace_back(0, 0, 1);

				// create uvs	
				mesh.textureCoordinates.emplace_back(position.x, position.y);

				// create vertex colors
				mesh.vertexColors.push_back(randomVec3Fast());
			}
		}

		// create indices
		for(UInt y = 0; y < TERRAIN_MAP_SIZE - 1; y++)
		{
			// very top row doesn't have triangle strip
			for(UInt x = 0; x < TERRAIN_MAP_SIZE; x++)
			{
				if(x == 0 && y > 0) mesh.indices.push_back(x + TERRAIN_MAP_SIZE * (y + 1));
				mesh.indices.push_back(x + TERRAIN_MAP_SIZE * (y + 1));
				mesh.indices.push_back(x + TERRAIN_MAP_SIZE * y);
				if(x == TERRAIN_MAP_SIZE - 1 && y < TERRAIN_MAP_SIZE - 1) mesh.indices.push_back(x + TERRAIN_MAP_SIZE * y);
			}
		}

		mesh.calculateSmoothNormals();

		mesh.checkIntegrity();
		return mesh;
	}
	logError("Heightmap not loaded!");
}
