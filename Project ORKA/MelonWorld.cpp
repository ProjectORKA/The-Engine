
#include "MelonWorld.hpp"
#include "Renderer.hpp"
#include "MelonPlayer.hpp"
#include "Random.hpp"

void MelonWorld::render(Engine& engine, Renderer& renderer) {
	m.lock();

	Vector<Vec4> arr;

	for (UInt x = 0; x < ROCKGRIDSIZE; x++) {
		for (UInt y = 0; y < ROCKGRIDSIZE; y++) {
			arr.push_back(rocks[x][y]);
		}
	}
	renderer.uniforms().customColor(Vec4(0.1, 0.1, 0.1, 1));
	renderer.matrixSystem.matrixArray(arr);
	renderer.renderMeshInstanced(engine, "stone");

	m.unlock();
};
void MelonWorld::update(MelonPlayer& player) {
	m.lock();
	if (nodes.size() == 0) nodes.push_back(player.location);

	Float generationSize = 1000;
	Float pathPersistencySize = generationSize / 1.5;
	Float sceneryPersistencySize = pathPersistencySize * 0.9;
	Float pathWidth = 30;
	Int sceneryCount = 800;

	generatePaths(generationSize, player, pathWidth, pathPersistencySize);

	generateScenery(sceneryPersistencySize, player, pathWidth);

	m.unlock();
}
void MelonWorld::generateScenery(Float& sceneryPersistencySize, MelonPlayer& player, Float& pathWidth)
{
	//while (scenery.size() < 1000) {
	//	Vec3 pos = Vec3(randomFloat(-sceneryPersistencySize, +sceneryPersistencySize), randomFloat(-sceneryPersistencySize, +sceneryPersistencySize), 0);
	//	pos += player.location;

	//	Vec3 closest = getClosestPoint(pos, nodes);

	//	Float size = distance(pos, closest) - pathWidth;

	//	if (size > 1 && size < 50) scenery.push_back(Vec4(pos, size));
	//}

	//List<Vec4>::iterator it = scenery.begin();
	//while (it != scenery.end())
	//{
	//	if (distance(*it, player.location) > sceneryPersistencySize) {
	//		scenery.erase(it);
	//		it = scenery.begin();
	//	}
	//	it++;
	//}
	Float gridSize = 8.0f;
	for (Int x = 0; x < ROCKGRIDSIZE; x++) {
		for (Int y = 0; y < ROCKGRIDSIZE; y++) {
			Vec3 pos = Vec3(gridSize * (x - (ROCKGRIDSIZE / 2)) + snap(player.location.x,gridSize), gridSize * (y - (ROCKGRIDSIZE / 2)) + snap(player.location.y, gridSize),0);
			Vec3 closest = getClosestPoint(pos, nodes);
			Float size = distance(pos, closest) - pathWidth;
			//size = max(size,0);
			Float border = 10;
			size = border - abs(size - border);
			size = clamp(size, 0, 10);
			rocks[x][y] = Vec4(pos,size);
		}
	}
}
void MelonWorld::generatePaths(Float& generationSize, MelonPlayer& player, Float& pathWidth, Float& pathPersistencySize)
{
	while (nodes.size() < 1000) {
		Vec3 newNode = Vec3(normalize(randomVec2(-1, 1)) * Vec2(generationSize) * randomFloat(0.75, 1), 0) + player.location;

		Index closestID;
		Vec3 closestPos = getClosestPoint(newNode, nodes);

		newNode = closestPos + normalize(newNode - closestPos) * Vec3(pathWidth/2);

		nodes.push_back(newNode);
	}

	List<Vec3>::iterator i = nodes.begin();
	while (i != nodes.end())
	{
		if (distance(*i, player.location) > pathPersistencySize) {
			nodes.erase(i);
			i = nodes.begin();
		}
		i++;
	}
}