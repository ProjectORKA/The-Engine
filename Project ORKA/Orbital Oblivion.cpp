#include "Orbital Oblivion.hpp"
#include "UserInterface.hpp"

void OrbitalOblivion::run(Engine& engine) {

	OrbitalOblivionRenderer game;
	
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
void OrbitalOblivionRenderer::inputEvent(Window& window, InputEvent input) {
	if (input == enter) window.captureCursor();
	if (input == exit) window.uncaptureCursor();
	if (input == toogleWireframe) window.renderer.wireframeMode = !window.renderer.wireframeMode;
	if (input == toggleBloom) bloom = !bloom;
	if (input == pause) {
		if (window.renderer.time.paused) window.renderer.time.unpause();
		else window.renderer.time.pause();
	}
	player.inputEvent(window, input);
}
void OrbitalOblivionRenderer::renderBloom(Engine& e, Renderer & r) {
	//get common variables
	FramebufferSystem& fs = r.framebufferSystem;

	//setup rendering
	r.setDepthTest(false);
	r.setWireframeMode(false);
	r.setAlphaBlending(false);

	//prefilter (remove fireflies)
	r.read("main");
	fs.currentRead().setAsTexture();
	r.draw("postProcess");
	r.fullScreenShader(e, "bloomPrefilter");

	//downsample pass
	r.read("postProcess");
	fs.currentRead().setAsTexture();
	r.draw("bloom1");
	r.fullScreenShader(e, "bloomDownsample");

	r.read("bloom1");
	fs.currentRead().setAsTexture();
	r.draw("bloom2");
	r.fullScreenShader(e, "bloomDownsample");

	r.read("bloom2");
	fs.currentRead().setAsTexture();
	r.draw("bloom3");
	r.fullScreenShader(e, "bloomDownsample");

	r.read("bloom3");
	fs.currentRead().setAsTexture();
	r.draw("bloom4");
	r.fullScreenShader(e, "bloomDownsample");

	r.read("bloom4");
	fs.currentRead().setAsTexture();
	r.draw("bloom5");
	r.fullScreenShader(e, "bloomDownsample");

	r.read("bloom5");
	fs.currentRead().setAsTexture();
	r.draw("bloom6");
	r.fullScreenShader(e, "bloomDownsample");

	r.read("bloom6");
	fs.currentRead().setAsTexture();
	r.draw("bloom7");
	r.fullScreenShader(e, "bloomDownsample");

	r.read("bloom7");
	fs.currentRead().setAsTexture();
	r.draw("bloom8");
	r.fullScreenShader(e, "bloomDownsample");

	//r.read("bloom8");
	//fs.currentRead().setAsTexture();
	//r.draw("bloom9");
	//r.fullScreenShader(e, "bloomDownsample");

	r.blendModeAdditive();

	//r.read("bloom9");
	//fs.currentRead().setAsTexture();
	//r.draw("bloom8");
	//r.fullScreenShader(e, "bloomUpsample");

	r.read("bloom8");
	fs.currentRead().setAsTexture();
	r.draw("bloom7");
	r.fullScreenShader(e, "bloomUpsample");

	r.read("bloom7");
	fs.currentRead().setAsTexture();
	r.draw("bloom6");
	r.fullScreenShader(e, "bloomUpsample");

	r.read("bloom6");
	fs.currentRead().setAsTexture();
	r.draw("bloom5");
	r.fullScreenShader(e, "bloomUpsample");

	r.read("bloom5");
	fs.currentRead().setAsTexture();
	r.draw("bloom4");
	r.fullScreenShader(e, "bloomUpsample");

	r.read("bloom4");
	fs.currentRead().setAsTexture();
	r.draw("bloom3");
	r.fullScreenShader(e, "bloomUpsample");

	r.read("bloom3");
	fs.currentRead().setAsTexture();
	r.draw("bloom2");
	r.fullScreenShader(e, "bloomUpsample");

	r.read("bloom2");
	fs.currentRead().setAsTexture();
	r.draw("bloom1");
	r.fullScreenShader(e, "bloomUpsample");

	r.read("bloom1");
	fs.currentRead().setAsTexture();
	r.draw("postProcess");
	r.fullScreenShader(e, "bloomUpsample");

	r.read("postProcess");
	fs.currentRead().setAsTexture();
	r.draw("main");
	//r.clearColor();
	r.fullScreenShader(e, "lastBloomUpsample");

	r.postProcess(e, "gammaCorrection");

	//r.screenSpace();
	//fonts.setFontSize(100);
	//r.textRenderSystem.render(e, r, toString(mod(r.time.total / 10, 0.2) + 0.8), Vec2(50), fonts.heading);
};

OOTeam::OOTeam(Index id, Color color) {
	this->id = id;
	this->color = color;
}

OOUnit::OOUnit(Index team) {
	this->team = team;
}

OOUnit::OOUnit(Index team, Vec2 location, Vec2 direction) {
	this->team = team;
	this->location = location;
	this->direction = direction;
}

//overrides update and inputEvent to work

void OrbitalOblivionPlayer::update(Window& window) {
	//get frequently used info
	Float delta = window.renderer.deltaTime();

	//set up temporary data
	Vec3 movementVector = Vec3(0);
	Float desiredSpeed = 0;

	//process input
	if (window.capturing) camera.rotate(window.mouseDelta * MouseMovement(mouseSensitivity));
	if (window.pressed(forward)) movementVector += camera.forwardVector;
	if (window.pressed(backward)) movementVector -= camera.forwardVector;
	if (window.pressed(right)) movementVector += camera.rightVector;
	if (window.pressed(left)) movementVector -= camera.rightVector;
	if (window.pressed(up)) movementVector += camera.upVector;
	if (window.pressed(down)) movementVector -= camera.upVector;

	//calculate movement
	if (length(movementVector) > 0) {					//if there is movement input
		desiredSpeed = pow(baseNumber, speedExponent);	//calculate speed
		movementVector = normalize(movementVector);		//get direction of movement (just direction)
		movementVector *= desiredSpeed * delta;			//add speed to direction
		camera.location += movementVector;				//add it to cameras location
	}
}

void OrbitalOblivionPlayer::inputEvent(Window& window, InputEvent input) {
	if (input == faster) speedExponent++;
	if (input == slower) speedExponent--;
}
