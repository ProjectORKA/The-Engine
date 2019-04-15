#pragma once

//needs to be included before "WindowHandler.hpp"
//all graphics-api code is in here

#define GLEW_STATIC
#include "GL/glew.h"
#include "GameWorld.hpp"

class Mesh {
public:
	std::vector<glm::vec2> vertecies;
	Mesh() {
		vertecies.push_back(glm::vec2(-1.0f, -1.0f));
		vertecies.push_back(glm::vec2(-1.0f, +1.0f));
		vertecies.push_back(glm::vec2(+1.0f, +0.0f));
	}
	void render() {
		glBegin(GL_TRIANGLES);
		for (int i = 0; i < vertecies.size(); i++) {
			glVertex2f(vertecies[i].x, vertecies[i].y);
		}
		glEnd();
	};
};

class RenderingSystem {
public:
	GameWorld & gameWorld;
	RenderingSystem(GameWorld & gameWorld) : gameWorld(gameWorld) {

	}
	void render(int width, int height) {
		glViewport(0, 0, width, height);
		renderSky(gameWorld.sky);
		renderEntities(gameWorld.entities);
	}

	void renderSky(Sky & sky) {
		glClearColor(sky.skyColor.r, sky.skyColor.g, sky.skyColor.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void renderEntities(std::vector<Entity *> & entities) {
		for (int i = 0; i < entities.size(); i++) {
			renderEntity(*entities[i]);
		}
	}

	void renderEntity(Entity & entity){
		for (int i = 0; i < entity.components.size(); i++) {
			renderComponents(entity.components[i]);
		}
	}

	void renderComponents(Component * component){
		if (component->componentType == "mesh") {
			std::string path = static_cast<MeshComponent *>(component)->path;
			renderMesh(path);
		}
	}

	void renderMesh(std::string path) {
		Mesh t;
		t.render();
	}
};