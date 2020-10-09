#include "Renderer.hpp"

void Renderer::create()
{
	renderTime.reset();
	cameraSystem.create();
	viewportSystem.create();
	renderObjectSystem.create();
	framebufferSystem.create(renderObjectSystem);

	glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);

	//cameraSystem.current().speedMultiplier = 1;
}
void Renderer::renderTest() {

};
void Renderer::render()
{
	mutex.lock();

	renderTime.update();
	dynamicallyAdjustValue(*this, worldSystemRenderDistance);
	cameraSystem.current().clampMovement = true;
	cameraSystem.current().processLocation(renderTime);

	//setup frame
	framebufferSystem.updateFramebuffers(renderObjectSystem.textureSystem);
	framebufferSystem.select(0);
	framebufferSystem.current().use();
	framebufferSystem.current().clear();

	updateWireframeMode(wireframeMode);
	viewportSystem.select("full");
	viewportSystem.render(framebufferSystem.adaptiveResolution.x, framebufferSystem.adaptiveResolution.y);


	//render sky
	culling(false);
	depthTest(false);
	resetModelMatrix();
	uniforms().setVec3("chunkOffsetVector", Vec3(0));
	uniforms().setVec3("cameraVector", cameraSystem.current().forwardVector);
	uniforms().setMatrix("vpMatrix", cameraSystem.current().projectionMatrix(viewportSystem.current().aspectRatio()) * cameraSystem.current().viewMatrixOnlyRot());
	renderObjectSystem.render("sky");

	//setup actual scene
	culling(true);
	clearDepth();
	depthTest(true);

	cameraSystem.current().render(*this);

	createWorldRenderData(gameSimulation->world);
	renderWorld(worldRenderData);

	framebufferSystem.selectFinal();
	depthTest(false);
	culling(false);
	updateWireframeMode(false);
	clearColor(Color(0, 0, 0, 0.75));
	clearDepth();

	renderObjectSystem.render("postProcess");

	/////////////////
	pollGraphicsAPIError();

	mutex.unlock();
}
void Renderer::sync()
{
	mutex.lock();
	mutex.unlock();
}
void Renderer::destroy()
{
	viewportSystem.destroy();
	cameraSystem.destroy();
	renderObjectSystem.destroy();
}
void Renderer::resetModelMatrix()
{
	uniforms().setMatrix("mMatrix", Matrix(1));
}
void Renderer::clearDepth() {
#ifdef GRAPHICS_API_OPENGL
	glClear(GL_DEPTH_BUFFER_BIT);
#endif // GRAPHICS_API_OPENGL
}
void Renderer::clearColor(Color color) {
#ifdef GRAPHICS_API_OPENGL
	glClearColor(color.r, color.g, color.b, color.a);
	glClear(GL_COLOR_BUFFER_BIT);
#endif // GRAPHICS_API_OPENGL
}
void Renderer::updateUniforms() {
	uniforms().setMatrix("mMatrix", Matrix(1));
}

void Renderer::createWorldRenderData(WorldChunk& chunk)
{
	bool isLowestLevel;
	bool tooBig;
	bool hasRenderableContent;
	bool isInRenderDistance;
	bool renderNow;

	chunk.mutex.lock_shared();

	long long x = chunk.location.x + 1 << 64 - chunk.level;
	long long y = chunk.location.y + 1 << 64 - chunk.level;
	long double z = chunk.location.z << 64 - chunk.level;

	x -= cameraSystem.current().chunkLocation.x;
	y -= cameraSystem.current().chunkLocation.y;
	z -= cameraSystem.current().chunkLocation.z;

	unsigned long long offset = ULLONG_MAX >> (chunk.level + 1);
	x -= offset;
	y -= offset;

	glm::highp_dvec3 delta(x, y, z);

	delta -= glm::highp_dvec3(cameraSystem.current().location);

	//imitate scale by moving it closer/further instead of scaling (allows for constant clipping)
	delta /= pow(2, 64 - chunk.level);

	delta -= glm::highp_dvec3(0.5, 0.5, 0);

	//check visibility
	isLowestLevel = chunk.level >= CHUNK_LEVEL_MAX;
	tooBig = chunk.level < 3;
	hasRenderableContent = (chunk.location.z == 0) | chunk.worldData.terrain.hasTerrain | (chunk.worldData.entityIDs.size() > 0);

	//isInRenderDistance = glm::length(delta) < worldSystemRenderDistance;
	isInRenderDistance = chunk.isInRenderDistance(cameraSystem.current().chunkLocation, 2);

	renderNow = false;


	//tell game its in use now
	chunk.setIsInUse();
	
	//is chunk in view
	if (isInRenderDistance) {
		//does it have children
		if (chunk.subdivided) {
			//render children instead
			createWorldRenderData(*chunk.tfr);
			createWorldRenderData(*chunk.tfl);
			createWorldRenderData(*chunk.tbr);
			createWorldRenderData(*chunk.tbl);
			createWorldRenderData(*chunk.bfr);
			createWorldRenderData(*chunk.bfl);
			createWorldRenderData(*chunk.bbr);
			createWorldRenderData(*chunk.bbl);
			if (chunk.level == 37) renderNow = true;
			else renderNow = false;
		}
		else {
			//will be subdivided soon, but needs to be rendered now
			renderNow = true;
		}
	}
	// is border of view and will be rendered
	else renderNow = true;


	chunk.mutex.unlock_shared();

	if (renderNow) {
		worldRenderData[chunk.level].emplace_back();
		worldRenderData[chunk.level].back().chunk = &chunk;
		worldRenderData[chunk.level].back().chunkOffsetVector = delta;
	}
}
Viewport& Renderer::currentViewport()
{
	return viewportSystem.current();
}
Uniforms& Renderer::uniforms()
{
	return renderObjectSystem.shaderSystem.uniforms;
}
void Renderer::renderWorld(WorldRenderData& world)
{

	//uniforms().setBool("distortion",worldDistortion);
	uniforms().setBool("distortion", false);

	uniforms().setMatrix("vpMatrix", cameraSystem.current().projectionMatrix(viewportSystem.current().aspectRatio()) * cameraSystem.current().viewMatrixOnlyRot());
	for (int i = 0; i <= CHUNK_LEVEL_MAX; i++) {

		auto& vec = worldRenderData[i];

		for (auto& data : vec) {

			uniforms().setFloat("cameraHeight", (long double(cameraSystem.current().chunkLocation.z) + cameraSystem.current().location.z) / pow(2, 64 - data.chunk->level));
			uniforms().setVec3("chunkOffsetVector", Vec3(data.chunkOffsetVector));
			uniforms().setVec4("worldOffset", Vec4(data.chunk->location, data.chunk->level));
			uniforms().setMatrix("mMatrix", Matrix(1));

			if (chunkBorders)renderObjectSystem.render("boundingBox");

			renderObjectSystem.render("terrain");

			if (data.chunk->location.z == 0) {

				switch (data.chunk->level) {
				case 37:renderObjectSystem.render("monkey0");
					break;
				case 36:renderObjectSystem.render("monkey1");
					break;
				case 35:renderObjectSystem.render("monkey2");
					break;
				case 34:renderObjectSystem.render("monkey3");
					break;
				case 33:renderObjectSystem.render("monkey4");
					break;
				case 32:renderObjectSystem.render("monkey5");
					break;
				case 31:renderObjectSystem.render("monkey6");
					break;
				case 30:renderObjectSystem.render("monkey7");
					break;
				default:
					break;
				}
			}
		}
		vec.clear();

		clearDepth();
	}

	uniforms().setBool("distortion", false);
}


void culling(bool isCulling) {
	if (isCulling) {
#ifdef GRAPHICS_API_OPENGL
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
#endif // GRAPHICS_API_OPENGL
	}
	else {
#ifdef GRAPHICS_API_OPENGL
		glDisable(GL_CULL_FACE);
#endif // GRAPHICS_API_OPENGL
	}
}
void depthTest(bool isUsingDepth) {
	if (isUsingDepth) {
#ifdef GRAPHICS_API_OPENGL
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
#endif // GRAPHICS_API_OPENGL
	}
	else {
#ifdef GRAPHICS_API_OPENGL
		glDisable(GL_DEPTH_TEST);
#endif // GRAPHICS_API_OPENGL
	}
}
void updateWireframeMode(bool wireframeMode) {
	if (wireframeMode) {
#ifdef GRAPHICS_API_OPENGL
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif // GRAPHICS_API_OPENGL
	}
	else {
#ifdef GRAPHICS_API_OPENGL
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif // GRAPHICS_API_OPENGL
	}
}

void pollGraphicsAPIError() {
#ifdef GRAPHICS_API_OPENGL
	GLenum error = glGetError();
	if (error) {
		std::cout << "OpenGl Error: " << error << "\n";
	}
#endif
}
void dynamicallyAdjustValue(Renderer& renderer, Float& value) {
	if (renderer.adjustRenderVariables) {
		//get current fps relative to target
		//>1 means too low fps <1 means too high
		static float timePerFrameRatio = 1;
		timePerFrameRatio = renderer.renderTime.delta * renderer.targetFrameRate;

		if ((timePerFrameRatio > 1.1) | (timePerFrameRatio < 0.9)) {
			float lerpFactor = renderer.renderTime.delta / 2;
			value += lerpFactor * (value / timePerFrameRatio);
			value /= 1 + lerpFactor;
		}
		logDebug(std::to_string(value));
		logDebug(std::to_string(renderer.worldRenderData[3].size()));
	}
}
void renderSpaceShip(Renderer& renderer, SpaceShip& spaceShip)
{
	spaceShip.rotation = getRotationBetweenVectors(Vec3(0, 0, 1), spaceShip.velocity);
	renderer.uniforms().setMatrix("mMatrix", glm::translate(Matrix(1), spaceShip.location) * glm::toMat4(spaceShip.rotation) * glm::scale(Matrix(1), Vec3(2)));
	//renderer.renderObjectSystem.render("spaceShip");
	renderer.renderObjectSystem.render("spaceShipLOD");
}