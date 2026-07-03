#include "ParticleSystem.hpp"
#include "Math.hpp"
#include "Random.hpp"
#include "Renderer.hpp"

namespace
{
	Vec3 quadraticBezier(const Vec3& start, const Vec3& control, const Vec3& end, const Float u)
	{
		const Float invU = 1.0f - u;
		return invU * invU * start + 2.0f * invU * u * control + u * u * end;
	}

	Vec3 quadraticBezierDerivative(const Vec3& start, const Vec3& control, const Vec3& end, const Float u)
	{
		return 2.0f * (1.0f - u) * (control - start) + 2.0f * u * (end - control);
	}

	Double quadraticBezierScalar(const Double start, const Double control, const Double end, const Float u)
	{
		const Double invU = 1.0 - u;
		return invU * invU * start + 2.0 * invU * static_cast<Double>(u) * control + static_cast<Double>(u) * static_cast<Double>(u) * end;
	}

	Double quadraticBezierScalarDerivative(const Double start, const Double control, const Double end, const Float u)
	{
		return 2.0 * (1.0 - static_cast<Double>(u)) * (control - start) + 2.0 * static_cast<Double>(u) * (end - control);
	}

	Float parameterForDistance(const Vector<Float>& cumulativeLength, const Float targetDistance)
	{
		if(cumulativeLength.empty()) return 0.0f;
		if(targetDistance <= 0.0f) return 0.0f;

		const Float totalLength = cumulativeLength.back();
		if(totalLength <= 0.0f) return 0.0f;
		if(targetDistance >= totalLength) return 1.0f;

		for(Int i = 1; i < static_cast<Int>(cumulativeLength.size()); ++i)
		{
			if(cumulativeLength[i] >= targetDistance)
			{
				const Float previousDistance = cumulativeLength[i - 1];
				const Float nextDistance     = cumulativeLength[i];
				const Float intervalDistance = max(nextDistance - previousDistance, 0.000001f);
				const Float alpha            = (targetDistance - previousDistance) / intervalDistance;
				const Float u0               = static_cast<Float>(i - 1) / static_cast<Float>(cumulativeLength.size() - 1);
				const Float u1               = static_cast<Float>(i) / static_cast<Float>(cumulativeLength.size() - 1);
				return lerp(u0, u1, alpha);
			}
		}

		return 1.0f;
	}

	Float overwriteRelativeAgeFloor(const UInt slot, const UInt newestSlot, const UInt slotCount)
	{
		if(slotCount <= 1) return 0.0f;

		// Slot directly after newestSlot is overwritten next, so it should be near end-of-life.
		const UInt slotsUntilOverwrite = (slot + slotCount - newestSlot - 1) % slotCount;
		return 1.0f - static_cast<Float>(slotsUntilOverwrite) / static_cast<Float>(slotCount - 1);
	}
}

ParticleSystem::~ParticleSystem()
{
	if(loaded)
	{
		delete[] transformation;
		delete[] spawnPosition;
		delete[] spawnVelocity;
		delete[] maxSize;
		delete[] maxLifetime;
		delete[] lifetime;
		delete[] alive;

		transformation = nullptr;
		spawnPosition  = nullptr;
		spawnVelocity  = nullptr;
		maxSize        = nullptr;
		maxLifetime    = nullptr;
		lifetime       = nullptr;
		alive          = nullptr;
	}
	else logError("ParticleSystem was not initialized before destruction, error must have happened!");
}

ParticleSystem::ParticleSystem(const U16 particleCount)
{
	this->particleCount = particleCount;

	if(!loaded)
	{
		alive          = new Bool[particleCount];
		spawnPosition  = new Vec3[particleCount];
		spawnVelocity  = new Vec3[particleCount];
		maxSize        = new Float[particleCount];
		lifetime       = new Float[particleCount];
		maxLifetime    = new Float[particleCount];
		transformation = new Vec4[particleCount];
		loaded         = true;

		for(UInt i = 0; i < particleCount; ++i)
		{
			alive[i]          = false;
			spawnPosition[i]  = Vec3(0);
			spawnVelocity[i]  = Vec3(0);
			maxSize[i]        = 0.0f;
			lifetime[i]       = 0.0f;
			maxLifetime[i]    = 1.0f;
			transformation[i] = Vec4(0);
		}
	}
}

void ParticleSystem::spawn(const Vec3 loc, const Vec3 vel, const Float visualForce, const Float initialAge)
{
	// advance to next particle slot
	currentId++;
	if(currentId >= particleCount) currentId = 0;

	const Float force = max(visualForce, 0.0f);

	spawnPosition[currentId] = loc;
	spawnVelocity[currentId] = -randomFloatFast(0.2f) * vel + randomUnitVec3Fast() * randomFloatFast(0.01f * force);
	spawnVelocity[currentId].z = abs(spawnVelocity[currentId].z);
	maxSize[currentId]         = 0.15f + force * 0.4f * randomFloatFast();
	maxLifetime[currentId]     = randomFloatFast(0.5f, 1.5f) * (3.0f + powf(force / 10000.0f, 1.5f));
	lifetime[currentId]        = max(initialAge, 0.0f);
	alive[currentId]           = lifetime[currentId] < maxLifetime[currentId];

	if(alive[currentId])
	{
		const Vec3 currentPosition = spawnPosition[currentId] + (spawnVelocity[currentId] + ambientVelocity) * lifetime[currentId];
		const Float relativeLife   = clamp(lifetime[currentId] / maxLifetime[currentId], 0.0f, 1.0f);
		transformation[currentId]  = Vec4(currentPosition, particleSizeFunction(relativeLife) * maxSize[currentId]);
	}
}

Float particleSizeFunction(const Float particleRelativeLifeTime)
{
	return particleRelativeLifeTime - powf(particleRelativeLifeTime, 5);
}

void ParticleSystem::update(const Vec3 newPosition, const Renderer& renderer)
{
	const Float  deltaTime = renderer.time.getDelta();
	const Double now       = renderer.time.getTotal();

	for(UInt i = 0; i < particleCount; i++)
	{
		if(!alive[i]) continue;

		lifetime[i] += deltaTime;
		const Float minimumRelativeAge = overwriteRelativeAgeFloor(i, currentId, particleCount);
		const Float minimumAge         = minimumRelativeAge * maxLifetime[i];
		lifetime[i]                    = max(lifetime[i], minimumAge);
		if(lifetime[i] >= maxLifetime[i])
		{
			alive[i] = false;
			continue;
		}

		const Float relativeLife = clamp(lifetime[i] / maxLifetime[i], 0.0f, 1.0f);
		const Vec3 currentPosition = spawnPosition[i] + (spawnVelocity[i] + ambientVelocity) * lifetime[i];
		transformation[i]          = Vec4(currentPosition, particleSizeFunction(relativeLife) * maxSize[i]);
	}

	if(!hasNewerSample)
	{
		hasNewerSample = true;
		newerSamplePos = newPosition;
		newerSampleTime = now;
		this->position = newPosition;
		return;
	}

	if(!hasOlderSample)
	{
		hasOlderSample = true;
		olderSamplePos = newerSamplePos;
		olderSampleTime = newerSampleTime;
		newerSamplePos = newPosition;
		newerSampleTime = now;
		this->position = newPosition;
		return;
	}

	const Vec3 sample2Pos  = newPosition;
	const Double sample2Time = now;

	const Vec3 startPos   = (olderSamplePos + newerSamplePos) * 0.5f;
	const Vec3 controlPos = newerSamplePos;
	const Vec3 endPos     = (newerSamplePos + sample2Pos) * 0.5f;

	const Double startTime   = (olderSampleTime + newerSampleTime) * 0.5;
	const Double controlTime = newerSampleTime;
	const Double endTime     = (newerSampleTime + sample2Time) * 0.5;

	if(endTime > startTime)
	{
		const Float spacing = 1.0f / max(particlesPerUnit, 0.0001f);
		const Float controlPolygonLength = distance(startPos, controlPos) + distance(controlPos, endPos);
		const Int subdivisions = clamp(static_cast<Int>(ceil(controlPolygonLength * particlesPerUnit * 2.0f)), 8, 256);
		Vector<Float> cumulativeLength(static_cast<SizeT>(subdivisions + 1), 0.0f);

		Vec3 previousPoint = startPos;
		for(Int i = 1; i <= subdivisions; ++i)
		{
			const Float u     = static_cast<Float>(i) / static_cast<Float>(subdivisions);
			const Vec3 point  = quadraticBezier(startPos, controlPos, endPos, u);
			const Float delta = distance(previousPoint, point);
			cumulativeLength[static_cast<SizeT>(i)] = cumulativeLength[static_cast<SizeT>(i - 1)] + delta;
			previousPoint = point;
		}

		const Float segmentLength = cumulativeLength.back();
		if(segmentLength > 0.0f)
		{
			const Float combinedDistance = distanceSinceLastSpawn + segmentLength;
			const Int spawnCount = static_cast<Int>(floor(combinedDistance / spacing));

			for(Int spawnIndex = 0; spawnIndex < spawnCount; ++spawnIndex)
			{
				const Float sampleDistance = (spacing - distanceSinceLastSpawn) + static_cast<Float>(spawnIndex) * spacing;
				const Float u = parameterForDistance(cumulativeLength, sampleDistance);

				const Vec3 spawnPoint = quadraticBezier(startPos, controlPos, endPos, u);
				const Vec3 dPosDu = quadraticBezierDerivative(startPos, controlPos, endPos, u);
				const Double dTimeDu = quadraticBezierScalarDerivative(startTime, controlTime, endTime, u);

				if(abs(dTimeDu) < 0.0000001) continue;

				const Vec3 sampleVelocity = dPosDu / static_cast<Float>(dTimeDu);
				const Double spawnTime = quadraticBezierScalar(startTime, controlTime, endTime, u);
				const Float initialAge = max(static_cast<Float>(now - spawnTime), 0.0f);
				const Float segmentDuration = max(static_cast<Float>(endTime - startTime), 0.000001f);
				const Float visualForce = length(sampleVelocity) * segmentDuration;
				spawn(spawnPoint, sampleVelocity, visualForce, initialAge);
			}

			distanceSinceLastSpawn = combinedDistance - static_cast<Float>(spawnCount) * spacing;
		}
		else
		{
			distanceSinceLastSpawn = min(distanceSinceLastSpawn, spacing);
		}
	}

	olderSamplePos  = newerSamplePos;
	olderSampleTime = newerSampleTime;
	newerSamplePos  = sample2Pos;
	newerSampleTime = sample2Time;
	this->position  = newPosition;
}

void ParticleSystem::render(Renderer& renderer) const
{
	static Vector<Matrix> pos;
	pos.clear();
	for(Int i = 0; i < particleCount; i++) if(alive[i]) pos.push_back(matrixFromPositionAndSize(transformation[i]));
	renderer.renderMeshInstanced("melonSmokeParticle", pos);
}
