#include "DungeonsAndDiscord.hpp"
#include "File.hpp"
#include "Renderer.hpp"
#include "Window.hpp"
#include "Random.hpp"

void DNDSimulation::destroy()
{
	SizeT entityCount = entities.size();

	OutFile save("Saves/dnd.save");
	save.write(reinterpret_cast<Char*>(&fieldOfView), sizeof(Float));
	save.write(reinterpret_cast<Char*>(&nearClipValue), sizeof(Float));
	save.write(reinterpret_cast<Char*>(&farClipValue), sizeof(Float));
	save.write(reinterpret_cast<Char*>(&position), sizeof(Vec3));
	save.write(reinterpret_cast<Char*>(&rotation), sizeof(DVec3));
	save.write(reinterpret_cast<Char*>(&speedExponent), sizeof(Int));

	save.write(reinterpret_cast<Char*>(&entityCount), sizeof(UInt));
	save.write(reinterpret_cast<Char*>(entities.data()), sizeof(DNDEntity) * entities.size());
}

void DNDSimulation::update(Float delta) {}

Int diceRoll(const Int diceCount)
{
	return 1 + randomIntSlow(diceCount);
}

void DNDRenderer::update(Window& window)
{
	player.update(window);

	for (Path& path : window.droppedFilePaths)
	{
		const Name name = path.stem().string();
		DNDEntity  e;
		e.meshName  = name;
		e.transform = Transform();
		world->entities.push_back(e);
	}

	window.droppedFilePaths.clear();
}

void DNDRenderer::destroy(Window& window)
{
	GameRenderer::destroy(window);
}

void DNDRenderer::connect(GameSimulation& simulation)
{
	this->world = static_cast<DNDSimulation*>(&simulation);
}

void DNDSimulation::create()
{
	InFile save("Saves/dnd.save");

	if (save.isOpen)
	{
		save.read(reinterpret_cast<Char*>(&fieldOfView), sizeof(Float));
		save.read(reinterpret_cast<Char*>(&nearClipValue), sizeof(Float));
		save.read(reinterpret_cast<Char*>(&farClipValue), sizeof(Float));
		save.read(reinterpret_cast<Char*>(&position), sizeof(Vec3));
		save.read(reinterpret_cast<Char*>(&rotation), sizeof(DVec3));
		save.read(reinterpret_cast<Char*>(&speedExponent), sizeof(Int));

		UInt entityCount;
		save.read(reinterpret_cast<Char*>(&entityCount), sizeof(UInt));
		entities.resize(entityCount);
		save.read(reinterpret_cast<Char*>(entities.data()), sizeof(DNDEntity) * entityCount);
	}
}

void DNDRenderer::inputEvent(Window& window, InputEvent& input)
{
	if (input == enter) ui.capture(*this);
	if (input == exit) ui.releaseCapture();
	if (input == select && ui.capturing)
	{
		const Index objectId = window.renderer.objectId;

		logDebug(objectId);

		if (objectId != static_cast<UInt>(-1))
		{
			if (!window.pressed(selectMultiple)) selectedObjects.clear();
			selectedObjects.push_back(objectId);
			lastSelectedObject = objectId;
		}
		else
		{
			if (!window.pressed(selectMultiple))
			{
				lastSelectedObject = -1;
				selectedObjects.clear();
			}
		}
	}
	if (input == toggleWireframeMode) window.renderer.wireframeMode = !window.renderer.wireframeMode;

	player.inputEvent(window, input);
}

void DNDRenderer::create(Window& window)
{
	player.speedExponent = world->speedExponent;
	player.camera.setFarClipValue(world->farClipValue);
	player.camera.setNearClipValue(world->nearClipValue);
	player.camera.setFieldOfView(world->fieldOfView);
	player.camera.setRotation(world->rotation);
	player.camera.setPosition(world->position);

	GameRenderer::create(window);
}

void DNDEntity::render(Renderer& renderer) const
{
	transform.render(renderer);
	renderer.renderMesh(meshName);
}

void DNDRenderer::renderScene(Window& window)
{
	Renderer& r = window.renderer;

	framebuffer.clear(r);

	// preprocess
	r.setWireframeMode();
	r.setCulling(false);
	r.setAlphaBlending(false);
	r.setDepthTest(false);
	r.fill(Color(1));

	// r.renderSky(player.camera);
	// r.renderAtmosphere(player, normalize(Vec3(1)));

	// r.blendModeAdditive();
	player.camera.renderOnlyRot(r);
	r.useTexture("dndSky");
	r.useShader("dndSky");
	r.renderMesh("centeredCube");
	// r.postProcess("tonemapping");

	r.setWireframeMode();

	player.render(window);

	// background scene
	r.useShader("dndUberShader");
	r.uniforms().setMMatrix(matrixFromPositionAndSize(Vec4(0, 0, 0, 100)));
	r.renderMesh("dndGroundPlane");
	r.uniforms().setMMatrix(matrixFromPositionAndSize(Vec4(0, 0, 0, 10000)));
	r.renderMesh("centeredDetailedGrid");

	// objects
	framebuffer.clearDepth(r);
	r.setDepthTest(true);
	r.setCulling(true);

	for (Int i = 0; i < world->entities.size(); i++)
	{
		// render selected objects with an orange highlight
		if (std::ranges::find(selectedObjects, i) != selectedObjects.end()) r.setColor(Color(0.4, 0.25, 0.1, 0));
		else r.setColor(Color(0, 0, 0, 0));

		// render last object with brighter highlight
		if (lastSelectedObject == i) r.setColor(Color(0.5, 0.4, 0.15, 0));

		world->entities[i].render(r);
	}

	world->speedExponent = player.speedExponent;
	world->fieldOfView   = player.camera.getFieldOfView();
	world->nearClipValue = player.camera.getNearClipValue();
	world->farClipValue  = player.camera.getFarClipValue();
	world->position      = player.camera.getPosition();
	world->rotation      = player.camera.getRotation();
}

void DNDRenderer::renderInteractive(Window& window)
{
	// player.render(window);

	// if(world->loaded)
	//	for(Int i = 0; i < static_cast<Int>(world->entities.size()); i++)
	//	{
	//		window.renderer.uniforms().objectId() = i;
	//		world->entities[i].render(window.renderer);
	//	}
}
