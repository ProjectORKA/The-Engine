
#pragma once

#include "Basics.hpp"
#include "Transform.hpp"

struct Renderer;

struct Particle {
	Transform transform;
	Vec3 velocity;
	Float lifetime = 0;
};

struct ParticleSystem {
	Float maxParticleSize = 0.3;
	Float maxLifeTime = 1.5;
	List<Particle> particles;
	Vec3 lastLocation = Vec3(0);
	UInt maxParticleCount = 1000;

	void spawn();
	void update(Vec3 location);
	void render(Renderer& renderer);
};

Float particleSizeFunction(Float particleRelativeLifeTime);