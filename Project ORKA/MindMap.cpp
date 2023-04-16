
#include "MindMap.hpp"
#include "Window.hpp"

void MindMap::update()
{
	static int t = 0;
	if (nodeCount < 1)
	{
		addNode();
	}
	t++;

	//keep distance to nodes
	for (UInt i = 0; i < connections.size(); i++)
	{
		MindmapConnection& c = connections[i];

		Vec2 delta = positions[c.b] - positions[c.a];
		//Float impact = length(delta/2.0f) - 2.0f;

		//addForce(c.a, normalize(delta) * impact);
		//addForce(c.b, -normalize(delta) * impact);

		addForce(c.a, +delta / 1.0f);
		addForce(c.b, -delta / 1.0f);

	}

	//calculate collisions
	//for every node
	for (UInt a = 0; a < nodeCount; a++)
	{
		//get every other node
		for (UInt b = a + 1; b < nodeCount; b++)
		{
			Vec2 delta = positions[b] - positions[a];
			if (delta != Vec2(0)) {
				Float intersectionDistance = 2 - length(delta);

				Float impact = 1 / length(delta);

				if (impact > 0.0) {
					//nodes colliding
					Vec2 force = normalize(delta) * impact;
					//add force to b
					addForce(b, force);
					addForce(a, -force);
				}
			}
		}
	}

	for (UInt i = 0; i < nodeCount; i++)
	{
		if (numforces[i])
		{
			positions[i] += forces[i] / numforces[i];
			forces[i] = Vec2(0);
			numforces[i] = 0;
		}
	}

	positions[0] = Vec2(-100, 0);

}
void MindMap::addNode()
{
	forces.emplaceBack();
	numforces.emplaceBack();
	for (Int i = 0; i < random(1) + 1; i++) connections.emplaceBack(random(nodeCount), nodeCount);
	if (nodeCount) positions.pushBack(randomVec2(-1, 1) + positions[connections.last().a]);
	else positions.pushBack(Vec2(0));
	nodeCount++;
}
void MindMap::render(ResourceManager& resourceManager, Renderer& renderer)
{

	renderer.clearColor();
	renderer.clearDepth();

	renderer.setDepthTest(false);
	renderer.useShader(resourceManager, "color");

	//render connections
	renderer.fill(Color(0.5, 0.5, 0.5, 1));
	for (UInt i = 0; i < connections.size(); i++) {
		renderer.line(positions[connections[i].a], positions[connections[i].b], 0.05);
	}

	//render nodes
	renderer.fill(Color(1));
	for (UInt a = 0; a < positions.size(); a++) {
		renderer.uniforms().mMatrix(matrixFromLocation(Vec3(positions[a], 0.0f)));
		renderer.renderMesh(resourceManager, "centeredPlane");
	}

	renderer.setDepthTest(true);
}
void MindMap::addForce(Index a, Vec2 force)
{
	forces[a] += force;
	numforces[a]++;
}
void MindMap::renderInteractive(ResourceManager& resourceManager, Window & window)
{
	Renderer& renderer = window.renderer;

	renderer.setDepthTest(false);
	renderer.useShader(resourceManager, "idShader");

	//render connections
	renderer.fill(Color(0.5, 0.5, 0.5, 1));
	for (UInt i = 0; i < connections.size(); i++)
	{
		renderer.uniforms().objectID(i);
		renderer.line(positions[connections[i].a], positions[connections[i].b], 0.05);
	}

	//render nodes
	renderer.fill(Color(1));
	for (UInt i = 0; i < positions.size(); i++)
	{
		renderer.uniforms().objectID(connections.size() - 1 + i);
		renderer.uniforms().mMatrix(matrixFromLocation(Vec3(positions[i], 0.0f)));
		renderer.renderMesh(resourceManager, "1x1planeCentered");
	}

	renderer.idFramebuffer.updateIdsUnderCursor(window);

	renderer.setDepthTest(true);
}