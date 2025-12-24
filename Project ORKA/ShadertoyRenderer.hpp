#pragma once

#include "Game.hpp"
#include "GPUTexture.hpp"

struct ShadertoyRenderer final : GameRenderer
{
	Framebuffer bufferA;
	Framebuffer bufferB;
	GPUTexture  ditherTexture;

	void create(Window& window) override;
	void destroy(Window& window) override;
	void renderScene(Window& window) override;
	void connect(GameSimulation& simulation) override;

private:
	void ensureBuffers(Renderer& renderer, const IVec2& size);
	void ensureDitherTexture(Renderer& renderer);
	void renderHeightmapPass(Renderer& renderer, const IVec2& size, const Vec4& mouseData);
	void renderDetailPass(Renderer& renderer, const IVec2& size, const Vec4& mouseData);
	void renderFinalPass(Renderer& renderer, const IVec2& size, const Vec4& mouseData);
	[[nodiscard]] Vec4 buildMouseData(const Window& window) const;
	void updateCommonUniforms(Renderer& renderer, const IVec2& size);
};

struct ShadertoyGame
{
	ShadertoyRenderer renderer;

	void run();
};

