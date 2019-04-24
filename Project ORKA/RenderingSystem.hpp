#pragma once
#ifndef RENDERINGSYSTEM_HPP
#define RENDERINGSYSTEM_HPP

//needs to be included before "WindowHandler.hpp"
//all graphics-api code is in here
#include "GameServer.hpp"
#include "Mesh.hpp"

void __stdcall DebugOutputCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

class RenderingSystem {
public:
	GameServer & gameServer;
	MeshHandler meshHandler;
	RenderingSystem(GameServer & gameWorld);

	void render();
	void renderWorld(WorldSystem & worldSystem);
	void renderChunks(Chunk & chunk);
	void renderSky(Sky & sky);
	void renderEntities(std::vector<Entity *> & entities);
	void renderEntity(Entity & entity);
	void adaptViewport(unsigned int width, unsigned int height);
	void setDebugMessageCallback();
};
#endif // !RENDERINGSYSTEM_HPP