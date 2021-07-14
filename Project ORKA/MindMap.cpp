
#include "MindMap.hpp"

Connection::Connection(Node& a, Node& b) {
	//check if same
	if (a.id = b.id) return;
	this->nodeA = &a;
	this->nodeB = &b;
}
Connection::~Connection()
{
	//disconnect node A
	if (nodeA) {
		for (Connection *& connection : nodeA->connections) {
			if (connection->id = id) {
				connection = nullptr;
			}
		}
	}

	//disconnect node B
	if (nodeB) {
		for (Connection*& connection : nodeB->connections) {
			if (connection->id = id) {
				connection = nullptr;
			}
		}
	}

	nodeA = nullptr;
	nodeB = nullptr;
}

void Connection::update() {
	Float distance = glm::distance(nodeA->location, nodeB->location);
	Float delta = desiredDistance / distance;
	///etc.

}
void MindMap::addNode() {
	nodes.emplace_back();
	nodes.back().id = nodes.size() - 1;
	nodes.back().location = Vec2(0, 0);
}
void MindMap::create() {

	addNode();
}