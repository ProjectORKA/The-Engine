#include "ShadertoyRenderer.hpp"

#include "CPUTexture.hpp"
#include "FileSystem.hpp"
#include "Image.hpp"
#include "Random.hpp"
#include "Renderer.hpp"
#include "Window.hpp"

namespace
{
	constexpr Int ditherSize = 256;

	void setupFloatFramebuffer(Renderer& renderer, Framebuffer& framebuffer, const String& name, const IVec2& size)
	{
		if (!framebuffer.isComplete())
		{
			framebuffer.create(renderer, name, size);
			framebuffer.add(renderer, WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
			framebuffer.add(renderer, WritePixelsFormat::Depth, DataType::Float, FramebufferAttachment::Depth, false, Wrapping::Clamped);
			framebuffer.checkComplete(renderer);
		}
		framebuffer.resize(renderer, size);
	}
}

Vec4 ShadertoyRenderer::buildMouseData(const Window& window) const
{
	const DVec2 localMouse = getMousePosBottomLeft(window);
	const Float leftDown   = window.mouseState[0] ? 1.0f : 0.0f;
	const Float rightDown  = window.mouseState[1] ? 1.0f : 0.0f;
	return Vec4(static_cast<Float>(localMouse.x), static_cast<Float>(localMouse.y), leftDown, rightDown);
}

void ShadertoyRenderer::updateCommonUniforms(Renderer& renderer, const IVec2& size)
{
	renderer.uniforms().setTime(static_cast<Float>(renderer.time.getTotal()));
	renderer.uniforms().setFramebufferWidth(size.x);
	renderer.uniforms().setFramebufferHeight(size.y);
}

void ShadertoyRenderer::ensureBuffers(Renderer& renderer, const IVec2& size)
{
	setupFloatFramebuffer(renderer, bufferA, "ErosionBufferA", size);
	setupFloatFramebuffer(renderer, bufferB, "ErosionBufferB", size);
}

void ShadertoyRenderer::ensureDitherTexture(Renderer& renderer)
{
	if (ditherTexture.isLoaded()) return;

	Vector<Byte> pixels(static_cast<SizeT>(ditherSize) * static_cast<SizeT>(ditherSize) * 3);
	for (Byte& p : pixels) p = static_cast<Byte>(randomIntFast(256));

	const Memory pixelMemory(pixels.data(), pixels.size());
	const Image  image(ditherSize, ditherSize, Channels::RGB, ImageDataType::Byte, pixelMemory);

	CpuTexture cpuTexture;
	cpuTexture.load(image, Filter::Nearest, Filter::Nearest, Wrapping::Repeat, "ErosionDither");
	ditherTexture.load(renderer, cpuTexture);
}

void ShadertoyRenderer::create(Window& window)
{
	GameRenderer::create(window);
	ensureDitherTexture(window.renderer);
}

void ShadertoyRenderer::destroy(Window& window)
{
	if (ditherTexture.isLoaded()) ditherTexture.unload(window.renderer);
	if (bufferA.isComplete()) bufferA.destroy(window.renderer);
	if (bufferB.isComplete()) bufferB.destroy(window.renderer);
	GameRenderer::destroy(window);
}

void ShadertoyRenderer::renderHeightmapPass(Renderer& renderer, const IVec2& size, const Vec4& mouseData)
{
	bufferA.bindDraw(renderer);
	renderer.setRenderRegion({0, 0}, size);
	renderer.setDepthTest(false);
	renderer.setCulling(false);
	renderer.setAlphaBlending(false);
	renderer.normalizedSpace();
	renderer.useShader("erosionBufferA");
	renderer.uniforms().setCustomColor(mouseData);
	renderer.uniforms().setMMatrix(scale(Matrix(1), Vec3(2.0f, 2.0f, 1.0f)));
	renderer.plane();
}

void ShadertoyRenderer::renderDetailPass(Renderer& renderer, const IVec2& size, const Vec4& mouseData)
{
	bufferB.bindDraw(renderer);
	renderer.setRenderRegion({0, 0}, size);
	renderer.setDepthTest(false);
	renderer.setCulling(false);
	renderer.setAlphaBlending(false);
	renderer.normalizedSpace();
	renderer.useShader("erosionBufferB");
	renderer.uniforms().setCustomColor(mouseData);
	renderer.uniforms().setMMatrix(scale(Matrix(1), Vec3(2.0f, 2.0f, 1.0f)));
	renderer.plane();
}

void ShadertoyRenderer::renderFinalPass(Renderer& renderer, const IVec2& size, const Vec4& mouseData)
{
	framebuffer.bindDraw(renderer);
	renderer.setRenderRegion({0, 0}, size);
	renderer.setDepthTest(false);
	renderer.setCulling(false);
	renderer.setAlphaBlending(false);
	renderer.normalizedSpace();

	renderer.useShader("erosionFinal");
	renderer.uniforms().setCustomColor(mouseData);
	bufferA.setAsTexture(renderer, 0);
	bufferB.setAsTexture(renderer, 1);
	ditherTexture.useTextureInSlot(renderer, 2);
	renderer.uniforms().setMMatrix(scale(Matrix(1), Vec3(2.0f, 2.0f, 1.0f)));
	renderer.plane();
}

void ShadertoyRenderer::renderScene(Window& window)
{
	Renderer& renderer = window.renderer;
	const IVec2 size   = framebuffer.getSize();

	if (size.x <= 0 || size.y <= 0) return;

	ensureBuffers(renderer, size);
	ensureDitherTexture(renderer);
	updateCommonUniforms(renderer, size);

	const Vec4 mouseData = buildMouseData(window);

	renderHeightmapPass(renderer, size, mouseData);
	renderDetailPass(renderer, size, mouseData);
	renderFinalPass(renderer, size, mouseData);
}

void ShadertoyRenderer::connect(GameSimulation& simulation) {}

void ShadertoyGame::run()
{
	Window& window = ui.window("Shadertoy Erosion", settings.defaultWindowWidth, settings.defaultWindowHeight, true, true, WindowState::Windowed);
	window.addElement(renderer);
	ui.run();
}

