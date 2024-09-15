#include "Orbital Oblivion.hpp"
#include "UserInterface.hpp"

//void OrbitalOblivionRenderer::renderBloom(Renderer& r)
//{
//	//get common variables
//	FramebufferSystem& fs = r.framebufferSystem;
//
//	//setup rendering
//	r.setDepthTest(false);
//	r.setWireframeMode(false);
//	r.setAlphaBlending(false);
//
//	//prefilter (remove fireflies)
//	r.read("main");
//	fs.currentRead().setAsTexture();
//	r.draw("postProcess");
//	r.fullScreenShader(e, "bloomPrefilter");
//
//	//downsample pass
//	r.read("postProcess");
//	fs.currentRead().setAsTexture();
//	r.draw("bloom1");
//	r.fullScreenShader(e, "bloomDownsample");
//
//	r.read("bloom1");
//	fs.currentRead().setAsTexture();
//	r.draw("bloom2");
//	r.fullScreenShader(e, "bloomDownsample");
//
//	r.read("bloom2");
//	fs.currentRead().setAsTexture();
//	r.draw("bloom3");
//	r.fullScreenShader(e, "bloomDownsample");
//
//	r.read("bloom3");
//	fs.currentRead().setAsTexture();
//	r.draw("bloom4");
//	r.fullScreenShader(e, "bloomDownsample");
//
//	r.read("bloom4");
//	fs.currentRead().setAsTexture();
//	r.draw("bloom5");
//	r.fullScreenShader(e, "bloomDownsample");
//
//	r.read("bloom5");
//	fs.currentRead().setAsTexture();
//	r.draw("bloom6");
//	r.fullScreenShader(e, "bloomDownsample");
//
//	r.read("bloom6");
//	fs.currentRead().setAsTexture();
//	r.draw("bloom7");
//	r.fullScreenShader(e, "bloomDownsample");
//
//	r.read("bloom7");
//	fs.currentRead().setAsTexture();
//	r.draw("bloom8");
//	r.fullScreenShader(e, "bloomDownsample");
//
//	//r.read("bloom8");
//	//fs.currentRead().setAsTexture();
//	//r.draw("bloom9");
//	//r.fullScreenShader(e, "bloomDownsample");
//
//	r.blendModeAdditive();
//
//	//r.read("bloom9");
//	//fs.currentRead().setAsTexture();
//	//r.draw("bloom8");
//	//r.fullScreenShader(e, "bloomUpsample");
//
//	r.read("bloom8");
//	fs.currentRead().setAsTexture();
//	r.draw("bloom7");
//	r.fullScreenShader(e, "bloomUpsample");
//
//	r.read("bloom7");
//	fs.currentRead().setAsTexture();
//	r.draw("bloom6");
//	r.fullScreenShader(e, "bloomUpsample");
//
//	r.read("bloom6");
//	fs.currentRead().setAsTexture();
//	r.draw("bloom5");
//	r.fullScreenShader(e, "bloomUpsample");
//
//	r.read("bloom5");
//	fs.currentRead().setAsTexture();
//	r.draw("bloom4");
//	r.fullScreenShader(e, "bloomUpsample");
//
//	r.read("bloom4");
//	fs.currentRead().setAsTexture();
//	r.draw("bloom3");
//	r.fullScreenShader(e, "bloomUpsample");
//
//	r.read("bloom3");
//	fs.currentRead().setAsTexture();
//	r.draw("bloom2");
//	r.fullScreenShader(e, "bloomUpsample");
//
//	r.read("bloom2");
//	fs.currentRead().setAsTexture();
//	r.draw("bloom1");
//	r.fullScreenShader(e, "bloomUpsample");
//
//	r.read("bloom1");
//	fs.currentRead().setAsTexture();
//	r.draw("postProcess");
//	r.fullScreenShader(e, "bloomUpsample");
//
//	r.read("postProcess");
//	fs.currentRead().setAsTexture();
//	r.draw("main");
//	//r.clearColor();
//	r.fullScreenShader(e, "lastBloomUpsample");
//
//	r.postProcess(e, "gammaCorrection");
//
//	//r.screenSpace();
//	//fonts.setFontSize(100);
//	//r.textRenderSystem.render(e, r, toString(mod(r.time.total / 10, 0.2) + 0.8), Vec2(50), fonts.heading);
//};

void OOUnit::updatePosition()
{
	position = position + direction * speed;
}

OOUnit::OOUnit(const Index team)
{
	this->team = team;
}

void OrbitalOblivionSimulation::destroy() {}

void OOPlanet::render(Renderer& r) const
{
	r.uniforms().setMMatrix(matrixFromPositionAndSize(position, 10));
	r.meshSystem.currentMesh().render(r.uniforms());
}

void OrbitalOblivionRenderer::update(Window& window) {}

void OrbitalOblivionRenderer::destroy(Window& window) {}

OOTeam::OOTeam(const Index id, const Color color)
{
	this->id    = id;
	this->color = color;
}

void OrbitalOblivionPlayer::update(Window& window)
{
	//get frequently used info
	const Float delta = window.renderer.deltaTime();

	//set up temporary data
	Vec3  movementVector = Vec3(0);

	//process input
	if(window.capturing) camera.rotate(window.mouseDelta * DVec2(mouseSensitivity));
	if(window.pressed(forward)) movementVector += camera.getForwardVector();
	if(window.pressed(backward)) movementVector -= camera.getForwardVector();
	if(window.pressed(right)) movementVector += camera.getRightVector();
	if(window.pressed(left)) movementVector -= camera.getRightVector();
	if(window.pressed(up)) movementVector += camera.getUpVector();
	if(window.pressed(down)) movementVector -= camera.getUpVector();

	//calculate movement
	if(length(movementVector) > 0)
	{
		//if there is movement input
		const Float desiredSpeed = powf(baseNumber, static_cast<Float>(speedExponent)); //calculate speed
		movementVector     = normalize(movementVector);									//get direction of movement (just direction)
		movementVector *= desiredSpeed * delta;											//add speed to direction
		camera.setPosition(camera.getPosition() + movementVector);						//add it to cameras position
	}
}

void OrbitalOblivionSimulation::update(const Float delta)
{
	if(delta != 0.0f)
	{
		for(OOUnit& u : units) u.updatePosition();
		for(OOUnit& u : units) u.updateDirection(units, planets);
	}
}

void OrbitalOblivionRenderer::connect(GameSimulation& simulation)
{
	this->sim = dynamic_cast<OrbitalOblivionSimulation*>(&simulation);
}

OOPlanet& OOUnit::getClosestPlanet(Vector<OOPlanet>& planets) const
{
	Float     shortestDistance = 10000000000000000000.0f;
	OOPlanet* current          = &planets.front();

	for(OOPlanet& p : planets)
	{
		if(const Float dist = distance(position, p.position); dist < shortestDistance)
		{
			current          = &p;
			shortestDistance = dist;
		}
	}

	return *current;
}

void OrbitalOblivionSimulation::create()
{
	//create teams
	teams.emplace_back(0, Color(0, 0.33333333333, 1, 1));
	teams.emplace_back(1, Color(0.65098039215, 0, 0, 1));

	//create planets
	while(planets.size() < planetCount)
	{
		Vec2 testPosition = randomVec2Fast(orbitalOblivionPlayfieldSize);

		for(const OOPlanet& p : planets) if(distance(p.position, testPosition) < planetDistance) goto skip;

		planets.emplace_back(testPosition);

	skip:;
	}

	//create units
	for(UInt i = 0; i < shipCount; i++)
	{
		units.emplace_back(0);
		units.emplace_back(1);
	}
}

OOUnit::OOUnit(const Index team, const Vec2 position, const Vec2 direction)
{
	this->team      = team;
	this->position  = position;
	this->direction = direction;
}

void OrbitalOblivionPlayer::inputEvent(Window& window, const InputEvent input)
{
	if(input == faster) speedExponent++;
	if(input == slower) speedExponent--;
}

void OOUnit::render(Renderer& renderer) const
{
	renderer.uniforms().setMMatrix(matrixFromPositionAndDirection(position, direction));
	renderer.renderMesh("unit");
}

void OrbitalOblivionRenderer::inputEvent(Window& window, const InputEvent input)
{
	if(input == enter) window.captureCursor();
	if(input == exit) window.releaseCursor();
	if(input == toggleWireframe) window.renderer.wireframeMode = !window.renderer.wireframeMode;
	if(input == toggleBloom) bloom = !bloom;
	if(input == pause)
	{
		if(window.renderer.time.isPaused())
		{
			window.renderer.time.unpause();
		}
		else
		{
			window.renderer.time.pause();
		}
	}
	player.inputEvent(window, input);
}

void OOUnit::updateDirection(Vector<OOUnit>& neighbors, Vector<OOPlanet>& planets)
{
	target = getClosestPlanet(planets).position;

	const Vec2 targetDelta = vectorFromAToB(position, target);

	Vec2 influence = direction + targetDelta;

	for(const OOPlanet& p : planets)
	{
		Vec2        delta    = vectorFromAToB(p.position, position);
		const Float distance = length(delta);
		const Float factor   = viewDistance * 800 / (distance * distance);

		influence += Vec2(factor) * normalize(delta);
	}

	for(OOUnit& n : neighbors)
	{
		Vec2        delta    = vectorFromAToB(n.position, position);
		const Float distance = length(delta);
		//if (distance > viewDistance)
		if(distance == 0.0f) continue;

		const Float factor = viewDistance / (distance * distance);

		influence += Vec2(factor / 2) * n.direction;
		influence += Vec2(factor * 2) * normalize(delta);
	}

	direction = normalize(direction + Vec2(turnRate) * normalize(influence));
}

void OrbitalOblivionRenderer::create(Window& window)
{
	//FramebufferSystem& fs = window.renderer.framebufferSystem;
	//fs.addFrameBuffer("bloom1", pow(0.5, 1));
	//fs.addFrameBuffer("bloom2", pow(0.5, 2));
	//fs.addFrameBuffer("bloom3", pow(0.5, 3));
	//fs.addFrameBuffer("bloom4", pow(0.5, 4));
	//fs.addFrameBuffer("bloom5", pow(0.5, 5));
	//fs.addFrameBuffer("bloom6", pow(0.5, 6));
	//fs.addFrameBuffer("bloom7", pow(0.5, 7));
	//fs.addFrameBuffer("bloom8", pow(0.5, 8));
	//fs.addFrameBuffer("bloom9", pow(0.5, 9));
	window.renderer.meshSystem.use("unit");
	unitMesh = &window.renderer.meshSystem.currentMesh();
}

void OrbitalOblivionRenderer::render(Window& window, TiledRectangle area)
{
	Renderer& r = window.renderer;

	r.drawToWindow();

	r.setWireframeMode(r.wireframeMode);

	r.clearBackground(Color(0, 0, 0, 1));
	r.setDepthTest(true);
	r.setCulling(true);

	player.update(window);
	player.render(window);

	r.useTexture("orbitalOblivionReflection");
	r.useShader("orbitalOblivion");

	Vector<Matrix> positions = Vector<Matrix>(sim->units.size());
	for(SizeT i = 0; i < sim->units.size(); i++) positions[i] = matrixFromPositionAndDirection(sim->units[i].position, sim->units[i].direction);
	r.renderMeshInstanced("unit", positions);

	r.useMesh("sphere");
	for(OOPlanet& p : sim->planets) p.render(r);

	//if(bloom) renderBloom(r);

	r.setWireframeMode(false);
	r.screenSpace();
	r.uniforms().setMMatrix(Matrix(1));
	r.textRenderSystem.setSize(16);
	r.textRenderSystem.setLetterSpacing(0.6f);
	r.textRenderSystem.render(r, "Framerate: " + toString(1 / r.time.getDelta()), Vec2(50, 50));
	r.textRenderSystem.render(r, "Ship Count: " + toString(static_cast<Int>(sim->units.size())), Vec2(50, 100));
}

void OrbitalOblivionRenderer::renderInteractive(Window& window, TiledRectangle area) {}
