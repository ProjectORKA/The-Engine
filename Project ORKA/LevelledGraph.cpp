
#include "LevelledGraph.hpp"
#include "Renderer.hpp"

void LevelledGraph::addNode() {
	positions.push_back(randomVec2(-1,1));
	forces.emplace_back();
	numforces.emplace_back();
	for(Int i = 0; i < random(2)+1; i++) connections.emplace_back(random(nodeCount), nodeCount);
	nodeCount++;
}

void LevelledGraph::addForce(Index a, Vec2 force) {
	forces[a] += force;
	numforces[a]++;
}

void LevelledGraph::update() {
	static int t = 0;
	if (nodeCount < 100 && t % 50 == 0) {
		addNode();
	}
	t++;

	//keep distance to nodes
	for (UInt i = 0; i < connections.size(); i++) {
		Connection& c = connections[i];
		
		Vec2 delta = positions[c.b] - positions[c.a];
		//Float impact = length(delta/2.0f) - 2.0f;

		//addForce(c.a, normalize(delta) * impact);
		//addForce(c.b, -normalize(delta) * impact);
		
		addForce(c.a,  delta / 2.0f);
		addForce(c.b, -delta / 2.0f);

	}

	//calculate collisions
	//for every node
	for (UInt a = 0; a < nodeCount; a++) {
		//get every other node
		for (UInt b = a + 1; b < nodeCount; b++) {
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

	for (UInt i = 0; i < nodeCount; i++) {
		if (numforces[i]) {
			positions[i] += forces[i] / numforces[i];
			forces[i] = Vec2(0);
			numforces[i] = 0;
		}
	}

	positions[0] = Vec2(-100, 0);

}

Vec3 LevelledGraph::getPos(Index x, Index y) {
	Float size = 2;
	return Vec3(x * size, positions[x][y] * size, 0);
}
void LevelledGraph::render(Engine & engine, Renderer& renderer) {
	renderer.setDepthTest(false);
	renderer.useShader(engine, "color");
	
	//render connections
	renderer.fill(Color(0.5, 0.5, 0.5, 1));
	for (UInt i = 0; i < connections.size(); i++) {
		renderer.line(positions[connections[i].a], positions[connections[i].b], 0.05);
	}

	//render nodes
	renderer.fill(Color(1));
	for (UInt a = 0; a < positions.size(); a++) {
		renderer.uniforms().mMatrix(matrixFromLocation(Vec3(positions[a],0.0f)));
		renderer.renderMesh(engine, "1x1planeCentered");
	}

	renderer.setDepthTest(true);
}
Vec3 LevelledGraph::getPos(Index x, Index y, Float z)
{
	Float size = 2;
	return Vec3(x * size, positions[x][y] * size, z);
}