
#include "VoxelSystem.hpp"

#include "Renderer.hpp"

Chunk::Chunk() {
	for (Int x = 0; x < VOXEL_CHUNK_SIZE; x++) {
		for (Int y = 0; y < VOXEL_CHUNK_SIZE; y++) {
			for (Int z = VOXEL_CHUNK_HEIGHT - 1; z > 0; z--) {
				voxels[x][y][z] = (z / Float(VOXEL_CHUNK_HEIGHT) * 2) < 1 + noise.octaveNoise0_1(x / Float(VOXEL_CHUNK_SIZE), y / Float(VOXEL_CHUNK_SIZE), 16);
			}
		}
	}
}

CPUMesh Chunk::generateMesh() {

	CPUMesh terrain("terrain", MeshDrawMode::staticMode, PrimitiveMode::Triangles);

	CPUMesh topFace("voxelTop");
	CPUMesh bottomFace("voxelBot");
	CPUMesh rightFace("voxelRight");
	CPUMesh leftFace("voxelLeft");
	CPUMesh frontFace("voxelFront");
	CPUMesh backFace("voxelBack");


	for (Int x = 0; x < VOXEL_CHUNK_SIZE; x++) {
		for (Int y = 0; y < VOXEL_CHUNK_SIZE; y++) {
			for (Int z = 0; z < VOXEL_CHUNK_HEIGHT; z++) {

				CPUMesh currentFace;

				if (voxels[x][y][z]) {
					//is terrain
					if (x < VOXEL_CHUNK_SIZE - 1 && !voxels[x + 1][y][z]) {
						currentFace = rightFace;
						currentFace.move(Vec3(2 * x, 2 * y, 2 * z));
						terrain.merge(currentFace);
					}
					if (x > 0 && !voxels[x - 1][y][z]) {
						currentFace = leftFace;
						currentFace.move(Vec3(2 * x, 2 * y, 2 * z));
						terrain.merge(currentFace);
					}

					if (y < VOXEL_CHUNK_SIZE - 1 && !voxels[x][y + 1][z]) {
						currentFace = frontFace;
						currentFace.move(Vec3(2 * x, 2 * y, 2 * z));
						terrain.merge(currentFace);
					}
					if (y > 0 && !voxels[x][y - 1][z]) {
						currentFace = backFace;
						currentFace.move(Vec3(2 * x, 2 * y, 2 * z));
						terrain.merge(currentFace);
					}
					if (z < VOXEL_CHUNK_HEIGHT && !voxels[x][y][z + 1]) {
						currentFace = topFace;
						currentFace.move(Vec3(2 * x, 2 * y, 2 * z));
						terrain.merge(currentFace);
					}

					if (z > 0 && !voxels[x][y][z - 1]) {
						currentFace = bottomFace;
						currentFace.move(Vec3(2 * x, 2 * y, 2 * z));
						terrain.merge(currentFace);
					}
				}
			}
		}
	}

	terrain.checkIntegrity();
	return terrain;
}

void VoxelRenderer::render(Renderer& renderer, VoxelWorld& world) {
	if (!terrainMesh.loaded)terrainMesh.upload(world.chunk.generateMesh());
	else terrainMesh.render(renderer.uniforms());
}