#include "Keva.hpp"

#include "Random.hpp"

void KevaSimulation::destroy() {}

void KevaSimulation::printOptions() const
{
	std::cout << "Options:\n";
	if (inBattle)
	{
		std::cout << "[0] exit\n";
		std::cout << "[1] fight\n";
	}
	else
	{
		std::cout << "[0] exit\n";
		std::cout << "[1] inventory\n";
		std::cout << "[2] enter battle\n";
	}
	std::cout << "\n";
}

void KevaSimulation::update(Float delta)
{
	printOptions();

	String input;
	std::cin >> input;

	if (input == "0")
	{
		clearConsole();
		println("Game shutting down...");
		sleep(1000);
		keeprunning = false;
	}

	if (inBattle)
	{
		if (input == "1")
		{
			clearConsole();
			println("Player is fighting...");
			sleep(3000);
			clearConsole();

			if (randomFloatSlow() > 0.5)
			{
				println("Player has won!");
			}
			else
			{
				println("Player has lost!");
			}
			inBattle = false;
		}
	}
	else
	{
		if (input == "1")
		{
			clearConsole();
			println("Player has:");
			for (Index i : player.inventory)
			{
				std::cout << items[i].name << "\n";
			}
			println("");
		}

		if (input == "2")
		{
			clearConsole();
			println("Player started battle phase.");
			inBattle = true;
		}
	}
}

void KevaSimulation::create()
{
	items.emplace_back("Nothing", 0);   //0
	items.emplace_back("Yellow Shirt"); //1
	items.emplace_back("Armor");        //2
	items.emplace_back("Ring");         //3
	items.emplace_back("Sword");        //4
	items.emplace_back("Belt");         //5
	items.emplace_back("Ring");         //6

	player.inventory.push_back(1);
	player.inventory.push_back(4);
}
