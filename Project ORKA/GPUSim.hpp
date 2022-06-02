#pragma once

#include "Game.hpp"

struct GPUSimRenderer : public GameRenderer {
	Bool initialized = false;
	
	Bool swap = true;

	UInt factionsID1 = 0;
	UInt factionsID2 = 0;

	void setup(Renderer & renderer) {
		UInt worldSize = 1024;

		CPUTexture cpuTexture;
		cpuTexture.channels = 1;
		cpuTexture.dataType = DataType::dataTypeFloat;
		cpuTexture.farFilter = Filter::nearest;
		cpuTexture.nearFilter = Filter::nearest;
		cpuTexture.width = worldSize;
		cpuTexture.height = worldSize;
		cpuTexture.wrapping = Wrapping::repeat;
		cpuTexture.bytePixels = nullptr;
		cpuTexture.loaded = true;

		//worldFramebufferTexture.texture.load(cpuTexture);

		//worldFramebuffer.add(1, DataType::dataTypeFloat, 0);
		//worldFramebuffer.resize(Area(worldSize));

		//worldFramebuffer.use();

		//renderer.rectangle();
		//
		//renderer.renderMesh("fullScreenQuad");
	}

	void update(Window& window) override {};
	void render(Window& window, TiledRectangle area) override {
		Renderer& renderer = window.renderer;

		if (!initialized) {
			renderer.framebufferSystem.framebuffers.emplace_back();
			renderer.framebufferSystem.framebuffers.back().create(Area(512));
			renderer.framebufferSystem.framebuffers.back().add(1, DataType::dataTypeFloat, 0);
			factionsID2 = renderer.framebufferSystem.framebuffers.size() - 1;
			
			renderer.framebufferSystem.framebuffers.back().draw();

			renderer.clearColor(Color(1));
			renderer.renderRegion.set(TiledRectangle(50, 50, 50, 50));
			renderer.useShader("color");
			renderer.fill(Color(0));
			renderer.renderMesh("fullScreenQuad");

			renderer.framebufferSystem.framebuffers.emplace_back();
			renderer.framebufferSystem.framebuffers.back().create(Area(512));
			renderer.framebufferSystem.framebuffers.back().add(1, DataType::dataTypeFloat, 0);
			factionsID1 = renderer.framebufferSystem.framebuffers.size() - 1;

			renderer.framebufferSystem.framebuffers.back().draw();

			renderer.clearColor(Color(1));
			renderer.renderMesh("fullScreenQuad");

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
		renderer.apectCorrectNormalizedSpace();
		//renderer.clearColor(Color(1));
		renderer.useShader("final");
		renderer.renderMesh("fullScreenQuad");


		//swap buffers
		//UInt tmp = factionsID1;
		//factionsID1 = factionsID2;
		//factionsID2 = tmp;
	};
};