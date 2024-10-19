#include "QuadtreeSystemRenderer.hpp"
#include "Renderer.hpp"
#include "PlanetSystemPlayer.hpp"
#include "RelativeChunkOffset.hpp"
#include "TerrainRendering.hpp"

QuadtreeNodeRenderData* needsSubDiv = nullptr;

void QuadtreeRenderSystem::destroy(Renderer& renderer)
{
	root.destroy(renderer);
}

void QuadtreeNodeRenderData::destroy(Renderer& renderer)
{
	unsubdivide(renderer);
	terrainMesh.unload(renderer);
	equivalentQuadtreeNode->decrementUsers();
}

void QuadtreeRenderSystem::count() const
{
	root.count();
}

void QuadtreeNodeRenderData::subdivide()
{
	if(!subdivided)
	{
		c00 = new QuadtreeNodeRenderData();
		c01 = new QuadtreeNodeRenderData();
		c10 = new QuadtreeNodeRenderData();
		c11 = new QuadtreeNodeRenderData();

		c00->create(*equivalentQuadtreeNode->c00);
		c01->create(*equivalentQuadtreeNode->c01);
		c10->create(*equivalentQuadtreeNode->c10);
		c11->create(*equivalentQuadtreeNode->c11);

		subdivided = true;
	}
}

void QuadtreeNodeRenderData::unsubdivide(Renderer& renderer)
{
	if(subdivided)
	{
		c00->destroy(renderer);
		c01->destroy(renderer);
		c10->destroy(renderer);
		c11->destroy(renderer);

		delete c00;
		delete c01;
		delete c10;
		delete c11;

		c00 = nullptr;
		c01 = nullptr;
		c10 = nullptr;
		c11 = nullptr;

		subdivided = false;
	}
}

void QuadtreeNodeRenderData::count() const
{
	static Int nodeCount = 0;
	if(equivalentQuadtreeNode->id.level == 0) nodeCount = 0;
	nodeCount++;
	if(subdivided)
	{
		c00->count();
		c01->count();
		c10->count();
		c11->count();
	}
	if(equivalentQuadtreeNode->id.level == 0) logDebug("QuadTreeNodeRenderData count: " + toString(nodeCount));
}

void QuadtreeNodeRenderData::loadTerrainMesh(Renderer& renderer)
{
	if(!terrainMesh.isLoaded() && equivalentQuadtreeNode->data.terrain)
	{
		const CpuMesh cpuMesh = createTerrainMesh(equivalentQuadtreeNode->data.terrain->heightmap);
		terrainMesh.upload(renderer, cpuMesh);
	}
}

void QuadtreeRenderSystem::update(Renderer& renderer, PlanetSystemPlayer& player)
{
	if(needsSubDiv)
	{
		needsSubDiv->subdivide();
		needsSubDiv = nullptr;
	}
	root.update(renderer, player);
}

void QuadtreeNodeRenderData::renderTerrain(Renderer& renderer)
{
	const QuadtreeNode& node = *equivalentQuadtreeNode;

	loadTerrainMesh(renderer);
	const Double actualHeight = node.data.terrain->heightmap.upperLimit - node.data.terrain->heightmap.lowerLimit;
	Double       heightScale;
	if(actualHeight == 0.0f)
	{
		heightScale = 1.0f;
	}
	else
	{
		const Double currentScale = pow(2, 64 - node.id.level);
		heightScale               = actualHeight / currentScale;
	}
	const Matrix m = matrixFromSize(Vec3(1, 1, heightScale));
	renderer.uniforms().setMMatrix(m);
	renderer.uniforms().setWorldOffset(Vec4(node.id.position.x, node.id.position.y, node.data.terrain->heightmap.lowerLimit / static_cast<LDouble>(ULLONG_MAX), node.id.level));

	if(node.id.level < 4)
	{
		// for the higher level chunks we duplicated them 
		for(Int x = -1; x < 2; x++)
		{
			for(Int y = -1; y < 2; y++)
			{
				renderer.uniforms().setCameraPos(Vec4(cameraPosition - Vec3(x * pow(2, node.id.level), y * pow(2, node.id.level), 0), 1));
				terrainMesh.render(renderer.uniforms());
			}
		}
	}
	else
	{
		renderer.uniforms().setCameraPos(Vec4(cameraPosition, 1));
		terrainMesh.render(renderer.uniforms());
	}
}

void QuadtreeNodeRenderData::create(QuadtreeNode& quadtreeNode)
{
	equivalentQuadtreeNode = &quadtreeNode;

	quadtreeNode.incrementUsers();
}

void QuadtreeNodeRenderData::update(Renderer& renderer, PlanetSystemPlayer& player)
{
	updateWithoutSubdivision(renderer, player);
}

void QuadtreeRenderSystem::renderLevel(const UShort level, Renderer& renderer)
{
	root.renderTerrainLevel(level, renderer);
}

void QuadtreeNodeRenderData::updateWithoutSubdivision(Renderer& renderer, PlanetSystemPlayer& player)
{
	// create 3d chunk id
	OctreeId tmp;

	const QuadtreeNode& eq = *equivalentQuadtreeNode;

	tmp.level      = eq.id.level;
	tmp.position.x = eq.id.position.x;
	tmp.position.y = eq.id.position.y;

	if(eq.data.terrain)
	{
		tmp.position.z = static_cast<ULL>(eq.data.terrain->heightmap.lowerLimit);
	}
	else
	{
		tmp.position.z                = 0;
	}

	// tmp.position.z = 0;

	// logDebug(LDouble(tmp.position.z) / pow(2, 64));

	// get camera relative position
	cameraPosition = relativeCameraPosition(tmp, player.chunkPosition, player.camera.getPosition());

	nodeDrawDistance          = length(cameraPosition - Vec3(0.5, 0.5, 0));
	const Bool inDrawDistance = nodeDrawDistance < drawDistance;
	Bool       notCulled      = dot(normalize(Vec3(0.5, 0.5, 0) - cameraPosition), player.camera.getForwardVector()) > 0.1 || nodeDrawDistance < 1.5;

	drawn = inDrawDistance; // &notCulled; // [TODO] enable back in

	if(!drawn) unsubdivide(renderer);

	if(subdivided)
	{
		c00->updateWithoutSubdivision(renderer, player);
		c01->updateWithoutSubdivision(renderer, player);
		c10->updateWithoutSubdivision(renderer, player);
		c11->updateWithoutSubdivision(renderer, player);
	}
	else
	{
		if(equivalentQuadtreeNode->subdivided && !subdivided && drawn)
		{
			if(needsSubDiv)
			{
				if(needsSubDiv->nodeDrawDistance > nodeDrawDistance) needsSubDiv = this;
			}
			else
			{
				needsSubDiv = this;
			}
		}
	}
}

void QuadtreeNodeRenderData::renderTerrainLevel(const UShort level, Renderer& renderer)
{
	if(subdivided)
	{
		c00->renderTerrainLevel(level, renderer);
		c01->renderTerrainLevel(level, renderer);
		c10->renderTerrainLevel(level, renderer);
		c11->renderTerrainLevel(level, renderer);
	}
	else
	{
		if(equivalentQuadtreeNode->id.level == level) renderTerrain(renderer);
	}
}
