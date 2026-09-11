#pragma once
#include "Framebuffer.hpp"
#include "Game.hpp"
#include "GraphicsAPI.hpp"
#include "Player.hpp"

enum class SdfPointcloudMode : UInt
{
	FullSdf    = 0,
	Pointcloud = 1
};

struct SdfPointcloudRenderer final : GameRenderer
{
	DebugPlayer             player;
	Framebuffer             sdfFramebuffer;
	OpenGLBuffer            pointCloudBuffer;
	OpenGLVertexArrayObject emptyVao;
	IVec2                   cloudSize        = IVec2(0);
	Vec3                    lastCameraPos    = Vec3(0);
	DVec3                   lastCameraRot    = DVec3(0);
	Float                   lastFov          = 0.0f;
	Bool                    forceFullRebuild = true;
	Bool                    resourcesCreated = false;
	Bool                    sdfReady         = false;
	SdfPointcloudMode       renderMode       = SdfPointcloudMode::Pointcloud;
	Bool                    worldDistortion  = true;
	Bool                    splatDistortion  = true;
	Bool                    showStepCount    = false;
	Bool                    showRecalcPixels = false;

	InputEvent enter           = InputEvent(InputType::Mouse, Lmb, true);
	InputEvent exit            = InputEvent(InputType::Mouse, Rmb, false);
	InputEvent reloadShaders   = InputEvent(InputType::KeyBoard, T, false);
	InputEvent switchToFullSdf = InputEvent(InputType::KeyBoard, One, true);
	InputEvent switchToPointcloud = InputEvent(InputType::KeyBoard, Two, true);
	InputEvent stepCountToggle = InputEvent(InputType::KeyBoard, Three, true);
	InputEvent recalcPixelsToggle = InputEvent(InputType::KeyBoard, Four, true);
	InputEvent splatDistortionToggle = InputEvent(InputType::KeyBoard, Five, true);
	InputEvent worldDistortionToggle = InputEvent(InputType::KeyBoard, K, true);

	void update(Window& window) override;
	void destroy(Window& window) override;
	void connect(GameSimulation& simulation) override;
	void inputEvent(Window& window, InputEvent& input) override;
	void create(Window& window) override;
	void renderScene(Window& window) override;
	void renderInteractive(Window& window) override;

private:
	void ensureResources(Renderer& renderer, IVec2 size);
	[[nodiscard]] Bool cameraChanged() const;
	void               storeCameraState();
	void               uploadCommonUniforms(Renderer& renderer, IVec2 size);
	void               runHoleFillCompute(Renderer& renderer, IVec2 size);
	void               renderFullSdfPass(Renderer& renderer, IVec2 size);
	void               renderPointcloudPass(Renderer& renderer, IVec2 size);
	void               blitSdfToGameFramebuffer(Renderer& renderer) const;
	void               drawOverlay(Renderer& renderer) const;
};

struct SdfPointcloud
{
	SdfPointcloudRenderer renderer;

	void run()
	{
		ui.window("SDF Pointcloud", settings.defaultWindowWidth, settings.defaultWindowHeight, true, true, WindowState::Windowed, renderer);
		ui.run();
	}
};
