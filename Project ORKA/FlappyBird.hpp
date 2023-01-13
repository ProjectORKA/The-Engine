#pragma once

#include "Basics.hpp"
#include "Game.hpp"
#include "Transform.hpp"

namespace FlappyBird {

	struct Bird {
		Transform transform;

		void render(Engine& engine, Window & window) {
			window.renderer.uniforms().mMatrix(matrixFromRotation(PI/2,0,PI/2));
			window.renderer.useShader(engine, "vertexColor");
			window.renderer.renderMesh(engine, "flappyBird");
		}
	};

	struct Pipe {

	};

struct Game : public GameRenderer{
	Bird bird;
	Vector<Pipe> pipes;

	void render(Engine & engine, Window& window, TiledRectangle area) override {
		Renderer& renderer = window.renderer;
		renderer.normalizedSpaceWithAspectRatio(2);

		renderer.clearColor(Color(0.1, 1.0, 0, 1));

		bird.render(engine, window);
	};

	void update(Window& window) {

	};
};

}