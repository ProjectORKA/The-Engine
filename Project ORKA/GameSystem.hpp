#pragma once

#include "Game.hpp"
#include "Threading.hpp"



//the gameSystem globally stores all game instances and processes them at a fixed time interval in a separate thread
//this allows multiple windows and user interface elements to render the same game instance and achieve high framerates without being held down by the games processing

struct GameSystem {
	Vector<Game *> games;		//stores all available games for global use
	Thread thread;				//processes all game instances in a separate thread
	
	void run();					//creates the gamesimulation thread which continually updates all games
	~GameSystem();				//destroys all game instances
	Game* add(Game * game);		//adds a new game to the list of globally acessible games
};

void gameSimulationThread(Thread& thread, GameSystem& gameSystem); //loops through all game instances at a fixed Hz rate and updates them

extern GameSystem gameSystem; //grants global access