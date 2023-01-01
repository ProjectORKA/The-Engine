#pragma once

#include "Game.hpp"
#include "NeuralNetwork.hpp"
#include "Random.hpp"

struct Window;

struct AIPlayground : public GameRenderer {
	
	NeuralNet network;

	AIPlayground();
	void render(Engine& engine, Window& window, TiledRectangle area) override;
};