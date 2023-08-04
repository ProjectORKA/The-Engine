#include "VoxelRenderer.hpp"
#include "Renderer.hpp"
#include "PerlinNoise.hpp"

Chunk::Chunk()
{
	for(Int x = 0; x < VOXEL_CHUNK_SIZE; x++)
	{
		for(Int y = 0; y < VOXEL_CHUNK_SIZE; y++)
		{
			for(Int z = VOXEL_CHUNK_HEIGHT - 1; z > 0; z--) voxels[x][y][z] = z / static_cast<Float>(VOXEL_CHUNK_HEIGHT) * 2 < 1 + noise.octaveNoise0_1(x / static_cast<Float>(VOXEL_CHUNK_SIZE), y / static_cast<Float>(VOXEL_CHUNK_SIZE), 16);
		}
	}
}

CPUMesh Chunk::generateMesh(ResourceManager& resourceManager) const
{
	CPUMesh terrain;
	terrain.name          = "terrain";
	terrain.drawMode      = BufferUsage::StaticDraw;
	terrain.primitiveMode = PrimitiveMode::Triangles;

	CPUMesh topFace;
	topFace.load(resourceManager, "voxelTop");
	CPUMesh bottomFace;
	bottomFace.load(resourceManager, "voxelBot");
	CPUMesh rightFace;
	rightFace.load(resourceManager, "voxelRight");
	CPUMesh leftFace;
	leftFace.load(resourceManager, "voxelLeft");
	CPUMesh frontFace;
	frontFace.load(resourceManager, "voxelFront");
	CPUMesh backFace;
	backFace.load(resourceManager, "voxelBack");

	for(Int x = 0; x < VOXEL_CHUNK_SIZE; x++)
	{
		for(Int y = 0; y < VOXEL_CHUNK_SIZE; y++)
		{
			for(Int z = 0; z < VOXEL_CHUNK_HEIGHT; z++)
			{
				CPUMesh currentFace;

				if(voxels[x][y][z])
				{
					// is terrain
					if(x < VOXEL_CHUNK_SIZE - 1 && !voxels[x + 1][y][z])
					{
						currentFace = rightFace;
						currentFace.move(Vec3(2 * x, 2 * y, 2 * z));
						terrain.merge(currentFace);
					}
					if(x > 0 && !voxels[x - 1][y][z])
					{
						currentFace = leftFace;
						currentFace.move(Vec3(2 * x, 2 * y, 2 * z));
						terrain.merge(currentFace);
					}

					if(y < VOXEL_CHUNK_SIZE - 1 && !voxels[x][y + 1][z])
					{
						currentFace = frontFace;
						currentFace.move(Vec3(2 * x, 2 * y, 2 * z));
						terrain.merge(currentFace);
					}
					if(y > 0 && !voxels[x][y - 1][z])
					{
						currentFace = backFace;
						currentFace.move(Vec3(2 * x, 2 * y, 2 * z));
						terrain.merge(currentFace);
					}
					if(z < VOXEL_CHUNK_HEIGHT && !voxels[x][y][z + 1])
					{
						currentFace = topFace;
						currentFace.move(Vec3(2 * x, 2 * y, 2 * z));
						terrain.merge(currentFace);
					}

					if(z > 0 && !voxels[x][y][z - 1])
					{
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

void VoxelRenderer::render(ResourceManager& resourceManager, Renderer& renderer)
{
	if(!terrainMesh.isLoaded()) terrainMesh.upload(world.chunk.generateMesh(resourceManager));
	else terrainMesh.render(renderer.uniforms());
}
