#pragma once

//needs to be included before "WindowHandler.hpp"
//all graphics-api code is in here

#define GLEW_STATIC
#include "GL/glew.h"
#include "GameWorld.hpp"

class RenderingSystem {
public:
	GameWorld & gameWorld;
	RenderingSystem(GameWorld & gameWorld) : gameWorld(gameWorld) {

	}
	void render() {
		renderSky(gameWorld.sky);
		renderTriangle(gameWorld.triangle);
	}

	void renderSky(Sky & sky) {
		glClearColor(sky.skyColor.r, sky.skyColor.g, sky.skyColor.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void renderTriangle(Triangle & triangle) {
		glBegin(GL_TRIANGLES);
		for (int i = 0; i < triangle.vertecies.size(); i++) {
			glVertex2f(triangle.vertecies[i].x, triangle.vertecies[i].y);
		}
		glEnd();

	};
};