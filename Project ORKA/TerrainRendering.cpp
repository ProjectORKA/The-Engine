#include "TerrainRendering.hpp"

CPUMesh createTerrainMesh(HeightmapForNormals& heightmapForNormals)
{
	CPUMesh mesh;
	//heightmap
	mesh.readyForUpload = false;
	mesh.name = "terrain";
	mesh.primitiveMode = PrimitiveMode::Triangles;

	mesh.vertices.clear();
	mesh.indices.clear();
	mesh.normals.clear();
	mesh.uvs.clear();

	Vector<Vector<Vec3>> positions;

	for (Int y = 0; y < HEIGHTMAP_FOR_NORMALS_SIZE; y++) {
		Vector<Vec3> positionsX;
		for (Int x = 0; x < HEIGHTMAP_FOR_NORMALS_SIZE; x++) {
			Vec3 tmp = Vec3(Float(x - 1) / Float(HEIGHTMAP_SIZE - 1), Float(y - 1) / Float(HEIGHTMAP_SIZE - 1), heightmapForNormals[y][x]);
			positionsX.push_back(tmp);
		}
		positions.push_back(positionsX);
	}

	for (Int y = 0; y < HEIGHTMAP_SIZE; y++) {
		for (Int x = 0; x < HEIGHTMAP_SIZE; x++) {

			//vertices
			mesh.vertices.push_back(positions[x + 1][y + 1]);
			mesh.uvs.push_back(Vec2(positions[x + 1][y + 1].x, positions[x + 1][y + 1].y));

			//normals
			Vec3 normalTopLeft = glm::cross(positions[x + 1][y + 2] - positions[x + 1][y + 1], positions[x + 2][y + 1] - positions[x + 1][y + 1]);
			Vec3 normalBotRight = glm::cross(positions[x + 1][y] - positions[x + 1][y + 1], positions[x][y + 1] - positions[x + 1][y + 1]);
			Vec3 normal = glm::normalize(normalTopLeft + normalBotRight);
			mesh.normals.push_back(normal);
		}
	}

	for (Int x = 0; x < HEIGHTMAP_SIZE - 1; x++) {
		for (Int y = 0; y < HEIGHTMAP_SIZE - 1; y++) {
			//first triangle
			mesh.indices.push_back((y) * (HEIGHTMAP_SIZE)+x);
			mesh.indices.push_back((y + 1) * (HEIGHTMAP_SIZE)+x);
			mesh.indices.push_back((y) * (HEIGHTMAP_SIZE)+x + 1);

			////second triangle
			mesh.indices.push_back((y) * (HEIGHTMAP_SIZE)+x + 1);
			mesh.indices.push_back((y + 1) * (HEIGHTMAP_SIZE)+x);
			mesh.indices.push_back((y + 1) * (HEIGHTMAP_SIZE)+x + 1);
		}
	}

	mesh.readyForUpload = true;
	return mesh;
}
CPUTexture createHeightmapTexture(HeightmapForNormals& heightmapForNormals)
{
	CPUTexture texture;
	//heightmap
	texture.channels = 1;
	texture.dataType = dataTypeFloat;
	texture.nearFilter = Filter::linear;
	texture.farFilter = Filter::linearMM;
	texture.height = HEIGHTMAP_SIZE;
	texture.name = "heightmap";
	texture.width = HEIGHTMAP_SIZE;
	texture.wrapping = clamped;
	texture.loaded = false;
	texture.floatPixels = new Float[HEIGHTMAP_SIZE * HEIGHTMAP_SIZE];

	for (int y = 0; y < HEIGHTMAP_SIZE; y++) {
		for (int x = 0; x < HEIGHTMAP_SIZE; x++) {
			texture.floatPixels[x * HEIGHTMAP_SIZE + y] = heightmapForNormals[x + 1][y + 1];
		}
	}

	texture.loaded = true;
	return texture;
}