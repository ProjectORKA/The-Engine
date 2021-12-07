#pragma once

#include "Game.hpp"

#include "Pong.hpp"
#include "Intro.hpp"
#include "Sandbox.hpp"
#include "Mooncrash.hpp"
#include "AIPlayground.hpp"

//the gameSystem globally stores all game instances and processes them at a fixed time interval in a separate thread
//this allows multiple windows and user interface elements to render the same game instance and achieve high framerates without being held down by the games processing

struct GameSystem {
	Vector<Game *> games;		//stores all available games for global use
	Thread thread;				//processes all game instances in a separate thread
	Game* add(Game * game);		//adds a new game to the list of globally acessible games
	GameSystem();				//initializes gameSimulationthread automatically
	~GameSystem();				//destroys all game instances
};

void gameSimulationThread(Thread& thread, GameSystem& gameSystem); //loops through all game instances at a fixed Hz rate and updates them

extern GameSystem gameSystem; //grants global access