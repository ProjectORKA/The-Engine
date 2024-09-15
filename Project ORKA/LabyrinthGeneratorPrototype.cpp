#include "LabyrinthGeneratorPrototype.hpp"

void LabyrinthGeneratorPrototype::action()
{
	static Index cutIndex = 0;
	if (cutIndices.empty())
	{
		findPath();
		cutIndices = shuffledIndices(path.size());
	}

	if (cutIndex >= cutIndices.size())
	{
		cutIndex = 0;
		findPath();
		cutIndices = shuffledIndices(path.size());
	}

	tryRemoveConnection(cutIndices[cutIndex]);

	cutIndex++;
}

void LabyrinthGeneratorPrototype::create()
{
	const int m = size - 1;

	// create grid
	nodes.resize(size * size);

	// connect neighbors
	for (int x = 0; x < size; x++)
	{
		for (int y = 0; y < size; y++)
		{
			Node& node    = nodes[xyToIndex(x, y, size)];
			node.position = Vec2(x, y);
			if (x < m) node.neighbors.push_back(&nodes[xyToIndex(x + 1, y, size)]);
			if (x > 0) node.neighbors.push_back(&nodes[xyToIndex(x - 1, y, size)]);
			if (y < m) node.neighbors.push_back(&nodes[xyToIndex(x, y + 1, size)]);
			if (y > 0) node.neighbors.push_back(&nodes[xyToIndex(x, y - 1, size)]);
		}
	}
}

void LabyrinthGeneratorPrototype::destroy()
{
	path.clear();
	nodes.clear();
	cutIndices.clear();
}

void LabyrinthGeneratorPrototype::findPath()
{
	calculateCost();
	createPath();
}

void LabyrinthGeneratorPrototype::createPath()
{
	// creates a list of nodes from the target position to the starting location
	// it takes random neighbor nodes with lower cost

	path.clear();

	Node* currentNodePtr = &nodes.back();

	// check if path can exist
	if (currentNodePtr->cost < MAX_COST)
	{
		path.push_back(&nodes.back());

		while (currentNodePtr->cost != 0)
		{
			// get a list of neighbor nodes with lower cost
			Vector<Node*> lowerCostNeighbors;
			for (Node* neighbor : currentNodePtr->neighbors)
			{
				if (neighbor->cost < currentNodePtr->cost)
				{
					lowerCostNeighbors.push_back(neighbor);
				}
			}

			// select a random neighbor node from the list
			if (!lowerCostNeighbors.empty())
			{
				Node* randomNeighbor = lowerCostNeighbors[rand() % lowerCostNeighbors.size()];
				path.push_back(randomNeighbor);
				currentNodePtr = randomNeighbor;
			}
			else
			{
				break;
			}
		}
	}
}

void LabyrinthGeneratorPrototype::calculateCost()
{
	for (int x = 0; x < size; x++)
	{
		for (int y = 0; y < size; y++)
		{
			nodes[xyToIndex(x, y, size)].processed = false;
			nodes[xyToIndex(x, y, size)].cost      = MAX_COST;
		}
	}

	Int cost = 0;

	nodes[0].cost      = cost;
	nodes[0].processed = true;

	Vector<Node*> neighbours = nodes[0].neighbors;

	while (!neighbours.empty())
	{
		cost++;

		Vector<Node*> newNeighbors;

		// process nodes
		for (Node* nn : neighbours)
		{
			nn->cost      = cost;
			nn->processed = true;
		}

		// gather new Nodes
		for (const Node* nn : neighbours)
		{
			for (Node* nnn : nn->neighbors)
			{
				if (!nnn->processed) newNeighbors.push_back(nnn);
			}
		}

		//remove duplicates
		std::ranges::sort(newNeighbors);
		const auto last = std::ranges::unique(newNeighbors).begin();
		newNeighbors.erase(last, newNeighbors.end());

		neighbours = newNeighbors;
	}
}

void LabyrinthGeneratorPrototype::tryRemoveConnection(const Index i)
{
	Node* a = nullptr;
	Node* b = nullptr;

	if (!path.empty())
	{
		const Index pathLength = path.size();

		const Index aid = i;
		Index       bid = aid + 1;
		if (aid == pathLength - 1) bid = aid - 1;

		a = path[aid];
		b = path[bid];
		mutex.lock();
		std::erase(a->neighbors, b);
		std::erase(b->neighbors, a);
		mutex.unlock();
	}

	findPath();

	if (path.empty() || path.size() != path[0]->cost + 1)
	{
		mutex.lock();
		a->neighbors.push_back(b);
		b->neighbors.push_back(a);
		mutex.unlock();
		findPath();
	}
}

void LabyrinthGeneratorPrototype::render(Renderer& r, const Player& player)
{
	mutex.lock();
	r.uniforms().setMMatrix(matrixFromPosition(Vec3(0, 0, 0.5)));
	r.textRenderSystem.alignText(Alignment::center, Alignment::center);
	r.textRenderSystem.setSize(0.5);

	Vector<Matrix> positions;
	Vec2Vector   lines;
	Vec2Vector   pathLines;

	for (auto& node : nodes)
	{
		positions.push_back(matrixFromPositionAndSize(node.position, circleRadius));
		for (const auto& neighbor : node.neighbors)
		{
			lines.push_back(node.position);
			lines.push_back(neighbor->position);
		}

		if (renderNumbers) r.textRenderSystem.render(r, toString(node.cost), node.position);
	}

	if (!path.empty())
	{
		for (int i = 0; i < path.size() - 1; i++)
		{
			pathLines.push_back(path[i]->position);
			pathLines.push_back(path[i + 1]->position);
		}
	}

	//red path highlighted
	r.useShader("color");
	if (renderPath)
	{
		r.uniforms().setMMatrix(matrixFromPosition(0.0f, 0.0f, 0.05f));
		r.fill(1, 0, 0);
		r.lines(pathLines);
	}

	if (renderConnections)
	{
		r.fill(0.1f);
		r.uniforms().setMMatrix(1);
		r.lines(lines);
	}

	if (renderNodes)
	{
		r.circles(positions);
	}
	mutex.unlock();
}

void LabyrinthGeneratorPrototype::inputEvent(Window& window, const InputEvent input)
{
	if (input == reset)
	{
		mutex.lock();
		destroy();
		create();
		mutex.unlock();
	}
}
