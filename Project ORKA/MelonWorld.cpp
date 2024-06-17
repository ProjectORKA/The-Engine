#include "MelonWorld.hpp"
#include "Renderer.hpp"
#include "MelonPlayer.hpp"
#include "Random.hpp"

void MelonWorld::update(const MelonPlayer& player)
{
	m.lock();
	if(nodes.empty()) nodes.push_back(player.position);

	constexpr Float generationSize         = 1000;
	constexpr Float pathPersistenceSize    = generationSize / 1.5f;
	Float           sceneryPersistenceSize = pathPersistenceSize * 0.9f;
	constexpr Float pathWidth              = 30;

	generatePaths(generationSize, player, pathWidth, pathPersistenceSize);

	generateScenery(sceneryPersistenceSize, player, pathWidth);

	m.unlock();
}

void MelonWorld::render(Renderer& renderer)
{
	m.lock();

	Vector<Vec4> arr;

	for(const auto& rock : rocks) for(UInt y = 0; y < ROCKGRIDSIZE; y++) arr.push_back(rock[y]);
	renderer.uniforms().setCustomColor(Vec4(0.1, 0.1, 0.1, 1));
	// renderer.matrixSystem.matrixArray(arr);
	// renderer.renderMeshInstanced("melonRock"); //[TODO] replace with instancing system

	m.unlock();
}

void MelonWorld::generateScenery(Float& sceneryPersistenceSize, const MelonPlayer& player, const Float& pathWidth)
{
	//while (scenery.size() < 1000) {
	//	Vec3 pos = Vec3(randomFloat(-sceneryPersistenceSize, +sceneryPersistenceSize), randomFloat(-sceneryPersistenceSize, +sceneryPersistenceSize), 0);
	//	pos += player.position;
	//	Vec3 closest = getClosestPoint(pos, nodes);
	//	Float size = distance(pos, closest) - pathWidth;
	//	if (size > 1 && size < 50) scenery.push_back(Vec4(pos, size));
	// }
	// List<Vec4>::iterator it = scenery.begin();
	// while (it != scenery.end())
	// {
	//	if (distance(*it, player.position) > sceneryPersistenceSize) {
	//		scenery.erase(it);
	//		it = scenery.begin();
	//	}
	//	it++;
	// }
	for(Int x = 0; x < ROCKGRIDSIZE; x++)
	{
		for(Int y = 0; y < ROCKGRIDSIZE; y++)
		{
			constexpr Float gridSize = 8.0f;
			auto            pos      = Vec3(gridSize * (x - ROCKGRIDSIZE / 2) + snap(player.position.x, gridSize), gridSize * (y - ROCKGRIDSIZE / 2) + snap(player.position.y, gridSize), 0);
			Vec3            closest  = getClosestPoint(pos, nodes);
			Float           size     = distance(pos, closest) - pathWidth;
			// size = max(size,0);
			constexpr Float border = 10.0f;
			size                   = border - abs(size - border);
			size                   = clamp(size, 0.0f, 10.0f);
			rocks[x][y]            = Vec4(pos, size);
		}
	}
}

void MelonWorld::generatePaths(const Float& generationSize, const MelonPlayer& player, const Float& pathWidth, const Float& pathPersistenceSize)
{
	while(nodes.size() < 1000)
	{
		Vec3 newNode    = Vec3(normalize(randomVec2Fast(-1.0f, 1.0f)) * Vec2(generationSize) * randomFloatFast(0.75f, 1.0f), 0) + player.position;
		Vec3 closestPos = getClosestPoint(newNode, nodes);

		newNode = closestPos + normalize(newNode - closestPos) * Vec3(pathWidth / 2);

		nodes.push_back(newNode);
	}

	auto i = nodes.begin();
	while(i != nodes.end())
	{
		if(distance(*i, player.position) > pathPersistenceSize)
		{
			nodes.erase(i);
			i = nodes.begin();
		}
		++i;
	}
}
