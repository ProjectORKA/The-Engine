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
	Vec3*  spawnPosition    = nullptr;
	Vec3*  spawnVelocity    = nullptr;
	Float* lifetime         = nullptr;
	Float* maxLifetime      = nullptr;
	Vec4*  transformation   = nullptr;
	// system data
	U16  currentId     = 0;
	U16  particleCount = 0;
	Bool loaded        = false;
	Vec3 position      = Vec3(0);
	Float distanceSinceLastSpawn = 0.0f;
	Vec3  ambientVelocity        = Vec3(0, 0, 0.06f);

	// timestamped cursor samples for sub-tick spline emission
	Bool   hasOlderSample = false;
	Bool   hasNewerSample = false;
	Vec3   olderSamplePos = Vec3(0);
	Vec3   newerSamplePos = Vec3(0);
	Double olderSampleTime = 0.0;
	Double newerSampleTime = 0.0;

	~ParticleSystem();
	ParticleSystem() = delete;
	explicit ParticleSystem(U16 particleCount);
	void     spawn(Vec3 position, Vec3 velocity, Float visualForce, Float initialAge);
	void     update(Vec3 position, const Renderer& renderer);
	void     render(Renderer& renderer) const;
};

Float particleSizeFunction(Float particleRelativeLifeTime);
