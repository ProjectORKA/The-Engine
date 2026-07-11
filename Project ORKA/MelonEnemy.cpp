#include "MelonEnemy.hpp"
#include <algorithm>

void MelonEnemy::updateMovement(const Vec3& playerPosition, Float dt)
{
	const Vec3 toPlayer = playerPosition - position;
	const Float distanceToPlayer = length(toPlayer);
	if(distanceToPlayer <= 0.0001f) return;

	Float moveSpeedUnitsPerSec = moveSpeedRhythmUnitsPerSec;
	if(type == MelonEnemyType::Weak) moveSpeedUnitsPerSec = moveSpeedWeakUnitsPerSec;
	if(type == MelonEnemyType::Heavy) moveSpeedUnitsPerSec = moveSpeedHeavyUnitsPerSec;

	const Vec3 toPlayerDir = toPlayer / distanceToPlayer;
	const Float radialError = distanceToPlayer - desiredDistanceToPlayerUnits;
	const Float radialDirection = radialError >= 0.0f ? 1.0f : -1.0f;
	const Float radialError01 = std::clamp(abs(radialError) / std::max(desiredDistanceToPlayerUnits, 0.001f), 0.15f, 1.0f);
	position += toPlayerDir * radialDirection * moveSpeedUnitsPerSec * radialError01 * dt;
}

Int MelonEnemy::shootEveryBeats() const
{
	if(type == MelonEnemyType::Weak) return shootEveryWeakBeats;
	if(type == MelonEnemyType::Heavy) return shootEveryHeavyBeats;
	return shootEveryRhythmBeats;
}

Float MelonEnemy::projectileSpeedUnitsPerSec() const
{
	if(type == MelonEnemyType::Weak) return projectileSpeedWeakUnitsPerSec;
	if(type == MelonEnemyType::Heavy) return projectileSpeedHeavyUnitsPerSec;
	return projectileSpeedRhythmUnitsPerSec;
}

Float MelonEnemy::projectileDamage() const
{
	if(type == MelonEnemyType::Weak) return projectileDamageWeak;
	if(type == MelonEnemyType::Heavy) return projectileDamageHeavy;
	return projectileDamageRhythm;
}

Float MelonEnemy::size() const
{
	if(type == MelonEnemyType::Weak) return sizeWeak;
	if(type == MelonEnemyType::Heavy) return sizeHeavy;
	return sizeRhythm;
}

Color MelonEnemy::renderColor() const
{
	if(type == MelonEnemyType::Weak) return Color(1.0f, 0.85f, 0.2f, 1.0f);
	if(type == MelonEnemyType::Heavy) return Color(0.95f, 0.2f, 0.95f, 1.0f);
	return Color(1.0f, 0.22f, 0.22f, 1.0f);
}
