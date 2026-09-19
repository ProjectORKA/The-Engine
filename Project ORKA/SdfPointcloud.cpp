#include "SdfPointcloud.hpp"
#include "Renderer.hpp"
#include "Window.hpp"

namespace
{
	constexpr UInt PointSsboBinding = 1;
	constexpr ULL  PointStrideBytes = sizeof(Vec4) * 2; // worldPos + color
	// Mooncrash uses pow(2, worldOffset.w) as planet radius; level 10 => R=1024.
	constexpr Float PlanetDistortionLevel = 10.0f;
	const Vec4      SunDir                = Vec4(normalize(Vec3(-0.8f, -0.3f, 0.4f)), 0.0f);
	constexpr UInt  ComputeFillBarrier =
		static_cast<UInt>(MemoryBarrierBit::ShaderImageAccess) |
		static_cast<UInt>(MemoryBarrierBit::ShaderStorage) |
		static_cast<UInt>(MemoryBarrierBit::TextureFetch);
}

void SdfPointcloudRenderer::ensureResources(Renderer& renderer, const IVec2 size)
{
	if (size.x <= 0 || size.y <= 0) return;

	if (!resourcesCreated)
	{
		emptyVao.create(renderer.openGlContext);
		sdfFramebuffer.create(renderer, "SdfPointcloudScene", size);
		sdfFramebuffer.add(renderer, WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
		sdfFramebuffer.add(renderer, WritePixelsFormat::Depth, DataType::Float, FramebufferAttachment::Depth, false, Wrapping::Clamped);
		sdfFramebuffer.checkComplete(renderer);
		resourcesCreated = true;
		forceFullRebuild = true;
	}

	if (cloudSize == size) return;

	sdfFramebuffer.resize(renderer, size);

	const ULL byteCount = static_cast<ULL>(size.x) * static_cast<ULL>(size.y) * PointStrideBytes;
	pointCloudBuffer.allocate(renderer.openGlContext, BufferTarget::ShaderStorage, byteCount, BufferUsage::DynamicDraw, "SdfPointcloudSSBO", PointSsboBinding);
	cloudSize        = size;
	forceFullRebuild = true;
	sdfReady         = false;
}

Bool SdfPointcloudRenderer::cameraChanged() const
{
	const Camera& cam = player.camera;
	return forceFullRebuild || cam.getPosition() != lastCameraPos || cam.getRotation() != lastCameraRot || cam.getFieldOfView() != lastFov;
}

void SdfPointcloudRenderer::storeCameraState()
{
	lastCameraPos    = player.camera.getPosition();
	lastCameraRot    = player.camera.getRotation();
	lastFov          = player.camera.getFieldOfView();
	forceFullRebuild = false;
}

void SdfPointcloudRenderer::update(Window& window)
{
	player.update(window);
}

void SdfPointcloudRenderer::create(Window& window)
{
	GameRenderer::create(window);
	player.camera.setPosition(Vec3(0.0f, -8.0f, 3.0f));
	// Raymarch can store hits closer than the default 0.1 near plane; those get frustum-clipped on splat.
	player.camera.setNearClipValue(0.001f);
	player.speedExponent = 5;
	forceFullRebuild = true;
}

void SdfPointcloudRenderer::destroy(Window& window)
{
	if (resourcesCreated)
	{
		if (cloudSize.x > 0 && cloudSize.y > 0) pointCloudBuffer.destroy(window.renderer.openGlContext);
		emptyVao.destroy(window.renderer.openGlContext);
		sdfFramebuffer.destroy(window.renderer);
		resourcesCreated = false;
		cloudSize        = IVec2(0);
		sdfReady         = false;
	}
	GameRenderer::destroy(window);
}

void SdfPointcloudRenderer::connect(GameSimulation& simulation) {}

void SdfPointcloudRenderer::renderInteractive(Window& window) {}

void SdfPointcloudRenderer::uploadCommonUniforms(Renderer& renderer, const IVec2 size)
{
	renderer.uniforms().setFramebufferWidth(size.x);
	renderer.uniforms().setFramebufferHeight(size.y);
	renderer.uniforms().setMMatrix(Matrix(1));
	renderer.uniforms().setSunDir(Vec4(Vec3(SunDir), splatDistortion ? 1.0f : 0.0f));
	renderer.uniforms().setWorldOffset(Vec4(0.0f, 0.0f, 0.0f, PlanetDistortionLevel));
	renderer.uniforms().setCustomInt1(worldDistortion);
	renderer.uniforms().setCustom1(showStepCount ? 1.0f : 0.0f);
	renderer.uniforms().setCustom2(showRecalcPixels ? 1.0f : 0.0f);
	player.camera.renderOnlyRot(renderer);
}

void SdfPointcloudRenderer::blitSdfToGameFramebuffer(Renderer& renderer) const
{
	framebuffer.bindDraw(renderer);
	renderer.setRenderRegion(Vec2(0), framebuffer.getSize());
	renderer.setDepthTest(false);
	renderer.setCulling(false);
	renderer.setAlphaBlending(false);
	renderer.uniforms().resetMatrices();
	renderer.uniforms().setCustomInt1(0);

	sdfFramebuffer.setAsTexture(renderer, 0);
	renderer.useShader("texture");
	renderer.renderMesh("fullScreenQuad");
}

void SdfPointcloudRenderer::drawOverlay(Renderer& renderer) const
{
	renderer.uniforms().setCustomColor(Vec4(1.0f));
	renderer.uniforms().setMMatrix(Matrix(1));
	renderer.screenSpaceFromBottomLeft();
	renderer.setDepthTest(false);
	renderer.setCulling(false);
	renderer.fontSize(16.0f);
	renderer.letterSpacing(0.6f);
	renderer.textRenderSystem.alignText(Alignment::left, Alignment::bottom);

	const Float delta = max(renderer.time.getDelta(), 0.0001f);
	const Vec3  cam   = player.camera.getPosition();
	constexpr Float spacing = 20.0f;
	Int line = 1;
	renderer.text(renderMode == SdfPointcloudMode::FullSdf ? "Mode: Full SDF [1]" : "Mode: Pointcloud [2]", Vec2(spacing, spacing * static_cast<Float>(line++)));
	renderer.text("FPS: " + toString(static_cast<Int>(1.0f / delta)), Vec2(spacing, spacing * static_cast<Float>(line++)));
	renderer.text("Cam XY: " + toString(cam.x) + " | " + toString(cam.y), Vec2(spacing, spacing * static_cast<Float>(line++)));
	renderer.text("Cam height Z: " + toString(cam.z), Vec2(spacing, spacing * static_cast<Float>(line++)));
	renderer.text(worldDistortion ? "Planet curve (march): ON [K]" : "Planet curve (march): OFF [K]", Vec2(spacing, spacing * static_cast<Float>(line++)));
	renderer.text(splatDistortion ? "Splat curve: ON [5] (planet only)" : "Splat curve: OFF [5]", Vec2(spacing, spacing * static_cast<Float>(line++)));
	renderer.text(showStepCount ? "Step viz: ON [3] (blue=low, red=high)" : "Step viz: OFF [3]", Vec2(spacing, spacing * static_cast<Float>(line++)));
	renderer.text(showRecalcPixels ? "Recalc viz: ON [4] (red=filled this frame)" : "Recalc viz: OFF [4]", Vec2(spacing, spacing * static_cast<Float>(line++)));
}

void SdfPointcloudRenderer::runHoleFillCompute(Renderer& renderer, const IVec2 size)
{
	pointCloudBuffer.bindBase(renderer.openGlContext, PointSsboBinding);
	renderer.bindImageTexture(0, sdfFramebuffer.getColorTextureID(), ImageAccess::ReadWrite, SizedInternalFormat::RGBA32F);
	renderer.useTexture("noise", 1);
	renderer.useTexture("terrainNoise", 2);
	renderer.useShader("sdfPointcloudFill");

	const UInt groupsX = (static_cast<UInt>(size.x) + 7u) / 8u;
	const UInt groupsY = (static_cast<UInt>(size.y) + 7u) / 8u;
	renderer.dispatchCompute(groupsX, groupsY, 1);
	renderer.memoryBarrier(ComputeFillBarrier);

	OpenGL::apiBindImageTexture(renderer.openGlContext, 0, 0, 0, false, 0, ImageAccess::ReadWrite, SizedInternalFormat::RGBA32F);
}

void SdfPointcloudRenderer::renderFullSdfPass(Renderer& renderer, const IVec2 size)
{
	uploadCommonUniforms(renderer, size);

	sdfFramebuffer.bindDraw(renderer);
	renderer.setRenderRegion(Vec2(0), size);
	// Alpha 0 so every pixel is treated as a hole and raymarched.
	sdfFramebuffer.clearColor(renderer, Vec4(0.0f, 0.0f, 0.0f, 0.0f));

	OpenGL::apiBindDrawFramebuffer(renderer.openGlContext, 0);
	runHoleFillCompute(renderer, size);

	sdfReady = true;
	storeCameraState();
}

void SdfPointcloudRenderer::renderPointcloudPass(Renderer& renderer, const IVec2 size)
{
	uploadCommonUniforms(renderer, size);
	pointCloudBuffer.bindBase(renderer.openGlContext, PointSsboBinding);

	sdfFramebuffer.bindDraw(renderer);
	renderer.setRenderRegion(Vec2(0), size);
	sdfFramebuffer.clearColor(renderer, Vec4(0.0f, 0.0f, 0.0f, 0.0f));
	sdfFramebuffer.clearDepth(renderer);

	renderer.setCulling(false);
	renderer.setAlphaBlending(false);
	renderer.setDepthTest(true);
	renderer.setProgramPointSize(true);

	renderer.useShader("sdfPointcloudSplat");
	emptyVao.bind(renderer.openGlContext);
	renderer.drawArrays(PrimitiveMode::Points, 0, size.x * size.y);
	emptyVao.unbind(renderer.openGlContext);

	renderer.setProgramPointSize(false);

	OpenGL::apiBindDrawFramebuffer(renderer.openGlContext, 0);
	runHoleFillCompute(renderer, size);

	sdfReady = true;
	storeCameraState();
}

void SdfPointcloudRenderer::inputEvent(Window& window, InputEvent& input)
{
	if (input == enter) ui.capture(*this);
	if (input == exit) ui.releaseCapture();
	if (input == reloadShaders)
	{
		window.renderer.shaderSystem.rebuild(window.renderer);
		forceFullRebuild = true;
	}
	if (input == switchToFullSdf)
	{
		renderMode       = SdfPointcloudMode::FullSdf;
		forceFullRebuild = true;
	}
	if (input == switchToPointcloud)
	{
		renderMode       = SdfPointcloudMode::Pointcloud;
		forceFullRebuild = true;
	}
	if (input == worldDistortionToggle)
	{
		worldDistortion  = !worldDistortion;
		forceFullRebuild = true;
	}
	if (input == stepCountToggle)
	{
		showStepCount = !showStepCount;
		forceFullRebuild = true;
	}
	if (input == recalcPixelsToggle)
	{
		showRecalcPixels = !showRecalcPixels;
		forceFullRebuild = true;
	}
	if (input == splatDistortionToggle)
	{
		splatDistortion  = !splatDistortion;
		forceFullRebuild = true;
	}

	player.inputEvent(window, input);
}

void SdfPointcloudRenderer::renderScene(Window& window)
{
	Renderer&   r  = window.renderer;
	const IVec2 sz = framebuffer.getSize();
	ensureResources(r, sz);

	if (!resourcesCreated || cloudSize.x <= 0 || cloudSize.y <= 0) return;

	if (showStepCount || renderMode == SdfPointcloudMode::FullSdf)
	{
		// Full raymarch every frame (required for step viz and full SDF mode).
		renderFullSdfPass(r, sz);
	}
	else if (showRecalcPixels || cameraChanged() || !sdfReady)
	{
		// Recalc viz needs a pass every frame so splat restores prior pixels and only new fills stay red.
		renderPointcloudPass(r, sz);
	}

	blitSdfToGameFramebuffer(r);
	drawOverlay(r);
}
