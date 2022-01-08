//
//#pragma once
//
//#include "Basics.hpp"
//#include "GPUMesh.hpp"
//#include "GPUTexture.hpp"
//
//struct Renderer;
//struct Player;
//
//struct Sun {
//	Vec3 sunDirection = normalize(Vec3(1, 1, 1));
//};
//
//struct Atmosphere {
//	GPUMesh skybox;
//	GPUTexture atmosphere;
//	
//	Float zRotation = 0.0f;
//
//	void render(Sun & sun, Player& player, Renderer& renderer);
//};