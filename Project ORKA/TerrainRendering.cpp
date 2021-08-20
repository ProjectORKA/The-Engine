#include "TerrainRendering.hpp"
#include "Math.hpp"

CPUMesh createTerrainMesh(HeightmapForNormals& heightmapForNormals)
{
	CPUMesh mesh;
	mesh.readyForUpload = false;
	mesh.name = "terrain";
	mesh.primitiveMode = PrimitiveMode::TriangleStrip;
	mesh.vertices.clear();
	mesh.indices.clear();
	mesh.normals.clear();
	mesh.uvs.clear();

	for (Int y = 0; y < HEIGHTMAP_FOR_NORMALS_SIZE; y++) {
		for (Int x = 0; x < HEIGHTMAP_FOR_NORMALS_SIZE; x++) {
			//create vertecies
			Vec3 position;
			position.x = Float(x - 1) / Float(HEIGHTMAP_SIZE - 1);
			position.y = Float(y - 1) / Float(HEIGHTMAP_SIZE - 1);
			if ((x == 0) || (x == (HEIGHTMAP_FOR_NORMALS_SIZE - 1)) || (y == 0) || (y == (HEIGHTMAP_FOR_NORMALS_SIZE - 1))) position.z = heightmapForNormals[x][y] - 0.025;
			else position.z = heightmapForNormals[x][y];

			mesh.vertices.push_back(position);
			//create uvs
			mesh.uvs.push_back(Vec2(position.x, position.y));
		}
	}

	//create indices
	for (Int y = 0; y < HEIGHTMAP_FOR_NORMALS_SIZE - 1; y++) {	//very top row doesn't have triangle strip
		for (Int x = 0; x < HEIGHTMAP_FOR_NORMALS_SIZE; x++) {
			if ((x == 0) && (y > 0))mesh.indices.push_back(x + HEIGHTMAP_FOR_NORMALS_SIZE * (y + 1));

			mesh.indices.push_back(x + HEIGHTMAP_FOR_NORMALS_SIZE * (y + 1));
			mesh.indices.push_back(x + HEIGHTMAP_FOR_NORMALS_SIZE * y);

			if ((x == (HEIGHTMAP_FOR_NORMALS_SIZE - 1)) && (y < (HEIGHTMAP_FOR_NORMALS_SIZE - 1))) mesh.indices.push_back(x + HEIGHTMAP_FOR_NORMALS_SIZE * y);
		}
	}

	//create normals
	for (Int y = 0; y < HEIGHTMAP_FOR_NORMALS_SIZE; y++) {		//dont calculate border
		for (Int x = 0; x < HEIGHTMAP_FOR_NORMALS_SIZE; x++) {	//dont calculate border
			Int customX = clamp(x, 1, HEIGHTMAP_FOR_NORMALS_SIZE - 2);
			Int customY = clamp(y, 1, HEIGHTMAP_FOR_NORMALS_SIZE - 2);
			Float distance = 2.0 / HEIGHTMAP_SIZE;
			
			Float normalX = (heightmapForNormals[customX + 1][customY] - heightmapForNormals[customX - 1][customY]);
			Float normalY = (heightmapForNormals[customX][customY + 1] - heightmapForNormals[customX][customY - 1]);
			Float normalZ = - 4 * distance;

			Vec3 normal = -normalize(Vec3(normalX, normalY, normalZ));
			mesh.normals.push_back(normal);
		}
	}
	mesh.readyForUpload = true;
	return mesh;
}