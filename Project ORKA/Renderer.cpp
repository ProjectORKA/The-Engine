#include "Renderer.hpp"
#include "Random.hpp"
#include "Camera.hpp" // remove
#include "Player.hpp" // remove
#include "Profiler.hpp"

void Renderer::end()
{
	OPTICK_EVENT();
	setWireframeMode(false);
	setAlphaBlending(true);
	setRenderRegion(TiledRectangle(windowSize));
	drawToWindow();
	pollGraphicsApiError();
	OpenGL::apiFinish();
	mutex.unlock();
	frameCount++;
}

void Renderer::sync()
{
	mutex.lock();
	mutex.unlock();
}

void Renderer::plane()
{
	primitivesRenderer.plane(uniforms());
}

void Renderer::rectangle()
{
	primitivesRenderer.rectangle(uniforms());
}

void Renderer::create()
{
	OpenGL::apiInit(openGlContext);

	OpenGL::apiSetDebugging(openGlContext, true);

	if (printDeviceInfo) openGlContext.printOpenGLInfo();

	randomizeSeed();
	// basic systems
	time.reset();
	textureSystem.create(*this);
	meshSystem.create(*this);
	shaderSystem.create();
	primitivesRenderer.create(*this);

	// advanced systems
	textRenderSystem.create(*this);
	renderObjectSystem.create(*this);

	idFramebuffer.create("IDFramebuffer", Area(1920, 1080));
	idFramebuffer.add(*this, WritePixelsFormat::RGBAInteger, DataType::UInt, FramebufferAttachment::Color0, true, Wrapping::Clamped);
	idFramebuffer.add(*this, WritePixelsFormat::Depth, DataType::Float, FramebufferAttachment::Depth, false, Wrapping::Clamped);
	idFramebuffer.checkComplete();

	postProcessFramebuffer.create("PostProcessFramebuffer", Area(1920, 1080));
	postProcessFramebuffer.add(*this, WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
	postProcessFramebuffer.add(*this, WritePixelsFormat::Depth, DataType::Float, FramebufferAttachment::Depth, false, Wrapping::Clamped);
	postProcessFramebuffer.checkComplete();

	OpenGL::apiClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
	OpenGL::apiSetScissorTest(openGlContext, true);
}

void Renderer::destroy()
{
	primitivesRenderer.destroy(*this);

	idFramebuffer.destroy(*this);
	postProcessFramebuffer.destroy(*this);

	textRenderSystem.destroy(*this);
	renderObjectSystem.destroy();

	meshSystem.destroy(*this);
	shaderSystem.destroy();
	textureSystem.destroy(*this);
}

void Renderer::screenSpace()
{
	uniforms().setVMatrix(getScreenSpaceMatrix());
	uniforms().setPMatrix(Matrix(1));
}

Area Renderer::getArea() const
{
	return windowSize;
}

Uniforms& Renderer::uniforms()
{
	return shaderSystem.uniforms;
}

void Renderer::wireframeCube()
{
	primitivesRenderer.wireframeCube(uniforms());
}

void Renderer::normalizedSpace()
{
	uniforms().setVMatrix(Matrix(1));
	uniforms().setPMatrix(Matrix(1));
}

Bool Renderer::getCulling() const
{
	return openGlContext.culling;
}

Float Renderer::deltaTime() const
{
	return time.getDelta();
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

void Renderer::fill(const Int color)
{
	fill(Vec4(Vec3(static_cast<Float>(color)), 1));
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

void Renderer::begin(const Area size)
{
	time.update(); // advances the time

	mutex.lock(); // used for synchronizing other threads
	setFramebufferSize(*this, size);
	setRenderRegion(TiledRectangle(size));

	uniforms().setWindowWidth(size.x);
	uniforms().setWindowHeight(size.y);
	uniforms().setTime(time.getDelta()); // makes time available to shaders
	drawToWindow();
}

void Renderer::fill(const Float color)
{
	fill(Vec4(Vec3(color), 1.0f));
}

void Renderer::normalizedScreenSpace()
{
	const auto width  = static_cast<Float>(getWindowWidth());
	const auto height = static_cast<Float>(getWindowHeight());

	uniforms().setVMatrix(scale(Matrix(1), Vec3(2.0f / width, 2.0f / height, 0.0f)));
	uniforms().setPMatrix(Matrix(1));
}

void Renderer::wireframeCubeCentered()
{
	primitivesRenderer.wireframeCubeCentered(uniforms());
}

Float Renderer::getAspectRatio() const
{
	return renderRegion.getAspectRatio();
}

void Renderer::fill(const Double color)
{
	fill(Vec4(Vec3(static_cast<Float>(color)), 1.0f));
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

void Renderer::blendModeAdditive()
{
	OpenGL::apiSetBlending(openGlContext, true);
	OpenGL::apiBlendFunc(BlendFunction::One, BlendFunction::One);
	OpenGL::apiBlendEquation(BlendEquation::Add);
}

void Renderer::useMesh(const Name& name)
{
	// selects a mesh to be rendered but doesn't render it
	meshSystem.use(*this, name);
}

void Renderer::setColor(const Color color)
{
	uniforms().setCustomColor(color);
}

void Renderer::pollGraphicsApiError() const
{
	if (const UInt error = OpenGL::apiGetError()) logError("Opengl Error: " + toString(error));
}

void Renderer::renderMesh(const Name& name)
{
	meshSystem.render(*this, uniforms(), name);
}

void Renderer::useTexture(const Name& name)
{
	textureSystem.use(*this, name);
}

Index Renderer::useShader(const Name& name)
{
	return shaderSystem.use(name);
}

void Renderer::renderMesh(const Index meshId)
{
	meshSystem.render(uniforms(), meshId);
}

void Renderer::aspectCorrectNormalizedSpace()
{
	const Float aspect = getAspectRatio();
	uniforms().setVMatrix(Matrix(1));
	if (aspect >= 1.0f)
	{
		uniforms().setPMatrix(glm::ortho(-aspect, aspect, -1.0f, 1.0f, 1.0f, -1.0f));
	}
	else
	{
		uniforms().setPMatrix(glm::ortho(-1.0f, 1.0f, -1 / aspect, 1 / aspect, 1.0f, -1.0f));
	}
}

Matrix Renderer::getScreenSpaceMatrix() const
{
	const auto width  = static_cast<Float>(renderRegion.getWidth());
	const auto height = static_cast<Float>(renderRegion.getHeight());
	return screenSpaceMatrix(width, height);
}

void Renderer::line(const Vec2Vector& line)
{
	primitivesRenderer.line(line, uniforms());
}

void Renderer::line(const Vec3Vector& line)
{
	primitivesRenderer.line(line, uniforms());
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

void Renderer::lines(const Vec2Vector& lines)
{
	primitivesRenderer.lines(lines, uniforms());
}

void Renderer::lines(const Vec3Vector& lines)
{
	primitivesRenderer.lines(lines, uniforms());
}

void Renderer::points(const Vec2Vector& points)
{
	primitivesRenderer.points(points, uniforms());
}

void Renderer::points(const Vec3Vector& points)
{
	primitivesRenderer.points(points, uniforms());
}

void Renderer::fullScreenShader(const Name& name)
{
	setWireframeMode(false);
	useShader(name);
	meshSystem.renderFullscreen(uniforms());
	setWireframeMode(wireframeMode);
}

void Renderer::setFramebufferSize(Renderer& renderer, const Area area)
{
	windowSize = area;
	idFramebuffer.resize(renderer, area);
	postProcessFramebuffer.resize(renderer, area);
}

void Renderer::setCulling(const Bool culling)
{
	OpenGL::apiSetCulling(openGlContext, culling);
}

void Renderer::line(const Vec3 start, const Vec3 end)
{
	const Vec3Vector line = {start, end};
	primitivesRenderer.line(line, uniforms());
}

void Renderer::line(const Vec2 start, const Vec2 end)
{
	const Vec3Vector line = {Vec3(start, 0.0f), Vec3(end, 0.0f)};
	primitivesRenderer.line(line, uniforms());
}

void Renderer::wireframeCubeCentered(const Float size)
{
	uniforms().setMMatrix(Matrix(size));
	primitivesRenderer.wireframeCubeCentered(uniforms());
}

void Renderer::setWireframeMode(const Bool mode) const
{
	if (mode)
	{
		OpenGL::apiPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		OpenGL::apiPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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
	primitivesRenderer.circles(matrices, uniforms());
}

void Renderer::clearBackground(const Color color)
{
	OpenGL::apiSetClearColor(openGlContext, color);
	OpenGL::apiClearColor();
	OpenGL::apiClearDepth();
}

void Renderer::circle(const Vec2 pos, const Float radius)
{
	uniforms().setMMatrix(matrixFromPositionAndSize(pos, radius));
	renderMesh("circle");
}

void Renderer::setDepthTest(const Bool isUsingDepth)
{
	OpenGL::apiSetDepthTest(openGlContext, isUsingDepth);
}

void Renderer::setAlphaBlending(const Bool blending)
{
	if (openGlContext.blending != blending)
	{
		if (blending)
		{
			OpenGL::apiSetBlending(openGlContext, true);
			OpenGL::apiBlendFunc(BlendFunction::SrcAlpha, BlendFunction::OneMinusSrcAlpha);
			OpenGL::apiBlendEquation(BlendEquation::Add);
		}
		else
		{
			OpenGL::apiSetBlending(openGlContext, false);
		}
	}
}

void Renderer::setRenderRegion(const TiledRectangle region)
{
	renderRegion.set(*this, region);
}

void Renderer::wireframeCubes(const Vector<Matrix>& matrices)
{
	primitivesRenderer.wireframeCubes(matrices, uniforms());
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
	primitivesRenderer.line(line, uniforms());
}

void Renderer::lines(const Vec2Vector& lines, const Matrix& matrix)
{
	uniforms().setMMatrix(matrix);
	primitivesRenderer.lines(lines, uniforms());
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

void Renderer::addRenderObject(const RenderObjectNames& renderObjectNames)
{
	renderObjectSystem.addRenderObject(renderObjectNames);
}

void Renderer::renderAtmosphere(const Player& player, const Vec3 sunDirection)
{
	const Bool culling = getCulling();
	setDepthTest(false);
	player.camera.renderOnlyRot(*this);
	uniforms().setCameraPos(Vec4(Vec3(0), 1));
	setCulling(false);
	uniforms().setSunDir(Vec4(sunDirection, 1));
	useShader("atmosphere");
	/*framebufferSystem.current().colorTexture.use(0);*/
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
	meshSystem.renderInstanced(*this, uniforms(), name, transforms);
}

void Renderer::renderMeshInstanced(const Name& name, const Vec3Vector& positions, const Float size)
{
	renderMeshInstanced(name, matrixArrayFromPositionsAndSize(positions, size));
}

void Renderer::postProcess(const Name& name, const Framebuffer& source, const Framebuffer& destination)
{
	// draw to second buffer
	DepthTestOverride depthOverride(*this, false);

	setRenderRegion(TiledRectangle(destination.getWidth(), destination.getHeight()));

	uniforms().resetMatrices();

	source.bindRead();
	source.setAsTexture(0);

	destination.bindDraw(*this);
	destination.clear();

	useShader(name);
	renderMesh("fullScreenQuad");
}

void Renderer::setAlphaBlending(const Bool enable, const BlendFunction src, const BlendFunction dst, const BlendEquation eq)
{
	OpenGL::apiSetBlending(openGlContext, enable);
	OpenGL::apiBlendFunc(src, dst);
	OpenGL::apiBlendEquation(eq);
}

void Renderer::renderMeshInstanced(const Name& name, const Vec2Vector& positions, const Vec2Vector& directions, const Vector<Float>& scales)
{
	renderMeshInstanced(name, matrixArrayFromPositionsDirectionsAndSizes(positions, directions, scales));
}
