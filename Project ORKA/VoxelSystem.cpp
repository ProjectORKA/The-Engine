#include "VoxelSystem.hpp"

#include "Renderer.hpp"

Chunk::Chunk() {
	for (Int x = 0; x < VOXEL_CHUNK_SIZE; x++) {
		for (Int y = 0; y < VOXEL_CHUNK_SIZE; y++) {
			for (Int z = VOXEL_CHUNK_HEIGHT - 1; z > 0; z--) {
				voxels[x][y][z] = (z / static_cast<Float>(VOXEL_CHUNK_HEIGHT) * 2) < 1 + noise.octaveNoise0_1(
					x / static_cast<Float>(VOXEL_CHUNK_SIZE), y / static_cast<Float>(VOXEL_CHUNK_SIZE), 16);
			}
		}
	}
}

CpuMesh Chunk::generateMesh(ResourceManager& resourceManager) const {
	CpuMesh terrain;
	terrain.name = "terrain";
	terrain.drawMode = MeshDrawMode::staticMode;
	terrain.primitiveMode = PrimitiveMode::Triangles;

	CpuMesh topFace;
	topFace.load(resourceManager, "voxelTop");
	CpuMesh bottomFace;
	bottomFace.load(resourceManager, "voxelBot");
	CpuMesh rightFace;
	rightFace.load(resourceManager, "voxelRight");
	CpuMesh leftFace;
	leftFace.load(resourceManager, "voxelLeft");
	CpuMesh frontFace;
	frontFace.load(resourceManager, "voxelFront");
	CpuMesh backFace;
	backFace.load(resourceManager, "voxelBack");

	for (Int x = 0; x < VOXEL_CHUNK_SIZE; x++) {
		for (Int y = 0; y < VOXEL_CHUNK_SIZE; y++) {
			for (Int z = 0; z < VOXEL_CHUNK_HEIGHT; z++) {
				CpuMesh currentFace;

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
	terrain.removeDoubles();
	terrain.calculateSmoothNormals();

	return terrain;
}

void VoxelRenderer::render(ResourceManager& resourceManager, Renderer& renderer) {
	if (!terrainMesh.loaded)terrainMesh.upload(world.chunk.generateMesh(resourceManager));
	else terrainMesh.render(renderer.uniforms());
}
