#pragma once

#include "Basics.hpp"
#include "Game.hpp"

namespace FlappyBird {

	struct Bird {
		Transform transform;

		void render(Window & window) {
			window.renderer.uniforms().mMatrix(matrixFromRotation(PI/2,0,PI/2));
			window.renderer.useShader("vertexColor");
			window.renderer.renderMesh("flappyBird");
		}
	};

	struct Pipe {

	};
}

struct FlappyBirdGame : public GameRenderer{
	FlappyBird::Bird bird;
	Vector<FlappyBird::Pipe> pipes;

	void render(Window& window, TiledRectangle area) override {
		Renderer& renderer = window.renderer;
		renderer.normalizedSpaceWithAspectRatio(2);

		renderer.clearColor(Color(0.1, 1.0, 0, 1));

		bird.render(window);
	};

	void update(Window& window) {

	};
};