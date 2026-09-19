#include "SimpleRTS.hpp"

#include "Renderer.hpp"
#include "Crosshair.hpp"
#include "Random.hpp"
#include "UserInterface.hpp"

Bool debugSimpleRts = true;

// wolves howl, stretch, kiss, sit, lay and play when idle
// wolves avoid walking in water in cold temperatures
// wolves play before assembling into a pack to hunt
// wolves howl to gather their own pack and keep other packs away
// wolf pack territory looks like voronoi cells
// a wolfs howl can reach max 10 km
// wolves have a hierarchy within the pack
// wolf cubs live in a den
// wolves eat deer, ducks, rabbits
// wolves usually run when seeing a human
// wolves get more agressive the closer you get to their den
// wolves are scared of dogs -> herds should be protected by dogs

void SimpleRts::run()
{
	sim.start();
	sim.waitUntilLoaded();
	renderer.connect(sim);
	ui.window("Simple RTS", settings.defaultWindowWidth, settings.defaultWindowHeight, true, true, WindowState::Windowed, renderer);
	ui.run();
	sim.stop();
}

void SimpleRtsSimulation::destroy()
{
	loaded = false;
}

void SimpleRtsSimulation::reset()
{
	destroy();
	create();
}

void SimpleRtsSimulation::create()
{
	systems.push_back(&terrainSystem);

	systems.push_back(&bodySystem);
	systems.push_back(&bushSystem);
	systems.push_back(&treeSystem);
	systems.push_back(&wolfSystem);
	systems.push_back(&humanSystem);
	systems.push_back(&rabbitSystem);
	systems.push_back(&berryBushSystem);
	for (const auto s : systems) s->create(*this);

	quadtreeSystem.create(*this);
	quadtreeSystem.update(*this, mapSize);

	loaded = true;
}

void SimpleRtsRenderer::create(Window& window)
{
	GameRenderer::create(window);
}

void SimpleRtsRenderer::destroy(Window& window)
{
	GameRenderer::destroy(window);
}

Float SimpleRtsSimulation::timeStep() const
{
	if (paused) return 0.0;
	return timeScale / frameRate;
}

void SimpleRtsSimulation::update(Float delta)
{
	totalTime += timeStep();
	//logDebug("Frame: " + toString(frame++));

	//Vector<Thread> threads;
	for (const auto s : systems)
	{
		//threads.emplace_back([this,s]()
		//{
		s->update(*this);
		//});
	}
	//for (auto& thread : threads){
	//	thread.join();
	//}
	limitFramerate(frameRate);
}

Float SimpleRtsSimulation::getTerrainHeight(const Vec2 vec) const
{
	return terrainSystem.terrainFunction(vec.x/mapSize, vec.y/mapSize, mapSize);
}

void SimpleRtsRenderer::update(Window& window)
{
	player.update(window);
	Vec3 pos = player.camera.getPosition();
	pos.z    = max(0.0f, pos.z);
	player.camera.setPosition(pos);
}

void SimpleRtsSimulation::render(Renderer& r) const
{
	for (const auto s : systems) s->render(r);

	//r.uniforms().setMMatrix(Matrix(mapSize * 2));
	//r.renderMesh("SRTSGround");

	//r.useShader("color");

	//r.fill(0, 0, 0);
	//quadtreeSystem.render(r);

	//r.fill(1, 0, 0);

	//r.plane();
	//r.wireframeCube();
}

void SimpleRtsSimulation::waitUntilLoaded() const
{
	while (!loaded) {}
}

void SimpleRtsRenderer::connect(GameSimulation& simulation)
{
	this->sim = dynamic_cast<SimpleRtsSimulation*>(&simulation);
}

Vec3 SimpleRtsSimulation::getRandomSpawnPos(const Float radius) const
{
	Vec2 pos;
	Int  tries = 0;
	Bool doesSpawnCollide;
	do
	{
		pos              = randomVec2Fast(-mapSize, mapSize);
		doesSpawnCollide = doesCollide(pos, radius);
		tries++;
	}
	while (terrainSystem.isWater(*this, pos) || doesSpawnCollide);
	return {pos, getTerrainHeight(pos)};
}

Vec3 SimpleRtsSimulation::getRandomSpawnPos() const
{
	Vec2 pos;
	Int  tries = 0;
	do
	{
		pos              = randomVec2Fast(-mapSize, mapSize);
		tries++;
	}
	while (terrainSystem.isWater(*this, pos));
	return {pos, getTerrainHeight(pos)};
}

void SimpleRtsRenderer::renderInteractive(Window& window) {}

void SimpleRtsRenderer::renderScene(Window& window)
{
	Renderer& r = window.renderer;

	mutex.lock();

	//defaults
	r.setWireframeMode();
	r.clearBackground(Color(0, 0, 0, 1));
	r.uniforms().setMMatrix(Matrix(1));
	r.setWireframeMode(wireframeMode);

	//sky
	r.setCulling(false);
	r.setDepthTest(false);
	r.useShader("simpleRTSSky");
	r.useTexture("simpleRTSTexture");
	player.camera.renderOnlyRot(r);
	r.uniforms().setSunDir(Vec4(normalize(Vec3(0.5, 0.25, 1)), 1));
	r.renderMesh("SimpleRTSSky");
	r.setDepthTest(true);
	r.setCulling(true);

	//world
	player.render(window);
	r.fill(1.0f);
	r.useTexture("simpleRTSTexture");
	r.useShader("simpleRTS");
	sim->render(r);
	if (!terrainMesh.isLoaded()) terrainMesh.upload(r, sim->terrainSystem.terrainMesh);
	terrainMesh.render(r);

	//ui
	r.setDepthTest(false);
	r.screenSpaceFromBottomLeft();
	r.fill(Color(1));
	r.uniforms().setMMatrix(Matrix(1));
	r.fontSize(16.0f);
	r.letterSpacing(0.6f);
	r.textRenderSystem.alignText(Alignment::left, Alignment::bottom);
	r.text(toString(1.0f / r.time.getDelta()), Vec2(50));
	r.text(toString(sim->totalTime), Vec2(50,100));

	performanceGraph.addTime(r.time.getDelta());
	performanceGraph.render(window);
	renderCrosshairDot(window);
	////////////////////////
	mutex.unlock();
}

void SimpleRtsRenderer::inputEvent(Window& window, InputEvent& input)
{
	if (input == enter) ui.capture(*this);
	if (input == pause) sim->paused = !sim->paused;
	if (input == exit) ui.releaseCapture();
	if (input == toggleWireframe) wireframeMode = !wireframeMode;
	if (input == reset) sim->reset();
	player.inputEvent(window, input);
}

Bool SimpleRtsSimulation::doesCollide(const Vec2 pos, const Float radius) const
{
	if (treeSystem.doesCollide(pos, radius)) return true;
	if (bushSystem.doesCollide(pos, radius)) return true;
	if (berryBushSystem.doesCollide(pos, radius)) return true;
	//if (humanSystem.doesCollide(pos, radius)) return true;
	//if (rabbitSystem.doesCollide(pos, radius)) return true;
	//if (wolfSystem.doesCollide(pos, radius)) return true;
	return false;
}
