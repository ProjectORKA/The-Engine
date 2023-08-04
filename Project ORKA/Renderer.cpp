#include "Renderer.hpp"
#include "Random.hpp"
#include "Camera.hpp" // remove
#include "Player.hpp" // remove
#include "Profiler.hpp"

void Renderer::end()
{
	OPTICK_EVENT();
	setWireframeMode(false);
	setAlphaBlending(false);
	setRenderRegion(TiledRectangle(windowSize));
	drawToWindow();
	pollGraphicsAPIError();
	OpenGL::apiFinish();
	mutex.unlock();
	frameCount++;
}

void Renderer::sync()
{
	mutex.lock();
	mutex.unlock();
}

void Renderer::destroy()
{
	idFramebuffer.destroy();
	postProcessFramebuffer.destroy();

	textRenderSystem.destroy();
	renderObjectSystem.destroy();

	meshSystem.destroy();
	shaderSystem.destroy();
	textureSystem.destroy();
}

void Renderer::screenSpace()
{
	uniforms().setVMatrix(getScreenSpaceMatrix());
	uniforms().setPMatrix(Matrix(1));
}

void Renderer::rerenderMesh()
{
	// simply renders the previous mesh again (saves performance)
	meshSystem.currentMesh().render(uniforms());
}

Area Renderer::getArea() const
{
	return windowSize;
}

Uniforms& Renderer::uniforms()
{
	return shaderSystem.uniforms;
}

void Renderer::normalizedSpace()
{
	uniforms().setVMatrix(Matrix(1));
	uniforms().setPMatrix(Matrix(1));
}

Bool Renderer::getCulling() const
{
	return openGlState.culling;
}

Float Renderer::deltaTime() const
{
	return time.delta;
}

void Renderer::drawToWindow() const
{
	OpenGL::apiBindDrawFramebuffer(0);
}

Area Renderer::getWindowSize() const
{
	return windowSize;
}

Int Renderer::getWindowWidth() const
{
	return windowSize.x;
}

Int Renderer::getWindowHeight() const
{
	return windowSize.y;
}

void Renderer::fill(const Vec3 color)
{
	fill(Vec4(color, 1));
}

void Renderer::fill(const Vec4 color)
{
	uniforms().setCustomColor(color);
}

void Renderer::normalizedScreenSpace()
{
	const auto width  = static_cast<Float>(getWindowWidth());
	const auto height = static_cast<Float>(getWindowHeight());

	uniforms().setVMatrix(scale(Matrix(1), Vec3(2.0f / width, 2.0f / height, 0.0f)));
	uniforms().setPMatrix(Matrix(1));
}

Float Renderer::getAspectRatio() const
{
	return renderRegion.getAspectRatio();
}

void Renderer::setWireframeMode() const
{
	setWireframeMode(wireframeMode);
}

void Renderer::screenSpaceFromTopLeft()
{
	const auto width  = static_cast<Float>(getWindowWidth());
	const auto height = static_cast<Float>(getWindowHeight());
	auto       matrix = Matrix(1);
	matrix            = translate(matrix, Vec3(-1, 1, 0));
	matrix            = scale(matrix, Vec3(2, 2, 0));
	matrix            = scale(matrix, Vec3(1.0f / width, 1.0f / height, 0.0f));
	uniforms().setVMatrix(Matrix(1));
	uniforms().setPMatrix(matrix);
}

DepthTestOverride::~DepthTestOverride()
{
	this->renderer->setDepthTest(stored);
}

void Renderer::blendModeAdditive() const
{
	OpenGL::apiSetBlending(true);
	OpenGL::apiBlendFunc(BlendFunction::One, BlendFunction::One);
	OpenGL::apiBlendEquation(BlendEquation::Add);
}

void Renderer::setColor(const Color color)
{
	uniforms().setCustomColor(color);
}

void Renderer::begin(const Area windowSize)
{
	time.update(); // advances the time

	mutex.lock(); // used for syncronizing other threads
	setFramebufferSize(windowSize);
	setRenderRegion(TiledRectangle(windowSize));

	uniforms().setWindowWidth(windowSize.x);
	uniforms().setWindowHeight(windowSize.y);
	uniforms().setTime(time.total); // makes time available to shaders
	drawToWindow();
}

void Renderer::pollGraphicsAPIError() const
{
	const UInt error = OpenGL::apiGetError();
	if(error) logError("Opengl Error: " + toString(error));
}

void Renderer::renderMesh(const Index meshId)
{
	meshSystem.render(uniforms(), meshId);
}

void Renderer::aspectCorrectNormalizedSpace()
{
	const Float aspect = getAspectRatio();
	uniforms().setVMatrix(Matrix(1));
	if(aspect >= 1.0f) uniforms().setPMatrix(glm::ortho(-aspect, aspect, -1.0f, 1.0f, 1.0f, -1.0f));
	else uniforms().setPMatrix(glm::ortho(-1.0f, 1.0f, -1 / aspect, 1 / aspect, 1.0f, -1.0f));
}

Matrix Renderer::getScreenSpaceMatrix() const
{
	const auto width  = static_cast<Float>(getWindowWidth());
	const auto height = static_cast<Float>(getWindowHeight());
	return screenSpaceMatrix(width, height);
}

void Renderer::lines(const Vector<Line3D>& lines)
{
	lineRenderer.renderLines(*this, lines);
}

void Renderer::setFramebufferSize(const Area area)
{
	windowSize = area;
	idFramebuffer.resize(area);
	postProcessFramebuffer.resize(area);
}

void Renderer::setCulling(const Bool culling) const
{
	OpenGL::apiSetCulling(culling);
}

void Renderer::clearBackground(const Color color) const
{
	OpenGL::apiSetClearColor(color);
	OpenGL::apiClearColor();
	OpenGL::apiClearDepth();
}

void Renderer::create(ResourceManager& resourceManager)
{
	OpenGL::apiInit();

	OpenGL::apiSetDebugging(true);

	randomizeSeed();
	// basic systems
	time.reset();
	textureSystem.create();
	meshSystem.create();
	shaderSystem.create(resourceManager);
	lineRenderer.create();

	// advanced systems
	textRenderSystem.create(resourceManager, *this);
	renderObjectSystem.create(*this);
	rectangleRenderer.create(resourceManager, *this);

	idFramebuffer.create("IDFramebuffer", Area(1920, 1080));
	idFramebuffer.add(WritePixelsFormat::RGBAInteger, DataType::UInt, FramebufferAttachment::Color0, true, Wrapping::Clamped);
	idFramebuffer.add(WritePixelsFormat::Depth, DataType::Float, FramebufferAttachment::Depth, false, Wrapping::Clamped);
	idFramebuffer.checkComplete();

	postProcessFramebuffer.create("PostProcessFramebuffer", Area(1920, 1080));
	postProcessFramebuffer.add(WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
	postProcessFramebuffer.add(WritePixelsFormat::Depth, DataType::Float, FramebufferAttachment::Depth, false, Wrapping::Clamped);
	postProcessFramebuffer.checkComplete();

	OpenGL::apiClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
	OpenGL::apiSetScissorTest(true);
}

void Renderer::setDepthTest(const Bool isUsingDepth) const
{
	OpenGL::apiSetDepthTest(isUsingDepth);
}

void Renderer::setAlphaBlending(const Bool blending) const
{
	if(blending)
	{
		OpenGL::apiSetBlending(true);
		OpenGL::apiBlendFunc(BlendFunction::SrcAlpha, BlendFunction::OneMinusSrcAlpha);
		OpenGL::apiBlendEquation(BlendEquation::Add);
	}
	else
	{
		OpenGL::apiSetBlending(false);
	}
}

void Renderer::setRenderRegion(const TiledRectangle region)
{
	renderRegion.set(region);
}

void Renderer::setWireframeMode(const Bool wireframeMode) const
{
	if(wireframeMode) OpenGL::apiPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else OpenGL::apiPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Renderer::fill(const Float r, const Float g, const Float b)
{
	fill(Vec4(r, g, b, 1));
}

void Renderer::normalizedSpaceWithAspectRatio(const Float aspectRatio)
{
	const Float renderAspectRatio = getAspectRatio();

	Vec2 view;

	if(aspectRatio >= 1.0f)
	{
		if(renderAspectRatio >= aspectRatio)
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
		if(renderAspectRatio >= aspectRatio)
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

void Renderer::line(const Vec3 start, const Vec3 end, const Float width)
{
	lineRenderer.renderLine(*this, start, end, width);
}

void Renderer::line(const Vec2 start, const Vec2 end, const Float width)
{
	lineRenderer.renderLine(*this, Vec3(start, 0.0f), Vec3(end, 0.0f), width);
}

void Renderer::useMesh(ResourceManager& resourceManager, const Name& name)
{
	// selects a mesh to be rendered but doesn't render it
	meshSystem.use(resourceManager, name);
}

void Renderer::addRenderObject(const RenderObjectNames& renderObjectNames)
{
	renderObjectSystem.addRenderObject(renderObjectNames);
}

DepthTestOverride::DepthTestOverride(const Bool value, Renderer& renderer)
{
	this->renderer = &renderer;
	stored         = openGlState.depthTest;
	this->renderer->setDepthTest(value);
}

void Renderer::renderMesh(ResourceManager& resourceManager, const Name& name)
{
	meshSystem.render(resourceManager, uniforms(), name);
}

void Renderer::useTexture(ResourceManager& resourceManager, const Name& name)
{
	textureSystem.use(resourceManager, name);
}

Index Renderer::useShader(ResourceManager& resourceManager, const Name& name)
{
	return shaderSystem.use(resourceManager, name);
}

void Renderer::renderSky(ResourceManager& resourceManager, const Camera& camera)
{
	setCulling(false);
	setDepthTest(false);
	camera.renderOnlyRot(*this);
	useShader(resourceManager, "sky");
	useTexture(resourceManager, "sky");
	renderMesh(resourceManager, "sky");
	setCulling(true);
	setDepthTest(true);
}

void Renderer::fullScreenShader(ResourceManager& resourceManager, const Name& name)
{
	setWireframeMode(false);
	useShader(resourceManager, name);
	meshSystem.renderFullscreen(uniforms());
	setWireframeMode(wireframeMode);
}

void Renderer::arrow(ResourceManager& resourceManager, const Vec2 start, const Vec2 end)
{
	arrow(resourceManager, Vec3(start, 0), Vec3(end, 0));
}

void Renderer::arrow(ResourceManager& resourceManager, const Vec3 start, const Vec3 end)
{
	useShader(resourceManager, "color");
	uniforms().setCustomColor(Vec4(1, 0, 0, 1));
	const Float       length    = distance(start, end) / 20;
	const Vec3        direction = end - start;
	const Orientation o(direction, Vec3(0, 0, 1));
	uniforms().setMMatrix(matrixFromOrientation(o, start, length));
	renderMesh(resourceManager, "arrow");
}

void Renderer::circle(ResourceManager& resourceManager, const Vec2 pos, const Float radius)
{
	uniforms().setMMatrix(matrixFromLocationAndSize(pos, radius));
	renderMesh(resourceManager, "circle");
}

void Renderer::rectangle(ResourceManager& resourceManager, const Vec2 pos, const Vec2 size)
{
	rectangleRenderer.render(resourceManager, *this, pos, size);
}

void Renderer::useTexture(ResourceManager& resourceManager, const Name& name, const Index location)
{
	textureSystem.use(resourceManager, name, location);
}

void Renderer::renderAtmosphere(ResourceManager& resourceManager, const Player& player, const Vec3 sunDirection)
{
	const Bool culling = getCulling();
	setDepthTest(false);
	player.camera.renderOnlyRot(*this);
	uniforms().setCameraPos(Vec4(Vec3(0), 1));
	setCulling(false);
	uniforms().setSunDir(Vec4(sunDirection, 1));
	useShader(resourceManager, "atmosphere");
	/*framebufferSystem.current().colorTexture.use(0);*/
	renderMesh(resourceManager, "centeredCube");
	setCulling(culling);
	setDepthTest(true);
}

void Renderer::renderMeshInstanced(ResourceManager& resourceManager, const Name& name, const Vector<Matrix>& transforms)
{
	meshSystem.renderInstanced(resourceManager, uniforms(), name, transforms);
}

void Renderer::setAlphaBlending(const Bool enable, const BlendFunction src, const BlendFunction dst, const BlendEquation eq) const
{
	OpenGL::apiSetBlending(enable);
	OpenGL::apiBlendFunc(src, dst);
	OpenGL::apiBlendEquation(eq);
}

void Renderer::postProcess(ResourceManager& resourceManager, const Name& name, const Framebuffer& source, const Framebuffer& destination)
{
	// draw to second buffer
	DepthTestOverride depthOverride(false, *this);

	setRenderRegion(TiledRectangle(destination.getWidth(), destination.getHeight()));

	uniforms().resetMatrices();

	source.bindRead();
	source.setAsTexture(0);

	destination.bindDraw();
	destination.clear();

	useShader(resourceManager, name);
	renderMesh(resourceManager, "fullScreenQuad");
}
