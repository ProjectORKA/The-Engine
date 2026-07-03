#pragma once

#include "Game.hpp"
#include "GPUTexture.hpp"
#include "Player.hpp"

struct ShadertoyRenderer final : GameRenderer
{
	UInt heightmapSize = 4096;
	Framebuffer bufferA;
	Framebuffer bufferB;
	Framebuffer hdrFramebuffer;
	GPUTexture  starsTexture;
	Float       lastMaxLum = 1.0f;
	DebugPlayer player;
	InputEvent  enter = InputEvent(InputType::Mouse, Lmb, true);
	InputEvent  exit  = InputEvent(InputType::Mouse, Rmb, false);
	InputEvent  reloadShaders = InputEvent(InputType::KeyBoard, T, true);

	void create(Window& window) override;
	void update(Window& window) override;
	void destroy(Window& window) override;
	void renderScene(Window& window) override;
	void connect(GameSimulation& simulation) override;
	void inputEvent(Window& window, InputEvent& input) override;

private:
	void ensureBuffers(Renderer& renderer, const IVec2& heightmapSize, const IVec2& windowSize);
	void ensureDitherTexture(Renderer& renderer);
	void renderHeightmapPass(Renderer& renderer, const IVec2& size);
	void renderDetailPass(Renderer& renderer, const IVec2& size);
	void renderLightingPass(Renderer& renderer, const IVec2& size);
	void updateExposureSample(Renderer& renderer, const IVec2& size);
	void renderToneMapPass(Renderer& renderer, const IVec2& size);
	void updateCommonUniforms(Renderer& renderer, const IVec2& size);
	void updateCameraUniforms(Renderer& renderer) const;
};

struct ShadertoyGame
{
	ShadertoyRenderer renderer;

	void run();
};

