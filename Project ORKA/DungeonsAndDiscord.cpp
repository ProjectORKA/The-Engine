#include "DungeonsAndDiscord.hpp"
#include "File.hpp"
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

	for(Path& path : window.droppedFilePaths)
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
	framebuffer.destroy();
}

void DNDRenderer::connect(GameSimulation& simulation)
{
	this->world = static_cast<DNDSimulation*>(&simulation);
}

void DNDSimulation::create()
{
	InFile save("Saves/dnd.save");

	if(save.isOpen)
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

void DNDRenderer::inputEvent(Window& window, const InputEvent input)
{
	if(input == enter) window.captureCursor();
	if(input == exit) window.releaseCursor();
	if(input == select && window.capturing)
	{
		const Index objectId = window.renderer.objectId;

		logDebug(objectId);

		if(objectId != static_cast<UInt>(-1))
		{
			if(!window.pressed(selectMultiple)) selectedObjects.clear();
			selectedObjects.push_back(objectId);
			lastSelectedObject = objectId;
		}
		else
		{
			if(!window.pressed(selectMultiple))
			{
				lastSelectedObject = -1;
				selectedObjects.clear();
			}
		}
	}
	if(input == toggleWireframeMode) window.renderer.wireframeMode = !window.renderer.wireframeMode;

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

	framebuffer.create("MainFramebuffer", Area(1920, 1080));
	framebuffer.add(WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
	framebuffer.add(WritePixelsFormat::Depth, DataType::Float, FramebufferAttachment::Depth, false, Wrapping::Clamped);
	framebuffer.checkComplete();
}

void DNDEntity::render(Renderer& renderer) const
{
	transform.render(renderer);
	renderer.renderMesh(meshName);
}

void DNDRenderer::render(Window& window, const TiledRectangle area)
{
	Renderer& r = window.renderer;

	framebuffer.resize(area.size);
	framebuffer.clear();
	framebuffer.bindDraw();

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
	framebuffer.clearDepth();
	r.setDepthTest(true);
	r.setCulling(true);

	for(Int i = 0; i < world->entities.size(); i++)
	{
		// render selected objects with an orange highlight
		if(std::find(selectedObjects.begin(), selectedObjects.end(), i) != selectedObjects.end()) r.setColor(Color(0.4, 0.25, 0.1, 0));
		else r.setColor(Color(0, 0, 0, 0));

		// render last object with brighter highlight
		if(lastSelectedObject == i) r.setColor(Color(0.5, 0.4, 0.15, 0));

		world->entities[i].render(r);
	}

	r.setDepthTest(false);
	framebuffer.setAsTexture(0);
	r.drawToWindow();
	r.fullScreenShader("tonemapping");

	world->speedExponent = player.speedExponent;
	world->fieldOfView   = player.camera.getFieldOfView();
	world->nearClipValue = player.camera.getNearClipValue();
	world->farClipValue  = player.camera.getFarClipValue();
	world->position      = player.camera.getPosition();
	world->rotation      = player.camera.getRotation();
}

void DNDRenderer::renderInteractive(Window& window, TiledRectangle area)
{
	// player.render(window);

	// if(world->loaded)
	//	for(Int i = 0; i < static_cast<Int>(world->entities.size()); i++)
	//	{
	//		window.renderer.uniforms().objectId() = i;
	//		world->entities[i].render(window.renderer);
	//	}
}
