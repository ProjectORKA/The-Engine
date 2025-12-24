#pragma once

#include "Game.hpp"
#include "GPUTexture.hpp"

struct SoftwareRenderer final : GameRenderer
{
	Name            textureName = "SoftwareRendererTexture";
	GPUTexture      softwareTexture;
	Vector<Vec3>    pixelBuffer;

	void create(Window& window) override;
	void destroy(Window& window) override;
	void renderScene(Window& window) override;
	void connect(GameSimulation& simulation) override;

private:
	void renderCpuImage(const IVec2& size, Float timeSeconds);
	void uploadBuffer(Renderer& renderer, const IVec2& size);
};

struct SoftwareRendererGame
{
	SoftwareRenderer renderer;

	void run();
};

