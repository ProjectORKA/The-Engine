
#include "DungeonsAndDiscord.hpp"
#include "Window.hpp"
#include "Random.hpp"

void DungeonsAndDiscord::update() {

	//Bool exit = false;

	//randomizeSeed();

	//system("cls");

	//while (!exit) {
	//	std::cout << "Command: ";

	//	String s;
	//	std::cin >> s;

	//	if (s == "dice") {
	//		std::cout << "Dice Count: ";
	//		std::cin >> s;

	//		Int diceCount = std::stoi(s);
	//		std::cout << "Dice is: " << diceRoll(diceCount) << "\n\n";
	//	}

	//	if (s == "vis") {
	//		std::cout << "Perception/Stealth Count: ";
	//		std::cin >> s;

	//		Int diceCount = std::stoi(s);
	//		std::cout << "Dice is: " << randomInt(diceCount/2+1) + diceCount / 2 << "\n\n";
	//	}

	//	if (s == "exit") exit = true;
	//}
}

void DungeonsAndDiscord::render(Renderer& renderer) {
	//update camera right before rendering
	if (forward.pressed)	player.accelerationVector += player.camera.forwardVector;
	if (backward.pressed)	player.accelerationVector -= player.camera.forwardVector;
	if (upward.pressed)		player.accelerationVector += player.camera.upVector;
	if (downward.pressed)	player.accelerationVector -= player.camera.upVector;
	if (right.pressed)		player.accelerationVector += player.camera.rightVector;
	if (left.pressed)		player.accelerationVector -= player.camera.rightVector;

	//gpu upload
	if (importObjects.size()) {
		Path path = importObjects.front();

		String fileName = path.filename().string();
		Name name = fileName.substr(0, fileName.size() - 4).c_str();
		objects.emplace_back();
		objects.back().meshName = name;
		importObjects.pop_front();
	}

	//preprocess
	renderer.setWireframeMode();
	renderer.setCulling(true);
	renderer.setDepthTest(true);
	renderer.clearDepth();
	renderer.renderSky(player.camera);

	//postprocess
	renderer.renderAtmosphere(player, normalize(Vec3(1)));

	//scene
	player.render(renderer);
	renderer.useShader("dndUberShader");
	for (Object& object : objects) {
		object.render(renderer);
	}
	renderer.renderMesh("suzanne");

	
	//ui
	renderer.screenSpace();
	Int i = 0;
	for (Object& object : objects) {
		i++;
		renderer.renderText(String(object.meshName.data), Vec2(0, i * 10), fonts.paragraph);
	}
}
void DungeonsAndDiscord::filesDropped(Window& window, Vector<Path> paths) {
	for (Path& path : paths) {
		importObjects.push_back(path);
	}
}
void DungeonsAndDiscord::buttonIsPressed(Window& window, Int keyID, Int action, Int modifiers) {
	if (action == GLFW_PRESS) {
		switch (keyID) {
		case GLFW_KEY_F: window.renderer.wireframeMode = !window.renderer.wireframeMode;
			break;
		case GLFW_KEY_W: forward.pressed = true;
			break;
		case GLFW_KEY_S: backward.pressed = true;
			break;
		case GLFW_KEY_A: left.pressed = true;
			break;
		case GLFW_KEY_D: right.pressed = true;
			break;
		case GLFW_KEY_Q: downward.pressed = true;
			break;
		case GLFW_KEY_E: upward.pressed = true;
			break;
		default:
			break;
		}
	}

	if (action == GLFW_RELEASE) {
		switch (keyID) {
		case GLFW_KEY_W: forward.pressed = false;
			break;
		case GLFW_KEY_S: backward.pressed = false;
			break;
		case GLFW_KEY_A: left.pressed = false;
			break;
		case GLFW_KEY_D: right.pressed = false;
			break;
		case GLFW_KEY_Q: downward.pressed = false;
			break;
		case GLFW_KEY_E: upward.pressed = false;
			break;
		default:
			break;
		}
	}
}
void DungeonsAndDiscord::mouseIsScrolled(Window& window, Double xAxis, Double yAxis) {
	player.speedExponent += yAxis;
}
void DungeonsAndDiscord::mouseIsPressed(Window& window, Int button, Int action, Int modifiers) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) inputManager.captureCursor(window);
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) inputManager.uncaptureCursor(window);
}
void DungeonsAndDiscord::mouseIsMoving(Window& window, IVec2 position) {
	if (inputManager.isCapturing(window))player.camera.rotate(Vec2(position) * Vec2(mouseSensitivity));
}

Int diceRoll(Int diceCount) {
	return 1 + randomInt(diceCount);
}
void Object::render(Renderer& renderer) {
	transform.render(renderer);
	renderer.renderMesh(meshName);
}
