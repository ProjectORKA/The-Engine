#include "Renderer.hpp"

void Renderer::create()
{
	renderTime.reset();

	cameraSystem.create();
	viewportSystem.create();
	renderObjectSystem.create();

	//cameraSystem.current().speedMultiplier = 1;
}
void Renderer::renderTest() {

};
void Renderer::render()
{
	mutex.lock();

	renderTime.update();
	dynamicallyAdjustValue(*this, settings.worldSystemRenderDistance);
	cameraSystem.current().clampMovement = true;
	cameraSystem.current().processLocation(renderTime);

	//setup frame
	clearColor(Color(0, 0, 0, 0.75));
	updateWireframeMode(wireframeMode);

	viewportSystem.select("full");
	viewportSystem.render(framebuffer);

	//render sky
	culling(false);
	depthTest(false);
	resetModelMatrix();
	renderObjectSystem.shaderSystem.uniforms.vec3s["chunkOffsetVector"] = Vec3(0);
	renderObjectSystem.shaderSystem.uniforms.matrices["vpMatrix"] = cameraSystem.current().projectionMatrix(viewportSystem.current().aspectRatio()) * cameraSystem.current().viewMatrixOnlyRot();
	
	renderObjectSystem.render("sky");

	//setup actual scene
	culling(true);
	clearDepth();
	depthTest(true);

	cameraSystem.current().render(*this);

	//renderObjectSystem.render("monkey");

	//render earth
	//renderObjectSystem.shaderSystem.uniforms.matrices["mMatrix"] = glm::scale(Matrix(1), Vec3(100));
	//renderObjectSystem.render("earth");

	//render spaceShips
	//for (SpaceShip& ship : gameSimulation->spaceShips) {
	//	ship.render(*this);
	//}

	createWorldRenderData(gameSimulation->world);
	renderWorld(worldRenderData);


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
	renderObjectSystem.shaderSystem.uniforms.matrices["mMatrix"] = Matrix(1);
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
	renderObjectSystem.shaderSystem.uniforms.matrices["mMatrix"] = Matrix(1);
}

void Renderer::createWorldRenderData(Chunk& world)
{
	world.mutex.lock_shared();

	long long x = world.location.x + 1 << 64 - world.level;
	long long y = world.location.y + 1 << 64 - world.level;
	long double z = world.location.z << 64 - world.level;

	x -= cameraSystem.current().chunkLocation.x;
	y -= cameraSystem.current().chunkLocation.y;
	z -= cameraSystem.current().chunkLocation.z;

	unsigned long long offset = ULLONG_MAX >> (world.level + 1);
	x -= offset;
	y -= offset;

	glm::highp_dvec3 delta(x, y, z);

	delta -= glm::highp_dvec3(cameraSystem.current().location);

	//imitate scale by moving it closer/further instead of scaling (allows for constant clipping)
	delta /= pow(2, 64 - world.level);

	delta -= glm::highp_dvec3(0.5, 0.5, 0);

	//check visibility
	bool isLowestLevel = world.level >= CHUNK_LEVEL_MAX;
	bool tooBig = world.level < 3;
	bool hasRenderableContent = world.terrain.hasTerrain;
	bool isInRenderDistance = glm::length(delta) < pow(CHUNK_DISTANCE_MULTIPLIER, settings.worldSystemRenderDistance);

	bool renderNow = false;
	//chunks need entities inside to be rendered
	if (hasRenderableContent | tooBig) {
		if (tooBig | (isInRenderDistance & !isLowestLevel)) {
			//too close to be rendered
			//will be subdivided and checked
			world.setIsInUse();
			if (world.subdivided) {
				createWorldRenderData(*world.tfr);
				createWorldRenderData(*world.tfl);
				createWorldRenderData(*world.tbr);
				createWorldRenderData(*world.tbl);
				createWorldRenderData(*world.bfr);
				createWorldRenderData(*world.bfl);
				createWorldRenderData(*world.bbr);
				createWorldRenderData(*world.bbl);

				renderNow = false;
			}
			else {
				renderNow = true;
			}
		}
		else {
			renderNow = true;
		}
	}
	world.mutex.unlock_shared();

	if (renderNow & world.terrain.hasTerrain) {
		worldRenderData[world.level].emplace_back();
		worldRenderData[world.level].back().chunk = &world;
		worldRenderData[world.level].back().chunkOffsetVector = delta;
	}

}
void Renderer::renderWorld(WorldRenderData& world)
{

	renderObjectSystem.shaderSystem.uniforms.bools["distortion"] = worldDistortion;

	renderObjectSystem.shaderSystem.uniforms.matrices["vpMatrix"] = cameraSystem.current().projectionMatrix(viewportSystem.current().aspectRatio()) * cameraSystem.current().viewMatrixOnlyRot();
	for (auto& vec : world) {
		for (auto& data : vec) {
			renderObjectSystem.shaderSystem.uniforms.floats["cameraHeight"] = (long double(cameraSystem.current().chunkLocation.z) + cameraSystem.current().location.z) / pow(2, 64 - data.chunk->level);
			renderObjectSystem.shaderSystem.uniforms.vec3s["chunkOffsetVector"] = Vec3(data.chunkOffsetVector);
			renderObjectSystem.shaderSystem.uniforms.vec4s["worldOffset"] = Vec4(data.chunk->location, data.chunk->level);
			renderObjectSystem.shaderSystem.uniforms.matrices["mMatrix"] = Matrix(1);

			renderObjectSystem.shaderSystem.useShader("debug");
			renderObjectSystem.meshSystem.renderMesh("terrain");

		}
		vec.clear();
		clearDepth();
	}

	renderObjectSystem.shaderSystem.uniforms.bools["distortion"] = false;
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
void dynamicallyAdjustValue(Renderer& renderer, unsigned int& value) {
	if (renderer.settings.adjustRenderVariables) {
		static float minimumFrameTimeMatchup = 1;
		static float maximumFrameTimeMatchup = 1;

		const float lerpFactor = 0.1;

		minimumFrameTimeMatchup += lerpFactor * renderer.renderTime.delta * renderer.settings.minimumFrameRate;
		maximumFrameTimeMatchup += lerpFactor * renderer.renderTime.delta * renderer.settings.maximumFrameRate;

		minimumFrameTimeMatchup /= 1 + lerpFactor;
		maximumFrameTimeMatchup /= 1 + lerpFactor;

		if (minimumFrameTimeMatchup > 1) {
			if (value != 0) {
				value /= minimumFrameTimeMatchup;
				//value--;
			}
		}
		else {
			if (maximumFrameTimeMatchup < 1) {
				value++;
			}
		}
	}
}