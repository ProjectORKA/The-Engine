#pragma once

#include "Game.hpp"

struct MSDFTextRenderer : public GameRenderer {
	void render(ResourceManager& resourceManager, Window& window, TiledRectangle area) override {
		Renderer& renderer = window.renderer;

		renderer.clearColor(Color(0, 0, 0, 1));
		renderer.aspectCorrectNormalizedSpace();

		//renderer.fill(Color(1));
		renderer.useShader(resourceManager, "msdfFont");
		renderer.useTexture(resourceManager, "msdfFont");

		renderer.uniforms().mMatrix(matrixFromLocationAndSize2D(Vec2(0), Vec2(2)));
		renderer.renderMesh(resourceManager, "centeredPlane");
	};
};
