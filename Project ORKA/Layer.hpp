
#pragma once

#include "Renderer.hpp"
#include "Basics.hpp"

struct Layer {
	Time time;
	Renderer* rendererPtr = nullptr;

	void create();
	void render();
	void destroy();

	Layer(Renderer& renderer, void(*createFunction)(Layer& layer), void(*renderFunction)(Layer& layer), void(*destroyFunction)(Layer& layer));

private:
	void (*createFunctionPtr)(Layer& layer) = nullptr;
	void (*renderFunctionPtr)(Layer& layer) = nullptr;
	void (*destroyFunctionPtr)(Layer& layer) = nullptr;
};

void createLogoLayer(Layer & layer);
void renderLogoLayer(Layer & layer);

void createMainMenuLayer(Layer& layer);
void renderMainMenuLayer(Layer& layer);

void createGameInstanceLayer(Layer& layer);
void renderGameInstanceLayer(Layer& layer);