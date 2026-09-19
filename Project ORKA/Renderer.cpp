#include "Renderer.hpp"
#include "Random.hpp"
#include "Camera.hpp" // remove
#include "Player.hpp" // remove
#include "RenderingOverrides.hpp"
#include "Window.hpp"
#include "OptickProfiling.hpp"
#include "UI/Render/UiRenderer.hpp"

namespace
{
	Mutex modernUiRenderersMutex;
	Map<Renderer*, UniquePointer<Ui::UiRenderer>> modernUiRenderers;

	Ui::UiRenderer* modernUiRendererFor(Renderer& renderer)
	{
		const LockGuard<Mutex> lock(modernUiRenderersMutex);
		const auto found = modernUiRenderers.find(&renderer);
		return found == modernUiRenderers.end() ? nullptr : found->second.get();
	}
}

void Renderer::end(Window& window)
{
	OPTICK_EVENT();
	setWireframeMode(false);
	setAlphaBlending(true);
	setRenderRegion(TiledRectangle(window.getContentSize()));
	drawToWindow();
	capturePresentFrame(window.getContentSize());
	pollGraphicsApiError();
	if constexpr (openglStateTracking) OpenGL::apiFinish(openGlContext);
	
	// Disable OpenGL logging if it was enabled for this frame
	if (window.openGLLoggingEnabled)
	{
		window.openGLLoggingEnabled = false;
		openGlContext.disableLogging();
		logDebug("OpenGL logging disabled after frame capture");
	}
	
	mutex.unlock();
	frameCount++;
}

void Renderer::sync()
{
	// Wait for the render thread to finish the current frame so resize callbacks
	// never observe a half-presented or cleared default framebuffer.
	const LockGuard<Mutex> lock(mutex);
}

void Renderer::plane()
{
	primitivesRenderer.plane(*this);
}

void Renderer::create()
{
	OpenGL::apiInit(openGlContext);

	OpenGL::apiSetDebugging(openGlContext, debugLoggingIsEnabled);

	if (printDeviceInfo) openGlContext.printOpenGLInfo();

	randomizeSeed();
	// basic systems
	time.reset();
	textureSystem.create(*this);
	meshSystem.create(*this);
	shaderSystem.create(*this);
	primitivesRenderer.create(*this);
	{
		auto backend = std::make_unique<Ui::UiRenderer>();
		backend->create(*this);
		const LockGuard<Mutex> lock(modernUiRenderersMutex);
		modernUiRenderers[this] = std::move(backend);
	}

	// advanced systems
	textRenderSystem.create(*this);
	//renderObjectSystem.create(*this);

	idFramebuffer.create(*this, "IDFramebuffer", IVec2(settings.defaultWindowWidth, settings.defaultWindowHeight));
	idFramebuffer.add(*this, WritePixelsFormat::RGBAInteger, DataType::UInt, FramebufferAttachment::Color0, true, Wrapping::Clamped);
	idFramebuffer.add(*this, WritePixelsFormat::Depth, DataType::Float, FramebufferAttachment::Depth, false, Wrapping::Clamped);
	idFramebuffer.checkComplete(*this);

	postProcessFramebuffer.create(*this, "PostProcessFramebuffer", IVec2(settings.defaultWindowWidth, settings.defaultWindowHeight));
	postProcessFramebuffer.add(*this, WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
	postProcessFramebuffer.add(*this, WritePixelsFormat::Depth, DataType::Float, FramebufferAttachment::Depth, false, Wrapping::Clamped);
	postProcessFramebuffer.checkComplete(*this);

	presentRetainFramebuffer.create(*this, "PresentRetainFramebuffer", IVec2(settings.defaultWindowWidth, settings.defaultWindowHeight));
	presentRetainFramebuffer.add(*this, WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
	presentRetainFramebuffer.checkComplete(*this);

	OpenGL::apiClipControl(openGlContext, GL_LOWER_LEFT, GL_ZERO_TO_ONE);
	OpenGL::apiSetScissorTest(openGlContext, true);
}

void Renderer::destroy()
{
	{
		UniquePointer<Ui::UiRenderer> backend;
		{
			const LockGuard<Mutex> lock(modernUiRenderersMutex);
			const auto found = modernUiRenderers.find(this);
			if (found != modernUiRenderers.end())
			{
				backend = std::move(found->second);
				modernUiRenderers.erase(found);
			}
		}
		if (backend) backend->destroy(*this);
	}
	primitivesRenderer.destroy(*this);

	idFramebuffer.destroy(*this);
	postProcessFramebuffer.destroy(*this);
	presentRetainFramebuffer.destroy(*this);
	presentRetainValid = false;
	presentedSize = IVec2(0);

	textRenderSystem.destroy(*this);
	//renderObjectSystem.destroy();

	meshSystem.destroy(*this);
	shaderSystem.destroy(*this);
	textureSystem.destroy(*this);
}

void Ui::submitUi(Renderer& renderer, const UiRenderPacket& packet)
{
	if (UiRenderer* backend = modernUiRendererFor(renderer)) backend->submit(renderer, packet);
}

void Ui::setUiRendererCallbacks(
	Renderer& renderer, UiRendererCallbacks callbacks)
{
	if (UiRenderer* backend = modernUiRendererFor(renderer))
		backend->setCallbacks(std::move(callbacks));
}

void Ui::uploadUiGlyphAtlas(
	Renderer& renderer, const UiResourceRef& resource,
	const UiGlyphPixelFormat format, const UInt width, const UInt height,
	const UiGlyphRect& rect, const Byte* pixels, const SizeT byteCount)
{
	if (UiRenderer* backend = modernUiRendererFor(renderer))
		backend->uploadGlyphAtlas(
			renderer, resource, format, width, height, rect, pixels, byteCount);
}

void Ui::releaseUiGlyphAtlases(Renderer& renderer, const U32 serviceId)
{
	if (UiRenderer* backend = modernUiRendererFor(renderer))
		backend->releaseGlyphAtlases(serviceId);
}

void Ui::uploadUiImageTexture(
	Renderer& renderer, const UiResourceRef& resource, const U32 ownerId,
	const UInt width, const UInt height, const Byte* rgbaPixels, const SizeT byteCount)
{
	if (UiRenderer* backend = modernUiRendererFor(renderer))
		backend->uploadImageTexture(
			renderer, resource, ownerId, width, height, rgbaPixels, byteCount);
}

void Ui::releaseUiImageTextures(Renderer& renderer, const U32 ownerId)
{
	if (UiRenderer* backend = modernUiRendererFor(renderer))
		backend->releaseImageTextures(ownerId);
}

void Renderer::uiSpace(const Window& window)
{
	uniforms().setVMatrix(getUISpaceMatrix(window));
	uniforms().setPMatrix(Matrix(1));
}

void Renderer::rectangle()
{
	primitivesRenderer.rectangle(*this);
}

void Renderer::rectangle(const IVec2 pos, const IVec2 size) {
	uniforms().setMMatrix(matrixFromPositionAndSize(pos, size));
	rectangle();
}

void Renderer::UIRectangle(const IVec2 pos, const IVec2 size)
{
	rectangle(pos, size);
}

void Renderer::renderUITexture(const IVec2 pos, const IVec2 size)
{
	rectangle(pos, size);
}

void Renderer::screenSpaceFromBottomLeft()
{
	uniforms().setVMatrix(getScreenSpaceMatrix());
	uniforms().setPMatrix(Matrix(1));
}

Uniforms& Renderer::uniforms()
{
	return shaderSystem.uniforms;
}

void Renderer::enableScissor()
{
	OpenGL::apiSetScissorTest(openGlContext, true);
}

void Renderer::wireframeCube()
{
	primitivesRenderer.wireframeCube(*this);
}

void Renderer::disableScissor()
{
	OpenGL::apiSetScissorTest(openGlContext, false);
}

void Renderer::normalizedSpace()
{
	uniforms().setVMatrix(Matrix(1));
	uniforms().setPMatrix(Matrix(1));
}

Bool Renderer::getCulling() const
{
	return openGlContext.getCulling();
}

Float Renderer::deltaTime() const
{
	return time.getDelta();
}

void Renderer::blendModeAdditive()
{
	OpenGL::apiSetBlending(openGlContext, true);
	OpenGL::apiBlendFunc(openGlContext, BlendFunction::One, BlendFunction::One);
	OpenGL::apiBlendEquation(openGlContext, BlendEquation::Add);
}

void Renderer::drawToWindow()
{
	OpenGL::apiBindDrawFramebuffer(openGlContext, 0);
}

void Renderer::fill(const Int color)
{
	fill(Vec4(Vec3(static_cast<Float>(color)), 1));
}

void Renderer::fill(const Vec3 color)
{
	fill(Vec4(color, 1));
}

void Renderer::fill(const Vec4 color)
{
	uniforms().setCustomColor(color);
}

void Renderer::stretchPresentRetainToWindow(const IVec2 targetSize)
{
	if (!presentRetainValid || presentedSize.x <= 0 || presentedSize.y <= 0) return;
	if (!presentRetainFramebuffer.isComplete()) return;

	presentRetainFramebuffer.bindRead(*this);
	drawToWindow();
	OpenGL::apiViewport(openGlContext, 0, 0, targetSize.x, targetSize.y);
	OpenGL::apiScissor(openGlContext, 0, 0, targetSize.x, targetSize.y);
	OpenGL::apiBlitFramebuffer(
		openGlContext,
		0, 0, presentedSize.x, presentedSize.y,
		0, 0, targetSize.x, targetSize.y,
		GL_COLOR_BUFFER_BIT, Filter::Linear);
	OpenGL::apiClearDepth(openGlContext);
}

void Renderer::capturePresentFrame(const IVec2 size)
{
	if (size.x <= 0 || size.y <= 0) return;

	presentRetainFramebuffer.resize(*this, size);
	if (!presentRetainFramebuffer.isComplete()) return;

	OpenGL::apiBindReadFramebuffer(openGlContext, 0);
	presentRetainFramebuffer.bindDraw(*this);
	OpenGL::apiBlitFramebuffer(
		openGlContext,
		0, 0, size.x, size.y,
		0, 0, size.x, size.y,
		GL_COLOR_BUFFER_BIT, Filter::Nearest);
	drawToWindow();

	presentedSize = size;
	presentRetainValid = true;
}

void Renderer::begin(const IVec2 size, const Color backgroundColor)
{
	time.update(); // advances the time

	mutex.lock(); // used for synchronizing other threads
	drawToWindow();
	setRenderRegion(TiledRectangle(size));
	OpenGL::apiViewport(openGlContext, 0, 0, size.x, size.y);
	OpenGL::apiScissor(openGlContext, 0, 0, size.x, size.y);

	const Bool sizeChanged = size != presentedSize;
	if (sizeChanged && presentRetainValid)
		stretchPresentRetainToWindow(size);
	else
		clearBackground(backgroundColor);

	setFramebufferSize(*this, size);

	uniforms().setWindowWidth(size.x);
	uniforms().setWindowHeight(size.y);
	uniforms().setTime(time.getDelta()); // makes time available to shaders
}

void Renderer::fill(const Float color)
{
	fill(Vec4(Vec3(color), 1.0f));
}

void Renderer::normalizedScreenSpace(const Window & window)
{
	const Float width  = static_cast<Float>(window.getWidth());
	const Float height = static_cast<Float>(window.getHeight());
	uniforms().setVMatrix(scale(Matrix(1), Vec3(2.0f / width, 2.0f / height, 0.0f)));
	uniforms().setPMatrix(Matrix(1));
}

void Renderer::wireframeCubeCentered()
{
	primitivesRenderer.wireframeCubeCentered(*this);
}

Float Renderer::getAspectRatio() const
{
	return renderRegion.getAspectRatio();
}

void Renderer::fill(const Double color)
{
	fill(Vec4(Vec3(static_cast<Float>(color)), 1.0f));
}

void Renderer::setWireframeMode()
{
	setWireframeMode(wireframeMode);
}

void Renderer::screenSpaceFromTopLeft(const Window & window)
{
	const auto width  = static_cast<Float>(window.getWidth());
	const auto height = static_cast<Float>(window.getHeight());
	auto       matrix = Matrix(1);
	matrix            = translate(matrix, Vec3(-1, 1, 0));
	matrix            = scale(matrix, Vec3(2.0f / width, -2.0f / height, 1.0f));
	uniforms().setVMatrix(matrix);
	uniforms().setPMatrix(Matrix(1));
}

void Renderer::useMesh(const Name& name)
{
	// selects a mesh to be rendered but doesn't render it
	meshSystem.use(*this, name);
}

Matrix Renderer::getUISpaceMatrix(const Window & window) const
{
	// Maps clip space [-1, 1] into a 0..width / 0..height pixel grid
	// with origin at the bottom-left and +Y pointing up.
	Matrix m = Matrix(1);
	m[0]     = Vec4(2.0f / window.getWidth(), 0.0f, 0.0f, 0.0f);
	m[1]     = Vec4(0.0f, 2.0f / window.getHeight(), 0.0f, 0.0f);
	m[2]     = Vec4(0.0f, 0.0f, 1.0f / 100000.0f, 0.0f);
	m[3]     = Vec4(-1.0f, -1.0f, 0.0, 1.0);
	return m;
}

void Renderer::setColor(const Color color)
{
	uniforms().setCustomColor(color);
}

void Renderer::pollGraphicsApiError()
{
	if (const UInt error = OpenGL::apiGetError(openGlContext)) logError("Opengl Error: " + toString(error));
}

void Renderer::renderMesh(const Name& name)
{
	meshSystem.render(*this, name);
}

void Renderer::useTexture(const Name& name)
{
	textureSystem.use(*this, name);
}

Index Renderer::useShader(const Name& name)
{
	return shaderSystem.use(*this, name);
}

void Renderer::line(const Vec2Vector& line)
{
	primitivesRenderer.line(*this, line);
}

void Renderer::line(const Vec3Vector& line)
{
	primitivesRenderer.line(*this, line);
}

void Renderer::renderMesh(const Index meshId)
{
	meshSystem.render(*this, meshId);
}

void Renderer::aspectCorrectNormalizedSpace()
{
	const Float aspect = getAspectRatio();
	uniforms().setVMatrix(Matrix(1));
	if (aspect >= 1.0f) uniforms().setPMatrix(glm::ortho(-aspect, aspect, -1.0f, 1.0f, 1.0f, -1.0f));
	else uniforms().setPMatrix(glm::ortho(-1.0f, 1.0f, -1 / aspect, 1 / aspect, 1.0f, -1.0f));
}

Matrix Renderer::getScreenSpaceMatrix() const
{
	const auto width  = static_cast<Float>(renderRegion.getWidth());
	const auto height = static_cast<Float>(renderRegion.getHeight());
	return screenSpaceMatrix(width, height);
}

void Renderer::lines(const Vec2Vector& lines)
{
	primitivesRenderer.lines(*this, lines);
}

void Renderer::lines(const Vec3Vector& lines)
{
	primitivesRenderer.lines(*this, lines);
}

void Renderer::setCulling(const Bool culling)
{
	OpenGL::apiSetCulling(openGlContext, culling);
}

void Renderer::renderSky(const Camera& camera)
{
	setCulling(false);
	setDepthTest(false);
	camera.renderOnlyRot(*this);
	useShader("sky");
	useTexture("sky");
	renderMesh("sky");
	setCulling(true);
	setDepthTest(true);
}

void Renderer::points(const Vec2Vector& points)
{
	primitivesRenderer.points(*this, points);
}

void Renderer::points(const Vec3Vector& points)
{
	primitivesRenderer.points(*this, points);
}

void Renderer::fullScreenShader(const Name& name)
{
	setWireframeMode(false);
	useShader(name);
	meshSystem.renderFullscreen(*this);
	setWireframeMode(wireframeMode);
}

void Renderer::clearBackground(const Color color)
{
	OpenGL::apiSetClearColor(openGlContext, color);
	OpenGL::apiClearColor(openGlContext);
	OpenGL::apiClearDepth(openGlContext);
}

void Renderer::setAlphaBlending(const Bool blending)
{
	if (openGlContext.getBlending() != blending)
	{
		if (blending)
		{
			OpenGL::apiSetBlending(openGlContext, true);
			OpenGL::apiBlendFunc(openGlContext, BlendFunction::SrcAlpha, BlendFunction::OneMinusSrcAlpha);
			OpenGL::apiBlendEquation(openGlContext, BlendEquation::Add);
		}
		else OpenGL::apiSetBlending(openGlContext, false);
	}
}

void Renderer::setDepthTest(const Bool isUsingDepth)
{
	if (openGlContext.getDepthTest() != isUsingDepth){
		OpenGL::apiSetDepthTest(openGlContext, isUsingDepth);
	}
}

void Renderer::line(const Vec3 start, const Vec3 end)
{
	const Vec3Vector line = {start, end};
	primitivesRenderer.line(*this, line);
}

void Renderer::line(const Vec2 start, const Vec2 end)
{
	const Vec3Vector line = {Vec3(start, 0.0f), Vec3(end, 0.0f)};
	primitivesRenderer.line(*this, line);
}

void Renderer::wireframeCubeCentered(const Float size)
{
	uniforms().setMMatrix(Matrix(size));
	primitivesRenderer.wireframeCubeCentered(*this);
}

void Renderer::setWireframeMode(const Bool wireframeMode)
{
	if(openGlContext.getWireframeMode() != wireframeMode){
		if (wireframeMode) OpenGL::apiPolygonMode(openGlContext, GL_FRONT_AND_BACK, GL_LINE);
		else OpenGL::apiPolygonMode(openGlContext, GL_FRONT_AND_BACK, GL_FILL);
	}
}

void Renderer::arrow(const Vec2 start, const Vec2 end)
{
	arrow(Vec3(start, 0), Vec3(end, 0));
}

void Renderer::arrow(const Vec3 start, const Vec3 end)
{
	useShader("color");
	fill(Vec4(1, 0, 0, 1));
	const Vec3 direction = end - start;
	uniforms().setMMatrix(matrixFromDirectionAndPosition(direction, start));
	renderMesh("arrow");
}

void Renderer::circles(const Vector<Matrix>& matrices)
{
	primitivesRenderer.circles(*this, matrices);
}

void Renderer::circle(const Vec2 pos, const Float radius)
{
	uniforms().setMMatrix(matrixFromPositionAndSize(pos, radius));
	renderMesh("circle");
}

void Renderer::setRenderRegion(const TiledRectangle region)
{
	renderRegion.set(*this, region);
}

void Renderer::wireframeCubes(const Vector<Matrix>& matrices)
{
	primitivesRenderer.wireframeCubes(*this, matrices);
}

void Renderer::fill(const Float r, const Float g, const Float b)
{
	fill(Vec4(r, g, b, 1));
}

void Renderer::useTexture(const Name& name, const Index position)
{
	textureSystem.use(*this, name, position);
}

void Renderer::line(const Vec2Vector& line, const Matrix& matrix)
{
	uniforms().setMMatrix(matrix);
	primitivesRenderer.line(*this, line);
}

void Renderer::lines(const Vec2Vector& lines, const Matrix& matrix)
{
	uniforms().setMMatrix(matrix);
	primitivesRenderer.lines(*this, lines);
}

void Renderer::setScissorTest(const IVec2 position, const IVec2 size)
{
	OpenGL::apiSetScissorTest(openGlContext, true);
	OpenGL::apiScissor(openGlContext, position.x, position.y, size.x, size.y);
}

void Renderer::setRenderRegion(const IVec2 position, const IVec2 size)
{
	renderRegion.set(*this, TiledRectangle(position, size));
}

void Renderer::normalizedSpaceWithAspectRatio(const Float aspectRatio)
{
	const Float renderAspectRatio = getAspectRatio();

	Vec2 view;

	if (aspectRatio >= 1.0f)
	{
		if (renderAspectRatio >= aspectRatio)
		{
			view.x = renderAspectRatio;
			view.y = 1.0f;
		}
		else
		{
			view.x = aspectRatio;
			view.y = 1.0f / renderAspectRatio * aspectRatio;
		}
	}
	else
	{
		if (renderAspectRatio >= aspectRatio)
		{
			view.x = renderAspectRatio * 1.0f / aspectRatio;
			view.y = 1.0f / aspectRatio;
		}
		else
		{
			view.x = 1.0f;
			view.y = 1.0f / renderAspectRatio;
		}
	}

	uniforms().setVMatrix(Matrix(1.0f));
	constexpr Matrix pMatrix(1.0f);
	uniforms().setPMatrix(scale(pMatrix, Vec3(Vec2(1.0f) / view, 1.0f)));
}

void Renderer::setFramebufferSize(Renderer& renderer, const IVec2 area)
{
	idFramebuffer.resize(renderer, area);
	postProcessFramebuffer.resize(renderer, area);
}

//void Renderer::addRenderObject(const RenderObjectNames& renderObjectNames)
//{
//	renderObjectSystem.addRenderObject(renderObjectNames);
//}

void Renderer::renderAtmosphere(const Player& player, const Vec3 sunDirection)
{
	const Bool culling = getCulling();
	setDepthTest(false);
	player.camera.renderOnlyRot(*this);
	uniforms().setCameraPos(Vec4(Vec3(0), 1));
	setCulling(false);
	uniforms().setSunDir(Vec4(sunDirection, 1));
	useShader("atmosphere");
	renderMesh("centeredCube");
	setCulling(culling);
	setDepthTest(true);
}

void Renderer::fill(const Float r, const Float g, const Float b, const Float a)
{
	fill(Vec4(r, g, b, a));
}

void Renderer::renderMeshInstanced(const Name& name, const Vec2Vector& positions)
{
	renderMeshInstanced(name, matrixArrayFromPositions(positions));
}

void Renderer::renderMeshInstanced(const Name& name, const Vec3Vector& positions)
{
	renderMeshInstanced(name, matrixArrayFromPositions(positions));
}

void Renderer::renderMeshInstanced(const Name& name, const Vector<Matrix>& transforms)
{
	meshSystem.renderInstanced(*this, name, transforms);
}

void Renderer::renderMeshInstanced(const Name& name, const Vec3Vector& positions, const Float size)
{
	renderMeshInstanced(name, matrixArrayFromPositionsAndSize(positions, size));
}

void Renderer::postProcess(const Name& name, const Framebuffer& source, const Framebuffer& destination)
{
	// draw to second buffer
	WireframeOverride wireframeGuard(*this, false);
	DepthTestOverride depthOverride(*this, false);

	setRenderRegion(TiledRectangle(destination.getWidth(), destination.getHeight()));

	uniforms().resetMatrices();

	source.bindRead(*this);
	source.setAsTexture(*this, 0);

	destination.bindDraw(*this);
	destination.clear(*this);

	useShader(name);
	renderMesh("fullScreenQuad");
}

void Renderer::setAlphaBlending(const Bool enable, const BlendFunction src, const BlendFunction dst, const BlendEquation eq)
{
	OpenGL::apiSetBlending(openGlContext, enable);
	OpenGL::apiBlendFunc(openGlContext, src, dst);
	OpenGL::apiBlendEquation(openGlContext, eq);
}

void Renderer::dispatchCompute(const UInt numGroupsX, const UInt numGroupsY, const UInt numGroupsZ)
{
	uniforms().upload(openGlContext);
	OpenGL::apiDispatchCompute(openGlContext, numGroupsX, numGroupsY, numGroupsZ);
}

void Renderer::memoryBarrier(const UInt barriers)
{
	OpenGL::apiMemoryBarrier(openGlContext, barriers);
}

void Renderer::bindImageTexture(const UInt unit, const TextureID texture, const ImageAccess access, const SizedInternalFormat format)
{
	OpenGL::apiBindImageTexture(openGlContext, unit, texture, 0, false, 0, access, format);
}

void Renderer::setProgramPointSize(const Bool enabled)
{
	if (enabled) OpenGL::apiEnable(openGlContext, EnableParameters::ProgramPointSize);
	else OpenGL::apiDisable(openGlContext, EnableParameters::ProgramPointSize);
}

void Renderer::drawArrays(const PrimitiveMode mode, const Int first, const Int count)
{
	uniforms().upload(openGlContext);
	OpenGL::VertexArray::apiDrawArrays(openGlContext, mode, first, count);
}

void Renderer::renderMeshInstanced(const Name& name, const Vec2Vector& positions, const Vec2Vector& directions, const Vector<Float>& scales)
{
	renderMeshInstanced(name, matrixArrayFromPositionsDirectionsAndSizes(positions, directions, scales));
}

void Renderer::renderMeshInstanced(const Name& name, const Vec3Vector& positions, const Vec2Vector& directions, const Vector<Float>& scales)
{
	renderMeshInstanced(name, matrixArrayFromPositionsDirectionsAndSizes(positions, directions, scales));
}

void Renderer::text(const String& text)
{
	textRenderSystem.render(*this, text);
}

void Renderer::text(const String& text, const Vec2& position)	
{
	textRenderSystem.render(*this, text, position);
}

void Renderer::fontSize(const Float size)
{
	textRenderSystem.setSize(size);
}

void Renderer::letterSpacing(const Float spacing)
{
	textRenderSystem.setLetterSpacing(spacing);
}