#include "Keva.hpp"

void KevaSimulation::destroy()
{

}

void KevaSimulation::update(Float delta)
{
}

void KevaSimulation::create()
{
	items.emplace_back("Nothing", 1.0, 1.0); // 0
	items.emplace_back("Yellow Shirt", 1.0, 1.0); // 1


	items.emplace_back("Armor", 1.0, 1.0);
	items.emplace_back("Ring", 1.0, 1.0); // 0
	items.emplace_back("Sword", 1.0, 1.0);
	items.emplace_back("Belt", 1.0, 1.0);
	items.emplace_back("Ring", 1.0, 1.0);

	player.inventory.push_back();
}

void KevaRenderer::connect(GameSimulation& simulation)
{

}
