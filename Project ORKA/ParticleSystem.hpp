#pragma once

#include "Basics.hpp"
#include "Transform.hpp"
#include "ResourceManager.hpp"

struct Renderer;

struct ParticleSystem {
	//particle data
	Vec4* transformation = nullptr;
	Vec3* velocity = nullptr;
	Float* maxSize = nullptr;
	Float* maxLifetime = nullptr;
	Float* lifetime = nullptr;
	Bool* alive = nullptr;
	Float particlesPerUnit = 64;
	//system data
	U16 currentID = 0;
	Bool loaded = false;
	U16 particleCount = 0;
	Vec3 location = Vec3(0);

	//advanced data for smooth interpolation
	Vec3 previous1 = Vec3(0);
	Vec3 previous2 = Vec3(0);
	Float delta = 0;

	~ParticleSystem();
	ParticleSystem() = delete;
	ParticleSystem(U16 particleCount);
	void spawn(Vec3 location, Vec3 velocity);
	void update(Vec3 location, const Renderer& renderer);
	void render(ResourceManager& resourceManager, Renderer& renderer);
};

Float particleSizeFunction(Float particleRelativeLifeTime);
