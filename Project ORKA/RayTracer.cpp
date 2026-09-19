#include "RayTracer.hpp"

#include "Renderer.hpp"
#include "RenderingOverrides.hpp"
#include "Window.hpp"

namespace
{
	void setupFloatColorFramebuffer(Renderer& renderer, Framebuffer& framebuffer, const String& name, const IVec2& size)
	{
		if (!framebuffer.isComplete())
		{
			framebuffer.create(renderer, name, size);
			framebuffer.add(renderer, WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
			framebuffer.checkComplete(renderer);
		}
		framebuffer.resize(renderer, size);
	}
}

void RayTracerRenderer::ensureBuffers(Renderer& renderer, const IVec2& size)
{
	setupFloatColorFramebuffer(renderer, accumFramebuffer, "CornellAccum", size);
}

void RayTracerRenderer::updateCameraUniforms(Renderer& renderer) const
{
	const Vec3 camPos   = player.camera.getPosition();
	const Vec3 camDir   = player.camera.getForwardVector();
	const Vec3 camRight = player.camera.getRightVector();
	const Vec3 camUp    = player.camera.getUpVector();

	renderer.uniforms().setCameraPos(Vec4(camPos, 1.0f));
	renderer.uniforms().setCameraVec(Vec4(camDir, 1.0f));
	renderer.uniforms().setWorldOffset(Vec4(camRight, 0.0f));
	renderer.uniforms().setCustomColor(Vec4(camUp, enableDepthOfField ? player.camera.getFieldOfView() : verticalFov));
}

void RayTracerRenderer::markSceneDirty()
{
	resetAccumulation = true;
	frameSamples      = 0;
	accumulationEpoch++;
}

void RayTracerRenderer::renderTracePass(Renderer& renderer, const IVec2& size)
{
	accumFramebuffer.bindDraw(renderer);
	renderer.setRenderRegion({0, 0}, size);
	renderer.setDepthTest(false);
	renderer.setCulling(false);
	renderer.setAlphaBlending(false);
	renderer.normalizedSpace();

	renderer.useShader("cornellPathtracer");
	updateCameraUniforms(renderer);
	renderer.uniforms().setTime(static_cast<Float>(renderer.time.getTotal()));
	renderer.uniforms().setFramebufferWidth(size.x);
	renderer.uniforms().setFramebufferHeight(size.y);
	renderer.uniforms().setCustom1(static_cast<Float>(frameSamples));
	renderer.uniforms().setCustom2(static_cast<Float>(accumulationEpoch));
	renderer.uniforms().setCustomInt1(resetAccumulation ? 1 : 0);

	if (!resetAccumulation)
		accumFramebuffer.setAsTexture(renderer, 0);

	renderer.uniforms().setMMatrix(scale(Matrix(1), Vec3(2.0f, 2.0f, 1.0f)));
	renderer.plane();
}

void RayTracerRenderer::renderDisplayPass(Renderer& renderer, const IVec2& size)
{
	framebuffer.bindDraw(renderer);
	renderer.setRenderRegion({0, 0}, size);
	renderer.setDepthTest(false);
	renderer.setCulling(false);
	renderer.setAlphaBlending(false);
	renderer.normalizedSpace();

	renderer.useShader("cornellPathtracerDisplay");
	accumFramebuffer.setAsTexture(renderer, 0);
	renderer.uniforms().setCustom1(static_cast<Float>(max(frameSamples, 1u)));
	renderer.uniforms().setCustomInt1(0);
	renderer.uniforms().setMMatrix(scale(Matrix(1), Vec3(2.0f, 2.0f, 1.0f)));
	renderer.plane();
}

void RayTracerRenderer::drawOverlay(Renderer& renderer)
{
	if (!showOverlay) return;

	WireframeOverride wireframeGuard(renderer, false);
	DepthTestOverride depthGuard(renderer, false);

	renderer.setDepthTest(false);
	renderer.setCulling(false);
	renderer.setAlphaBlending(true);
	renderer.screenSpaceFromBottomLeft();

	constexpr Float panelX      = 8.0f;
	constexpr Float panelY      = 8.0f;
	constexpr Float panelWidth  = 660.0f;
	constexpr Float panelHeight = 136.0f;
	constexpr Float textSpacing = 20.0f;
	constexpr Float textInset   = 12.0f;

	renderer.useShader("color");
	renderer.fill(Vec4(0.06f, 0.06f, 0.06f, 0.88f));
	renderer.uniforms().setMMatrix(matrixFromPositionAndSize(Vec2(panelX, panelY), Vec2(panelWidth, panelHeight)));
	renderer.rectangle();

	renderer.useShader("text");
	renderer.fill(Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	renderer.uniforms().setMMatrix(Matrix(1));
	renderer.fontSize(16.0f);
	renderer.letterSpacing(0.6f);
	renderer.textRenderSystem.alignText(Alignment::left, Alignment::bottom);

	Int line = 1;
	renderer.text("Cornell Box — GPU PBR path tracer", Vec2(textInset, textInset + textSpacing * static_cast<Float>(line++)));
	renderer.text("Progressive accumulation: image converges as samples increase.", Vec2(textInset, textInset + textSpacing * static_cast<Float>(line++)));
	renderer.text("Samples: " + toString(frameSamples), Vec2(textInset, textInset + textSpacing * static_cast<Float>(line++)));
	renderer.text(tracerStatus, Vec2(textInset, textInset + textSpacing * static_cast<Float>(line++)));
	renderer.text("LMB capture mouse · R reset · H toggle UI · T reload shaders", Vec2(textInset, textInset + textSpacing * static_cast<Float>(line++)));
	renderer.text("WASD QE move · scroll speed · mouse look", Vec2(textInset, textInset + textSpacing * static_cast<Float>(line++)));
}

void RayTracerRenderer::create(Window& window)
{
	GameRenderer::create(window);
	setName("RayTracerRenderer");
	player.camera.setPosition(Vec3(0.0f, -0.15f, 1.02f));
	player.camera.setRotation(0.0, 0.0, 0.0);
	lastCameraPosition  = player.camera.getPosition();
	lastCameraRotation  = player.camera.getRotation();
	tracerStatus        = "GPU path tracer ready";
	markSceneDirty();
}

void RayTracerRenderer::destroy(Window& window)
{
	if (accumFramebuffer.isComplete()) accumFramebuffer.destroy(window.renderer);
	GameRenderer::destroy(window);
}

void RayTracerRenderer::update(Window& window)
{
	const Vec3  previousPosition = player.camera.getPosition();
	const DVec3 previousRotation = player.camera.getRotation();
	player.update(window);

	if (player.camera.getPosition() != previousPosition || player.camera.getRotation() != previousRotation) markSceneDirty();
}

void RayTracerRenderer::renderScene(Window& window)
{
	Renderer&   renderer = window.renderer;
	const IVec2 fbSize   = framebuffer.getSize();
	if (fbSize.x <= 0 || fbSize.y <= 0) return;

	ensureBuffers(renderer, fbSize);
	renderTracePass(renderer, fbSize);

	if (resetAccumulation)
	{
		frameSamples      = 1;
		resetAccumulation = false;
	}
	else
		frameSamples++;

	renderDisplayPass(renderer, fbSize);
	drawOverlay(renderer);
}

void RayTracerRenderer::renderInteractive(Window& window) {}

void RayTracerRenderer::connect(GameSimulation& simulation) {}

void RayTracerRenderer::inputEvent(Window& window, InputEvent& input)
{
	if (input == captureMouse) ui.capture(*this);
	if (input == releaseMouse) ui.releaseCapture();
	if (input == resetSamples) markSceneDirty();
	if (input == reloadShaders)
	{
		window.renderer.shaderSystem.rebuild(window.renderer);
		markSceneDirty();
	}
	if (input == toggleOverlay) showOverlay = !showOverlay;

	player.inputEvent(window, input);
}

void RayTracerGame::run()
{
	Window& window = ui.window("Cornell Box Ray Tracer", 1280, 720, true, true, WindowState::Windowed);
	window.addElement(renderer);
	ui.run();
}
