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
	const Vec2  baseDir   = normalize(-baseVelocity);
	position[currentId] = pos;
	velocity[currentId] = -baseVelocity * Vec2(2); //+ randomUnitVec2Fast() * randomFloatFast(1.0f);
	lifetime[currentId]    = 0.0f;
	maxLifetime[currentId] = randomFloatFast(0.2f, 1.0f);
	sizeStart[currentId]   = 0.03f;
	alive[currentId] = true;
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
		velocity[i] *= 0.995f; // gentle slowdown
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
		const Float size = sizeStart[i] * (1.0f - t);
		if(size <= 0) continue;

		transforms.emplace_back(matrixFromPositionAndSize(position[i], size));
	}

	if(!transforms.empty()) renderer.renderMeshInstanced("centeredPlane", transforms);
}

