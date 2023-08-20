#include "GPUSim.hpp"

void GPUSim::run()
{
	resourceManager.create();
	ui.create();
	ui.window("ORKA GPU Simulation", Area(settings.defaultWindowWidth, settings.defaultWindowHeight), true, true, WindowState::Windowed, gpuSimRenderer, resourceManager);
	ui.run();
}

void GPUSimRenderer::update(Window& window) {}

void GPUSimRenderer::destroy(Window& window)
{
	framebuffer1.destroy();
	framebuffer2.destroy();
}

void GPUSimRenderer::connect(GameSimulation& simulation) {}

void GPUSimRenderer::inputEvent(Window& window, InputEvent input) {}

void GPUSimRenderer::create(ResourceManager& rm, Window& window)
{
	Renderer& r = window.renderer;

	framebuffer1.create("GPUSimTexture1", Area(worldSize));
	framebuffer1.add(WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Repeat);
	framebuffer1.add(WritePixelsFormat::Depth, DataType::Float, FramebufferAttachment::Depth, false, Wrapping::Repeat);
	framebuffer1.checkComplete();

	// draw rectangle on it
	framebuffer1.bindDraw();
	framebuffer1.clearColor(Color(0, 0, 0, 1));

	r.setRenderRegion(TiledRectangle(worldSize));
	r.useShader(rm, "color");
	r.uniforms().setMMatrix(Matrix(1));
	r.setColor(Color(1));
	r.renderMesh(rm, "suzanne");

	framebuffer2.create("GPUSimTexture1", Area(worldSize));
	framebuffer2.add(WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Repeat);
	framebuffer2.add(WritePixelsFormat::Depth, DataType::Float, FramebufferAttachment::Depth, false, Wrapping::Repeat);
	framebuffer2.checkComplete();
}

void GPUSimRenderer::render(ResourceManager& rm, Window& window, TiledRectangle area)
{
	Renderer& r = window.renderer;

	// since were changing to framebuffers with different size we need to modify the render region
	r.setRenderRegion(area);

	// we also don't use any depth
	DepthTestOverride depthOverride(false, r);

	// we make sure the background is clear
	r.clearBackground(Color(0.1, 0.1, 0.1, 1));

	// and simulate from one texture to the other
	if(flipFlop) r.postProcess(rm, "gpuSim", framebuffer1, framebuffer2);
	else r.postProcess(rm, "gpuSim", framebuffer2, framebuffer1);

	// then we render that texture to the screen

	r.setRenderRegion(area);
	r.uniforms().setMMatrix(scale(Matrix(1), Vec3(2)));
	r.aspectCorrectNormalizedSpace();
	r.drawToWindow();

	if(flipFlop) framebuffer2.setAsTexture(0);
	else framebuffer1.setAsTexture(0);

	r.useShader(rm, "texture");
	r.renderMesh(rm, "centeredPlane");

	flipFlop = !flipFlop;
}

void GPUSimRenderer::renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) {}
