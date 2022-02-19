
#include "DungeonsAndDiscord.hpp"
#include "Window.hpp"
#include "Random.hpp"
#include "FileSystem.hpp"
#include "GameSystem.hpp"


Int diceRoll(Int diceCount) {
	return 1 + randomInt(diceCount);
}

DNDRenderer::DNDRenderer(DNDWorld* world) {
	this->world = world;
	player.speedExponent = world->speedExponent;
	player.camera.farClipValue = world->farClipValue;
	player.camera.nearClipValue = world->nearClipValue;
	player.camera.fieldOfView = world->fieldOfView;
	player.camera.rotationX = world->rotationX;
	player.camera.rotationZ = world->rotationZ;
	player.camera.location = world->location;
	player.camera.update();
}
void DNDRenderer::update(Renderer& renderer)
{
	if (forward.pressed)	player.accelerationVector += player.camera.forwardVector;
	if (backward.pressed)	player.accelerationVector -= player.camera.forwardVector;
	if (upward.pressed)		player.accelerationVector += player.camera.upVector;
	if (downward.pressed)	player.accelerationVector -= player.camera.upVector;
	if (right.pressed)		player.accelerationVector += player.camera.rightVector;
	if (left.pressed)		player.accelerationVector -= player.camera.rightVector;

	player.update(renderer);
}

void DNDRenderer::mouseIsMoving(Window& window, IVec2 position) {
	if (inputManager.isCapturing(window))player.camera.rotate(Vec2(position) * Vec2(mouseSensitivity));
}
void DNDRenderer::render(TiledRectangle area, Renderer& renderer) {
	//preprocess
	renderer.setWireframeMode();
	renderer.setCulling(false);
	renderer.setDepthTest(true);
	//renderer.clearColor();
	//renderer.clearDepth();
	renderer.renderSky(player.camera);

	renderer.renderAtmosphere(player, normalize(Vec3(1)));

	renderer.useShader("dndUberShader");
	renderer.uniforms().mMatrix(matrixFromLocationAndSize(Vec4(0, 0, -1,1000)));
	renderer.renderMesh("plane");

	renderer.clearDepth();
	//scene
	renderer.setAlphaBlending(false);

	player.render(renderer);

	PixelIDs ids;
	if (inputManager.capturing) ids = renderer.framebufferSystem.idFramebuffer().getIDsAtCenter();
	else ids = renderer.framebufferSystem.idFramebuffer().getIDsAtLocation(inputManager.cursorPosition.x, inputManager.cursorPosition.y);

	renderer.setCulling(false);
	renderer.useShader("dndUberShader");
	for (Int i = 0; i < world->entities.size(); i++) {
		//render selected objects with an orange highlight
		if (!inputManager.capturing) {
			if (std::find(selectedObjects.begin(), selectedObjects.end(), i) != selectedObjects.end()) renderer.setColor(Color(0.4, 0.25, 0.1, 0));
			else renderer.setColor(Color(0, 0, 0, 0));

			//render last object with brighter highlight
			if (lastSelectedObject == i) renderer.setColor(Color(0.5, 0.4, 0.15, 0));
		} else renderer.setColor(Color(0, 0, 0, 0));

		world->entities[i].render(renderer);
	}

	renderer.uniforms().reset();

	renderer.setWireframeMode(false);
	renderer.screenSpace();
	Int i = 0;
	for (DNDEntity& e : world->entities) {
		i++;
		renderer.renderText(String(e.meshName.data), Vec2(15, i * 15), fonts.paragraph);
	}

	world->speedExponent = player.speedExponent;
	world->fieldOfView = player.camera.fieldOfView;
	world->nearClipValue = player.camera.nearClipValue;
	world->farClipValue = player.camera.farClipValue;
	world->location = player.camera.location;
	world->rotationX = player.camera.rotationX;
	world->rotationZ = player.camera.rotationZ;
}
void DNDRenderer::filesDropped(Window& window, Vector<Path> paths) {
	for (Path& path : paths) {
		if (path.filename().extension() == ".fbx") copyFile(path, std::filesystem::absolute(Path(String("Data/objects/"))));

		String fileName = path.filename().string();
		Name name = fileName.substr(0, fileName.size() - 4).c_str();
		
		DNDEntity e;
		e.meshName = name;
		e.transform = Transform();
		world->entities.push_back(e);
	}
}
void DNDRenderer::renderInteractive(TiledRectangle area, Renderer& renderer)
{
	player.render(renderer);

	for (Int i = 0; i < world->entities.size(); i++) {
		renderer.uniforms().objectID() = i;
		world->entities[i].render(renderer);
	}
}
void DNDRenderer::mouseIsScrolled(Window& window, Double xAxis, Double yAxis) {
	player.speedExponent += yAxis;
}
void DNDRenderer::buttonIsPressed(Window& window, Key key, ActionState action, Int modifiers) {
	if (action == ActionState::Press) {
		switch (key) {
		case Key::F: window.renderer.wireframeMode = !window.renderer.wireframeMode;
			break;
		case Key::W: forward.pressed = true;
			break;
		case Key::S: backward.pressed = true;
			break;
		case Key::A: left.pressed = true;
			break;
		case Key::D: right.pressed = true;
			break;
		case Key::Q: downward.pressed = true;
			break;
		case Key::E: upward.pressed = true;
			break;
		case Key::SPACE:
			if (inputManager.capturing)inputManager.uncaptureCursor(window);
			else		inputManager.captureCursor(window);
			break;
		default:
			break;
		}
	}

	if (action == ActionState::Release) {
		switch (key) {
		case Key::W: forward.pressed = false;
			break;
		case Key::S: backward.pressed = false;
			break;
		case Key::A: left.pressed = false;
			break;
		case Key::D: right.pressed = false;
			break;
		case Key::Q: downward.pressed = false;
			break;
		case Key::E: upward.pressed = false;
			break;
		default:
			break;
		}
	}
}
void DNDRenderer::mouseIsPressed(Window& window, MouseButton button, ActionState action, Int modifiers) {
	if (action == ActionState::Press) {
		if (button == MouseButton::LEFT) {
			if (!inputManager.isCapturing(window)) {

				Index objectID = window.renderer.framebufferSystem.idFramebuffer().currentIds.objectID;

				logDebug(objectID);

				if (objectID != -1) {

					if (!window.isKeyPressed(Key::SHIFT_L) && !window.isKeyPressed(Key::SHIFT_R)) {
						selectedObjects.clear();
					}
					selectedObjects.push_back(objectID);
					lastSelectedObject = objectID;

				}
				else {
					lastSelectedObject = -1;
					selectedObjects.clear();
				}
			}
		}
	}
}

void DNDWorld::save() {
	UInt entityCount = entities.size();
	if (entityCount) {
		OutFile save("data/saves/dnd.save");

		save.write((Char*)&fieldOfView, sizeof(Float));
		save.write((Char*)&nearClipValue, sizeof(Float));
		save.write((Char*)&farClipValue, sizeof(Float));
		save.write((Char*)&location, sizeof(Vec3));
		save.write((Char*)&rotationX, sizeof(Float));
		save.write((Char*)&rotationZ, sizeof(Float));
		save.write((Char*)&speedExponent, sizeof(Int));

		save.write((Char*)&entityCount, sizeof(UInt));
		save.write((Char*)&entities[0], sizeof(Entity) * entities.size());
	}
}
void DNDWorld::load() {

	logDebug("hello");

	Path saveGameLocation = "data/saves/dnd.save";

	if (doesPathExist(saveGameLocation)) {
		InFile save("data/saves/dnd.save");

		save.read((Char*)&fieldOfView, sizeof(Float));
		save.read((Char*)&nearClipValue, sizeof(Float));
		save.read((Char*)&farClipValue, sizeof(Float));
		save.read((Char*)&location, sizeof(Vec3));
		save.read((Char*)&rotationX, sizeof(Float));
		save.read((Char*)&rotationZ, sizeof(Float));
		save.read((Char*)&speedExponent, sizeof(Int));

		UInt entityCount;
		save.read((Char*)&entityCount, sizeof(UInt));
		entities.resize(entityCount);
		save.read((Char*)&entities[0], sizeof(Entity) * entityCount);
	}
}
void DNDWorld::create() {
	load();
}
void DNDWorld::destroy() {
	save();
}

DND::DND() {
	gameSystem.add(world);
	renderer = DNDRenderer(&world);
}
