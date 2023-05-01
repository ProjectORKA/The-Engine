#pragma once

#include "Game.hpp"
#include "ResourceManager.hpp"

struct GPUSimRenderer : public GameRenderer {
	Bool initialized = false;

	Bool swap = true;

	UInt factionsID1 = 0;
	UInt factionsID2 = 0;

	void setup(Renderer& renderer) {
		UInt worldSize = 1024;

		CPUTexture cpuTexture;
		cpuTexture.channels = 1;
		cpuTexture.dataType = DataType::dataTypeFloat;
		cpuTexture.farFilter = Filter::nearest;
		cpuTexture.nearFilter = Filter::nearest;
		cpuTexture.width = worldSize;
		cpuTexture.height = worldSize;
		cpuTexture.wrapping = Wrapping::repeat;
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
	void render(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
};