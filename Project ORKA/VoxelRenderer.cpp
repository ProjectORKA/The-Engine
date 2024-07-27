#include "VoxelRenderer.hpp"
#include "Renderer.hpp"
#include "PerlinNoise.hpp"

Chunk::Chunk()
{
	for(Int x = 0; x < voxelChunkSize; x++)
	{
		for(Int y = 0; y < voxelChunkSize; y++)
		{
			for(Int z = voxelChunkHeight - 1; z > 0; z--) voxels[x][y][z] = z / static_cast<Float>(voxelChunkHeight) * 2 < 1 + noise.octaveNoise0_1(x / static_cast<Float>(voxelChunkSize), y / static_cast<Float>(voxelChunkSize), 16);
		}
	}
}

CpuMesh Chunk::generateMesh() const
{
	CpuMesh terrain;
	terrain.name          = "terrain";
	terrain.drawMode      = BufferUsage::StaticDraw;
	terrain.primitiveMode = PrimitiveMode::Triangles;

	CpuMesh topFace;
	topFace.load("voxelTop");
	CpuMesh bottomFace;
	bottomFace.load("voxelBot");
	CpuMesh rightFace;
	rightFace.load("voxelRight");
	CpuMesh leftFace;
	leftFace.load("voxelLeft");
	CpuMesh frontFace;
	frontFace.load("voxelFront");
	CpuMesh backFace;
	backFace.load("voxelBack");

	for(Int x = 0; x < voxelChunkSize; x++)
	{
		for(Int y = 0; y < voxelChunkSize; y++)
		{
			for(Int z = 0; z < voxelChunkHeight; z++)
			{
				CpuMesh currentFace;

				if(voxels[x][y][z])
				{
					// is terrain
					if(x < voxelChunkSize - 1 && !voxels[x + 1][y][z])
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

					if(y < voxelChunkSize - 1 && !voxels[x][y + 1][z])
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
					if(z < voxelChunkHeight && !voxels[x][y][z + 1])
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

void VoxelRenderer::render(Renderer& renderer)
{
	if(!terrainMesh.isLoaded()) terrainMesh.upload(world.chunk.generateMesh());
	else terrainMesh.render(renderer.uniforms());
}
