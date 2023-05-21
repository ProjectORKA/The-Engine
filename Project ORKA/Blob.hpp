#pragma once

#include "Game.hpp"
#include "Player.hpp"

struct BlobRenderer : GameRenderer {
	DebugPlayer player;

	InputEvent enter = InputEvent(InputType::Mouse, LMB, true);
	InputEvent exit = InputEvent(InputType::Mouse, RMB, false);
	InputEvent reloadShaders = InputEvent(InputType::KeyBoard, T, false);

	void update(Window& window) override { player.update(window); };

	void inputEvent(Window& window, const InputEvent input) override {
		if (input == enter) window.captureCursor();
		if (input == exit) window.unCaptureCursor();
		if (input == reloadShaders) window.renderer.shaderSystem.rebuild();

		player.inputEvent(window, input);
	};

	void render(ResourceManager& resourceManager, Window& window, TiledRectangle area) override {
		Renderer& r = window.renderer;

		//setup
		r.setCulling(false);
		r.setDepthTest(false);
		r.uniforms().mMatrix(Matrix(1));

		//render scene
		player.camera.renderOnlyRot(r);
		r.uniforms().sunDir(Vec4(normalize(Vec3(0.445776, 0.77546453, 1)), 0));
		r.uniforms().cameraPos(Vec4(player.camera.location, 0));
		r.useTexture(resourceManager, "noise", 1);
		r.useShader(resourceManager, "blob2");
		r.renderMesh(resourceManager, "centeredCube");
	}
};

struct Blob {
	BlobRenderer renderer;
	ResourceManager resourceManager;
	UserInterface ui;

	void run() {
		ui.window("Blob", Area(1920, 1080), true, WindowState::Windowed, renderer, resourceManager);
		ui.run();
	}
};
