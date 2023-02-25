
#include "DungeonsAndDiscord.hpp"
#include "Window.hpp"
#include "Random.hpp"
#include "FileSystem.hpp"

Int diceRoll(Int diceCount) {
	return 1 + randomInt(diceCount);
}

void DNDRenderer::create(Engine& engine, Window& window) {
	player.speedExponent = world->speedExponent;
	player.camera.farClipValue = world->farClipValue;
	player.camera.nearClipValue = world->nearClipValue;
	player.camera.fieldOfView = world->fieldOfView;
	player.camera.setRotation(world->rotation);
	player.camera.location = world->location;

	world->load();
}

void DNDRenderer::destroy(Window& window) {
	world->save();
}

void DNDRenderer::update(Window& window)
{
	player.update(window);

	for (Path& path : window.droppedFilePaths) {
		Name name = path.stem().string();

		DNDEntity e;
		e.meshName = name;
		e.transform = Transform();
		world->entities.pushBack(e);
	}
	window.droppedFilePaths.clear();
}
void DNDRenderer::render(Engine& e, Window& window, TiledRectangle area) {
	Renderer& r = window.renderer;
	
	//preprocess
	r.setWireframeMode();
	r.setCulling(false);
	r.setAlphaBlending(false);
	r.setDepthTest(false);
	r.fill(Color(1));
	openglState;
	r.clearColor();
	r.clearDepth();
	//renderer.renderSky(player.camera);
	//renderer.renderAtmosphere(engine, player, normalize(Vec3(1)));

	r.blendModeAdditive();
	player.camera.renderOnlyRot(r);
	r.useTexture(e, "dndSky");
	r.useShader(e, "dndSky");
	r.renderMesh(e, "centeredCube");

	r.setDepthTest(true);

	r.postProcess(e, "tonemapping");

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
void DNDRenderer::inputEvent(Window& window, InputEvent input) {

	if (input == enter) window.captureCursor();
	if (input == exit) window.uncaptureCursor();

	if (input == select && window.capturing) {
		Index objectID = window.renderer.idFramebuffer.objectID;

		logDebug(objectID);

		if (objectID != -1) {
			if (!window.pressed(selectMultiple)) selectedObjects.clear();
			selectedObjects.pushBack(objectID);
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
void DNDRenderer::renderInteractive(Engine& engine, Window& window, TiledRectangle area)
{
	player.render(engine, window);

	if (world->loaded) {
		for (Int i = 0; i < world->entities.size(); i++) {
			window.renderer.uniforms().objectID() = i;
			world->entities[i].render(engine, window.renderer);
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
void DNDWorld::load() {
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
}
void DNDEntity::render(Engine& engine, Renderer& renderer) {
	transform.render(renderer);
	renderer.renderMesh(engine, meshName);
}
