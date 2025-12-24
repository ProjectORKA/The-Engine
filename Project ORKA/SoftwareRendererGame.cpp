#include "SoftwareRendererGame.hpp"

#include "CPUTexture.hpp"
#include "Image.hpp"
#include "Renderer.hpp"
#include "Window.hpp"
#include <cmath>

namespace
{
	constexpr Float tau = 6.28318530718f;
}

void SoftwareRenderer::create(Window& window)
{
	GameRenderer::create(window);
}

void SoftwareRenderer::destroy(Window& window)
{
	softwareTexture.unload(window.renderer);
	GameRenderer::destroy(window);
}

void SoftwareRenderer::renderCpuImage(const IVec2& size, const Float timeSeconds)
{
	const SizeT pixelCount = static_cast<SizeT>(size.x) * static_cast<SizeT>(size.y);
	if (pixelBuffer.size() != pixelCount) pixelBuffer.resize(pixelCount);

	for (Int y = 0; y < size.y; ++y)
	{
		const Float v = static_cast<Float>(y) / static_cast<Float>(size.y);
		for (Int x = 0; x < size.x; ++x)
		{
			const Float u    = static_cast<Float>(x) / static_cast<Float>(size.x);
			const Float wave = 0.5f + 0.5f * std::sin(timeSeconds + u * tau) * std::cos(timeSeconds * 0.5f + v * tau);
			pixelBuffer[static_cast<SizeT>(y) * static_cast<SizeT>(size.x) + static_cast<SizeT>(x)] = Vec3(u, v, wave);
		}
	}
}

void SoftwareRenderer::uploadBuffer(Renderer& renderer, const IVec2& size)
{
	if (pixelBuffer.empty()) return;

	const Memory pixelMemory(pixelBuffer.data(), pixelBuffer.size() * sizeof(Vec3));
	const Image  image(size.x, size.y, Channels::RGB, ImageDataType::Float, pixelMemory);

	CpuTexture cpuTexture;
	cpuTexture.load(image, Filter::Nearest, Filter::Nearest, Wrapping::Clamped, textureName);
	softwareTexture.load(renderer, cpuTexture);
}

void SoftwareRenderer::renderScene(Window& window)
{
	Renderer& r        = window.renderer;
	const IVec2 fbSize = framebuffer.getSize();

	if (fbSize.x <= 0 || fbSize.y <= 0) return;

	renderCpuImage(fbSize, static_cast<Float>(r.time.getTotal()));
	uploadBuffer(r, fbSize);

	framebuffer.clearColor(r, Color(0, 0, 0, 1));
	framebuffer.clearDepth(r);

	r.setCulling(false);
	r.setDepthTest(false);
	r.normalizedSpace();
	r.useShader("texture");
	r.uniforms().setCustomInt1(0);
	r.uniforms().setMMatrix(scale(Matrix(1), Vec3(2.0f, 2.0f, 1.0f))); // cover full clip-space

	softwareTexture.useTextureInSlot(r, 0);
	r.plane();
}

void SoftwareRenderer::connect(GameSimulation& simulation) {}

void SoftwareRendererGame::run()
{
	Window& window = ui.window("Software Renderer", settings.defaultWindowWidth, settings.defaultWindowHeight, true, true, WindowState::Windowed);
	window.addElement(renderer);
	ui.run();
}

