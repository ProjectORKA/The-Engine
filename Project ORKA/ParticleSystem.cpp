
#include "ParticleSystem.hpp"
#include "Random.hpp"
#include "Renderer.hpp"

void ParticleSystem::spawn() {
	//advance to next particle slot
	currentID++;
	if (currentID >= MAXPARTICLECOUNT) currentID = 0;

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

	for (UInt i = 0; i < MAXPARTICLECOUNT; i++) {
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
	for (Int i = 0; i < MAXPARTICLECOUNT; i++) {
		if (alive[i])pos.push_back(Vec4(transformation[i]));
	}
	renderer.renderMeshInstanced("particle", pos);
}

Float particleSizeFunction(Float particleRelativeLifeTime) {
	return particleRelativeLifeTime - pow(particleRelativeLifeTime, 5);
}