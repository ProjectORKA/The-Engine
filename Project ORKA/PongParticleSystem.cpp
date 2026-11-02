#include "PongParticleSystem.hpp"

#include "Random.hpp"
#include "Renderer.hpp"

PongParticleSystem::~PongParticleSystem()
{
	if(loaded)
	{
		delete[] alive;
		delete[] position;
		delete[] velocity;
		delete[] lifetime;
		delete[] maxLifetime;
		delete[] sizeStart;

		alive       = nullptr;
		position    = nullptr;
		velocity    = nullptr;
		lifetime    = nullptr;
		maxLifetime = nullptr;
		sizeStart   = nullptr;
	}
}

PongParticleSystem::PongParticleSystem(const U16 particleCount)
{
	this->particleCount = particleCount;

	if(!loaded)
	{
		alive       = new Bool[particleCount];
		position    = new Vec2[particleCount];
		velocity    = new Vec2[particleCount];
		lifetime    = new Float[particleCount];
		maxLifetime = new Float[particleCount];
		sizeStart   = new Float[particleCount];
		loaded      = true;

		for(U16 i = 0; i < particleCount; i++) alive[i] = false;
	}
}

void PongParticleSystem::spawn(const Vec2 pos, const Vec2 baseVelocity)
{
	currentId++;
	if(currentId >= particleCount) currentId = 0;

	const Float incomingSpeed = length(baseVelocity);
	Vec2 away(0.0f, 1.0f);
	if(incomingSpeed > 0.0001f) away = -baseVelocity / incomingSpeed;

	// Even fan around the bounce direction. A shared velocity makes one streak, not a burst.
	const Float angle = randomFloatFast(-1.15f, 1.15f);
	const Float c     = cos(angle);
	const Float s     = sin(angle);
	const Vec2 direction(away.x * c - away.y * s, away.x * s + away.y * c);

	// A fraction of ball speed. Drag in update() stops the spark before it crosses the court.
	const Float kick  = clamp(incomingSpeed * 1.1f, 1.75f, 4.0f);
	const Float speed = randomFloatFast(kick * 0.4f, kick);

	position[currentId]    = pos;
	velocity[currentId]    = direction * speed;
	lifetime[currentId]    = 0.0f;
	maxLifetime[currentId] = randomFloatFast(0.4f, 0.9f);
	sizeStart[currentId]   = randomFloatFast(0.011f, 0.02f);
	alive[currentId]       = true;
}

void PongParticleSystem::update(const Float deltaTime)
{
	for(U16 i = 0; i < particleCount; i++)
	{
		if(!alive[i]) continue;

		lifetime[i] += deltaTime;
		if(lifetime[i] >= maxLifetime[i])
		{
			alive[i] = false;
			continue;
		}

		position[i] += velocity[i] * deltaTime;
		velocity[i] *= exp(-14.0f * deltaTime);
	}
}

void PongParticleSystem::render(Renderer& renderer) const
{
	static Vector<Matrix> transforms;
	transforms.clear();

	for(U16 i = 0; i < particleCount; i++)
	{
		if(!alive[i]) continue;

		const Float t    = lifetime[i] / maxLifetime[i];
		const Float fade = 1.0f - t;
		const Float size = sizeStart[i] * fade * fade;
		if(size <= 0) continue;

		transforms.emplace_back(matrixFromPositionAndSize(position[i], size));
	}

	if(!transforms.empty()) renderer.renderMeshInstanced("centeredPlane", transforms);
}

