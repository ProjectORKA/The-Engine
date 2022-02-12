
#include "ParticleSystem.hpp"
#include "Random.hpp"
#include "Renderer.hpp"

ParticleSystem::ParticleSystem(Vec3* location, U16 particleCount) {
	this->location = location;
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

void ParticleSystem::spawn() {
	//advance to next particle slot
	currentID++;
	if (currentID >= particleCount) currentID = 0;

	Float force = length(playerDelta);

	transformation[currentID] = Vec4(*location, 0);
	velocity[currentID] = randomUnitVec3() * (randomFloat(0.01 * force)) - randomFloat(0.2) * playerDelta;
	velocity[currentID].z = abs(velocity[currentID].z);
	maxSize[currentID] = 0.1 + force * 0.5 * randomFloat();
	maxLifetime[currentID] = 1 + sq(force / 100);
	lifetime[currentID] = 0;
	alive[currentID] = true;
}
void ParticleSystem::update() {
	playerDelta = *location - lastLocation;

	for (UInt i = 0; i < particleCount; i++) {
		if (alive[i]) {
			transformation[i] += Vec4(velocity[i] + Vec3(0, 0, 0.001), 0);
			lifetime[i] += 1.0 / 144.0;
			transformation[i].w = particleSizeFunction(lifetime[i] / maxLifetime[i]) * maxSize[i];
			alive[i] = lifetime[i] < maxLifetime[i];
		}
	}

	lastLocation = *location;
}
void ParticleSystem::render(Renderer& renderer) {
	static Vector<Vec4> pos;
	pos.clear();
	for (Int i = 0; i < particleCount; i++) {
		if (alive[i])pos.push_back(Vec4(transformation[i]));
	}
	renderer.renderMeshInstanced("particle", pos);
}
Float particleSizeFunction(Float particleRelativeLifeTime) {
	return particleRelativeLifeTime - pow(particleRelativeLifeTime, 5);
}