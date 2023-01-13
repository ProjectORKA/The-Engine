#include "GPUSim.hpp"


inline void GPUSimRenderer::render(Engine & engine, Window& window, TiledRectangle area) {
	Renderer& renderer = window.renderer;

	if (!initialized) {
		renderer.framebufferSystem.framebuffers.emplaceBack();
		renderer.framebufferSystem.framebuffers.last().create(Area(512));
		renderer.framebufferSystem.framebuffers.last().add(1, DataType::dataTypeFloat, 0);
		factionsID2 = renderer.framebufferSystem.framebuffers.size() - 1;

		renderer.framebufferSystem.framebuffers.last().draw();

		renderer.clearColor(Color(1));
		renderer.renderRegion.set(TiledRectangle(50, 50, 50, 50));
		renderer.useShader(engine, "color");
		renderer.fill(Color(0));
		renderer.renderMesh(engine, "fullScreenQuad");

		renderer.framebufferSystem.framebuffers.emplaceBack();
		renderer.framebufferSystem.framebuffers.last().create(Area(512));
		renderer.framebufferSystem.framebuffers.last().add(1, DataType::dataTypeFloat, 0);
		factionsID1 = renderer.framebufferSystem.framebuffers.size() - 1;

		renderer.framebufferSystem.framebuffers.last().draw();

		renderer.clearColor(Color(1));
		renderer.renderMesh(engine, "fullScreenQuad");

		initialized = true;
	}

	//renderer.normalizedSpace();
	//renderer.framebufferSystem.framebuffers[factionsID1].setAsTexture(0);
	//renderer.framebufferSystem.framebuffers[factionsID2].draw();
	//renderer.useShader("gpuSimShader");
	//renderer.renderMesh("fullScreenQuad");

	//render on screen
	renderer.framebufferSystem.framebuffers[factionsID1].setAsTexture(0);
	renderer.framebufferSystem.draw(renderer, 0);
	renderer.aspectCorrectNormalizedSpace();
	//renderer.clearColor(Color(1));
	renderer.useShader(engine, "final");
	renderer.renderMesh(engine, "fullScreenQuad");


	//swap buffers
	//UInt tmp = factionsID1;
	//factionsID1 = factionsID2;
	//factionsID2 = tmp;
}
