
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
	clearColor(Color(0, 0 ,0, 1));		//and clears its contents
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
	uniforms().vMatrix() = getScreenSpaceMatrix();
	uniforms().pMatrix() = Matrix(1);
}
void Renderer::rerenderMesh() {
	//simply renders the previous mesh again (saves performance)
	meshSystem.currentMesh().render(uniforms());
}
void Renderer::read(Name name) {
	framebufferSystem.read(*this, name);
}
void Renderer::draw(Name name) {
	framebufferSystem.draw(*this, name);
}
void Renderer::fill(Vec3 color) {
	fill(Vec4(color, 1));
}
void Renderer::fill(Vec4 color) {
	uniforms().customColor(color);
}
void Renderer::normalizedSpace() {
	uniforms().vMatrix() = Matrix(1);
	uniforms().pMatrix() = Matrix(1);
}
void Renderer::setWireframeMode() {
	setWireframeMode(wireframeMode);
}
void Renderer::blendModeAdditive() {
	apiSetBlending(true);
	apiBlendFunc(BlendFunction::one, BlendFunction::one);
	apiBlendEquation(BlendEquation::add);
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
void Renderer::setCulling(Bool value) {
	apiSetCulling(value);
}
void Renderer::clearColor(Color color) {
	apiSetClearColor(color);
	apiClearColor();
}
void Renderer::normalizedScreenSpace() {
	Float width = renderRegion.region.size.x;
	Float height = renderRegion.region.size.y;

	uniforms().vMatrix() = glm::scale(Matrix(1), Vec3(2.0 / width, 2.0 / height, 0));
	uniforms().pMatrix() = Matrix(1);
}
void Renderer::screenSpaceFromTopLeft() {
	Float width = renderRegion.region.size.x;
	Float height = renderRegion.region.size.y;
	Matrix matrix = Matrix(1);
	matrix = glm::translate(matrix, Vec3(-1, 1, 0));
	matrix = glm::scale(matrix, Vec3(2, 2, 0));
	matrix = glm::scale(matrix, Vec3(1.0 / width, 1.0 / height, 0));
	uniforms().vMatrix() = Matrix(1);
	uniforms().pMatrix() = matrix;
}
void Renderer::renderMesh(Index meshID) {
	meshSystem.render(uniforms(), meshID);
}
void Renderer::setDepthTest(Bool value) {
	apiSetDepthTest(value);
}
void Renderer::aspectCorrectNormalizedSpace() {
	Float aspect = getAspectRatio();
	uniforms().vMatrix() = Matrix(1);
	if (aspect > 1) uniforms().pMatrix() = scale(Matrix(1), Vec3(1 / getAspectRatio(), 1, 1));
	else uniforms().pMatrix() = scale(Matrix(1), Vec3(1, getAspectRatio(), 1));
}
void Renderer::setAlphaBlending(Bool blending)
{
	if (blending) {
		apiSetBlending(true);
		apiBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		apiBlendEquation(BlendEquation::add);
	}
	else apiSetBlending(false);
}
void Renderer::fill(Float r, Float g, Float b) {
	fill(Vec4(r, g, b, 1));
}
void Renderer::create(ResourceManager& resourceManager, Area size)
{
	randomizeSeed();
	//basic systems
	time.reset();
	textureSystem.create();
	meshSystem.create(resourceManager);
	shaderSystem.create(resourceManager);
	lineRenderer.create();
	
	//advanced systems
	framebufferSystem.create(*this, size);
	textRenderSystem.create(resourceManager, *this);
	renderObjectSystem.create(*this);
	idFramebuffer.create(*this); //needs to be below framebuffer system
	rectangleRenderer.create(resourceManager, *this);

	apiClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
	apiSetScissorTest(true);
}
void Renderer::useMesh(ResourceManager& resourceManager, Name name) {
	//selects a mesh to be rendered but doesent render it
	meshSystem.use(resourceManager,name);
}
void Renderer::setWireframeMode(Bool wireframeMode) {
	if (wireframeMode) {
		apiPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		apiPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}
void Renderer::renderMesh(ResourceManager& resourceManager, Name name) {
	meshSystem.render(resourceManager, uniforms(), name);
}
void Renderer::useTexture(ResourceManager& resourceManager, Name name) {
	textureSystem.use(resourceManager, name);
}
void Renderer::postProcess(ResourceManager& resourceManager, Name name) {
	//draw to second buffer
	DepthTestOverride(false,*this);

	uniforms().resetMatrices();
	read("main");
	framebufferSystem.currentRead().setAsTexture(0);
	draw("postProcess");
	clearColor();
	clearDepth();
	useShader(resourceManager, name);
	renderMesh(resourceManager, "fullScreenQuad");

	read("postProcess");
	framebufferSystem.currentRead().setAsTexture(0);
	draw("main");
	useShader(resourceManager, "texture");
	renderMesh(resourceManager, "fullScreenQuad");
}
void Renderer::line(Vec3 start, Vec3 end, Float width) {
	lineRenderer.renderLine(*this, start, end, width);
}
void Renderer::line(Vec2 start, Vec2 end, Float width) {
	lineRenderer.renderLine(*this, Vec3(start,0.0f), Vec3(end,0.0f), width);
}
void Renderer::renderSky(ResourceManager& resourceManager, Camera& camera) {
	setCulling(false);
	setDepthTest(false);
	camera.renderOnlyRot(*this);
	useShader(resourceManager, "sky");
	useTexture(resourceManager, "sky");
	renderMesh(resourceManager, "sky");
	setCulling(true);
	setDepthTest(true);
}
void Renderer::fullScreenShader(ResourceManager& resourceManager, Name name) {
	useShader(resourceManager, name);
	meshSystem.renderFullscreen(uniforms());
}
void Renderer::arrow(ResourceManager& resourceManager, Vec2 start, Vec2 end) {
	arrow(resourceManager, Vec3(start, 0), Vec3(end, 0));
}
void Renderer::arrow(ResourceManager& resourceManager, Vec3 start, Vec3 end) {
	useShader(resourceManager, "color");
	uniforms().customColor(Vec4(1, 0, 0, 1));
	Float length = distance(start, end) / 20;
	//if (length >= 0.075) return; //[TODO] erase this
	Vec3 direction = end - start;
	Orientation o(direction, Vec3(0, 0, 1));
	uniforms().mMatrix() = matrixFromOrientation(o, start, length);
	renderMesh(resourceManager, "arrow");
}
void Renderer::circle(ResourceManager& resourceManager, Vec2 pos, Float radius)
{
	uniforms().mMatrix(matrixFromLocationAndSize(pos,radius));
	renderMesh(resourceManager, "circle");
}
void Renderer::rectangle(ResourceManager& resourceManager, Vec2 pos, Vec2 size) {
	rectangleRenderer.render(resourceManager, *this, pos, size);
}
void Renderer::renderMeshInstanced(ResourceManager& resourceManager, Name name)
{
	meshSystem.renderInstanced(resourceManager, uniforms(), name, matrixSystem.modelMatrixArray);
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
void Renderer::addRenderObject(RenderObjectNames renderObjectNames)
{
	renderObjectSystem.addRenderObject(renderObjectNames);
}
void Renderer::useTexture(ResourceManager& resourceManager, Name name, Index location) {
	textureSystem.use(resourceManager, name, location);
}
void Renderer::renderAtmosphere(ResourceManager& resourceManager, Player& player, Vec3 sunDirection) {
	Bool culling = getCulling();
	setDepthTest(false);
	player.camera.renderOnlyRot(*this);
	uniforms().cameraPos() = Vec4(Vec3(0), 1);
	setCulling(false);
	uniforms().sunDir() = Vec4(sunDirection, 1);
	useShader(resourceManager, "atmosphere");
	/*framebufferSystem.current().colorTexture.use(0);*/
	renderMesh(resourceManager, "centeredCube");
	setCulling(culling);
	setDepthTest(true);
}
void Renderer::setAlphaBlending(Bool enable, BlendFunction src, BlendFunction dst, BlendEquation eq) {
	apiSetBlending(enable);
	apiBlendFunc(src, dst);
	apiBlendEquation(eq);
}

Area Renderer::getArea() {
	return framebufferSystem.windowSize;
}
Bool Renderer::getCulling() {
	return openglState.culling;
}
Float Renderer::deltaTime() {
	return time.delta;
}
Uniforms& Renderer::uniforms()
{
	return shaderSystem.uniforms;
}
Matrix Renderer::getScreenSpaceMatrix() {
	Float width = renderRegion.region.size.x;
	Float height = renderRegion.region.size.y;
	return screenSpaceMatrix(width, height);
}
Float Renderer::getAspectRatio(){
	return framebufferSystem.currentDraw().aspectRatio();
}
Index Renderer::useShader(ResourceManager& resourceManager, Name name) {
	return shaderSystem.use(resourceManager, name);
}