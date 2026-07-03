#pragma once

#include "Game.hpp"
#include "GPUTexture.hpp"
#include "Player.hpp"

struct RayTracerRenderer final : GameRenderer
{
	Name       textureName = "RayTracerTexture";
	GPUTexture displayTexture;
	DebugPlayer player;

	Vector<Vec3> colorAccumulation;
	Vector<UInt> sampleCounts;
	IVec2        renderSize = IVec2(0);
	UInt         frameSamples;
	Bool         sceneDirty = true;

	Int maxPathDepth      = 12;
	Int samplesPerFrame   = 1;
	Bool enableDepthOfField = false;
	Float lensRadius      = 0.05f;
	Float focusDistance   = 3.5f;
	Float verticalFov     = 40.0f;

	InputEvent captureMouse = InputEvent(InputType::Mouse, Lmb, true);
	InputEvent releaseMouse = InputEvent(InputType::Mouse, Rmb, false);
	InputEvent resetSamples = InputEvent(InputType::KeyBoard, R, true);
	InputEvent toggleDof    = InputEvent(InputType::KeyBoard, D, true);

	void create(Window& window) override;
	void update(Window& window) override;
	void destroy(Window& window) override;
	void renderScene(Window& window) override;
	void renderInteractive(Window& window) override;
	void connect(GameSimulation& simulation) override;
	void inputEvent(Window& window, InputEvent& input) override;

private:
	void resetAccumulation(const IVec2& size);
	void markSceneDirty();
	void traceFrame(const IVec2& size, UInt seed);
	void uploadBuffer(Renderer& renderer, const IVec2& size);
	void drawOverlay(Renderer& renderer);
};

struct RayTracerGame
{
	RayTracerRenderer renderer;

	void run();
};
