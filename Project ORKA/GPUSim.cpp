#include "GPUSim.hpp"

void GPUSimRenderer::update(Window& window) {}

void GPUSimRenderer::destroy(Window& window)
{
	framebuffer1.destroy(window.renderer);
	framebuffer2.destroy(window.renderer);
}

void GPUSimRenderer::connect(GameSimulation& simulation) {}

void GPUSimRenderer::inputEvent(Window& window, InputEvent input) {}

void GPUSimRenderer::create(Window& window)
{
	Renderer& r = window.renderer;

	framebuffer1.create("GPUSimTexture1", Area(worldSize));
	framebuffer1.add(r, WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Repeat);
	framebuffer1.add(r, WritePixelsFormat::Depth, DataType::Float, FramebufferAttachment::Depth, false, Wrapping::Repeat);
	framebuffer1.checkComplete();

	// draw rectangle on it
	framebuffer1.bindDraw(r);
	framebuffer1.clearColor(Color(0, 0, 0, 1));

	r.setRenderRegion(TiledRectangle(worldSize));
	r.useShader("color");
	r.uniforms().setMMatrix(Matrix(1));
	r.setColor(Color(1));
	r.renderMesh("suzanne");

	framebuffer2.create("GPUSimTexture1", Area(worldSize));
	framebuffer2.add(window.renderer, WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Repeat);
	framebuffer2.add(window.renderer, WritePixelsFormat::Depth, DataType::Float, FramebufferAttachment::Depth, false, Wrapping::Repeat);
	framebuffer2.checkComplete();
}

void GPUSimRenderer::render(Window& window, TiledRectangle area)
{
	Renderer& r = window.renderer;

	// since were changing to framebuffers with different size we need to modify the render region
	r.setRenderRegion(area);

	// we also don't use any depth
	DepthTestOverride depthOverride(r, false);

	// we make sure the background is clear
	r.clearBackground(Color(0.1, 0.1, 0.1, 1));

	// and simulate from one texture to the other
	if (flipFlop)
	{
		r.postProcess("gpuSim", framebuffer1, framebuffer2);
	}
	else
	{
		r.postProcess("gpuSim", framebuffer2, framebuffer1);
	}

	// then we render that texture to the screen

	r.setRenderRegion(area);
	r.uniforms().setMMatrix(scale(Matrix(1), Vec3(2)));
	r.aspectCorrectNormalizedSpace();
	r.drawToWindow();

	if (flipFlop)
	{
		framebuffer2.setAsTexture(0);
	}
	else
	{
		framebuffer1.setAsTexture(0);
	}

	r.useShader("texture");
	r.renderMesh("centeredPlane");

	flipFlop = !flipFlop;
}

void GPUSimRenderer::renderInteractive(Window& window, TiledRectangle area) {}
