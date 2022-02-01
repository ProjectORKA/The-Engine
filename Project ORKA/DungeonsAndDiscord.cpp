
#include "DungeonsAndDiscord.hpp"
#include "Window.hpp"
#include "Random.hpp"
#include "FileSystem.hpp"

namespace DND {
	void uploadObjects(DungeonsAndDiscord& dnd) {
		if (dnd.importObjects.size()) {
			Path path = dnd.importObjects.front();

			String fileName = path.filename().string();
			Name name = fileName.substr(0, fileName.size() - 4).c_str();
			dnd.objects.emplace_back();
			dnd.objects.back().meshName = name;
			dnd.importObjects.pop_front();
		}
	}

	void renderClickableObjects(DungeonsAndDiscord& dnd, Renderer& renderer) {
		renderer.framebufferSystem.idFramebuffer().use();
		renderer.useShader("idShader");
		renderer.setAlphaBlending(false);
		renderer.setCulling(false);
		for (Int i = 0; i < dnd.objects.size(); i++) {
			renderer.uniforms().objectID() = i + 1;
			dnd.objects[i].render(renderer);
		}
		renderer.setCulling(true);
		renderer.framebufferSystem.current().use();
		renderer.setAlphaBlending(true);
	}

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
		DND::uploadObjects(*this);

		//preprocess
		renderer.setWireframeMode();
		renderer.setCulling(true);
		renderer.setDepthTest(true);
		//renderer.clearColor();
		//renderer.clearDepth();
		renderer.renderSky(player.camera);

		renderer.renderAtmosphere(player, normalize(Vec3(1)));
		renderer.clearDepth();
		//scene
		renderer.setAlphaBlending(false);

		player.render(renderer);

		DND::renderClickableObjects(*this, renderer);

		PixelIDs ids;
		if (inputManager.capturing) ids = renderer.framebufferSystem.idFramebuffer().getIDsAtCenter();
		else ids = renderer.framebufferSystem.idFramebuffer().getIDsAtLocation(inputManager.cursorPosition.x, inputManager.cursorPosition.y);
		renderer.framebufferSystem.use(renderer, 0);

		renderer.useShader("dndUberShader");
		for (Int i = 0; i < objects.size(); i++) {
			if (ids.objectID - 1 == i) renderer.setWireframeMode(true);
			else renderer.setWireframeMode(false);
			objects[i].render(renderer);
		}

		renderer.uniforms().reset();

		renderer.setWireframeMode(false);
		renderer.screenSpace();
		Int i = 0;
		for (Object& object : objects) {
			i++;
			renderer.renderText(String(object.meshName.data), Vec2(15, i * 15), fonts.paragraph);
		}
	}
	void DungeonsAndDiscord::mouseIsMoving(Window& window, IVec2 position) {
		if (inputManager.isCapturing(window))player.camera.rotate(Vec2(position) * Vec2(mouseSensitivity));
	}
	void DungeonsAndDiscord::filesDropped(Window& window, Vector<Path> paths) {
		for (Path& path : paths) {

			if (path.filename().extension() == ".fbx") copyFile(path, std::filesystem::absolute(Path(String("Data/objects/"))));//.append(path.filename().string()))));
			//if (path.filename().extension() == ".png") copyFile(path, "Data/textures");
			//if (path.filename().extension() == ".mesh") copyFile(path, "Data/objects");
			//if (path.filename().extension() == ".vert" || path.filename().extension() == ".frag") copyFile(path, "Data/shaders");

			importObjects.push_back(path);
		}
	}
	void DungeonsAndDiscord::mouseIsScrolled(Window& window, Double xAxis, Double yAxis) {
		player.speedExponent += yAxis;
	}
	void DungeonsAndDiscord::mouseIsPressed(Window& window, Int button, Int action, Int modifiers) {
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) inputManager.captureCursor(window);
		if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) inputManager.uncaptureCursor(window);
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

	Int diceRoll(Int diceCount) {
		return 1 + randomInt(diceCount);
	}
	void Object::render(Renderer& renderer) {
		transform.render(renderer);
		renderer.renderMesh(meshName);
	}
}