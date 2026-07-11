#pragma once

#include "Basics.hpp"

enum class MelonEnemyType : unsigned char
{
	Rhythm = 0,
	Weak   = 1,
	Heavy  = 2
};

struct MelonEnemy final
{
	static constexpr Float moveSpeedWeakUnitsPerSec       = 100.0f;
	static constexpr Float moveSpeedRhythmUnitsPerSec     = 60.0f;
	static constexpr Float moveSpeedHeavyUnitsPerSec      = 10.0f;
	static constexpr Float desiredDistanceToPlayerUnits   = 22.0f;
	static constexpr Float spacingAngularFollowHz         = 5.0f;
	static constexpr Float maxTangentialSpeedUnitsPerSec  = 90.0f;

	// Rhythm now advances in half-beat steps (beat + snare interleaved).
	static constexpr Int   shootEveryWeakBeats            = 2;
	static constexpr Int   shootEveryRhythmBeats          = 6;
	static constexpr Int   shootEveryHeavyBeats           = 12;
	
	static constexpr Float projectileSpeedWeakUnitsPerSec   = 200.0f;
	static constexpr Float projectileSpeedRhythmUnitsPerSec = 120.0f;
	static constexpr Float projectileSpeedHeavyUnitsPerSec  = 20.0f;

	static constexpr Float projectileDamageWeak           = 5.0f;
	static constexpr Float projectileDamageRhythm         = 20.0f;
	static constexpr Float projectileDamageHeavy          = 60.0f;

	static constexpr Float sizeWeak                       = 2.0f;
	static constexpr Float sizeRhythm                     = 3.0f;
	static constexpr Float sizeHeavy                      = 5.0f;

	Vec3           position = Vec3(0);
	MelonEnemyType type     = MelonEnemyType::Rhythm;

	void updateMovement(const Vec3& playerPosition, Float dt);
	Int shootEveryBeats() const;
	Float projectileSpeedUnitsPerSec() const;
	Float projectileDamage() const;
	Float size() const;
	Color renderColor() const;
};
