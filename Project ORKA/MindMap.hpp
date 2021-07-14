#pragma once

#include "Basics.hpp"

Float desiredDistance = 1.0;

struct Connection;

struct Node {
	Index id = -1;
	Vec2 location;
	Vector<Connection*> connections;
};

struct Connection {
	Index id = -1;
	Node* nodeA = nullptr;
	Node* nodeB = nullptr;

	Connection(Node& a, Node& b);
	~Connection();
	void update();
};

struct MindMap {
	Vector<Node> nodes; //[TODO] turn into quadtree
	Vector<Connection> connections;

	void addNode();
	void create();
};