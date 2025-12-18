#include "GPUSim.hpp"

#include "RenderingOverrides.hpp"

void GPUSimRenderer::update(Window& window) {}

void GPUSimRenderer::destroy(Window& window)
{
	simulationFramebuffer.destroy(window.renderer);
	GameRenderer::destroy(window);
}

void GPUSimRenderer::connect(GameSimulation& simulation) {}

void GPUSimRenderer::inputEvent(Window& window, InputEvent input) {}

void GPUSimRenderer::create(Window& window)
{
	GameRenderer::create(window);
	Renderer& r = window.renderer;

	// draw rectangle on it
	framebuffer.bindDraw(r);
	framebuffer.clearColor(r, Color(0, 0, 0, 1));

	r.setRenderRegion(TiledRectangle(worldSize));
	r.useShader("color");
	r.uniforms().setMMatrix(Matrix(1));
	r.setColor(Color(1));
	r.renderMesh("suzanne");

	simulationFramebuffer.create(r, "GPUSimTexture1", IVec2(worldSize));
	simulationFramebuffer.add(window.renderer, WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Repeat);
	simulationFramebuffer.add(window.renderer, WritePixelsFormat::Depth, DataType::Float, FramebufferAttachment::Depth, false, Wrapping::Repeat);
	simulationFramebuffer.checkComplete(r);
}

void GPUSimRenderer::renderScene(Window& window)
{
	Renderer& r = window.renderer;

	if (flipFlop) simulationFramebuffer.setAsTexture(r, 0);
	else framebuffer.setAsTexture(r, 0);

	r.useShader("texture");
	r.renderMesh("centeredPlane");

	flipFlop = !flipFlop;

	// since were changing to framebuffers with different size we need to modify the render region
	r.setRenderRegion(getPosition(), getSize());

	// we also don't use any depth
	DepthTestOverride depthOverride(r, false);

	// we make sure the background is clear
	r.clearBackground(Color(0.1, 0.1, 0.1, 1));

	// and simulate from one texture to the other
	if (flipFlop) r.postProcess("gpuSim", framebuffer, simulationFramebuffer);
	else r.postProcess("gpuSim", simulationFramebuffer, framebuffer);
}

void GPUSimRenderer::renderInteractive(Window& window) {}
