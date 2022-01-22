
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

#define MAXPARTICLECOUNT 32768

struct ParticleSystem {
	//particle data
	Vec4 transformation[MAXPARTICLECOUNT];
	Vec3 velocity[MAXPARTICLECOUNT];
	Float maxSize[MAXPARTICLECOUNT];
	Float maxLifetime[MAXPARTICLECOUNT];
	Float lifetime[MAXPARTICLECOUNT];
	Bool alive[MAXPARTICLECOUNT];

	//array data
	UInt currentID = 0;


	//system data
	Vec3 * location;
	Vec3 playerDelta = Vec3(0);
	Vec3 lastLocation = Vec3(0);

	ParticleSystem(Vec3 * location) {
		this->location = location;
	}

	void spawn();
	void update();
	void render(Renderer& renderer);
};

Float particleSizeFunction(Float particleRelativeLifeTime);