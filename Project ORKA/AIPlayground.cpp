#include "AIPlayground.hpp"
#include "PerlinNoise.hpp"
#include "Random.hpp"
#include "Window.hpp"

void AIPlayground::run()
{
	resourceManager.create();
	ui.create();
	ui.window("ORKA AI Playground", Area(settings.defaultWindowWidth, settings.defaultWindowHeight), true, true, WindowState::Windowed, renderer, resourceManager);
	ui.run();
}

void AIPlaygroundRenderer::update(Window& window)
{
	Vector<Float> inputVector;
	//Vector<Float> targetVector;

	for(UInt i = 0; i < network.structure[0]; i++) inputVector.push_back(static_cast<Float>(noise.octaveNoise0_1(window.renderer.time.total, i, 16) * 2.0));

	network.input(inputVector);
	network.propagateForward();

	//network.propagateBackward(targetVector);
}

void AIPlaygroundRenderer::destroy(Window& window)
{
	framebuffer.destroy();
}

void AIPlaygroundRenderer::connect(GameSimulation& simulation) {}

void AIPlaygroundRenderer::inputEvent(Window& window, InputEvent input) {}

void AIPlaygroundRenderer::create(ResourceManager& resourceManager, Window& window)
{
	framebuffer.create("MainFramebuffer", Area(1920, 1080));
	framebuffer.add(WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
	framebuffer.add(WritePixelsFormat::Depth, DataType::Float, FramebufferAttachment::Depth, false, Wrapping::Clamped);
	framebuffer.checkComplete();
}

void AIPlaygroundRenderer::renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) { }

void AIPlaygroundRenderer::render(ResourceManager& resourceManager, Window& window, const TiledRectangle area)
{
	Renderer& r = window.renderer;

	r.setWireframeMode();
	r.setCulling(true);
	r.setDepthTest(true);

	framebuffer.resize(area.size);
	framebuffer.clear();
	framebuffer.bindDraw();

	r.screenSpace();
	r.useShader(resourceManager, "color");
	r.setAlphaBlending(true);

	network.render(resourceManager, r, area.size);

	r.screenSpace();
	r.uniforms().setMMatrix(Matrix(1));
	r.textRenderSystem.alignText(Alignment::left, Alignment::center);
	r.textRenderSystem.render(resourceManager, r, String(toString(1.0f / r.time.delta)), Vec2(50));

	framebuffer.setAsTexture(0);
	r.drawToWindow();
	r.fullScreenShader(resourceManager, "final");
}
