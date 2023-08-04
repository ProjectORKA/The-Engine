#include "MindMap.hpp"
#include "Random.hpp"
#include "Window.hpp"

void MindMap::update()
{
	static int t = 0;
	if(nodeCount < 1) addNode();
	t++;

	// keep distance to nodes
	for(const auto& c : connections)
	{
		Vec2 delta = positions[c.b] - positions[c.a];

		addForce(c.a, +delta / 1.0f);
		addForce(c.b, -delta / 1.0f);
	}

	// calculate collisions for every node
	for(UInt a = 0; a < nodeCount; a++)
	{
		// get every other node
		for(UInt b = a + 1; b < nodeCount; b++)
		{
			Vec2 delta = positions[b] - positions[a];
			if(delta != Vec2(0))
			{
				const Float impact = 1.0f / length(delta);

				if(impact > 0.0f)
				{
					// nodes colliding
					Vec2 force = normalize(delta) * impact;
					// add force to b
					addForce(b, force);
					addForce(a, -force);
				}
			}
		}
	}

	for(UInt i = 0; i < nodeCount; i++)
	{
		if(numForces[i])
		{
			positions[i] += forces[i] / Vec2(static_cast<Float>(numForces[i]));
			forces[i]    = Vec2(0);
			numForces[i] = 0;
		}
	}

	positions[0] = Vec2(0, 0);
}

void MindMap::addNode()
{
	forces.emplace_back();
	numForces.emplace_back();
	connections.emplace_back(random(nodeCount), nodeCount);
	if(nodeCount) positions.push_back(randomVec2(-1, 1) + positions[connections.back().a]);
	else positions.push_back(Vec2(0));
	nodeCount++;
}

void MindMap::addForce(const Index a, const Vec2 force)
{
	forces[a] += force;
	numForces[a]++;
}

MindMapConnection::MindMapConnection(const Index a, const Index b)
{
	this->a = a;
	this->b = b;
}

void MindMap::render(ResourceManager& resourceManager, Renderer& renderer) const
{
	renderer.setDepthTest(false);
	renderer.useShader(resourceManager, "color");

	// render connections
	renderer.fill(Color(0.5, 0.5, 0.5, 1));
	for(const auto connection : connections) renderer.line(positions[connection.a], positions[connection.b], 0.05f);

	// render nodes
	renderer.fill(Color(1));
	for(auto position : positions)
	{
		renderer.uniforms().setMMatrix(matrixFromLocation(Vec3(position, 0.0f)));
		renderer.renderMesh(resourceManager, "centeredPlane");
	}

	renderer.setDepthTest(true);
}

void MindMap::renderInteractive(ResourceManager& resourceManager, Window& window) const
{
	// Renderer& renderer = window.renderer;

	// renderer.setDepthTest(false);
	// renderer.useShader(resourceManager, "idShader");

	// render connections
	// renderer.fill(Color(0.5, 0.5, 0.5, 1));
	// for(UInt i = 0; i < connections.size(); i++)
	// {
	//	renderer.uniforms().setObjectId(i);
	//	renderer.line(positions[connections[i].a], positions[connections[i].b], 0.05f);
	// }

	// render nodes
	// renderer.fill(Color(1));
	// for(UInt i = 0; i < positions.size(); i++)
	// {
	//	renderer.uniforms().setObjectId(static_cast<UInt>(connections.size()) - static_cast<UInt>(1) + i);
	//	renderer.uniforms().setMMatrix(matrixFromLocation(Vec3(positions[i], 0.0f)));
	//	renderer.renderMesh(resourceManager, "1x1planeCentered");
	// }

	//renderer.idFramebuffer.updateIdsUnderCursor(window);

	//renderer.setDepthTest(true);
}
