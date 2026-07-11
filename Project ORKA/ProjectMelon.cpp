#include "ProjectMelon.hpp"
#include "Window.hpp"
#include "Random.hpp"
#include "AudioEngine.hpp"
#include "ResourceManager.hpp"
#include <algorithm>

namespace
{
	constexpr const Char* melonBeatSoundId  = "melon_beat";
	constexpr const Char* melonBeatSoundWav = "beat.wav";
	constexpr const Char* melonSnareSoundId  = "melon_snare";
	constexpr const Char* melonSnareSoundWav = "snare.wav";
}

void MelonRenderer::destroy(Window& window)
{
	audioEngine.destroyProceduralMotion();
	GameRenderer::destroy(window);
}

void MelonRenderer::connect(GameSimulation& simulation) {}

void MelonRenderer::spawnEnemyAroundPlayer()
{
	Vec2 dir2 = randomVec2Fast(-1.0f, 1.0f);
	if(length(dir2) < 0.0001f) dir2 = Vec2(0.0f, 1.0f);
	dir2 = normalize(dir2);

	MelonEnemy enemy;
	enemy.position = player.position + Vec3(dir2 * randomFloatFast(enemySpawnRadius * 0.45f, enemySpawnRadius), 0.0f);
	const Int enemyTypeRoll = randomIntFast(100);
	if(enemyTypeRoll < 60) enemy.type = MelonEnemyType::Rhythm;
	else if(enemyTypeRoll < 85) enemy.type = MelonEnemyType::Weak;
	else enemy.type = MelonEnemyType::Heavy;
	enemies.push_back(enemy);
}

void MelonRenderer::handleBeat()
{
	const Bool isDownBeat = (beatCount % 2) != 0;
	if(isDownBeat)
	{
		if(audioEngine.has(melonBeatSoundId)) audioEngine.play(melonBeatSoundId);
	}
	else
	{
		if(audioEngine.has(melonSnareSoundId)) audioEngine.play(melonSnareSoundId);
	}

	for(const MelonEnemy& enemy : enemies)
	{
		const Int beatInterval = max(enemy.shootEveryBeats(), 1);
		if(beatCount % beatInterval != 0) continue;

		const Vec3 shootDir = player.position - enemy.position;
		if(length(shootDir) <= 0.0001f) continue;

		MelonProjectile projectile;
		projectile.position = enemy.position;
		projectile.velocity = normalize(shootDir) * enemy.projectileSpeedUnitsPerSec();
		projectile.lifeSecs = 6.0f;
		projectile.damage   = enemy.projectileDamage();
		projectiles.push_back(projectile);
	}
}

void MelonRenderer::update(Window& window)
{
	player.ghostTrailDurationSecs = ghostTrailDurationSecs;
	player.mouseSensitivity       = mouseSensitivity;
	const Float dt = max(window.renderer.deltaTime(), 0.0001f);
	if(playerAlive) player.update(window);

	const Float actualSpeedUnitsPerSec = max(player.speedUnitsPerSec - 0.25f, 0.0f);
	if(actualSpeedUnitsPerSec >= speedWithFalloffUnitsPerSec)
	{
		// Instant attack: immediately follow acceleration spikes.
		speedWithFalloffUnitsPerSec = actualSpeedUnitsPerSec;
	}
	else
	{
		// Slow release: fall back smoothly when the actual speed drops.
		const Float releaseFollow = 1.0f - exp(-dt * speedFalloffReleaseHz);
		speedWithFalloffUnitsPerSec += (actualSpeedUnitsPerSec - speedWithFalloffUnitsPerSec) * releaseFollow;
	}
	const Float speed01 = clamp(speedWithFalloffUnitsPerSec / proceduralMotionMaxSpeedUnitsPerSec, 0.0f, 1.0f);
	Float dopplerDirection = 0.0f;
	if(playerAlive && length(player.virtualAudioDeltaPosition) > 0.0001f && length(player.position) > 0.0001f)
	{
		const Vec3 movementDirection = normalize(player.virtualAudioDeltaPosition);
		const Vec3 centerDirection = normalize(-player.position);
		dopplerDirection = clamp(dot(movementDirection, centerDirection), -1.0f, 1.0f);
	}
	audioEngine.updateProceduralMotion(speed01, dopplerDirection, dt);

	if(playerAlive)
	{
		while(static_cast<Int>(enemies.size()) < targetEnemyCount) spawnEnemyAroundPlayer();

		for(MelonEnemy& enemy : enemies) enemy.updateMovement(player.position, dt);

		if(enemies.size() >= 2)
		{
			Vector<SizeT> sortedEnemyIndices;
			sortedEnemyIndices.reserve(enemies.size());
			for(SizeT i = 0; i < enemies.size(); ++i) sortedEnemyIndices.push_back(i);

			auto enemyAngle = [&](const SizeT enemyIndex)
			{
				const Vec3 delta = enemies[enemyIndex].position - player.position;
				return atan2(delta.y, delta.x);
			};

			std::sort(sortedEnemyIndices.begin(), sortedEnemyIndices.end(),
			[&](const SizeT a, const SizeT b)
			{
				return enemyAngle(a) < enemyAngle(b);
			});

			for(SizeT rank = 0; rank < sortedEnemyIndices.size(); ++rank)
			{
				const SizeT currentIndex = sortedEnemyIndices[rank];
				const SizeT prevIndex = sortedEnemyIndices[(rank + sortedEnemyIndices.size() - 1) % sortedEnemyIndices.size()];
				const SizeT nextIndex = sortedEnemyIndices[(rank + 1) % sortedEnemyIndices.size()];

				Vec3 radial = enemies[currentIndex].position - player.position;
				radial.z = 0.0f;
				const Float radius = max(length(radial), 0.0001f);
				const Vec3 radialDir = radial / radius;
				const Vec3 tangentDir = Vec3(-radialDir.y, radialDir.x, 0.0f);

				const Float currentAngle = atan2(radialDir.y, radialDir.x);
				Float prevAngle = enemyAngle(prevIndex);
				Float nextAngle = enemyAngle(nextIndex);

				while(prevAngle > currentAngle) prevAngle -= static_cast<Float>(PI) * 2.0f;
				while(nextAngle < currentAngle) nextAngle += static_cast<Float>(PI) * 2.0f;

				const Float targetAngle = (prevAngle + nextAngle) * 0.5f;
				const Float angleDelta = targetAngle - currentAngle;
				const Float tangentSpeed = clamp(angleDelta * radius * MelonEnemy::spacingAngularFollowHz,
					-MelonEnemy::maxTangentialSpeedUnitsPerSec, MelonEnemy::maxTangentialSpeedUnitsPerSec);
				enemies[currentIndex].position += tangentDir * tangentSpeed * dt;
			}
		}

		// Half-beat step so we can interleave beat/snare in one rhythm stream.
		const Float beatIntervalSecs = 30.0f / max(beatsPerMinute, 1.0f);
		beatTimerSecs += dt;
		while(beatTimerSecs >= beatIntervalSecs)
		{
			beatTimerSecs -= beatIntervalSecs;
			++beatCount;
			handleBeat();
		}

		for(SizeT i = enemies.size(); i > 0; --i)
		{
			const SizeT enemyIndex = i - 1;
			const Float minDistanceToPlayer = enemies[enemyIndex].size() + MelonPlayer::baseSize;
			if(distance(enemies[enemyIndex].position, player.position) < minDistanceToPlayer)
			{
				enemies.erase(enemies.begin() + static_cast<Int>(enemyIndex));
			}
		}

		for(SizeT i = 0; i < enemies.size(); ++i)
		{
			for(SizeT j = i + 1; j < enemies.size(); ++j)
			{
				Vec3 delta = enemies[j].position - enemies[i].position;
				Float distanceBetween = length(delta);
				if(distanceBetween <= 0.0001f)
				{
					delta = Vec3(1.0f, 0.0f, 0.0f);
					distanceBetween = 1.0f;
				}

				const Float minDistance = enemies[i].size() + enemies[j].size();
				if(distanceBetween >= minDistance) continue;

				const Vec3 separationDir = delta / distanceBetween;
				const Float overlap = minDistance - distanceBetween;
				enemies[i].position -= separationDir * (overlap * 0.5f);
				enemies[j].position += separationDir * (overlap * 0.5f);
			}
		}
	}

	for(SizeT i = projectiles.size(); i > 0; --i)
	{
		const SizeT projectileIndex = i - 1;
		MelonProjectile& projectile = projectiles[projectileIndex];
		projectile.update(dt);

		const Bool expired = projectile.isExpired(player.position, enemySpawnRadius * 4.0f);
		const Bool hitPlayer = playerAlive && projectile.hitsPlayer(player.position, projectileCollisionRadius + MelonPlayer::baseSize);
		if(hitPlayer)
		{
			if(!debugDisableProjectileDamage)
			{
				playerHealth = max(playerHealth - projectile.damage, 0.0f);
				playerAlive = playerHealth > 0.0f;
			}
		}
		if(expired || hitPlayer) projectiles.erase(projectiles.begin() + static_cast<Int>(projectileIndex));
	}

	speedHistory.push_back(player.speedUnitsPerSec);
	speedWithFalloffHistory.push_back(speedWithFalloffUnitsPerSec);
	const SizeT maxSpeedSamples = 320;
	if(speedHistory.size() > maxSpeedSamples) speedHistory.erase(speedHistory.begin());
	if(speedWithFalloffHistory.size() > maxSpeedSamples) speedWithFalloffHistory.erase(speedWithFalloffHistory.begin());
	if(!speedHistory.empty())
	{
		speedHistoryMin = speedHistory.front();
		speedHistoryMax = speedHistory.front();
		for(const Float sample : speedHistory)
		{
			speedHistoryMin = min(speedHistoryMin, sample);
			speedHistoryMax = max(speedHistoryMax, sample);
		}
	}
	if(!speedWithFalloffHistory.empty())
	{
		speedWithFalloffHistoryMin = speedWithFalloffHistory.front();
		speedWithFalloffHistoryMax = speedWithFalloffHistory.front();
		for(const Float sample : speedWithFalloffHistory)
		{
			speedWithFalloffHistoryMin = min(speedWithFalloffHistoryMin, sample);
			speedWithFalloffHistoryMax = max(speedWithFalloffHistoryMax, sample);
		}
	}

	Vector<Float> waveform;
	Vector<Float> control;
	Vector<Float> audio;
	audioEngine.getProceduralMotionDebugWaveform(waveform, control, audio);
	waveformGraph.setSamples(waveform, control, audio);

	//world.update(player);
}

void MelonRenderer::create(Window& window)
{
	GameRenderer::create(window);
	ui.capture(*this);
	audioEngine.createProceduralMotion();
	audioEngine.load(melonBeatSoundId, resourceManager.getOrkaDataPath() / "Sounds" / melonBeatSoundWav, false);
	audioEngine.load(melonSnareSoundId, resourceManager.getOrkaDataPath() / "Sounds" / melonSnareSoundWav, false);
	ProceduralMotionTuning tuning = audioEngine.getProceduralMotionTuning();
	tuning.responseCurve = 0.25f;
	tuning.windGain = 0.6f;
	audioEngine.setProceduralMotionTuning(tuning);
	audioEngine.setProceduralMotionEnabled(true);

	playerAlive = true;
	playerHealth = playerMaxHealth;
	beatTimerSecs = 0.0f;
	beatCount = 0;
	enemies.clear();
	projectiles.clear();
	speedWithFalloffUnitsPerSec = 0.0f;
	speedHistory.clear();
	speedWithFalloffHistory.clear();
}

void MelonRenderer::inputEvent(Window& window, InputEvent& input)
{
	const Bool altPressed = window.pressed(window.altKey);

	if(input == toggleWireframe)
	{
		window.renderer.wireframeMode = !window.renderer.wireframeMode;
		input.consume();
	}
	// Keep Alt+Enter available for fullscreen toggle.
	if(input == captureMouse && !altPressed)
	{
		ui.capture(*this);
		input.consume();
	}
	if(input == respawnRun)
	{
		playerAlive = true;
		playerHealth = playerMaxHealth;
		beatTimerSecs = 0.0f;
		beatCount = 0;
		player.respawn();
		enemies.clear();
		projectiles.clear();
		speedWithFalloffUnitsPerSec = 0.0f;
		speedHistory.clear();
		speedWithFalloffHistory.clear();
		speedHistoryMin = 0.0f;
		speedHistoryMax = 0.0f;
		speedWithFalloffHistoryMin = 0.0f;
		speedWithFalloffHistoryMax = 0.0f;
		ui.capture(*this);
		input.consume();
	}
	if(playerAlive) player.inputEvent(window, input);
}

void MelonRenderer::renderScene(Window& window)
{
	Renderer& r = window.renderer;

	framebuffer.clear(r);

	r.uniforms().reset();

	r.setWireframeMode(r.wireframeMode);
	r.setCulling(true);
	r.setDepthTest(true);

	r.fill(Color(1, 0, 0, 1));
	r.uniforms().setSunDir(Vec4(normalize(Vec3(-0.666, 0.333, 1)), 1));

	r.useShader("MelonUberShader");
	player.render(window);
	r.useShader("color");
	r.uniforms().setMMatrix(Matrix(1));
	for(const MelonEnemy& enemy : enemies)
	{
		r.fill(enemy.renderColor());
		r.uniforms().setMMatrix(matrixFromPositionAndSize(enemy.position, enemy.size()));
		r.renderMesh("sphere");
	}
	for(const MelonProjectile& projectile : projectiles)
	{
		r.fill(1.0f, 0.72f, 0.2f, 1.0f);
		r.uniforms().setMMatrix(matrixFromPositionAndSize(projectile.position, projectileRenderSize));
		r.plane();
	}

	//ui
	r.uniforms().setMMatrix(Matrix(1));
	r.setCulling(false);
	r.setDepthTest(false);
	r.setAlphaBlending(true);
	r.screenSpaceFromBottomLeft();
	r.fontSize(20.0f);
	r.letterSpacing(0.6f);
	r.textRenderSystem.alignText(Alignment::left, Alignment::bottom);
	r.text("FPS: " + toString(1 / r.time.getDelta()), Vec2(30, 30));
	r.text("player Position: " + toString(player.position), Vec2(30, 60));
	r.text("Camera Position: " + toString(player.camera.getPosition()), Vec2(30, 90));
	r.text("Press Enter to capture mouse | RMB to release", Vec2(30, 120));
	r.text("Press F for wireframe mode", Vec2(30, 150));
	r.text("Press R to respawn", Vec2(30, 180));
	r.text("Ghost trail seconds: " + toString(ghostTrailDurationSecs), Vec2(30, 210));
	r.text("Speed units/sec (actual): " + toString(player.speedUnitsPerSec), Vec2(30, 240));
	r.text("Speed with falloff: " + toString(speedWithFalloffUnitsPerSec), Vec2(30, 270));
	r.text("Acceleration: " + toString(player.smoothedAccelerationUnitsPerSec2), Vec2(30, 300));
	const Int musicalBeatCount = beatCount / 2;
	r.text("Enemies: " + toString(enemies.size()) + " | Projectiles: " + toString(projectiles.size()) + " | Beat: " + toString(musicalBeatCount), Vec2(30, 330));
	r.text("Enemy rhythm steps: Rhythm x6 | Weak x2 | Heavy x12 (beat+snare interleaved)", Vec2(30, 360));
	if(debugDisableProjectileDamage) r.text("Projectile damage: OFF (debug)", Vec2(30, 390));
	const Float health01 = clamp(playerHealth / max(playerMaxHealth, 0.001f), 0.0f, 1.0f);
	const Float healthBarX = 30.0f;
	const Float healthBarY = 422.0f;
	const Float healthBarWidth = 340.0f;
	const Float healthBarHeight = 26.0f;
	r.fill(0.12f, 0.12f, 0.12f, 0.85f);
	r.uniforms().setMMatrix(matrixFromPositionAndSize(healthBarX + healthBarWidth * 0.5f, healthBarY + healthBarHeight * 0.5f, healthBarWidth, healthBarHeight));
	r.plane();
	r.fill(1.0f - health01 * 0.65f, 0.22f + health01 * 0.78f, 0.22f, 0.95f);
	r.uniforms().setMMatrix(matrixFromPositionAndSize(healthBarX + healthBarWidth * health01 * 0.5f, healthBarY + healthBarHeight * 0.5f, healthBarWidth * health01, healthBarHeight));
	r.plane();
	r.fill(1, 1, 1, 1);
	r.text("HP: " + toString(playerHealth) + " / " + toString(playerMaxHealth), Vec2(healthBarX + 8.0f, healthBarY + 4.0f));
	if(!playerAlive)
	{
		r.fill(1.0f, 0.3f, 0.3f, 1.0f);
		r.text("You died. Press R to respawn.", Vec2(30, 426));
		r.fill(1, 1, 1, 1);
	}
	const Float speedGraphX = 30.0f;
	const Float speedGraphY = 470.0f;
	const Float speedGraphWidth = 560.0f;
	const Float speedGraphHeight = 150.0f;
	r.useShader("color");
	r.fill(0, 0, 0, 0.45f);
	r.uniforms().setMMatrix(matrixFromPositionAndSize(speedGraphX + speedGraphWidth * 0.5f, speedGraphY + speedGraphHeight * 0.5f, speedGraphWidth, speedGraphHeight));
	r.plane();
	r.fill(0.55f, 0.55f, 0.55f, 0.65f);
	Vec2Vector speedMinLine = { Vec2(speedGraphX, speedGraphY), Vec2(speedGraphX + speedGraphWidth, speedGraphY) };
	r.uniforms().setMMatrix(Matrix(1));
	r.line(speedMinLine);
	if(speedHistory.size() > 1)
	{
		const Float graphMin = min(speedHistoryMin, speedWithFalloffHistoryMin);
		const Float graphMax = max(speedHistoryMax, speedWithFalloffHistoryMax);
		const Float speedRange = max(graphMax - graphMin, 0.001f);

		auto buildSpeedLine = [&](const Vector<Float>& samples)
		{
			Vec2Vector speedGraphLine;
			if(samples.size() <= 1) return speedGraphLine;
			const Float speedStepX = speedGraphWidth / static_cast<Float>(samples.size() - 1);
			speedGraphLine.reserve(samples.size());
			for(SizeT i = 0; i < samples.size(); ++i)
			{
				const Float normalized = clamp((samples[i] - graphMin) / speedRange, 0.0f, 1.0f);
				speedGraphLine.emplace_back(speedGraphX + static_cast<Float>(i) * speedStepX, speedGraphY + normalized * speedGraphHeight);
			}
			return speedGraphLine;
		};

		Vec2Vector speedGraphLine = buildSpeedLine(speedHistory);
		r.fill(0.2f, 1.0f, 0.35f, 1.0f);
		r.line(speedGraphLine);

		Vec2Vector speedWithFalloffLine = buildSpeedLine(speedWithFalloffHistory);
		if(!speedWithFalloffLine.empty())
		{
			r.fill(1.0f, 0.65f, 0.2f, 0.95f);
			r.line(speedWithFalloffLine);
		}
	}

	r.fill(1, 1, 1, 1);
	r.text("Speed over time (green = actual, orange = with falloff)", Vec2(speedGraphX, speedGraphY + speedGraphHeight + 24.0f));
	r.text("Min speed: " + toString(speedHistoryMin), Vec2(speedGraphX + speedGraphWidth + 20.0f, speedGraphY + speedGraphHeight - 10.0f));
	r.text("Max speed: " + toString(speedHistoryMax), Vec2(speedGraphX + speedGraphWidth + 20.0f, speedGraphY + speedGraphHeight - 40.0f));

	const Float waveformGraphY = speedGraphY + speedGraphHeight + 50.0f;
	waveformGraph.positionFromBottomLeft = Vec2(speedGraphX, waveformGraphY);
	waveformGraph.width                  = static_cast<Int>(speedGraphWidth);
	waveformGraph.height                 = 150;
	waveformGraph.render(window);

	r.useShader("color");
	r.screenSpaceFromBottomLeft();
	r.fill(1, 1, 1, 1);
	r.text("Procedural motion audio (magenta) + debug oscillator (cyan) + speed (orange)", Vec2(speedGraphX, waveformGraphY + waveformGraph.height + 24.0f));
}

void MelonRenderer::renderInteractive(Window& window) {}
