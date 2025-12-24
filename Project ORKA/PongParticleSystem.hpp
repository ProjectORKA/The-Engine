#pragma once

#include "Basics.hpp"
#include "Math.hpp"

struct Renderer;

// Lightweight 2D particle system for Pong impacts (small square bursts)
struct PongParticleSystem
{
	Bool*  alive       = nullptr;
	Vec2*  position    = nullptr;
	Vec2*  velocity    = nullptr;
	Float* lifetime    = nullptr;
	Float* maxLifetime = nullptr;
	Float* sizeStart   = nullptr;

	U16  currentId     = 0;
	U16  particleCount = 0;
	Bool loaded        = false;

	~PongParticleSystem();
	PongParticleSystem() = delete;
	explicit PongParticleSystem(U16 particleCount);

	void spawn(Vec2 pos, Vec2 baseVelocity);
	void update(Float deltaTime);
	void render(Renderer& renderer) const;
};

