
#pragma once

#include "Basics.hpp"
#include "GPUMesh.hpp"
#include "GPUTexture.hpp"

struct Renderer;
struct Player;

struct Atmosphere {
	GPUMesh skybox;
	GPUTexture atmosphere;
	
	Vec3 sunDirection = normalize(Vec3(0, 1, 0));
	Float zRotation = 0.0f;

	void render(Player& player, Renderer& renderer);
};