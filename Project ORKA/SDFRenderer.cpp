#include "SDFRenderer.hpp"

void SignedDistanceFieldRenderer::update(Window& window)
{
	player.update(window);
}

void SignedDistanceFieldRenderer::create(Window& window)
{
	framebuffer.create("MainFramebuffer", Area(1920, 1080));
	framebuffer.add(WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
	framebuffer.add(WritePixelsFormat::Depth, DataType::Float, FramebufferAttachment::Depth, false, Wrapping::Clamped);
	framebuffer.checkComplete();

	player.camera.setPosition(Vec3(1.0f, -5.0f, 2.0f));
}

void SignedDistanceFieldRenderer::destroy(Window& window)
{
	framebuffer.destroy();
}

void SignedDistanceFieldRenderer::connect(GameSimulation& simulation) {}

void SignedDistanceFieldRenderer::render(Window& window, const TiledRectangle area)
{
	Renderer& r = window.renderer;

	framebuffer.resize(area.size);
	framebuffer.clear();
	framebuffer.bindDraw();

	// setup
	r.setCulling(false);
	r.setDepthTest(false);
	r.uniforms().setMMatrix(Matrix(1));

	// render scene
	// in actual 3d space the camera stays in the center
	// the scene itself is entirely in the shader
	player.camera.renderOnlyRot(r);
	r.uniforms().setSunDir(Vec4(normalize(Vec3(0.445776, 0.77546453, 1)), 0));
	// we simply pass the camera position to the shader
	r.uniforms().setCameraPos(Vec4(player.camera.getPosition(), 0));
	r.useTexture("noise", 1);
	r.useShader("sdfScene");
	// always render a cube, so we can abuse the normalized vertex position as direction
	r.renderMesh("centeredCube");

	framebuffer.setAsTexture(0);
	r.drawToWindow();
	r.fullScreenShader("final");
}

void SignedDistanceFieldRenderer::renderInteractive(Window& window, TiledRectangle area) {}

void SignedDistanceFieldRenderer::inputEvent(Window& window, const InputEvent input)
{
	if(input == enter) window.captureCursor();
	if(input == exit) window.releaseCursor();
	if(input == reloadShaders) window.renderer.shaderSystem.rebuild();

	player.inputEvent(window, input);
}
