#include "Orbital Oblivion.hpp"
#include "UserInterface.hpp"

void OrbitalOblivion::run(Engine& engine) {

	OrbitalOblivionSimulation sim;
	OrbitalOblivionRenderer game(sim);
	
	window("Orbital Oblivion", Area(1920, 1080), true, WindowState::windowed, game, engine);

	ui.run();
}

void OrbitalOblivionRenderer::create(Window& window) {
	FramebufferSystem& fs = window.renderer.framebufferSystem;
	fs.addFrameBuffer("bloom1", pow(0.5,1));
	fs.addFrameBuffer("bloom2", pow(0.5,2));
	fs.addFrameBuffer("bloom3", pow(0.5,3));
	fs.addFrameBuffer("bloom4", pow(0.5,4));
	fs.addFrameBuffer("bloom5", pow(0.5,5));
	fs.addFrameBuffer("bloom6", pow(0.5,6));
	fs.addFrameBuffer("bloom7", pow(0.5,7));
	fs.addFrameBuffer("bloom8", pow(0.5,8));
	fs.addFrameBuffer("bloom9", pow(0.5,9));
}

OrbitalOblivionRenderer::OrbitalOblivionRenderer(OrbitalOblivionSimulation& sim) {
	this->sim = &sim;
}
void OrbitalOblivionRenderer::inputEvent(Window& window, InputEvent input) {
	if (input == enter) window.captureCursor();
	if (input == exit) window.uncaptureCursor();
	if (input == toogleWireframe) window.renderer.wireframeMode = !window.renderer.wireframeMode;
}
void OrbitalOblivionRenderer::render(Engine& engine, Window& window, TiledRectangle area) {

	Renderer& r  = window.renderer;

	r.draw("main");

	r.setWireframeMode(r.wireframeMode);

	r.clearColor(Color(0,0,0,1));
	r.clearDepth();
	r.setDepthTest(true);
	r.setCulling(true);
	
	player.update(window);
	player.render(window);

	r.useTexture(engine, "orbitalOblivionReflection");
	r.useShader(engine, "orbitalOblivion");
	r.renderMesh(engine, "unit");


	renderBloom(engine, r);

}

void OrbitalOblivionRenderer::renderBloom(Engine& e, Renderer & r) {
	//get common variables
	FramebufferSystem& fs = r.framebufferSystem;

	//setup rendering
	r.setDepthTest(false);
	r.setWireframeMode(false);

	r.postProcess(e, "bloomDownsample");

	//downsample pass
	//1
	//r.read("main");
	//fs.currentRead().setAsTexture();
	//r.draw("bloom1");
	//r.clearColor();
	//r.fullScreenShader(e, "bloomDownsample");

	//r.read("bloom1");
	//fs.currentRead().setAsTexture();
	//r.draw("bloom2");
	//r.clearColor();
	//r.fullScreenShader(e, "bloomDownsample");

	//r.read("bloom2");
	//fs.currentRead().setAsTexture();
	//r.draw("bloom3");
	//r.clearColor();
	//r.fullScreenShader(e, "bloomDownsample");

	//r.read("bloom3");
	//fs.currentRead().setAsTexture();
	//r.draw("bloom4");
	//r.clearColor();
	//r.fullScreenShader(e, "bloomDownsample");

	//r.read("bloom4");
	//fs.currentRead().setAsTexture();
	//r.draw("bloom5");
	//r.clearColor();
	//r.fullScreenShader(e, "bloomDownsample");

	//r.read("bloom5");
	//fs.currentRead().setAsTexture();
	//r.draw("bloom6");
	//r.clearColor();
	//r.fullScreenShader(e, "bloomDownsample");

	//r.read("bloom6");
	//fs.currentRead().setAsTexture();
	//r.draw("bloom7");
	//r.clearColor();
	//r.fullScreenShader(e, "bloomDownsample");

	//r.read("bloom7");
	//fs.currentRead().setAsTexture();
	//r.draw("bloom8");
	//r.clearColor();
	//r.fullScreenShader(e, "bloomDownsample");

	//r.read("bloom8");
	//fs.currentRead().setAsTexture();
	//r.draw("bloom9");
	//r.clearColor();
	//r.fullScreenShader(e, "bloomDownsample");

	//fs.setAsTexture("main", 0);
	//fs.setAsTexture("bloom1", 1);
	//fs.setAsTexture("bloom2", 1);
	//fs.setAsTexture("bloom3", 2);
	//fs.setAsTexture("bloom4", 3);
	//fs.setAsTexture("bloom5", 4);
	//fs.setAsTexture("bloom6", 5);
	//fs.setAsTexture("bloom7", 6);
	//fs.setAsTexture("bloom8", 7);
	//fs.setAsTexture("bloom9", 8);
	//r.draw("main");
	//r.fullScreenShader(e, "bloomUpsample");

	//r.postProcess(e, "bloomDownsample");

	
	//r.useShader(e, "bloomDownsample");
	//r.renderMesh(e, "fullScreenQuad");




	//r.uniforms().reset();

	//r.read("postProcess");
	//r.framebufferSystem.currentRead().setAsTexture(0);
	//r.draw("main");
	//r.useShader(e, "texture");
	//r.renderMesh(e, "fullScreenQuad");
};

OOTeam::OOTeam(Index id, Color color) {
	this->id = id;
	this->color = color;
}

OOUnit::OOUnit(Index team, Vec2 location, Vec2 direction) {
	this->team = team;
	this->location = location;
	this->direction = direction;
}

OrbitalOblivionSimulation::OrbitalOblivionSimulation() {
	//create teams
	teams.emplace_back(0, Color(0, 0.33333333333, 1, 1));
	teams.emplace_back(1, Color(0.65098039215, 0, 0, 1));

	//create units
	units.emplace_back(0, Vec2(-1, 0), Vec2(+1, 0));
	units.emplace_back(1, Vec2(+1, 0), Vec2(-1, 0));
}