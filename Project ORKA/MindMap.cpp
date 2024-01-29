#include "MindMap.hpp"
#include "Random.hpp"
#include "Window.hpp"

void MindMap::update()
{
	LockGuard  lock(mutex);
	static int t = 0;
	if(nodeCount < 1) addNode();
	t++;

	// keep distance to nodes
	for(const auto& c : connections)
	{
		const Vec2 delta = positions[c.b] - positions[c.a];
		forces[c.a] += delta / 1.0f;
		numForces[c.a]++;
		forces[c.b] -= delta / 1.0f;
		numForces[c.b]++;
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
					const Vec2 force = normalize(delta) * impact;
					forces[b] += force;
					numForces[b]++;
					forces[a] -= force;
					numForces[a]++;
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
	LockGuard lock(mutex);
	forces.emplace_back();
	numForces.emplace_back();
	Index a = randomIntFast(nodeCount);
	connections.emplace_back(a, nodeCount);
	if(nodeCount) positions.push_back(randomVec2Fast(-1.0f, 1.0f) + positions[connections.back().a]);
	else positions.emplace_back(0);
	nodeCount++;
}

MindMapConnection::MindMapConnection(const Index a, const Index b)
{
	this->a = a;
	this->b = b;
}

void MindMap::render(Renderer& renderer) const
{
	renderer.setDepthTest(false);
	renderer.useShader("color");

	// render connections
	renderer.fill(Color(0.5, 0.5, 0.5, 1));

	Vector<Vec2> lines;

	for(const auto connection : connections)
	{
		lines.push_back(positions[connection.a]);
		lines.push_back(positions[connection.b]);
	}
	renderer.lines(lines);

	// render nodes
	renderer.fill(Color(1));
	Vector<Vec3> posArray;
	for(auto position : positions) posArray.emplace_back(position, 0.0f);

	renderer.renderMeshInstanced("centeredPlane", posArray);
	renderer.setDepthTest(true);
}

void MindMap::renderInteractive(Window& window) const
{
	// Renderer& renderer = window.renderer;

	// renderer.setDepthTest(false);
	// renderer.useShader("idShader");

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
	//	renderer.uniforms().setMMatrix(matrixFromPosition(Vec3(positions[i], 0.0f)));
	//	renderer.renderMesh("1x1planeCentered");
	// }

	//renderer.idFramebuffer.updateIdsUnderCursor(window);

	//renderer.setDepthTest(true);
}
