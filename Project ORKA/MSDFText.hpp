
#pragma once

#include "Game.hpp"

struct MSDFTextRenderer : public GameRenderer{
	
	
	void render(Engine & engine, Window& window, TiledRectangle area) {
		Renderer& renderer = window.renderer;
		
		renderer.clearColor(Color(0,0,0,1));
		renderer.aspectCorrectNormalizedSpace();

		//renderer.fill(Color(1));
		renderer.useShader(engine,"msdfFont");
		renderer.useTexture(engine, "msdfFont");

		renderer.uniforms().mMatrix(matrixFromLocationAndSize2D(Vec2(0), Vec2(2)));
		renderer.renderMesh(engine, "centeredPlane");
	};
};