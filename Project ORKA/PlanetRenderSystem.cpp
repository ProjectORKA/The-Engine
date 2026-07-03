#include "PlanetRenderSystem.hpp"

#include "Framebuffer.hpp"
#include "PlanetSystem.hpp"
#include "PlanetSystemPlayer.hpp"
#include "Renderer.hpp"
#include "TerrainRendering.hpp"

namespace
{
// Calculate distance from camera to the chunk's bounding box using terrain limits.
Float distanceToChunkBounds(const Vec3& cameraPos, const QuadtreeNode& node)
{
	const Terrain* terrain = node.data.terrain;
	if (!terrain) return length(cameraPos - Vec3(0.5f, 0.5f, 0.0f));

	const Double heightRange = terrain->getUpperLimit() - terrain->getLowerLimit();
	const Double chunkScale  = pow(2.0, 64 - node.id.level);
	Float        heightScale = 0.0f;

	if (chunkScale > 0.0) heightScale = static_cast<Float>(heightRange / chunkScale);
	if (heightScale < 0.0f) heightScale = 0.0f;

	const Vec3 minBounds(0.0f, 0.0f, 0.0f);
	const Vec3 maxBounds(1.0f, 1.0f, heightScale);

	const Vec3 closestPoint = glm::clamp(cameraPos, minBounds, maxBounds);

	return length(cameraPos - closestPoint);
}
} // namespace

void PlanetRenderSystem::create(const PlanetSystem& planetSystem)
{
	for (auto& level : nodes) level.clear();
	for (auto& count : nodeCounts) count = 0;
	needsSubDiv = nullptr;

	nodes[0].emplace_back();
	createNode(nodes[0][0], *planetSystem.quadtreeSystem.root);
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
	auto& levelNodes = nodes[level];

	for (Index i = 0; i < levelNodes.size(); i++)
	{
		if (!levelNodes[i].isValid)
		{
			createNode(levelNodes[i], node);
			return i;
		}
	}

	levelNodes.emplace_back();
	createNode(levelNodes.back(), node);
	return static_cast<Index>(levelNodes.size() - 1);
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

	renderData.nodeDrawDistance = distanceToChunkBounds(renderData.cameraPosition, eq);
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

	const Name terrainShaderName = vertexColors ? Name("mooncrashVertexColor") : Name("terrain");

	const auto renderChunk = [&](const Vec3 cameraPosition)
	{
		renderer.uniforms().setCameraPos(Vec4(cameraPosition, 1));
		renderData.terrainMesh.render(renderer);

		if (chunkBoundingBoxes)
		{
			// Build a model matrix that keeps the box aligned to the chunk and offset relative to the camera.
			Matrix boundingBoxMatrix = m;
			boundingBoxMatrix[3]     = Vec4(-cameraPosition, 1);

			renderer.uniforms().setMMatrix(boundingBoxMatrix);
			renderer.shaderSystem.use(renderer, "debug");
			renderer.primitivesRenderer.wireframeCube(renderer);
			renderer.shaderSystem.use(renderer, terrainShaderName);
			renderer.uniforms().setMMatrix(m);
		}
	};

	if (level < 4)
	{
		// for the higher level chunks we duplicated them 
		for (Int x = -1; x < 2; x++)
		{
			for (Int y = -1; y < 2; y++)
			{
				const Vec3 cameraPosition = renderData.cameraPosition - Vec3(x * pow(2, level), y * pow(2, level), 0);
				renderChunk(cameraPosition);
			}
		}
	}
	else
	{
		renderChunk(renderData.cameraPosition);
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
