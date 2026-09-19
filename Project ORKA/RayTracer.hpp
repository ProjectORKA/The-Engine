#pragma once

#include "Game.hpp"
#include "Player.hpp"

struct RayTracerRenderer final : GameRenderer
{
	Framebuffer accumFramebuffer;
	DebugPlayer player;

	UInt   frameSamples     = 0;
	Vec3   lastCameraPosition;
	DVec3  lastCameraRotation;
	Bool   resetAccumulation = true;
	Bool   showOverlay       = true;
	UInt   accumulationEpoch = 1;
	String tracerStatus      = "Initializing GPU path tracer...";

	Float verticalFov        = 40.0f;
	Bool  enableDepthOfField = false;
	Float lensRadius         = 0.05f;
	Float focusDistance      = 3.5f;

	InputEvent captureMouse = InputEvent(InputType::Mouse, Lmb, true);
	InputEvent releaseMouse = InputEvent(InputType::Mouse, Rmb, false);
	InputEvent resetSamples = InputEvent(InputType::KeyBoard, R, true);
	InputEvent reloadShaders  = InputEvent(InputType::KeyBoard, T, true);
	InputEvent toggleOverlay  = InputEvent(InputType::KeyBoard, H, true);

	void create(Window& window) override;
	void update(Window& window) override;
	void destroy(Window& window) override;
	void renderScene(Window& window) override;
	void renderInteractive(Window& window) override;
	void connect(GameSimulation& simulation) override;
	void inputEvent(Window& window, InputEvent& input) override;

private:
	void ensureBuffers(Renderer& renderer, const IVec2& size);
	void updateCameraUniforms(Renderer& renderer) const;
	void markSceneDirty();
	void renderTracePass(Renderer& renderer, const IVec2& size);
	void renderDisplayPass(Renderer& renderer, const IVec2& size);
	void drawOverlay(Renderer& renderer);
};

struct RayTracerGame
{
	RayTracerRenderer renderer;

	void run();
};
