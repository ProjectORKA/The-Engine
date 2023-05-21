#include "GPUSim.hpp"

inline void GPUSimRenderer::render(ResourceManager& resourceManager, Window& window, TiledRectangle area)
{
	Renderer& renderer = window.renderer;

	if(! initialized)
	{
		renderer.framebufferSystem.framebuffers.emplace_back();
		renderer.framebufferSystem.framebuffers.back().create("GPUSimTexture1", Area(512));
		renderer.framebufferSystem.framebuffers.back().add(1, DataType::dataTypeFloat, 0);
		factionsId2 = renderer.framebufferSystem.framebuffers.size() - 1;

		renderer.framebufferSystem.framebuffers.back().draw();

		renderer.clearColor(Color(1));
		renderer.renderRegion.set(TiledRectangle(50, 50, 50, 50));
		renderer.useShader(resourceManager, "color");
		renderer.fill(Color(0));
		renderer.renderMesh(resourceManager, "fullScreenQuad");

		renderer.framebufferSystem.framebuffers.emplace_back();
		renderer.framebufferSystem.framebuffers.back().create("GPUSimTexture2", Area(512));
		renderer.framebufferSystem.framebuffers.back().add(1, DataType::dataTypeFloat, 0);
		factionsId1 = renderer.framebufferSystem.framebuffers.size() - 1;

		renderer.framebufferSystem.framebuffers.back().draw();

		renderer.clearColor(Color(1));
		renderer.renderMesh(resourceManager, "fullScreenQuad");

		initialized = true;
	}

	//renderer.normalizedSpace();
	//renderer.framebufferSystem.framebuffers[factionsId1].setAsTexture(0);
	//renderer.framebufferSystem.framebuffers[factionsId2].draw();
	//renderer.useShader("gpuSimShader");
	//renderer.renderMesh("fullScreenQuad");

	//render on screen
	renderer.framebufferSystem.framebuffers[factionsId1].setAsTexture(0);
	renderer.framebufferSystem.draw(renderer, 0);
	renderer.aspectCorrectNormalizedSpace();
	//renderer.clearColor(Color(1));
	renderer.useShader(resourceManager, "final");
	renderer.renderMesh(resourceManager, "fullScreenQuad");

	//swap buffers
	//UInt tmp = factionsId1;
	//factionsId1 = factionsId2;
	//factionsId2 = tmp;
}