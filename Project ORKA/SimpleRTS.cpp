#include "SimpleRTS.hpp"

#include "Random.hpp"
#include "UserInterface.hpp"

void SimpleRts::run()
{
	ui.create();
	sim.start();
	renderer.connect(sim);
	ui.window("Simple RTS", Area(1920, 1080), true, true, WindowState::Windowed, renderer);
	ui.run();
	sim.stop();
}

void SimpleRtsSimulation::destroy() {}

void SimpleRtsSimulation::create()
{
	systems.push_back(&bodySystem);
	systems.push_back(&bushSystem);
	systems.push_back(&treeSystem);
	systems.push_back(&humanSystem);
	systems.push_back(&rabbitSystem);
	systems.push_back(&terrainSystem);
	systems.push_back(&berryBushSystem);
	for (const auto s : systems) s->create(*this);

	quadtreeSystem.create(*this);
	quadtreeSystem.update(*this, mapSize);
}

void SimpleRtsRenderer::destroy(Window& window) {}

Float SimpleRtsSimulation::timeStep() const
{
	if (paused) return 0.0;
	return timeScale / frameRate;
}

void SimpleRtsSimulation::update(Float delta)
{
	time += timeStep();

	Vector<Thread> threads;
	for (auto s : systems)
	{
		threads.emplace_back([this,s]()
		{
			s->update(*this);
		});
	}

	//threads.emplace_back([this]()
	//{
	//	treeSystem.update(*this);
	//});
	//threads.emplace_back([this]()
	//{
	//	humanSystem.update(*this);
	//});
	//threads.emplace_back([this]()
	//{
	//	bushSystem.update(*this);
	//});
	//threads.emplace_back([this]()
	//{
	//	berryBushSystem.update(*this);
	//});
	for (auto& thread : threads) if (thread.joinable()) thread.join();
	limitFramerate(frameRate);
}

void SimpleRtsRenderer::update(Window& window)
{
	player.update(window);
	Vec3 pos = player.camera.getPosition();
	pos.z    = max(0.0f, pos.z);
	player.camera.setPosition(pos);
}

void SimpleRtsRenderer::create(Window& window) {}

void SimpleRtsSimulation::render(Renderer& renderer) const
{
	for (const auto s : systems) s->render(renderer);

	renderer.useShader("color");

	renderer.fill(0, 0, 0);
	quadtreeSystem.render(renderer);

	renderer.fill(255, 0, 0);
	renderer.uniforms().setMMatrix(mapSize * 2);
	renderer.plane();
	//renderer.wireframeCube();
}

void SimpleRtsRenderer::connect(GameSimulation& simulation)
{
	this->sim = static_cast<SimpleRtsSimulation*>(&simulation);
}

void SimpleRtsRenderer::render(Window& window, const TiledRectangle area)
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
	r.fill(1.0);
	r.useTexture("simpleRTSTexture");
	r.useShader("simpleRTS");
	sim->render(r);

	//ui
	r.setDepthTest(false);
	r.screenSpace();
	r.fill(Color(1));
	r.uniforms().setMMatrix(Matrix(1));
	r.textRenderSystem.setSize(16.0f);
	r.textRenderSystem.setLetterSpacing(0.6f);
	r.textRenderSystem.render(r, toString(1.0f / r.time.getDelta()), Vec2(50));

	performanceGraph.addTime(r.time.getDelta());
	performanceGraph.render(window, area);
	////////////////////////

	mutex.unlock();
}

Vec2 SimpleRtsSimulation::getRandomSpawnPos(const Float radius) const
{
	Vec2 pos;
	Int  tries = 0;
	do
	{
		pos = randomVec2Fast(-mapSize, mapSize);
		tries++;
	}
	while (doesCollide(pos, radius) && tries < 100);
	return pos;
}

void SimpleRtsRenderer::renderInteractive(Window& window, TiledRectangle area) {}

void SimpleRtsRenderer::inputEvent(Window& window, const InputEvent input)
{
	if (input == enter) window.captureCursor();
	if (input == pause) sim->paused = !sim->paused;
	if (input == exit) window.releaseCursor();
	if (input == toggleWireframe) wireframeMode = !wireframeMode;
	player.inputEvent(window, input);
}

Bool SimpleRtsSimulation::doesCollide(const Vec2 pos, const Float radius) const
{
	if (treeSystem.doesCollide(pos, radius)) return true;
	if (bushSystem.doesCollide(pos, radius)) return true;
	if (humanSystem.doesCollide(pos, radius)) return true;
	if (rabbitSystem.doesCollide(pos, radius)) return true;
	return false;
}
