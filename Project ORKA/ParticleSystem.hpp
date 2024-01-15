#pragma once

#include "Basics.hpp"
#include "Transform.hpp"


struct Renderer;

struct ParticleSystem
{
	// particle data
	Float  particlesPerUnit = 64;
	Bool*  alive            = nullptr;
	Float* maxSize          = nullptr;
	Vec3*  velocity         = nullptr;
	Float* lifetime         = nullptr;
	Float* maxLifetime      = nullptr;
	Vec4*  transformation   = nullptr;
	// system data
	U16  currentId     = 0;
	U16  particleCount = 0;
	Bool loaded        = false;
	Vec3 location      = Vec3(0);

	// advanced data for smooth interpolation
	Float delta     = 0;
	Vec3  previous1 = Vec3(0);
	Vec3  previous2 = Vec3(0);

	~ParticleSystem();
	ParticleSystem() = delete;
	explicit ParticleSystem(U16 particleCount);
	void     spawn(Vec3 location, Vec3 velocity);
	void     update(Vec3 location, const Renderer& renderer);
	void     render(Renderer& renderer) const;
};

Float particleSizeFunction(Float particleRelativeLifeTime);
