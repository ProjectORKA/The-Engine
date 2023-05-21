#include "DungeonsAndDiscord.hpp"
#include "Window.hpp"
#include "Random.hpp"
#include "FileSystem.hpp"

Int diceRoll(const Int diceCount) { return 1 + randomInt(diceCount); }

void DNDRenderer::create(ResourceManager& resourceManager, Window& window) {
	player.speedExponent = world->speedExponent;
	player.camera.farClipValue = world->farClipValue;
	player.camera.nearClipValue = world->nearClipValue;
	player.camera.fieldOfView = world->fieldOfView;
	player.camera.setRotation(world->rotation);
	player.camera.location = world->location;
}

void DNDRenderer::update(Window& window) {
	player.update(window);

	for (Path& path : window.droppedFilePaths) {
		//Name name = path.stem().string();

		//DNDEntity e;
		//e.meshName = name;
		//e.transform = Transform();
		//world->entities.push_back(e);
	}
	window.droppedFilePaths.clear();
}

void DNDRenderer::render(ResourceManager& resourceManager, Window& window, TiledRectangle area) {
	Renderer& r = window.renderer;

	//preprocess
	r.setWireframeMode();
	r.setCulling(false);
	r.setAlphaBlending(false);
	r.setDepthTest(false);
	r.fill(Color(1));
	openGlState;
	r.clearColor();
	r.clearDepth();
	//renderer.renderSky(player.camera);
	//renderer.renderAtmosphere(resourceManager, player, normalize(Vec3(1)));

	r.blendModeAdditive();
	player.camera.renderOnlyRot(r);
	r.useTexture(resourceManager, "dndSky");
	r.useShader(resourceManager, "dndSky");
	r.renderMesh(resourceManager, "centeredCube");

	r.setDepthTest(true);

	r.postProcess(resourceManager, "tonemapping");

	//r.setCulling(true);


	//player.render(e, window);

	////scene
	//r.setDepthTest(false);

	//r.useShader(e, "dndUberShader");
	//r.uniforms().mMatrix(matrixFromLocationAndSize(Vec4(0, 0, 0, 10000)));
	//r.renderMesh(e, "dndGroundPlane");
	//r.uniforms().mMatrix(matrixFromLocationAndSize(Vec4(0, 0, 0, 1000000)));
	//r.renderMesh(e, "dndGroundPlane");

	//r.clearDepth();
	//r.setDepthTest(true);


	//for (Int i = 0; i < world->entities.size(); i++) {
	//	//render selected objects with an orange highlight
	//	if (true) {
	//		if (std::find(selectedObjects.begin(), selectedObjects.end(), i) != selectedObjects.end()) r.setColor(Color(0.4, 0.25, 0.1, 0));
	//		else r.setColor(Color(0, 0, 0, 0));

	//		//render last object with brighter highlight
	//		if (lastSelectedObject == i) r.setColor(Color(0.5, 0.4, 0.15, 0));
	//	}
	//	else r.setColor(Color(0, 0, 0, 0));

	//	world->entities[i].render(e, r);
	//}

	world->speedExponent = player.speedExponent;
	world->fieldOfView = player.camera.fieldOfView;
	world->nearClipValue = player.camera.nearClipValue;
	world->farClipValue = player.camera.farClipValue;
	world->location = player.camera.location;
	world->rotation = player.camera.getRotation();
}

void DNDRenderer::inputEvent(Window& window, const InputEvent input) {
	if (input == enter) window.captureCursor();
	if (input == exit) window.unCaptureCursor();

	if (input == select && window.capturing) {
		const Index objectId = window.renderer.idFramebuffer.objectId;

		logDebug(objectId);

		if (objectId != -1) {
			if (!window.pressed(selectMultiple)) selectedObjects.clear();
			selectedObjects.push_back(objectId);
			lastSelectedObject = objectId;
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

void DNDRenderer::renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) {
	player.render(resourceManager, window);

	if (world->loaded) {
		for (Int i = 0; i < world->entities.size(); i++) {
			window.renderer.uniforms().objectId() = i;
			world->entities[i].render(resourceManager, window.renderer);
		}
	}
}

void DNDWorld::save() {
	UInt entityCount = entities.size();

	OutFile save("Saves/dnd.save");
	save.write((Char*)&fieldOfView, sizeof(Float));
	save.write((Char*)&nearClipValue, sizeof(Float));
	save.write((Char*)&farClipValue, sizeof(Float));
	save.write((Char*)&location, sizeof(Vec3));
	save.write((Char*)&rotation, sizeof(DVec3));
	save.write((Char*)&speedExponent, sizeof(Int));

	save.write((Char*)&entityCount, sizeof(UInt));
	save.write((Char*)&entities[0], sizeof(DNDEntity) * entities.size());
}

void DNDWorld::load(ResourceManager& resourceManager) {
	InFile save("Saves/dnd.save");

	if (save.isOpen) {
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

		loaded = true;
	}

	scene.importFBX("dnd", resourceManager);
}

void DNDEntity::render(ResourceManager& resourceManager, Renderer& renderer) {
	transform.render(renderer);
	renderer.renderMesh(resourceManager, meshName);
}

void DungeonsAndDiscord::run() {
	sim.load(resourceManager);

	renderer.world = &sim;
	ui.window("Simple RTS", Area(1920, 1080), true, WindowState::Windowed, renderer, resourceManager);
	ui.run();

	sim.save();
}
