
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
	void spawn(Vec3 location, Vec3 velocity);
	ParticleSystem() = delete;
	void render(Renderer& renderer);
	ParticleSystem(U16 particleCount);
	void update(Vec3 location, Renderer& renderer);
};

Float particleSizeFunction(Float particleRelativeLifeTime);