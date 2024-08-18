#pragma once
#include "Input.hpp"
#include "Prototype.hpp"

#define MAX_COST 10000000000000000000

struct LabyrinthGeneratorPrototype final : Prototype
{
	struct Node
	{
		Vector<Node*> neighbors;
		Bool          processed = false;
		Vec2          position  = Vec2(0);
		Int           cost      = MAX_COST;
	};

	const Int        size              = 75;
	const Float      circleRadius      = 0.5f;
	const Bool       renderPath        = true;
	const Bool       renderConnections = true;
	const Bool       renderNodes       = false;
	const Bool       renderNumbers     = false;
	const InputEvent reset             = InputEvent(InputType::KeyBoard, R, true);

	Vector<Node*> path;
	Mutex         mutex;
	Vector<Node>  nodes;
	Vector<Index> cutIndices;

	void findPath();
	void createPath();
	void calculateCost();
	void action() override;
	void create() override;
	void destroy() override;
	void tryRemoveConnection(Index i);
	void render(Renderer& r, const Player& player) override;
	void inputEvent(Window& window, InputEvent input) override;
};
