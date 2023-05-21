#include "ParticleSystem.hpp"
#include "Random.hpp"
#include "Renderer.hpp"

ParticleSystem::~ParticleSystem() {
	if (loaded) {
		delete transformation;
		delete velocity;
		delete maxSize;
		delete maxLifetime;
		delete lifetime;
		delete alive;

		transformation = nullptr;
		velocity = nullptr;
		maxSize = nullptr;
		maxLifetime = nullptr;
		lifetime = nullptr;
		alive = nullptr;
	}
	else logError("ParticleSystem was not initialized before destruction, error must have happened!");
}

void ParticleSystem::spawn(const Vec3 loc, const Vec3 vel) {
	//advance to next particle slot
	currentId++;
	if (currentId >= particleCount) currentId = 0;

	const Float force = length(vel);

	transformation[currentId] = Vec4(loc, 0);
	velocity[currentId] = -randomFloat(0.2) * vel + randomUnitVec3() * (randomFloat(0.01 * force));
	velocity[currentId].z = abs(velocity[currentId].z);
	maxSize[currentId] = 0.15f + force * 0.4f * randomFloat();
	maxLifetime[currentId] = (1.0f + sq(force / 100)) / 2.0f;
	lifetime[currentId] = 0;
	alive[currentId] = true;
}

void ParticleSystem::render(ResourceManager& resourceManager, Renderer& renderer) {
	static Vector<Vec4> pos;
	pos.clear();
	for (Int i = 0; i < particleCount; i++) { if (alive[i])pos.push_back(Vec4(transformation[i])); }
	renderer.matrixSystem.matrixArray(pos);
	renderer.renderMeshInstanced(resourceManager, "melonSmokeParticle");
}

ParticleSystem::ParticleSystem(const U16 particleCount) {
	this->particleCount = particleCount;

	if (!loaded) {
		alive = new Bool[particleCount];
		velocity = new Vec3[particleCount];
		maxSize = new Float[particleCount];
		lifetime = new Float[particleCount];
		maxLifetime = new Float[particleCount];
		transformation = new Vec4[particleCount];
		loaded = true;
	}
}

Float particleSizeFunction(const Float particleRelativeLifeTime) {
	return particleRelativeLifeTime - pow(particleRelativeLifeTime, 5);
	//return 1-particleRelativeLifeTime;
}

void ParticleSystem::update(const Vec3 location, const Renderer& renderer) {
	delta += renderer.time.delta;
	if (location != this->location) {
		previous2 = previous1;
		previous1 = this->location;
		this->location = location;

		const Vec3 endLast = (previous2 + previous1) / 2.0f;
		const Vec3 startNext = (previous1 + location) / 2.0f;

		const Float distanceTravelled = length(endLast - previous1) + length(startNext - previous1);

		for (Float i = 1.0f / particlesPerUnit; i < distanceTravelled; i += 1.0f / particlesPerUnit) {
			Vec3 currentparticleLocation = quadraticInterpolation(endLast, previous1, startNext, i / distanceTravelled);
			Vec3 previousParticleLocation = quadraticInterpolation(endLast, previous1, startNext,
			                                                       -1 / distanceTravelled);
			const Vec3 currentParticleVelocity = currentparticleLocation - previousParticleLocation;
			spawn(currentparticleLocation, currentParticleVelocity);
		}
	}

	for (UInt i = 0; i < particleCount; i++) {
		if (alive[i]) {
			transformation[i] += Vec4(velocity[i] + Vec3(0, 0, 0.001), 0);
			lifetime[i] += renderer.time.delta;
			transformation[i].w = particleSizeFunction(lifetime[i] / maxLifetime[i]) * maxSize[i];
			alive[i] = lifetime[i] < maxLifetime[i];
		}
	}
}
