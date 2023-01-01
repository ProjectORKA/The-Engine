
#include "Renderer.hpp"
#include "Profiler.hpp"
#include "Random.hpp"
#include "Camera.hpp" // remove
#include "Player.hpp" //remove
#include "Fonts.hpp"

void Renderer::end()
{
	OPTICK_EVENT();
	pollGraphicsAPIError();
	mutex.unlock();
	frameCount++;
}
void Renderer::sync()
{
	mutex.lock();
	mutex.unlock();
}
void Renderer::begin()
{
	mutex.lock();						//used for syncronizing other threads
	time.update();						//advances the time
	uniforms().time() = time.total;		//makes time available to shaders
	framebufferSystem.deselect();		//selects the backbuffer of the window
	clearColor(Color(Vec3(0), 0.0));	//and clears its contents
	clearDepth();						//clears depth as to not accidentally hide geometry
}
void Renderer::destroy()
{
	textRenderSystem.destroy();
	renderObjectSystem.destroy();

	meshSystem.destroy();
	shaderSystem.destroy();
	textureSystem.destroy();

	framebufferSystem.destroy();
}
void Renderer::clearDepth() {
	apiClearDepth();
}
void Renderer::clearColor()
{
	apiClearColor();
}
void Renderer::screenSpace() {
	Float width = renderRegion.region.size.x, height = renderRegion.region.size.y;
	Matrix matrix = Matrix(1);
	matrix = glm::translate(matrix, Vec3(-1, -1, 0));
	matrix = glm::scale(matrix, Vec3(2, 2, 0));
	matrix = glm::scale(matrix, Vec3(1.0 / width, 1.0 / height, 0));
	uniforms().vMatrix() = Matrix(1);
	uniforms().pMatrix() = matrix;
}
void Renderer::normalizedScreenSpace() {
	Float width = renderRegion.region.size.x;
	Float height = renderRegion.region.size.y;
	Matrix matrix = Matrix(1);
	matrix = glm::translate(matrix, Vec3(-1, -1, 0));
	matrix = glm::scale(matrix, Vec3(2, 2, 0));
	matrix = glm::scale(matrix, Vec3(1.0 / width, 1.0 / height, 0));

	uniforms().vMatrix() = matrix;
	uniforms().pMatrix() = Matrix(1);
}
void Renderer::rerenderMesh() {
	//simply renders the previous mesh again (saves performance)
	meshSystem.currentMesh().render(uniforms());
}
void Renderer::fill(Vec4 color) {
	uniforms().customColor(color);
}
void Renderer::normalizedSpace() {
	uniforms().vMatrix() = Matrix(1);
	uniforms().pMatrix() = Matrix(1);
}
void Renderer::create(Engine& engine, Area size)
{
	randomizeSeed();
	//basic systems
	time.reset();
	textureSystem.create();
	meshSystem.create(engine);
	shaderSystem.create(engine);
	lineRenderer.create();
	
	//advanced systems
	framebufferSystem.create(*this, size);
	textRenderSystem.create(engine, *this);
	renderObjectSystem.create(*this);
	idFramebuffer.create(*this); //needs to be below framebuffer system
	rectangleRenderer.create(engine, *this);

	apiClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
	apiEnable(GL_SCISSOR_TEST);
}
void Renderer::useMesh(Engine& engine, Name name) {
	//selects a mesh to be rendered but doesent render it
	meshSystem.use(engine,name);
}
void Renderer::setWireframeMode() {
	setWireframeMode(wireframeMode);
}
void Renderer::renderMesh(Engine& engine, Name name) {
	meshSystem.render(engine, uniforms(), name);
}
void Renderer::useTexture(Engine& engine, Name name) {
	textureSystem.use(engine, name);
}
void Renderer::setColor(Color color) {
	uniforms().customColor(color);
}
void Renderer::pollGraphicsAPIError() {
	Int error = apiGetError();
	if (error) {
		logError(String("Opengl Error: ").append(toString(error)));
	}
}
void Renderer::clearColor(Color color) {
	apiSetClearColor(color);
	apiClearColor();
}
void Renderer::renderMesh(Index meshID) {
	meshSystem.render(uniforms(), meshID);
}
void Renderer::renderSky(Engine& engine, Camera& camera) {
	setCulling(false);
	setDepthTest(false);
	camera.renderOnlyRot(*this);
	useShader(engine, "sky");
	useTexture(engine, "sky");
	renderMesh(engine, "sky");
	setCulling(true);
	setDepthTest(true);
}
void Renderer::setCulling(Bool isCulling) {
	if (isCulling) {
		apiEnable(GL_CULL_FACE);
		apiCullFace(GL_BACK);
	}
	else {
		apiDisable(GL_CULL_FACE);
	}
}
void Renderer::arrow(Engine& engine, Vec3 start, Vec3 end) {
	useShader(engine, "color");
	uniforms().customColor(Vec4(1, 0, 0, 1));
	Float length = distance(start, end) / 20;
	//if (length >= 0.075) return; //[TODO] erase this
	Vec3 direction = end - start;
	Orientation o(direction, Vec3(0, 0, 1));
	uniforms().mMatrix() = matrixFromOrientation(o, start, length);
	renderMesh(engine, "arrow");
}
void Renderer::aspectCorrectNormalizedSpace() {
	Float aspect = getAspectRatio();
	uniforms().vMatrix() = Matrix(1);
	if (aspect > 1) uniforms().pMatrix() = scale(Matrix(1), Vec3(1 / getAspectRatio(), 1, 1));
	else uniforms().pMatrix() = scale(Matrix(1), Vec3(1, getAspectRatio(), 1));
}
void Renderer::setDepthClamp(Bool depthClamp)
{
	if (depthClamp) {
		apiEnable(GL_DEPTH_CLAMP);
	}
	else {
		apiDisable(GL_DEPTH_CLAMP);
	}
}
void Renderer::circle(Engine& engine, Vec2 pos, Float radius)
{
	uniforms().mMatrix(matrixFromLocationAndSize(pos,radius));
	renderMesh(engine, "circle");
}
void Renderer::rectangle(Engine& engine, Vec2 pos, Vec2 size) {
	rectangleRenderer.render(engine, *this, pos, size);
}
void Renderer::renderMeshInstanced(Engine& engine, Name name)
{
	meshSystem.renderInstanced(engine, uniforms(), name, matrixSystem.modelMatrixArray);
}
void Renderer::setAlphaBlending(Bool blending)
{
	if (blending) {
		apiEnable(GL_BLEND);
		apiBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendEquation(GL_FUNC_ADD);
		//apiBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_DST_ALPHA);
		//apiBlendEquationSeparate(GL_FUNC_ADD, GL_MAX);
	}
	else apiDisable(GL_BLEND);
}
void Renderer::setDepthTest(Bool isUsingDepth) {
	if (isUsingDepth) {
		apiEnable(GL_DEPTH_TEST);
		apiDepthFunc(GL_LESS);
	}
	else {
		apiDisable(GL_DEPTH_TEST);
	}
}
void Renderer::setWireframeMode(Bool wireframeMode) {
	if (wireframeMode) {
		apiPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		apiPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}
void Renderer::useTexture(Engine& engine, Name name, Index location) {
	textureSystem.use(engine, name, location);
}
void Renderer::line(Vec3 start, Vec3 end, Float width) {
	lineRenderer.renderLine(*this, start, end, width);
}
void Renderer::line(Vec2 start, Vec2 end, Float width) {
	lineRenderer.renderLine(*this, Vec3(start,0.0f), Vec3(end,0.0f), width);
}
void Renderer::normalizedSpaceWithAspectRatio(Float aspectRatio) {
	Float renderAspectRatio = getAspectRatio();

	Vec2 view = Vec2(1);

	if (aspectRatio >= 1.0f) {
		view = Vec2(aspectRatio, 1);
		if (renderAspectRatio >= aspectRatio) {
			view.x = renderAspectRatio;
			view.y = 1;
		}
		else {
			view.x = aspectRatio;
			view.y = 1 / renderAspectRatio * aspectRatio;
		}
	}
	else {
		view = Vec2(1, 1 / aspectRatio);
		if (renderAspectRatio >= aspectRatio) {
			view.x = renderAspectRatio * 1 / aspectRatio;
			view.y = 1 / aspectRatio;
		}
		else {
			view.x = 1;
			view.y = 1 / renderAspectRatio;
		}
	}



	uniforms().vMatrix() = Matrix(1);
	Matrix pMatrix(1);
	uniforms().pMatrix() = scale(pMatrix, Vec3(Vec2(1) / view, 1));
}
void Renderer::renderAtmosphere(Engine& engine, Player& player, Vec3 sunDirection) {
	Bool culling = getCulling();
	setDepthTest(false);
	player.camera.renderOnlyRot(*this);
	uniforms().cameraPos() = Vec4(Vec3(0), 1);
	setCulling(false);
	uniforms().sunDir() = Vec4(sunDirection, 1);
	useShader(engine, "atmosphere");
	/*framebufferSystem.current().colorTexture.use(0);*/
	renderMesh(engine, "centeredCube");
	setCulling(culling);
	setDepthTest(true);
}
void Renderer::addRenderObject(RenderObjectNames renderObjectNames)
{
	renderObjectSystem.addRenderObject(renderObjectNames);
}
void Renderer::renderText(Engine& engine, String text, Vec2 position, FontStyle font) {
	textRenderSystem.render(engine, *this, text, position, font);
}

Area Renderer::getArea() {
	return framebufferSystem.windowSize;
}

Bool Renderer::getCulling() {
	return apiGetCullFace();
}
Float Renderer::deltaTime() {
	return time.delta;
}

Float Renderer::getAspectRatio(){
	return framebufferSystem.currentDraw().aspectRatio();
}

Uniforms& Renderer::uniforms()
{
	return shaderSystem.uniforms;
}

Index Renderer::useShader(Engine& engine, Name name) {
	return shaderSystem.use(engine, name);
}

void Renderer::postProcess(Engine& engine, Name name) {
	//draw to second buffer
	uniforms().resetMatrices();
	uniforms().customColor(Color(1, 0, 0, 1));
	read("main");
	framebufferSystem.currentRead().setAsTexture(0);
	draw("postProcess");
	clearColor();
	clearDepth();
	useShader(engine, name);
	renderMesh(engine, "fullScreenQuad");

	read("postProcess");
	framebufferSystem.currentRead().setAsTexture(0);
	draw("main");
	useShader(engine, "texture");
	renderMesh(engine, "fullScreenQuad");
}

void Renderer::fullScreenShader(Engine& engine, Name name) {
	useShader(engine, name);
	meshSystem.renderFullscreen(uniforms());
}

//framebuffers

void Renderer::read(Name name) {
	framebufferSystem.read(*this, name);
}

void Renderer::draw(Name name) {
	framebufferSystem.draw(*this, name);
}
