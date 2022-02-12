
#pragma once

#include "Basics.hpp"
#include "Transform.hpp"

struct Renderer;

//struct Particle {
//	Vec3 velocity;
//	Float maxSize = 1;
//	Float maxLifetime = 1;
//	Float lifetime = 0;
//	Bool alive = false;
//	Float size = 1;
//};

struct ParticleSystem {
	//particle data
	Vec4* transformation = nullptr;
	Vec3* velocity = nullptr;
	Float* maxSize = nullptr;
	Float* maxLifetime = nullptr;
	Float* lifetime = nullptr;
	Bool* alive = nullptr;

	//system data
	U16 currentID = 0;
	Bool loaded = false;
	U16 particleCount = 0;
	Vec3* location = nullptr;
	Vec3 playerDelta = Vec3(0);
	Vec3 lastLocation = Vec3(0);

	void spawn();
	void update();
	~ParticleSystem();
	ParticleSystem() = delete;
	void render(Renderer& renderer);
	ParticleSystem(Vec3* location, U16 particleCount);
};

Float particleSizeFunction(Float particleRelativeLifeTime);