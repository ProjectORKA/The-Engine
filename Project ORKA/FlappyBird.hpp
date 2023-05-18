#pragma once

#include "Basics.hpp"
#include "Game.hpp"
#include "Transform.hpp"

namespace FlappyBird {
	struct Bird {
		Transform transform;

		void render(ResourceManager& resourceManager, Window& window) {
			window.renderer.uniforms().mMatrix(matrixFromRotation(PI / 2, 0,PI / 2));
			window.renderer.useShader(resourceManager, "vertexColor");
			window.renderer.renderMesh(resourceManager, "flappyBird");
		}
	};

	struct Pipe { };

	struct Game : public GameRenderer {
		Bird bird;
		Vector<Pipe> pipes;

		void render(ResourceManager& resourceManager, Window& window, TiledRectangle area) override {
			Renderer& renderer = window.renderer;
			renderer.normalizedSpaceWithAspectRatio(2);

			renderer.clearColor(Color(0.1, 1.0, 0, 1));

			bird.render(resourceManager, window);
		};

		void update(Window& window) override { };
	};
}
