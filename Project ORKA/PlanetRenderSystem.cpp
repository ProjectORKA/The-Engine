#include "PlanetRenderSystem.hpp"

#include "Framebuffer.hpp"
#include "PlanetSystem.hpp"
#include "PlanetSystemPlayer.hpp"
#include "Renderer.hpp"
#include "TerrainRendering.hpp"

void PlanetRenderSystem::create(const PlanetSystem& planetSystem)
{
	createNode(nodes[0][0], *planetSystem.quadtreeSystem.root);
	nodeCounts[0] = 1;
}

void PlanetRenderSystem::destroy(Renderer& renderer)
{
	destroyNode(nodes[0][0], renderer);
}

void PlanetRenderSystem::update(Renderer& renderer, const PlanetSystemPlayer& player)
{
	if (needsSubDiv)
	{
		subdivideNode(*needsSubDiv);
		needsSubDiv = nullptr;
	}
	for (auto& level : nodes) for (auto& renderData : level) updateNode(renderData, renderer, player);
}

//void PlanetRenderSystem::renderAllLevels(PlanetSystem& planetSystem, Renderer& renderer, const PlanetSystemPlayer& player, const Framebuffer& framebuffer)
//{
//	renderer.uniforms().setMMatrix(Matrix(1));
//	player.camera.renderOnlyRot(renderer);
//	renderer.setDepthTest(true);
//
//	for (UShort level = 0; level < MAX_CHUNK_LEVEL; level++)
//	{
//		framebuffer.clearDepth();
//		if (vertexColors)
//		{
//			renderer.shaderSystem.use("mooncrashVertexColor");
//		}
//		else
//		{
//			renderer.shaderSystem.use("terrain");
//		}
//		renderer.textureSystem.use(renderer, "terrainColor");
//		// [TODO] probably needs checks for subdivision
//		for (auto& node : nodes[level]) renderNode(node, renderer);
//	}
//}

void PlanetRenderSystem::renderLevel(Renderer& renderer, const PlanetSystemPlayer& player, const UShort level, const Framebuffer& framebuffer)
{
	// renderer.uniforms().customInt1() = Int(renderer.planetRenderSystem.worldDistortion);
	renderer.uniforms().setMMatrix(Matrix(1));
	player.camera.renderOnlyRot(renderer);
	renderer.setDepthTest(true);

	framebuffer.clearDepth(renderer);
	if (vertexColors) renderer.shaderSystem.use(renderer, "mooncrashVertexColor");
	else renderer.shaderSystem.use(renderer, "terrain");
	renderer.textureSystem.use(renderer, "terrainColor");
	// [TODO] probably needs checks for subdivision
	for (auto& node : nodes[level]) renderNode(node, renderer);
}

void PlanetRenderSystem::count() const
{
	Int nodeCount = 0;
	for (const auto& levelCount : nodeCounts)
	{
		nodeCount += levelCount;
	}
	logDebug("QuadTreeNodeRenderData count: " + toString(nodeCount));
}

Index PlanetRenderSystem::addNode(QuadtreeNode& node)
{
	const UShort level = node.id.level;
	for (Int i = 0; i < MAX_RENDER_CHUNKS; i++)
	{
		if (!nodes[level][i].isValid)
		{
			createNode(nodes[level][i], node);
			return i;
		}
	}
	logError("MAX_RENDER_CHUNKS too small!");
	return -1;
}

void PlanetRenderSystem::subdivideNode(QuadtreeNodeRenderData& renderData)
{
	if (!renderData.subdivided)
	{
		renderData.c00        = addNode(*renderData.equivalentQuadtreeNode->c00);
		renderData.c01        = addNode(*renderData.equivalentQuadtreeNode->c01);
		renderData.c10        = addNode(*renderData.equivalentQuadtreeNode->c10);
		renderData.c11        = addNode(*renderData.equivalentQuadtreeNode->c11);
		renderData.subdivided = true;
	}
}

void PlanetRenderSystem::createNode(QuadtreeNodeRenderData& renderData, QuadtreeNode& quadtreeNode)
{
	const UShort level = quadtreeNode.id.level;
	nodeCounts[level]++;
	renderData.equivalentQuadtreeNode = &quadtreeNode;
	quadtreeNode.incrementUsers();
	renderData.isValid = true;
}

void PlanetRenderSystem::updateNode(QuadtreeNodeRenderData& renderData, Renderer& renderer, const PlanetSystemPlayer& player)
{
	if (!renderData.isValid) return;
	// create 3d chunk id
	OctreeId tmp;

	const QuadtreeNode& eq = *renderData.equivalentQuadtreeNode;

	tmp.level      = eq.id.level;
	tmp.position.x = eq.id.position.x;
	tmp.position.y = eq.id.position.y;

	if (eq.data.terrain) tmp.position.z = static_cast<ULL>(eq.data.terrain->getLowerLimit());
	else tmp.position.z                 = 0;

	// get camera relative position
	renderData.cameraPosition = relativeCameraPosition(tmp, player.chunkPosition, player.camera.getPosition());

	renderData.nodeDrawDistance = length(renderData.cameraPosition - Vec3(0.5, 0.5, 0));
	const Bool inDrawDistance   = renderData.nodeDrawDistance < drawDistance;
	//Bool       notCulled        = dot(normalize(Vec3(0.5, 0.5, 0) - renderData.cameraPosition), player.camera.getForwardVector()) > 0.1 || renderData.nodeDrawDistance < 1.5;

	renderData.drawn = inDrawDistance; // &notCulled; // [TODO] enable back in

	if (!renderData.drawn) unsubdivideNode(renderData, renderer);

	if (renderData.subdivided) return;

	if (renderData.equivalentQuadtreeNode->subdivided && !renderData.subdivided && renderData.drawn)
	{
		if (needsSubDiv)
		{
			if (needsSubDiv->nodeDrawDistance > renderData.nodeDrawDistance) needsSubDiv = &renderData;
		}
		else needsSubDiv = &renderData;
	}
}

void PlanetRenderSystem::unsubdivideNode(QuadtreeNodeRenderData& renderData, Renderer& renderer)
{
	if (!renderData.subdivided) return;

	const UShort level = renderData.equivalentQuadtreeNode->id.level + 1;
	destroyNode(nodes[level][renderData.c00], renderer);
	destroyNode(nodes[level][renderData.c01], renderer);
	destroyNode(nodes[level][renderData.c10], renderer);
	destroyNode(nodes[level][renderData.c11], renderer);
	renderData.c00        = -1;
	renderData.c01        = -1;
	renderData.c10        = -1;
	renderData.c11        = -1;
	renderData.subdivided = false;
}

void PlanetRenderSystem::renderNode(QuadtreeNodeRenderData& renderData, Renderer& renderer) const
{
	if (!renderData.isValid) return;
	if (renderData.subdivided) return;

	const QuadtreeNode& node  = *renderData.equivalentQuadtreeNode;
	const UShort        level = node.id.level;

	if (!renderData.terrainMesh.isLoaded() && renderData.equivalentQuadtreeNode->data.terrain) renderData.terrainMesh.upload(renderer, createTerrainMesh(renderData.equivalentQuadtreeNode->data.terrain->getHeightmap()));

	const Double actualHeight = node.data.terrain->getUpperLimit() - node.data.terrain->getLowerLimit();
	Double       heightScale;
	if (actualHeight == 0.0f) heightScale = 1.0f;
	else
	{
		const Double currentScale = pow(2, 64 - level);
		heightScale               = actualHeight / currentScale;
	}
	const Matrix m = matrixFromSize(Vec3(1, 1, heightScale));
	renderer.uniforms().setMMatrix(m);
	renderer.uniforms().setWorldOffset(Vec4(node.id.position.x, node.id.position.y, node.data.terrain->getLowerLimit() / static_cast<LDouble>(ULLONG_MAX), level));

	if (level < 4)
	{
		// for the higher level chunks we duplicated them 
		for (Int x = -1; x < 2; x++)
		{
			for (Int y = -1; y < 2; y++)
			{
				renderer.uniforms().setCameraPos(Vec4(renderData.cameraPosition - Vec3(x * pow(2, level), y * pow(2, level), 0), 1));
				renderData.terrainMesh.render(renderer);
			}
		}
	}
	else
	{
		renderer.uniforms().setCameraPos(Vec4(renderData.cameraPosition, 1));
		renderData.terrainMesh.render(renderer);
	}
}

void PlanetRenderSystem::destroyNode(QuadtreeNodeRenderData& renderData, Renderer& renderer)
{
	if (!renderData.isValid) logError("Already destroyed!");

	const UShort level = renderData.equivalentQuadtreeNode->id.level;
	renderData.isValid = false;
	nodeCounts[level]--;
	unsubdivideNode(renderData, renderer);
	renderData.terrainMesh.unload(renderer);
	renderData.equivalentQuadtreeNode->decrementUsers();
}
