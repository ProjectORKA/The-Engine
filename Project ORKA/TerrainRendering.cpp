#include "TerrainRendering.hpp"
#include "Math.hpp"
#include "Random.hpp"

CPUMesh createTerrainMesh(AdvancedHeightMap & heightmap)
{
	if (heightmap.loaded) {
		CPUMesh mesh;
		mesh.name = "terrain";
		mesh.primitiveMode = PrimitiveMode::TriangleStrip;
		mesh.clearGeometry();

		for (Int y = 0; y < TERRAIN_MAP_SIZE; y++) {
			for (Int x = 0; x < TERRAIN_MAP_SIZE; x++) {
				//create vertices
				Vec3 position;
				position.x = Float(x) / Float(TERRAIN_MAP_SIZE - 1);
				position.y = Float(y) / Float(TERRAIN_MAP_SIZE - 1);
				if (ULL(heightmap.upperLimit) == ULL(heightmap.lowerLimit)) position.z = 0;
				else position.z = Float((heightmap.height[x][y] - heightmap.lowerLimit) / (heightmap.upperLimit - heightmap.lowerLimit));
				mesh.vertices.push_back(position);
				
				//create normals
				mesh.normals.push_back(Vec3(0,0,1));
				
				//create uvs	
				mesh.uvs.push_back(Vec2(position.x, position.y));

				//create vertex colors
				mesh.colors.push_back(randomVec3());
			}
		}
		
		//create indices
		for (UInt y = 0; y < TERRAIN_MAP_SIZE - 1; y++) {	//very top row doesn't have triangle strip
			for (UInt x = 0; x < TERRAIN_MAP_SIZE; x++) {
				if ((x == 0) && (y > 0))mesh.indices.push_back(x + TERRAIN_MAP_SIZE * (y + 1));
				mesh.indices.push_back(x + (TERRAIN_MAP_SIZE * (y + 1)));
				mesh.indices.push_back(x + (TERRAIN_MAP_SIZE * y));
				if ((x == (TERRAIN_MAP_SIZE - 1)) && (y < (TERRAIN_MAP_SIZE - 1))) mesh.indices.push_back(x + TERRAIN_MAP_SIZE * y);
			}
		}

		mesh.calculateSmoothNormals();

		mesh.checkIntegrity();
		return mesh;
	}
	else logError("Heightmap not loaded!");
}