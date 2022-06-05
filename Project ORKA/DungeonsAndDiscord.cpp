
#include "DungeonsAndDiscord.hpp"
#include "Window.hpp"
#include "Random.hpp"
#include "FileSystem.hpp"
#include "GameSystem.hpp"

Int diceRoll(Int diceCount) {
	return 1 + randomInt(diceCount);
}

DNDRenderer::DNDRenderer(DNDWorld& world) {
	this->world = &world;
	player.speedExponent = world.speedExponent;
	player.camera.farClipValue = world.farClipValue;
	player.camera.nearClipValue = world.nearClipValue;
	player.camera.fieldOfView = world.fieldOfView;
	player.camera.setRotation(world.rotation);
	player.camera.location = world.location;
}
void DNDRenderer::update(Window& window)
{
	player.update(window);

	for (Path& path : window.droppedFilePaths) {
		Name name = path.stem().string();

		DNDEntity e;
		e.meshName = name;
		e.transform = Transform();
		world->entities.push_back(e);
	}
	window.droppedFilePaths.clear();
}
void DNDRenderer::render(Window& window, TiledRectangle area) {
	Renderer& renderer = window.renderer;
	
	//preprocess
	renderer.setWireframeMode();
	renderer.setCulling(true);

	renderer.clearColor();
	renderer.clearDepth();
	//renderer.renderSky(player.camera);

	renderer.renderAtmosphere(player, normalize(Vec3(1)));

	renderer.fill(Color(1));

	renderer.setAlphaBlending(false);
	renderer.setCulling(true);


	player.render(window);

	//scene
	renderer.setDepthTest(false);

	renderer.useShader("dndUberShader");
	renderer.uniforms().mMatrix(matrixFromLocationAndSize(Vec4(0, 0, 0, 10000)));
	renderer.renderMesh("dndGroundPlane");
	renderer.uniforms().mMatrix(matrixFromLocationAndSize(Vec4(0, 0, 0, 1000000)));
	renderer.renderMesh("dndGroundPlane");

	renderer.clearDepth();
	renderer.setDepthTest(true);

	if (world->loaded) {

		for (Int i = 0; i < world->entities.size(); i++) {
			//render selected objects with an orange highlight
			if (true) {
				if (std::find(selectedObjects.begin(), selectedObjects.end(), i) != selectedObjects.end()) renderer.setColor(Color(0.4, 0.25, 0.1, 0));
				else renderer.setColor(Color(0, 0, 0, 0));

				//render last object with brighter highlight
				if (lastSelectedObject == i) renderer.setColor(Color(0.5, 0.4, 0.15, 0));
			}
			else renderer.setColor(Color(0, 0, 0, 0));

			world->entities[i].render(renderer);
		}

		//text
		//renderer.uniforms().reset();
		//renderer.setWireframeMode(false);
		//renderer.screenSpace();
		//Int i = 0;
		//for (DNDEntity& e : world->entities) {
		//	i++;
		//	renderer.renderText(String(&e.meshName.data[0]), Vec2(15, i * 15), fonts.paragraph);
		//}

		world->speedExponent = player.speedExponent;
		world->fieldOfView = player.camera.fieldOfView;
		world->nearClipValue = player.camera.nearClipValue;
		world->farClipValue = player.camera.farClipValue;
		world->location = player.camera.location;
		world->rotation = player.camera.getRotation();
	}
}
void DNDRenderer::inputEvent(Window& window, InputEvent input) {

	if (input == enter) window.captureCursor();
	if (input == exit) window.uncaptureCursor();

	if (input == select && window.capturing) {
		Index objectID = window.renderer.idFramebuffer.objectID;

		logDebug(objectID);

		if (objectID != -1) {
			if (!window.pressed(selectMultiple)) selectedObjects.clear();
			selectedObjects.push_back(objectID);
			lastSelectedObject = objectID;
		}
		else {
			if (!window.pressed(selectMultiple)) {
				lastSelectedObject = -1;
				selectedObjects.clear();
			}
		}
	}

	player.inputEvent(window, input);
}
void DNDRenderer::renderInteractive(Window& window, TiledRectangle area)
{
	player.render(window);

	if (world->loaded) {
		for (Int i = 0; i < world->entities.size(); i++) {
			window.renderer.uniforms().objectID() = i;
			world->entities[i].render(window.renderer);
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
		save.write((Char*)&rotation, sizeof(DVec3));
		save.write((Char*)&speedExponent, sizeof(Int));

		save.write((Char*)&entityCount, sizeof(UInt));
		save.write((Char*)&entities[0], sizeof(DNDEntity) * entities.size());
	}
}
void DNDWorld::load() {
	Path saveGameLocation = "data/saves/dnd.save";

	if (doesPathExist(saveGameLocation)) {
		InFile save("data/saves/dnd.save");

		save.read((Char*)&fieldOfView, sizeof(Float));
		save.read((Char*)&nearClipValue, sizeof(Float));
		save.read((Char*)&farClipValue, sizeof(Float));
		save.read((Char*)&location, sizeof(Vec3));
		save.read((Char*)&rotation, sizeof(DVec3));
		save.read((Char*)&speedExponent, sizeof(Int));

		UInt entityCount;
		save.read((Char*)&entityCount, sizeof(UInt));
		entities.resize(entityCount);
		save.read((Char*)&entities[0], sizeof(DNDEntity) * entityCount);
	}

	loaded = true;
}
void DNDEntity::render(Renderer& renderer) {
	transform.render(renderer);
	renderer.renderMesh(meshName);
}
