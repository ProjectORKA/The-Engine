
#include "ParticleSystem.hpp"
#include "Random.hpp"
#include "Renderer.hpp"

void ParticleSystem::spawn() {
	if (particles.size() < maxParticleCount) {
		particles.emplace_back();
		particles.back().transform.scale = 0;
	}
}
void ParticleSystem::update(Vec3 location) {
	Vec3 playerDelta = location - lastLocation;

	spawn();

	for (auto& p : particles) {
		if (p.lifetime >= maxLifeTime) {
			p.transform.location = location;
			p.velocity = randomUnitVec3() * randomFloat(0.01) + playerDelta * randomFloat(0, 0.2);
			p.transform.scale = 0;
			p.lifetime = 0;
		}
		else {
			p.transform.location += p.velocity + Vec3(0, 0, 0.001);
			p.lifetime += 1.0 / 144.0;
			p.transform.scale = particleSizeFunction(p.lifetime / maxLifeTime) * maxParticleSize;
		}
	}

	lastLocation = location;
}
void ParticleSystem::render(Renderer& renderer) {
	for (auto& p : particles) {
		p.transform.render(renderer);
		renderer.renderMesh("sphere");
	}
}

Float particleSizeFunction(Float particleRelativeLifeTime) {
	return particleRelativeLifeTime - pow(particleRelativeLifeTime, 6);
}
