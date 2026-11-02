#include "Pong.hpp"

#include "AudioEngine.hpp"
#include "Debug.hpp"
#include "Random.hpp"
#include "Renderer.hpp"
#include "ResourceManager.hpp"
#include "Settings.hpp"
#include "UI/Authoring/UiAuthoredHost.hpp"
#include "UI/Authoring/UiLogicalWindowCatalog.hpp"
#include "UI/Authoring/UiMarkupSurfaceRegistry.hpp"
#include "UI/Surfaces/UiInteractiveSurface.hpp"
#include "UI/UiHost.hpp"
#include "UI/Theme/UiAppHelpers.hpp"
#include "UI/Theme/UiAppTheme.hpp"
#include "UI/Widgets/UiWidgets.hpp"
#include "UIElement.hpp"
#include "UserInterface.hpp"
#include "Window.hpp"
#include "WindowAPI.hpp"

#include "PongParticleSystem.hpp"

#define PONG_DIFFICULTY_MODIFIER 1.05

Float ballSpeedReduction = 1.5;
Float paddleSpeed        = 2.5;

constexpr Vec4 pongWhite(1.0f, 1.0f, 1.0f, 1.0f);
constexpr Vec4 pongGray(0.62f, 0.62f, 0.62f, 1.0f);
constexpr Float pongPaddleHalfWidth = 0.1f;

struct Ball;

struct PongPlayer
{
	UInt score     = 0;
	Vec2 position  = Vec2(0, 0);
	Vec2 direction = Vec2(1, 0);
	Bool shoot     = false;
	Float target     = 0.0f;
	Float velocity   = 0.0f;
	Float horizontalSpeed = 0.0f;
	Float previousPositionX = 0.0f;
	Float difficulty = 7.0f;

	void update(Window& window);
	void updateHorizontalSpeed(Float deltaTime);
	void mouseInput(Float worldX);
	void ballPositionInput(Vector<Ball>& balls);
	void aiInput(Vector<Ball>& balls, Float deltaTime);
};

struct Ball
{
	Float desiredSpeed   = 2;
	Bool  stuckToPaddle  = true;
	Bool  stuckToPaddle1 = true;
	Vec2  position       = Vec2(0);
	Vec2  velocity       = Vec2(0, 1);

	static constexpr Int trailCapacity = 40;
	Vec2 trail[trailCapacity]{};
	Int  trailCount = 0;
	Int  trailHead  = 0;

	void clearTrail();
	void recordTrail(Vec2 from, Vec2 to);
	void update(Float deltaTime, PongPlayer players[2], PongParticleSystem* collisionParticles = nullptr);
	void render(Renderer& renderer) const;
};

Ball* getClosestBall(const PongPlayer& player, Vector<Ball>& balls);

namespace
{
	using namespace Ui;

	constexpr const Char* pongScoreSoundId  = "pong_score";
	constexpr const Char* pongScoreWav      = "score.wav";
	constexpr const Char* pongBounceSoundId = "pong_bounce";
	constexpr const Char* pongBounceWav     = "bounce.wav";
	constexpr const Char* pongShootSoundId  = "pong_shoot";
	constexpr const Char* pongShootWav      = "shoot.wav";

	inline void playPongScoreSound()
	{
		if (audioEngine.has(pongScoreSoundId)) audioEngine.play(pongScoreSoundId);
		else beep();
	}

	inline void playPongBounceSound()
	{
		if (audioEngine.has(pongBounceSoundId)) audioEngine.play(pongBounceSoundId);
		else beep();
	}

	inline void playPongShootSound()
	{
		if (audioEngine.has(pongShootSoundId)) audioEngine.play(pongShootSoundId);
		else beep();
	}

	inline void loadPongSounds()
	{
		const Path dataSounds = resourceManager.getOrkaDataPath() / "Sounds";

		audioEngine.load(pongScoreSoundId, dataSounds / pongScoreWav, false);
		audioEngine.load(pongBounceSoundId, dataSounds / pongBounceWav, false);
		audioEngine.load(pongShootSoundId, dataSounds / pongShootWav, false);
	}

	void publishPongState(UiPropertyRegistry& properties, const PongPlayer players[2], const Bool paused)
	{
		(void)properties.set("state.pong.score.p1", UiValue::fromString(toString(players[0].score)));
		(void)properties.set("state.pong.score.p2", UiValue::fromString(toString(players[1].score)));
		(void)properties.set("state.pong.aiDifficulty",
			UiValue::fromString("AI difficulty " + toString(players[1].difficulty)));
		(void)properties.set("state.pong.paused", UiValue::fromBool(paused));
	}

	// paused is written from the GLFW thread (Esc, Resume) and read on the window thread.
	struct PongPauseFlag
	{
		std::atomic<Bool> value{ false };

		[[nodiscard]] Bool load() const
		{
			return value.load(std::memory_order_acquire);
		}

		void store(const Bool paused)
		{
			value.store(paused, std::memory_order_release);
		}

		// Returns true when this call enters the paused state.
		Bool toggle()
		{
			Bool wasPaused = value.load(std::memory_order_acquire);
			while (!value.compare_exchange_weak(
				wasPaused, !wasPaused,
				std::memory_order_acq_rel, std::memory_order_acquire))
			{
			}
			return !wasPaused;
		}
	};

	// Pure black fills, white text and icons, gray borders. Highlight alpha stays 0 so
	// stacked surfaces do not pick up the app theme's gray overlays.
	UiAppTheme pongMonochromeTheme()
	{
		UiAppTheme theme = UiAppTheme::dark();
		const UiLinearColor black = UiLinearColor::fromStraightSrgb(Vec4(0, 0, 0, 1));
		const UiLinearColor white = UiLinearColor::fromStraightSrgb(pongWhite);
		const UiLinearColor gray  = UiLinearColor::fromStraightSrgb(pongGray);
		const UiLinearColor clear = UiLinearColor::fromStraightSrgb(Vec4(0, 0, 0, 0));
		theme.colors.surfaceRoot = black;
		theme.colors.surfaceHighlight = clear;
		theme.colors.surfacePanel = black;
		theme.colors.surfaceControl = black;
		theme.colors.surfaceControlHover = black;
		theme.colors.surfaceControlActive = black;
		theme.colors.surfaceListItem = black;
		theme.colors.surfaceListItemHover = black;
		theme.colors.surfaceListItemActive = black;
		theme.colors.surfaceBadge = black;
		theme.colors.surfaceMenu = black;
		theme.colors.surfaceMenuHover = black;
		theme.colors.surfaceMenuSelected = black;
		theme.colors.surfaceTrack = black;
		theme.colors.surfaceTrackFill = white;
		theme.colors.border = gray;
		theme.colors.borderHover = gray;
		theme.colors.borderFocus = gray;
		theme.colors.shadow = clear;
		theme.colors.accent = white;
		theme.colors.accentFocus = clear;
		theme.colors.toggleOn = white;
		theme.colors.toggleOff = black;
		theme.colors.thumb = white;
		theme.colors.textPrimary = white;
		theme.colors.textSecondary = white;
		theme.colors.textMuted = white;
		theme.colors.textInverse = black;
		theme.colors.textAccent = white;
		theme.colors.iconPrimary = white;
		theme.colors.iconMuted = white;
		theme.colors.iconOnAccent = black;
		return theme;
	}

	Bool nodeIsOrInsideGameSurface(const UiDocument& document, UiNodeHandle candidate, UiNodeHandle gameSurface)
	{
		while (document.isValid(candidate))
		{
			if (candidate == gameSurface) return true;
			const UiNode* node = document.get(candidate);
			if (!node) break;
			candidate = node->parentHandle();
		}
		return false;
	}

	Bool pointerTargetsGameSurface(
		const UiDocument& document,
		const UiEventRouter& router,
		const DVec2 pointer,
		const UiNodeHandle gameSurface)
	{
		if (!document.isValid(gameSurface)) return false;
		const UiNodeHandle hovered = router.hitTest(pointer);
		if (nodeIsOrInsideGameSurface(document, hovered, gameSurface)) return true;
		const UiNodeHandle focused = router.focusedNode();
		return nodeIsOrInsideGameSurface(document, focused, gameSurface);
	}

	// aspectCorrectNormalizedSpace keeps the court square. On a wide view, X runs
	// past ±1, so pointer X has to go through that same mapping.
	Float pongPointerToWorldX(const DVec2 pointer01, const IVec2 viewportSize)
	{
		if (viewportSize.x <= 0 || viewportSize.y <= 0) return 0;
		const Float aspect = static_cast<Float>(viewportSize.x) / static_cast<Float>(viewportSize.y);
		if (aspect >= 1.0f)
			return static_cast<Float>(pointer01.x * 2.0 * static_cast<Double>(aspect) - aspect);
		return static_cast<Float>(pointer01.x * 2.0 - 1.0);
	}

	struct PongGame
	{
		Vector<Ball>       balls;
		PongPlayer         players[2];
		PongParticleSystem collisionParticles = PongParticleSystem(256);
		UInt               ballCount          = 1;
		PongPauseFlag      paused;
		InputId      shootButton1secondary = InputId(InputType::Mouse, 0);
		InputId      moveLeftButton        = InputId(InputType::KeyBoard, A);
		InputId      moveRightButton       = InputId(InputType::KeyBoard, D);
		InputId      moveLeftArrow         = InputId(InputType::KeyBoard, Left);
		InputId      moveRightArrow        = InputId(InputType::KeyBoard, Right);
		InputId      shootButton1          = InputId(InputType::KeyBoard, Space);

		void resetScores()
		{
			players[0].score = 0;
			players[1].score = 0;
			players[1].difficulty = 7.0f;
			balls.clear();
			while (balls.size() < ballCount) balls.emplace_back();
		}

		void create()
		{
			resetScores();
			loadPongSounds();
		}

		void update(Window& window)
		{
			if (paused.load()) return;
			const Float delta = window.renderer.deltaTime();
			if (window.pressed(moveLeftButton) || window.pressed(moveLeftArrow))
				players[0].position.x -= paddleSpeed * delta;
			if (window.pressed(moveRightButton) || window.pressed(moveRightArrow))
				players[0].position.x += paddleSpeed * delta;
			players[0].shoot = window.pressed(shootButton1) || window.pressed(shootButton1secondary);

			for (PongPlayer& player : players) player.update(window);
		}

		void simulateAndRender(
			Window& window,
			const IVec2 viewportPosition,
			const IVec2 viewportSize,
			const DVec2 mouseLocalBottomLeft,
			const Bool useMouse)
		{
			if (viewportSize.x <= 0 || viewportSize.y <= 0) return;

			Renderer& r = window.renderer;
			r.setRenderRegion(viewportPosition, viewportSize);

			r.setCulling(true);
			r.setDepthTest(true);

			r.clearBackground(Color(0, 0, 0, 1));
			r.aspectCorrectNormalizedSpace();

			players[0].position.y = -0.9f;
			players[1].position.y = +0.9f;

			if (!paused.load())
			{
				if (useMouse)
				{
					const DVec2 normalized(
						mouseLocalBottomLeft.x / static_cast<Double>(viewportSize.x),
						mouseLocalBottomLeft.y / static_cast<Double>(viewportSize.y));
					players[0].mouseInput(pongPointerToWorldX(normalized, viewportSize));
				}

				players[1].aiInput(balls, r.time.getDelta());
				for (PongPlayer& player : players) player.updateHorizontalSpeed(r.time.getDelta());

				for (Ball& ball : balls)
					ball.update(r.time.getDelta(), players, &collisionParticles);

				collisionParticles.update(r.time.getDelta());
			}

			r.aspectCorrectNormalizedSpace();
			r.useShader("color");
			r.uniforms().setCustomColor(pongWhite);

			r.uniforms().setMMatrix(scale(translate(Matrix(1), Vec3(players[0].position, 0)), Vec3(0.2, 0.01, 0.5)));
			r.renderMesh("centeredCube");
			r.uniforms().setMMatrix(scale(translate(Matrix(1), Vec3(players[1].position, 0)), Vec3(0.2, 0.01, 0.5)));
			r.renderMesh("centeredCube");
			for (Ball& ball : balls) ball.render(r);

			r.uniforms().setCustomColor(pongGray);
			r.setAlphaBlending(true);
			collisionParticles.render(r);
			r.setAlphaBlending(false);

			r.uniforms().setCustomColor(pongGray);
			r.uniforms().setMMatrix(scale(translate(Matrix(1), Vec3(0, 1, 0)), Vec3(2, 0.01, 0.5)));
			r.renderMesh("centeredCube");
			r.uniforms().setMMatrix(scale(translate(Matrix(1), Vec3(0, -1, 0)), Vec3(2, 0.01, 0.5)));
			r.renderMesh("centeredCube");
			r.uniforms().setMMatrix(scale(translate(Matrix(1), Vec3(1, 0, 0)), Vec3(0.01, 2, 0.5)));
			r.renderMesh("centeredCube");
			r.uniforms().setMMatrix(scale(translate(Matrix(1), Vec3(-1, 0, 0)), Vec3(0.01, 2, 0.5)));
			r.renderMesh("centeredCube");

			r.setRenderRegion(IVec2(0), window.getSize());
			r.uiSpace(window);
			r.setDepthTest(false);
			r.setCulling(false);
		}
	};

	struct PongSurface final : UIElement
	{
		UiAppTheme theme = pongMonochromeTheme();
		UiHost host;
		UniquePointer<UiAuthoredHost> authored;
		PongGame game;
		UiWidgetHandle gameViewport;
		UiNodeHandle gameSurfaceNode;
		Bool authoredReady = false;
		Window* owningWindow = nullptr;
		InputEvent captureCursor = InputEvent(InputType::Mouse, Lmb, true);
		InputEvent releaseCursor = InputEvent(InputType::Mouse, Rmb, false);

		PongSurface()
		{
			addElement(host);
			growX(true);
			growY(true);
			background(Color(0, 0, 0, 0));
		}

		[[nodiscard]] Bool ownsCursorCapture() const
		{
			return ui.capturing.load() && ui.focusedElement == this;
		}

		void refreshBoundState()
		{
			if (!authored) return;
			publishPongState(authored->properties(), game.players, game.paused.load());
			authored->syncFromProperties();
		}

		void wireInteractiveSurface()
		{
			if (!authored) return;
			gameViewport = authored->widgetBridge().findById("pong-game");
			gameSurfaceNode = authored->widgets().node(gameViewport);
			authored->widgets().setInteractiveSurfaceRender(gameViewport,
				[this](const UiInteractiveSurfaceContext& context)
				{
					if (!owningWindow) return;
					const UiInteractiveSurfaceGlViewport viewport =
						interactiveSurfaceGlViewport(context, owningWindow->getHeight());
					if (viewport.size.x <= 0 || viewport.size.y <= 0) return;

					const DVec2 pointer = owningWindow->frameMousePositionTopLeft();
					const Bool captured = ownsCursorCapture();
					Bool useMouse = captured || interactiveSurfaceContainsPointer(context, pointer);
					if (!useMouse)
					{
						useMouse = pointerTargetsGameSurface(
							host.document(), host.eventRouter(), pointer, gameSurfaceNode);
					}
					DVec2 mouseLocal = DVec2(0);
					if (useMouse)
					{
						mouseLocal = interactiveSurfacePointerLocalBottomLeft(context, pointer);
						if (captured)
						{
							mouseLocal.x = clamp(mouseLocal.x, 0.0, static_cast<Double>(context.size.x));
							mouseLocal.y = clamp(mouseLocal.y, 0.0, static_cast<Double>(context.size.y));
						}
					}

					game.simulateAndRender(
						*owningWindow, viewport.position, viewport.size, mouseLocal, useMouse);
				});
		}

		void create(Window& window) override
		{
			owningWindow = &window;
			const IVec2 size = window.getSize();
			setWidth(size.x);
			setHeight(size.y);
			host.setWidth(size.x);
			host.setHeight(size.y);

			host.useAppTheme(theme);
			UIElement::create(window);
			installHotkeys(window);
			game.create();

			authored = std::make_unique<UiAuthoredHost>(host);

			(void)authored->commands().registerCommand("Pong.Reset", {},
				[this](const Vector<UiValue>&)
				{
					game.resetScores();
					refreshBoundState();
					return true;
				});
			(void)authored->commands().registerCommand("Pong.Resume", {},
				[this](const Vector<UiValue>&)
				{
					game.paused.store(false);
					return true;
				});
			(void)authored->commands().registerCommand("Pong.Exit", {},
				[this](const Vector<UiValue>&)
				{
					if (ownsCursorCapture()) ui.releaseCapture();
					if (!owningWindow) return false;
					owningWindow->keepRunning = false;
					apiSetWindowShouldClose(owningWindow->apiWindow, true);
					return true;
				});

			const Path uiDirectory = resourceManager.getOrkaDataPath() / "UI";
			if (!authored->loadFromFiles(uiDirectory / "pong.ui.html", uiDirectory / "pong.ui.scss"))
			{
				logError("Pong: failed to load authored UI bundle.");
				for (const UiDiagnostic& diagnostic : authored->diagnostics())
					logError("  " + diagnostic.message);
				return;
			}

			if (!authored->mount())
			{
				logError("Pong: failed to mount authored UI.");
				for (const UiDiagnostic& diagnostic : authored->diagnostics())
					logError("  " + diagnostic.message);
				return;
			}

			authored->ensureNodeLayout("pong-root", [](UiLayoutStyle& style)
				{
					style.width = UiSizeSpec::percent(1);
					style.height = UiSizeSpec::percent(1);
				});
			authored->ensureNodeLayout("pong-shell", [](UiLayoutStyle& style)
				{
					style.width = UiSizeSpec::percent(1);
					style.height = UiSizeSpec::percent(1);
				});
			authored->ensureNodeLayout("pong-playfield", [](UiLayoutStyle& style)
				{
					style.flexGrow = 1;
					style.flexShrink = 1;
				});
			authored->ensureNodeLayout("pong-game", [](UiLayoutStyle& style)
				{
					style.flexGrow = 1;
					style.flexShrink = 1;
				});

			(void)UiMarkupSurfaceRegistry::global();
			wireInteractiveSurface();
			authoredReady = true;
			refreshBoundState();
			if (authored->bundle())
				syncPrimaryOsWindowTitle(window, authored->bundle()->blueprint);
		}

		void installHotkeys(Window& window)
		{
			// Monitor changes have to happen on the GLFW thread. inputEvent
			// consumes these presses so Window does not apply them again.
			const Function<void(Int, Int, Int, Int)> uiKeys = window.keyInputCallback;
			window.keyInputCallback =
				[uiKeys, this](const Int key, const Int scanCode, const Int action, const Int modifiers)
				{
					if (uiKeys) uiKeys(key, scanCode, action, modifiers);
					if (action != GLFW_PRESS || !owningWindow) return;
					if (key == Esc)
					{
						const Bool nowPaused = game.paused.toggle();
						if (nowPaused && ownsCursorCapture()) ui.releaseCapture();
					}
					if ((modifiers & GLFW_MOD_ALT) == 0) return;
					if (key != Enter && key != NpEnter) return;

					if (owningWindow->windowState == WindowState::Windowed)
						owningWindow->windowState = WindowState::Fullscreen;
					else owningWindow->windowState = WindowState::Windowed;
					owningWindow->updateWindowState();
					if (ui.capturing && ui.focusedWindow == owningWindow)
						apiWindowDisableCursor(owningWindow->apiWindow);
				};
		}

		void update(Window& window) override
		{
			const IVec2 size = window.getSize();
			setWidth(size.x);
			setHeight(size.y);
			host.setWidth(size.x);
			host.setHeight(size.y);

			game.update(window);
			if (authoredReady)
			{
				refreshBoundState();
				authored->syncControlLayouts();
			}
			host.update(window);
		}

		void inputEvent(Window& window, InputEvent& input) override
		{
			// The key callback already paused or toggled fullscreen. routeInput
			// does not reach the nested UiHost unless that host is focused, and
			// cursor capture skips it entirely, so Window would handle the same
			// press again and fullscreen would toggle straight back off.
			if (input.state && input.id.type == InputType::KeyBoard)
			{
				const Bool altEnter = window.pressed(window.altKey) &&
					(input.id.id == Enter || input.id.id == NpEnter);
				if (input.id.id == Esc || altEnter) input.consume();
			}

			// While capturing, routeInput delivers mouse here (UiHost is bypassed).
			if (!game.paused.load() && input == captureCursor && !ownsCursorCapture() &&
				pointerTargetsGameSurface(
					host.document(), host.eventRouter(), window.mousePosition(), gameSurfaceNode))
			{
				ui.capture(*this);
			}
			if (input == releaseCursor && ownsCursorCapture()) ui.releaseCapture();
		}

		void destroy(Window& window) override
		{
			if (ownsCursorCapture()) ui.releaseCapture();
			authored.reset();
			authoredReady = false;
			host.destroy(window);
			owningWindow = nullptr;
		}
	};
}

void PongPlayer::update(Window& window)
{
	(void)window;
	position.x = clamp(position.x, -0.9f, 0.9f);
}

void PongPlayer::updateHorizontalSpeed(const Float deltaTime)
{
	if (deltaTime > 0.0001f)
		horizontalSpeed = (position.x - previousPositionX) / deltaTime;
	else horizontalSpeed = 0.0f;

	previousPositionX = position.x;
}

void PongPlayer::ballPositionInput(Vector<Ball>& balls)
{
	const Ball* ball = getClosestBall(*this, balls);
	if (ball) position.x = clamp(ball->position.x, -0.9f, 0.9f);
}

void PongPlayer::mouseInput(const Float worldX)
{
	position.x = clamp(worldX, -0.9f, 0.9f);
}

void Ball::clearTrail()
{
	trailCount = 0;
	trailHead = 0;
}

void Ball::recordTrail(const Vec2 from, const Vec2 to)
{
	const Vec2 delta = to - from;
	const Float distance = length(delta);
	if (distance < 0.0001f) return;

	constexpr Float spacing = 0.012f;
	Int steps = static_cast<Int>(distance / spacing);
	if (steps < 1) steps = 1;
	if (steps > 12) steps = 12;

	for (Int step = 1; step <= steps; ++step)
	{
		const Float t = static_cast<Float>(step) / static_cast<Float>(steps);
		trail[trailHead] = from + delta * t;
		trailHead = (trailHead + 1) % trailCapacity;
		if (trailCount < trailCapacity) ++trailCount;
	}
}

void Ball::update(const Float deltaTime, PongPlayer players[2], PongParticleSystem* collisionParticles)
{
	auto spawnImpactParticles = [&](const Vec2& impactPosition, const Vec2& incomingVelocity)
	{
		if (!collisionParticles) return;

		constexpr Int particlesPerImpact = 18;
		for (Int i = 0; i < particlesPerImpact; i++) collisionParticles->spawn(impactPosition, incomingVelocity);
	};

	auto bounceOffPaddle = [&](const PongPlayer& paddle, const Float verticalDirection)
	{
		const Float hitOffset = clamp(
			(position.x - paddle.position.x) / pongPaddleHalfWidth, -1.0f, 1.0f);
		const Float paddleMotion = clamp(paddle.horizontalSpeed / paddleSpeed, -1.0f, 1.0f);

		// Edge hits establish the main angle, while paddle movement adds spin in
		// the direction of travel. Keeping Y at one prevents flat, endless rallies.
		const Float horizontalDirection = clamp(
			hitOffset * 1.25f + paddleMotion * 0.75f, -1.8f, 1.8f);
		velocity = normalize(Vec2(horizontalDirection, verticalDirection)) * desiredSpeed;
	};

	const Vec2 previous = position;
	const Bool wasStuck = stuckToPaddle;

	if (stuckToPaddle)
	{
		if (stuckToPaddle1)
		{
			position.y = -0.85f;
			position.x = players[0].position.x;
			if (players[0].shoot)
			{
				stuckToPaddle = false;
				velocity.y    = 1;
				velocity.x    = randomFloatFast(-1.0f, 1.0f);
				desiredSpeed  = 2;
				playPongShootSound();
			}
		}
		else
		{
			position.y = 0.85f;
			position.x = players[1].position.x;
			if (players[1].shoot)
			{
				stuckToPaddle = false;
				velocity.y    = -1;
				velocity.x    = randomFloatFast(-1.0f, 1.0f);
				desiredSpeed  = 2;
				playPongShootSound();
			}
		}
	}
	else
	{
		desiredSpeed += deltaTime / 45.0f;

		velocity = normalize(velocity) * desiredSpeed;
		position += velocity * deltaTime;
		if (velocity.y > 0)
		{
			if (position.y > 1)
			{
				players[0].score++;
				playPongScoreSound();
				stuckToPaddle  = true;
				stuckToPaddle1 = true;
				players[1].difficulty *= PONG_DIFFICULTY_MODIFIER;
			}
			else if (position.y > 0.895 && position.y < 0.95 &&
				abs(position.x - players[1].position.x) < pongPaddleHalfWidth)
			{
				spawnImpactParticles(position, velocity);
				bounceOffPaddle(players[1], -1.0f);
				playPongBounceSound();
			}
		}
		else
		{
			if (position.y < -1)
			{
				players[1].score++;
				playPongScoreSound();
				stuckToPaddle  = true;
				stuckToPaddle1 = false;
				players[1].difficulty /= PONG_DIFFICULTY_MODIFIER;
			}
			else if (position.y < -0.895 && position.y > -0.95 &&
				abs(position.x - players[0].position.x) < pongPaddleHalfWidth)
			{
				spawnImpactParticles(position, velocity);
				bounceOffPaddle(players[0], 1.0f);
				playPongBounceSound();
			}
		}
	}

	if (position.x > +1 && velocity.x > 0)
	{
		spawnImpactParticles(position, velocity);
		velocity.x = -velocity.x;
		playPongBounceSound();
	}
	if (position.x < -1 && velocity.x < 0)
	{
		spawnImpactParticles(position, velocity);
		velocity.x = -velocity.x;
		playPongBounceSound();
	}

	if (stuckToPaddle) clearTrail();
	else if (!wasStuck) recordTrail(previous, position);
}

Ball* getClosestBall(const PongPlayer& player, Vector<Ball>& balls)
{
	Ball* closestBall = nullptr;

	Float closestY = 100;
	for (Ball& ball : balls)
	{
		const Float delta = ball.position.y - player.position.y;
		if ((delta >= 0 && ball.velocity.y <= 0) || (delta <= 0 && ball.velocity.y >= 0))
		{
			const Float distance = abs(delta);
			if (distance < closestY)
			{
				closestY    = distance;
				closestBall = &ball;
			}
		}
	}

	return closestBall;
}

void PongPlayer::aiInput(Vector<Ball>& balls, const Float deltaTime)
{
	shoot = randomIntFast(1000) < 1;
	const Ball* ball = getClosestBall(*this, balls);
	if (ball)
	{
		const Float reactionTime = deltaTime * difficulty;

		Float predictedPosition = ball->position.x;
		if (abs(ball->velocity.y) > 0.001f)
			predictedPosition = ball->position.x + (position.y - ball->position.y) / ball->velocity.y * ball->velocity.x;

		target = (target + reactionTime * ((predictedPosition + ball->position.x) / 2)) / (1 + reactionTime);

		const Float targetVelocity = target - position.x;

		velocity += targetVelocity - velocity;

		position.x += velocity;

		position.x = clamp(position.x, -0.9f, 0.9f);
	}
}

void Ball::render(Renderer& renderer) const
{
	renderer.setDepthTest(false);
	if (trailCount > 0)
	{
		renderer.setAlphaBlending(true);
		const Int oldest = trailCount < trailCapacity ? 0 : trailHead;
		for (Int i = 0; i < trailCount; ++i)
		{
			const Vec2 sample = trail[(oldest + i) % trailCapacity];
			const Float along = trailCount == 1 ? 1.0f :
				static_cast<Float>(i) / static_cast<Float>(trailCount - 1);
			const Float fade = along * along;
			renderer.uniforms().setCustomColor(Vec4(1.0f, 1.0f, 1.0f, fade * 0.85f));
			const Float size = 0.004f + 0.006f * fade;
			renderer.uniforms().setMMatrix(scale(translate(Matrix(1), Vec3(sample, 0)), Vec3(size, size, 0.5f)));
			renderer.renderMesh("centeredCube");
		}
		renderer.setAlphaBlending(false);
	}

	renderer.uniforms().setCustomColor(pongWhite);
	renderer.uniforms().setMMatrix(scale(translate(Matrix(1), Vec3(position, 0)), Vec3(0.01f, 0.01f, 0.5f)));
	renderer.renderMesh("centeredCube");
	renderer.setDepthTest(true);
}

void Pong::run()
{
	PongSurface surface;
	Window& window = ui.window("Pong", settings.defaultWindowWidth, settings.defaultWindowHeight, true, true, WindowState::Windowed);
	window.addElement(surface);
	ui.run();
}
