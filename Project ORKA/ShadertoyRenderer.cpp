#include "ShadertoyRenderer.hpp"

#include "CPUTexture.hpp"
#include "FileSystem.hpp"
#include "Image.hpp"
#include "Math.hpp"
#include "Player.hpp"
#include "Renderer.hpp"
#include "Window.hpp"
#include "Random.hpp"

namespace
{
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

	void setupFloatColorOnlyFramebuffer(Renderer& renderer, Framebuffer& framebuffer, const String& name, const IVec2& size)
	{
		if (!framebuffer.isComplete())
		{
			framebuffer.create(renderer, name, size);
			framebuffer.add(renderer, WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
			framebuffer.checkComplete(renderer);
		}
		framebuffer.resize(renderer, size);
	}

	[[nodiscard]] Vector<IVec2> buildExposureChainSizes(const IVec2& hdrSize)
	{
		Vector<IVec2> sizes;
		IVec2         levelSize = IVec2(std::max(1, hdrSize.x / 2), std::max(1, hdrSize.y / 2));
		while (true)
		{
			sizes.push_back(levelSize);
			if (levelSize.x == 1 && levelSize.y == 1) break;
			levelSize = IVec2(std::max(1, levelSize.x / 2), std::max(1, levelSize.y / 2));
		}
		return sizes;
	}
}

void ShadertoyRenderer::updateCommonUniforms(Renderer& renderer, const IVec2& size)
{
	renderer.uniforms().setTime(static_cast<Float>(renderer.time.getTotal()));
	renderer.uniforms().setFramebufferWidth(size.x);
	renderer.uniforms().setFramebufferHeight(size.y);
}

void ShadertoyRenderer::updateCameraUniforms(Renderer& renderer) const
{
	// Expose camera basis and FOV through existing uniform slots.
	const Vec3 camPos   = player.camera.getPosition();
	const Vec3 camDir   = player.camera.getForwardVector();
	const Vec3 camRight = player.camera.getRightVector();
	const Vec3 camUp    = player.camera.getUpVector();

	renderer.uniforms().setCameraPos(Vec4(camPos, 1.0f));
	renderer.uniforms().setCameraVec(Vec4(camDir, 1.0f));
	renderer.uniforms().setWorldOffset(Vec4(camRight, 0.0f)); // reuse worldOffset for camRight
	renderer.uniforms().setCustomColor(Vec4(camUp, player.camera.getFieldOfView())); // up.xyz, fov in w
}

void ShadertoyRenderer::ensureBuffers(Renderer& renderer, const IVec2& heightmapSize, const IVec2& windowSize)
{
	setupFloatFramebuffer(renderer, bufferA, "ErosionBufferA", heightmapSize);
	setupFloatFramebuffer(renderer, bufferB, "ErosionBufferB", heightmapSize);
	setupFloatFramebuffer(renderer, hdrFramebuffer, "ErosionHDR", windowSize);
}

void ShadertoyRenderer::ensureDitherTexture(Renderer& renderer)
{
	if (starsTexture.isLoaded()) return;

	CpuTexture cpuTexture;
	cpuTexture.load(Path("Data/Textures/milky-way.jpg"), Filter::Linear, Filter::Linear, Wrapping::Repeat);
	starsTexture.load(renderer, cpuTexture);
}

void ShadertoyRenderer::create(Window& window)
{
	setName("ShadertoyRenderer");
	player.camera.setPosition(Vec3(0.0f, -5.0f, 0.5f));
	GameRenderer::create(window);
	ensureDitherTexture(window.renderer);
}

void ShadertoyRenderer::update(Window& window)
{
	player.update(window);
}

void ShadertoyRenderer::destroy(Window& window)
{
	if (starsTexture.isLoaded()) starsTexture.unload(window.renderer);
	if (bufferA.isComplete()) bufferA.destroy(window.renderer);
	if (bufferB.isComplete()) bufferB.destroy(window.renderer);
	if (hdrFramebuffer.isComplete()) hdrFramebuffer.destroy(window.renderer);
	GameRenderer::destroy(window);
}

void ShadertoyRenderer::renderHeightmapPass(Renderer& renderer, const IVec2& size)
{
	bufferA.bindDraw(renderer);
	renderer.setRenderRegion({0, 0}, size);
	renderer.setDepthTest(false);
	renderer.setCulling(false);
	renderer.setAlphaBlending(false);
	renderer.normalizedSpace();
	renderer.useShader("erosionBufferA");
	renderer.uniforms().setCustomColor(Vec4(0));
	renderer.uniforms().setMMatrix(scale(Matrix(1), Vec3(2.0f, 2.0f, 1.0f)));
	renderer.plane();
}

void ShadertoyRenderer::renderDetailPass(Renderer& renderer, const IVec2& size)
{
	bufferB.bindDraw(renderer);
	renderer.setRenderRegion({0, 0}, size);
	renderer.setDepthTest(false);
	renderer.setCulling(false);
	renderer.setAlphaBlending(false);
	renderer.normalizedSpace();
	renderer.useShader("erosionBufferB");
	renderer.uniforms().setCustomColor(Vec4(0));
	renderer.uniforms().setMMatrix(scale(Matrix(1), Vec3(2.0f, 2.0f, 1.0f)));
	renderer.plane();
}

void ShadertoyRenderer::renderLightingPass(Renderer& renderer, const IVec2& size)
{
	hdrFramebuffer.bindDraw(renderer);
	renderer.setRenderRegion({0, 0}, size);
	renderer.setDepthTest(false);
	renderer.setCulling(false);
	renderer.setAlphaBlending(false);
	renderer.normalizedSpace();

	renderer.useShader("erosionFinal");
	updateCameraUniforms(renderer);
	bufferA.setAsTexture(renderer, 0);
	bufferB.setAsTexture(renderer, 1);
	starsTexture.useTextureInSlot(renderer, 2);
	renderer.uniforms().setMMatrix(scale(Matrix(1), Vec3(2.0f, 2.0f, 1.0f)));
	renderer.plane();
}

void ShadertoyRenderer::updateExposureSample(Renderer& renderer, const IVec2& size)
{
	if (size.x <= 0 || size.y <= 0) return;

	// Ensure reads come from the HDR buffer instead of the default backbuffer (which is LDR).
	hdrFramebuffer.bindRead(renderer);

	// Sample a few random pixels from the HDR buffer this frame.
	const int samples = 128;
	Float     sampleMax = 0.0f;
	for (int i = 0; i < samples; ++i)
	{
		const Vec4 c = hdrFramebuffer.readPixelsVec4(renderer, randomIntFast(size.x), randomIntFast(size.y), FramebufferMode::Color0);
		const Float l = std::max(dot(Vec3(c), Vec3(0.2126f, 0.7152f, 0.0722f)), 0.0f);
		sampleMax     = std::max(sampleMax, l);
	}

	if (!std::isfinite(sampleMax)) return;
	
	const Float exposureAdaptionSpeed = 0.01;
	lastMaxLum += sampleMax * exposureAdaptionSpeed;
	lastMaxLum /= 1.0 + exposureAdaptionSpeed;
}

void ShadertoyRenderer::renderToneMapPass(Renderer& renderer, const IVec2& size)
{
	framebuffer.bindDraw(renderer);
	renderer.setRenderRegion({0, 0}, size);
	renderer.setDepthTest(false);
	renderer.setCulling(false);
	renderer.setAlphaBlending(false);
	renderer.normalizedSpace();

	renderer.useShader("erosionTonemap");
	hdrFramebuffer.setAsTexture(renderer, 0);
	starsTexture.useTextureInSlot(renderer, 1);
	renderer.uniforms().setCustomColor(Vec4(0, lastMaxLum, 0.0f, 0.0f));
	renderer.uniforms().setMMatrix(scale(Matrix(1), Vec3(2.0f, 2.0f, 1.0f)));
	renderer.plane();
	renderer.uniforms().setCustomColor(Vec4(0.0f)); // reset
}

void ShadertoyRenderer::renderScene(Window& window)
{
	Renderer& renderer = window.renderer;
	const IVec2 windowSize = framebuffer.getSize();
	const IVec2 heightmapSizeVec = IVec2(heightmapSize, heightmapSize);

	if (windowSize.x <= 0 || windowSize.y <= 0) return;

	ensureBuffers(renderer, heightmapSizeVec, windowSize);
	ensureDitherTexture(renderer);
	updateCommonUniforms(renderer, heightmapSizeVec);

	renderHeightmapPass(renderer, heightmapSizeVec);
	renderDetailPass(renderer, heightmapSizeVec);
	updateCommonUniforms(renderer, windowSize);
	renderLightingPass(renderer, windowSize);
	updateExposureSample(renderer, windowSize);
	renderToneMapPass(renderer, windowSize);

	// Overlay gathered max brightness on screen for debugging.
	renderer.fill(1);
	renderer.setDepthTest(false);
	renderer.screenSpaceFromBottomLeft();
	renderer.uniforms().setMMatrix(Matrix(1));
	renderer.fontSize(16.0f);
	renderer.letterSpacing(0.6f);
	renderer.textRenderSystem.alignText(Alignment::left, Alignment::bottom);
	renderer.text("Max brightness: " + toString(lastMaxLum), Vec2(20.0f, 20.0f));
}

void ShadertoyRenderer::connect(GameSimulation& simulation) {}

void ShadertoyRenderer::inputEvent(Window& window, InputEvent& input)
{
	if (input == enter) ui.capture(*this);
	if (input == exit) ui.releaseCapture();
	if (input == reloadShaders) window.renderer.shaderSystem.rebuild(window.renderer);
	player.inputEvent(window, input);
}

void ShadertoyGame::run()
{
	Window& window = ui.window("Shadertoy Erosion", settings.defaultWindowWidth, settings.defaultWindowHeight, true, true, WindowState::Windowed);
	window.addElement(renderer);
	ui.run();
}

