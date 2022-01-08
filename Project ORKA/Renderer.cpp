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
	mutex.lock();							//used for syncronizing other threads
	renderTime.update();					//advances the time
	uniforms().time() = renderTime.total;	//makes time available to shaders
	framebufferSystem.deselect();			//selects the backbuffer of the window
	clearColor(Color(Vec3(0), 0.0));		//and clears its contents
	clearDepth();							//clears depth as to not accidentally hide geometry
}
void Renderer::create()
{
	randomizeSeed();
	//basic systems
	renderTime.reset();
	textureSystem.create();
	meshSystem.create();
	shaderSystem.create();

	//advanced systems
	framebufferSystem.create(*this);
	textRenderSystem.create(*this);
	renderObjectSystem.create(*this);
	planetRenderSystem.create(*this);

	apiClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
	apiEnable(GL_SCISSOR_TEST);
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
void Renderer::renderMesh(Name name) {
	meshSystem.render(uniforms(),name);
}
void Renderer::useTexture(Name name) {
	textureSystem.use(name);
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
	if (length >= 0.075) return;
	Vec3 z = normalize(end - start);
	Vec3 x = cross(normalize(z),Vec3(0,0,1));
	Vec3 y = cross(normalize(z), x);

	Matrix m;
	m[0] = Vec4(length * x,0);
	m[1] = Vec4(length * y,0);
	m[2] = Vec4(length * z,0);
	m[3] = Vec4(start,1);

	uniforms().mMatrix() = m;
	renderMesh("arrow");



	//Vec3 dir = normalize(start - end);
	//Vec3 extend = cross(dir, Vec3(0, 0, 1));

	//CPUMesh arrow;
	//arrow.drawMode = MeshDrawMode::dynamicMode;
	//arrow.indices.push_back(0);
	//arrow.indices.push_back(1);
	//arrow.indices.push_back(2);
	//arrow.indices.push_back(3);
	//arrow.name = "arrow";
	//arrow.normals.push_back(Vec3(0, 0, 1));
	//arrow.normals.push_back(Vec3(0, 0, 1));
	//arrow.normals.push_back(Vec3(0, 0, 1));
	//arrow.normals.push_back(Vec3(0, 0, 1));
	//arrow.primitiveMode = PrimitiveMode::TriangleStrip;
	//arrow.uvs.push_back(Vec2(0, 1));
	//arrow.uvs.push_back(Vec2(0, 0));
	//arrow.uvs.push_back(Vec2(1, 0));
	//arrow.uvs.push_back(Vec2(1, 1));
	//arrow.vertices.push_back(start + extend * width);
	//arrow.vertices.push_back(start - extend * width);
	//arrow.vertices.push_back(end);
	//arrow.vertices.push_back(end);
	//arrow.checkIntegrity();

	//GPUMesh gpuMesh;
	//gpuMesh.upload(arrow);
	//gpuMesh.render(uniforms());
	//gpuMesh.unload();
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
void Renderer::setAlphaBlending(Bool blending)
{
	if (blending) {
		apiEnable(GL_BLEND);
		apiBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		apiBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_DST_ALPHA);
		apiBlendEquationSeparate(GL_FUNC_ADD, GL_MAX);
	}
	else apiDisable(GL_BLEND);
}
void Renderer::setDepthTest(Bool isUsingDepth) {
	if (isUsingDepth) {
		apiEnable(GL_DEPTH_TEST);
		apiDepthFunc(GL_LEQUAL);
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

	Vec3 dir = normalize(start - end);
	Vec3 extend = cross(dir, Vec3(0, 0, 1));

	CPUMesh line;
	line.drawMode = MeshDrawMode::dynamicMode;
	line.indices.push_back(0);
	line.indices.push_back(1);
	line.indices.push_back(2);
	line.indices.push_back(3);
	line.name = "line";
	line.normals.push_back(Vec3(0, 0, 1));
	line.normals.push_back(Vec3(0, 0, 1));
	line.normals.push_back(Vec3(0, 0, 1));
	line.normals.push_back(Vec3(0, 0, 1));
	line.primitiveMode = PrimitiveMode::TriangleStrip;
	line.uvs.push_back(Vec2(0, 1));
	line.uvs.push_back(Vec2(0, 0));
	line.uvs.push_back(Vec2(1, 0));
	line.uvs.push_back(Vec2(1, 1));
	line.vertices.push_back(start + extend * width);
	line.vertices.push_back(start - extend * width);
	line.vertices.push_back(end + extend * width);
	line.vertices.push_back(end - extend * width);
	line.checkIntegrity();

	GPUMesh gpuMesh;
	gpuMesh.upload(line);
	gpuMesh.render(uniforms());
	gpuMesh.unload();
}
void Renderer::createBlurTexture(Index from, Index to)
{
	//this function renders a blurred version of a framebuffer to another framebuffer
	Index originalFramebufferID = framebufferSystem.currentFramebufferIndex;
	
	useShader("blur");
	framebufferSystem.framebuffers[from].colorTexture.use(0);
	framebufferSystem.use(*this, to);
	renderMesh("fullScreenQuad");

	framebufferSystem.use(*this, originalFramebufferID);
}

//sky

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
void Renderer::renderAtmosphere(Player& player, Vec3 sunDirection) {
	Bool culling = getCulling();
	setDepthTest(false);
	player.camera.renderOnlyRot(*this);
	uniforms().cameraPos() = Vec4(Vec3(0), 1);
	setCulling(false);
	uniforms().sunDir() = Vec4(sunDirection, 1);
	useShader("atmosphere");
	framebufferSystem.current().colorTexture.use(0);
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

Bool Renderer::getCulling() {
	return apiGetCullFace();
}
Float& Renderer::aspectRatio()
{
	return framebufferSystem.current().aspectRatio;
}
Uniforms& Renderer::uniforms()
{
	return shaderSystem.uniforms;
}
Index Renderer::useShader(Name name) {
	return shaderSystem.use(name);
}