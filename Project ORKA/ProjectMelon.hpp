#pragma once

#include "Game.hpp"
#include "KeyMap.hpp"
#include "MelonEnemy.hpp"
#include "MelonPlayer.hpp"
#include "MelonProjectile.hpp"
#include "MelonWorld.hpp"
#include "WaveformGraph.hpp"

struct Window;

// [TODO] maybe implement this stuff
// bullethell
// autofire / direction based dip
// parcour
// time travel self replication challenge
// skillbased
// dash
// morning star
// no movement stops time

// [TODO] next
// vertex colors
// smoke not affected by shadow
// collision
// collectables
// smooth acceleration estimate for particle generation

struct MelonRenderer final : GameRenderer
{
	MelonWorld  world;
	MelonPlayer player;
	WaveformGraph waveformGraph;
	Vector<Float> speedHistory;
	Vector<Float> speedWithFalloffHistory;
	Vector<MelonEnemy> enemies;
	Vector<MelonProjectile> projectiles;
	Float        speedHistoryMin = 0.0f;
	Float        speedHistoryMax = 0.0f;
	Float        speedWithFalloffHistoryMin = 0.0f;
	Float        speedWithFalloffHistoryMax = 0.0f;
	Float        speedWithFalloffUnitsPerSec      = 0.0f;
	Float        speedFalloffReleaseHz            = 9.0f;
	Float        enemySpawnRadius                 = 80.0f;
	Float        projectileCollisionRadius        = 1.0f;
	Float        projectileRenderSize             = 1.35f;
	Bool         debugDisableProjectileDamage     = true;
	Float        playerMaxHealth                  = 100.0f;
	Float        playerHealth                     = 100.0f;
	Float        beatsPerMinute                   = 120.0f;
	Float        beatTimerSecs                    = 0.0f;
	Int          beatCount                        = 0;
	Bool         playerAlive                      = true;
	Int          targetEnemyCount                 = 8;
	Float        ghostTrailDurationSecs           = 4.0f;
	Float        mouseSensitivity                 = 0.0015f;
	Vec3         mousePosA, mousePosB, mousePosC = Vec3(0);
	InputEvent   toggleWireframe                  = InputEvent(InputType::KeyBoard, F, true);
	InputEvent   toggleUi                         = InputEvent(InputType::KeyBoard, F1, true);
	InputEvent   captureMouse                     = InputEvent(InputType::Mouse, Lmb, true);
	InputEvent   respawnRun                       = InputEvent(InputType::KeyBoard, R, true);
	Bool         uiVisible                        = true;

	void spawnEnemyAroundPlayer();
	void handleBeat();

	void update(Window& window) override;
	void create(Window& window) override;
	void destroy(Window& window) override;
	void connect(GameSimulation& simulation) override;
	void renderScene(Window& window) override;
	void inputEvent(Window& window, InputEvent& input) override;
	void renderInteractive(Window& window) override;
};

struct ProjectMelon
{
	MelonRenderer renderer;

	void run()
	{
		ui.window("Project Melon", settings.defaultWindowWidth, settings.defaultWindowHeight, true, true, WindowState::Windowed, renderer);
		ui.run();
	}
};
