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
	for(auto s : systems) s->create(*this);
}

void SimpleRtsRenderer::destroy(Window& window) {}

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

Float SimpleRtsSimulation::timeStep() const
{
	if(paused) return 0.0;
	return timeScale / frameRate;
}

void SimpleRtsRenderer::update(Window& window)
{
	player.update(window);
	Vec3 pos = player.camera.getPosition();
	pos.z    = max(0.0f, pos.z);
	player.camera.setPosition(pos);
}

void SimpleRtsRenderer::create(Window& window)
{
	//terrainRenderingSystem.create();
}

void SimpleRtsSimulation::render(Renderer& renderer) const
{
	for (auto s : systems) s->render(renderer);
	renderer.useShader("color");
	renderer.fill(255,0,0);
	renderer.uniforms().setMMatrix(2000);
	renderer.plane();
	renderer.wireframeCube();
}

void SimpleRtsRenderer::connect(GameSimulation& simulation)
{
	this->sim = static_cast<SimpleRtsSimulation*>(&simulation);
}

void SimpleRtsRenderer::render(Window& window, TiledRectangle area)
{
	Renderer& renderer = window.renderer;

	mutex.lock();
	//default
	renderer.setWireframeMode();
	renderer.clearBackground(Color(0, 0, 0, 1));
	renderer.uniforms().setMMatrix(Matrix(1));
	renderer.setWireframeMode(wireframeMode);

	//sky
	renderer.setCulling(false);
	renderer.setDepthTest(false);
	renderer.useShader("simpleRTSSky");
	renderer.useTexture("simpleRTSTexture");
	player.camera.renderOnlyRot(renderer);
	renderer.uniforms().setSunDir(Vec4(normalize(Vec3(0.5, 0.25, 1)), 1));
	renderer.renderMesh("SimpleRTSSky");
	renderer.setDepthTest(true);
	renderer.setCulling(true);

	//world
	player.render(window);
	renderer.fill(1.0);
	renderer.useTexture("simpleRTSTexture");
	renderer.useShader("simpleRTS");
	sim->render(renderer);

	//ui
	renderer.setDepthTest(false);
	renderer.screenSpace();
	renderer.fill(Color(1));
	renderer.uniforms().setMMatrix(Matrix(1));
	renderer.textRenderSystem.setSize(16.0f);
	renderer.textRenderSystem.setLetterSpacing(0.6f);
	renderer.textRenderSystem.render(renderer, toString(1.0f / renderer.time.getDelta()), Vec2(50));
	////////////////////////

	mutex.unlock();
}

Bool SimpleRtsSimulation::doesCollide(const Vec2 pos, const Float radius) const
{
	if (treeSystem.doesCollide(pos, radius)) return true;
	if (bushSystem.doesCollide(pos, radius)) return true;
	if (humanSystem.doesCollide(pos, radius)) return true;
	if (rabbitSystem.doesCollide(pos, radius)) return true;
	return false;
}

Vec2 SimpleRtsSimulation::getRandomSpawnPos(const Float radius) const
{
	Vec2 pos;
	Int  tries       = 0;
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
