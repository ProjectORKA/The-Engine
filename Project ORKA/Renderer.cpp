#include "Renderer.hpp"
#include "Random.hpp"
#include "Player.hpp"
#include "Camera.hpp"

void Renderer::end()
{
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
	planetRenderSystem.destroy();
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
void Renderer::create(Area size)
{
	randomizeSeed();
	//basic systems
	time.reset();
	textureSystem.create();
	meshSystem.create();
	shaderSystem.create();
	lineRenderer.create();
	
	//advanced systems
	framebufferSystem.create(*this, size);
	textRenderSystem.create(*this);
	renderObjectSystem.create(*this);
	idFramebuffer.create(*this); //needs to be below framebuffer system

	apiClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
	apiEnable(GL_SCISSOR_TEST);
}
void Renderer::useMesh(Name name) {
	//selects a mesh to be rendered but doesent render it
	meshSystem.use(name);
}
void Renderer::setWireframeMode() {
	setWireframeMode(wireframeMode);
}
void Renderer::renderMesh(Name name) {
	meshSystem.render(uniforms(), name);
}
void Renderer::useTexture(Name name) {
	textureSystem.use(name);
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
void Renderer::renderSky(Camera& camera) {
	setCulling(false);
	setDepthTest(false);
	camera.renderOnlyRot(*this);
	useShader("sky");
	useTexture("sky");
	renderMesh("sky");
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
void Renderer::arrow(Vec3 start, Vec3 end) {
	useShader("color");
	uniforms().customColor(Vec4(1, 0, 0, 1));
	Float length = distance(start, end) / 20;
	//if (length >= 0.075) return; //[TODO] erase this
	Vec3 direction = end - start;
	Orientation o(direction, Vec3(0, 0, 1));
	uniforms().mMatrix() = matrixFromOrientation(o, start, length);
	renderMesh("arrow");
}
void Renderer::apectCorrectNormalizedSpace() {
	Float aspect = aspectRatio();
	uniforms().vMatrix() = Matrix(1);
	if (aspect > 1) uniforms().pMatrix() = scale(Matrix(1), Vec3(1 / aspectRatio(), 1, 1));
	else uniforms().pMatrix() = scale(Matrix(1), Vec3(1, aspectRatio(), 1));
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
void Renderer::circle(Vec2 pos, Float radius)
{
	uniforms().mMatrix(matrixFromLocationAndSize(pos,radius));
	renderMesh("circle");
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
void Renderer::useTexture(Name name, Index location) {
	textureSystem.use(name, location);
}
void Renderer::line(Vec3 start, Vec3 end, Float width) {
	lineRenderer.renderLine(*this, start, end, width);
}
void Renderer::line(Vec2 start, Vec2 end, Float width) {
	lineRenderer.renderLine(*this, Vec3(start,0.0f), Vec3(end,0.0f), width);
}

void Renderer::createBlurTexture(Index from, Index to)
{
	//this function renders a blurred version of a framebuffer to another framebuffer
	Index originalFramebufferID = framebufferSystem.currentDrawFramebufferIndex;

	useShader("blur");
	framebufferSystem.framebuffers[from].setAsTexture(0);
	framebufferSystem.draw(*this, to);
	renderMesh("fullScreenQuad");

	framebufferSystem.draw(*this, originalFramebufferID);
}
void Renderer::renderAtmosphere(Player& player, Vec3 sunDirection) {
	Bool culling = getCulling();
	setDepthTest(false);
	player.camera.renderOnlyRot(*this);
	uniforms().cameraPos() = Vec4(Vec3(0), 1);
	setCulling(false);
	uniforms().sunDir() = Vec4(sunDirection, 1);
	useShader("atmosphere");
	/*framebufferSystem.current().colorTexture.use(0);*/
	renderMesh("centeredCube");
	setCulling(culling);
	setDepthTest(true);
}
void Renderer::addRenderObject(RenderObjectNames renderObjectNames)
{
	renderObjectSystem.addRenderObject(renderObjectNames);
}
void Renderer::renderText(String text, Vec2 position, FontStyle font) {
	textRenderSystem.render(*this, text, position, font);
}
//void Renderer::renderMeshInstanced(Name name, Vector<Vec4>& transformations)
//{
//	if (transformations.size()) meshSystem.renderInstanced(uniforms(), name, transformations);
//}

void Renderer::renderMeshInstanced(Name name)
{
	meshSystem.renderInstanced(uniforms(), name, matrixSystem.modelMatrixArray);
}

//void Renderer::renderMeshInstanced(Name name, Vector<Vec2>& positions)
//{
//	if (positions.size()) meshSystem.renderInstanced(uniforms(), name, positions);
//}

Bool Renderer::getCulling() {
	return apiGetCullFace();
}
Float Renderer::deltaTime() {
	return time.delta;
}
Float Renderer::aspectRatio()
{
	return framebufferSystem.currentDraw().aspectRatio();
}
Uniforms& Renderer::uniforms()
{
	return shaderSystem.uniforms;
}
Index Renderer::useShader(Name name) {
	return shaderSystem.use(name);
}