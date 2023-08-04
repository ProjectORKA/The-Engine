#include "MoonCrash.hpp"
#include "Window.hpp"

void MoonCrash::run()
{
	resourceManager.create();
	ui.create();
	renderer.simulation = &sim;
	sim.start(resourceManager);
	ui.window("MoonCrash", Area(1920, 1080), true, true, WindowState::Windowed, renderer, resourceManager);
	ui.run();
	sim.stop();
}

MoonCrashPlayer::MoonCrashPlayer()
{
	chunkLocation.z = ULLONG_MAX / 2;
}

void MoonCrashSimulation::destroy()
{
	planetSystem.destroy();
}

void MoonCrashRenderer::destroy(Window& window) {}

void MoonCrashPlayer::update(Window& window)
{
	// get frequently used info
	Float delta = window.renderer.deltaTime();

	// set up temporary data
	auto  movementVector = Vec3(0);
	Float desiredSpeed   = 0;

	// process input
	if(window.capturing) camera.rotate(window.mouseDelta * MouseMovement(mouseSensitivity));
	if(window.pressed(forward)) movementVector += camera.getForwardVector();
	if(window.pressed(backward)) movementVector -= camera.getForwardVector();
	if(window.pressed(right)) movementVector += camera.getRightVector();
	if(window.pressed(left)) movementVector -= camera.getRightVector();
	if(window.pressed(up)) movementVector += camera.getUpVector();
	if(window.pressed(down)) movementVector -= camera.getUpVector();

	// calculate movement
	if(length(movementVector) > 0)
	{
		// if there is movement input
		desiredSpeed   = powf(baseNumber, speedExponent);			// calculate speed
		movementVector = normalize(movementVector);					// get direction of movement (just direction)
		movementVector *= desiredSpeed * delta;						// add speed to direction
		camera.setLocation(camera.getLocation() + movementVector);	// add it to cameras location
	}

	Float planetRadius = 4;
	Vec3  v1           = rotate(Vec3(skyRotation[0]), -movementVector.x * planetRadius / static_cast<Float>(ULLONG_MAX), Vec3(0, 1, 0));
	Vec3  v2           = rotate(Vec3(skyRotation[0]), +movementVector.y * planetRadius / static_cast<Float>(ULLONG_MAX), Vec3(1, 0, 0));
	Vec3  v3           = rotate(Vec3(skyRotation[1]), -movementVector.x * planetRadius / static_cast<Float>(ULLONG_MAX), Vec3(0, 1, 0));
	Vec3  v4           = rotate(Vec3(skyRotation[1]), +movementVector.y * planetRadius / static_cast<Float>(ULLONG_MAX), Vec3(1, 0, 0));
	skyRotation[0]     = Vec4(normalize(v1 + v2), 0);
	skyRotation[1]     = Vec4(normalize(v3 + v4), 0);
	skyRotation[2]     = Vec4(normalize(cross(Vec3(skyRotation[0]), Vec3(skyRotation[1]))), 0);
	skyRotation[3]     = Vec4(0, 0, 0, 1);

	// apply sub chunk location
	if(floor(camera.getLocation().x) != 0.0f)
	{
		if(floor(camera.getLocation().x) > 0.0f)
		{
			ULL chunkDelta = floor(camera.getLocation().x);
			camera.setLocation(camera.getLocation() - Vec3(chunkDelta, 0, 0));
			chunkLocation.x += chunkDelta;
		}
		else
		{
			ULL chunkDelta = -floor(camera.getLocation().x);
			camera.setLocation(camera.getLocation() + Vec3(chunkDelta, 0, 0));
			chunkLocation.x -= chunkDelta;
		}
	}
	if(floor(camera.getLocation().y) != 0)
	{
		if(floor(camera.getLocation().y) > 0)
		{
			ULL chunkDelta = floor(camera.getLocation().y);
			camera.setLocation(camera.getLocation() - Vec3(0, chunkDelta, 0));
			chunkLocation.y += chunkDelta;
		}
		else
		{
			ULL chunkDelta = -floor(camera.getLocation().y);
			camera.setLocation(camera.getLocation() + Vec3(0, chunkDelta, 0));
			chunkLocation.y -= chunkDelta;
		}
	}
	if(floor(camera.getLocation().z) != 0)
	{
		if(floor(camera.getLocation().z) > 0)
		{
			ULL chunkDelta = floor(camera.getLocation().z);
			if(chunkLocation.z + chunkDelta > chunkLocation.z)
			{
				camera.setLocation(camera.getLocation() - Vec3(0, 0, chunkDelta));
				chunkLocation.z += chunkDelta;
			}
			else
			{
				// camera is outside world bounds, but we will render anyway
				chunkDelta      = ULLONG_MAX - chunkLocation.z;
				chunkLocation.z = ULLONG_MAX;
				camera.setLocation(camera.getLocation() + Vec3(0, 0, chunkDelta));
			}
		}
		else
		{
			ULL chunkDelta = -floor(camera.getLocation().z);
			if(chunkDelta > chunkLocation.z)
			{
				chunkLocation.z = 0;
				camera.setLocation(camera.getLocation() * Vec3(1, 1, 0));
			}
			else
			{
				camera.setLocation(camera.getLocation() + Vec3(0, 0, chunkDelta));
				chunkLocation.z -= chunkDelta;
			}
		}
	}
}

void MoonCrashSimulation::update(Float delta)
{
	planetSystem.update();
}

void MoonCrashRenderer::update(Window& window)
{
	player.update(window);
	window.renderer.planetRenderSystem.update(simulation->planetSystem, player);
}

void MoonCrashSimulation::create(ResourceManager& resourceManager)
{
	planetSystem.create(resourceManager);
}

void MoonCrashRenderer::inputEvent(Window& window, const InputEvent input)
{
	if(input == wireFrameToggle) window.renderer.wireframeMode = !window.renderer.wireframeMode;
	if(input == countNodesButton) window.renderer.planetRenderSystem.quadtreeRenderSystem.count();
	if(input == worldDistortion) window.renderer.planetRenderSystem.worldDistortion = !window.renderer.planetRenderSystem.worldDistortion;
	if(input == vertexColorToggle) window.renderer.planetRenderSystem.vertexColors = !window.renderer.planetRenderSystem.vertexColors;
	if(input == rebuildShaders)
	{
		window.renderer.mutex.lock();
		window.renderer.shaderSystem.rebuild();
		window.renderer.mutex.unlock();
	}
	if(input == enter) window.captureCursor();
	if(input == exit) window.unCaptureCursor();

	player.inputEvent(window, input);
}

void MoonCrashPlayer::render(ResourceManager& resourceManager, Window& window)
{
	Renderer& renderer = window.renderer;

	camera.render(renderer);
}

void MoonCrashRenderer::create(ResourceManager& resourceManager, Window& window)
{
	player.speedExponent   = 200;
	player.chunkLocation.z = 16000000000000000000;

	mainFramebuffer.create("MainFramebuffer", Area(1920, 1080));
	mainFramebuffer.add(WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
	mainFramebuffer.add(WritePixelsFormat::Depth, DataType::Float, FramebufferAttachment::Depth, false, Wrapping::Clamped);
	mainFramebuffer.checkComplete();
}

void MoonCrashRenderer::render(ResourceManager& rm, Window& window, TiledRectangle area)
{
	Renderer& r = window.renderer;

	mainFramebuffer.resize(area.size);
	mainFramebuffer.bindDraw();
	mainFramebuffer.clear();

	player.render(rm, window);

	r.uniforms().setSunDir(player.skyRotation * Vec4(0, 0, 1, 0));

	r.setWireframeMode(r.wireframeMode);

	r.uniforms().setMMatrix(player.skyRotation);
	r.renderSky(rm, player.camera);

	// renderMoonCrashAtmosphere(rm, r, player, mainFramebuffer);

	renderPlanet(rm, r, simulation->planetSystem, player, mainFramebuffer);

	// render UI
	r.setWireframeMode(false);
	r.setDepthTest(false);
	r.screenSpace();
	r.textRenderSystem.setStyle(fonts.paragraph);
	constexpr Float textSpacing = 20;
	r.textRenderSystem.alignText(Alignment::left, Alignment::bottom);

	r.textRenderSystem.render(rm, r, "Speed: " + toString(player.speedExponent), Vec2(textSpacing, 6 * textSpacing));
	r.textRenderSystem.render(rm, r, "FPS: " + toString(1.0f / r.time.delta), Vec2(textSpacing, 5 * textSpacing));
	r.textRenderSystem.render(rm, r, "FrameTime: " + toString(r.time.delta), Vec2(textSpacing, 4 * textSpacing));
	r.textRenderSystem.render(rm, r, "Camera height float: " + toString(player.camera.getLocation().z), Vec2(textSpacing, 3 * textSpacing));
	r.textRenderSystem.render(rm, r, "Camera height ULL: " + toString(player.chunkLocation.z), Vec2(textSpacing, 2 * textSpacing));
	r.textRenderSystem.render(rm, r, "SunDir: " + toString(r.uniforms().getSunDir()), Vec2(textSpacing, 1 * textSpacing));

	mainFramebuffer.setAsTexture(0);
	r.drawToWindow();
	r.fullScreenShader(rm, "final");
}

void renderMoonCrashAtmosphere(ResourceManager& resourceManager, Renderer& renderer, const MoonCrashPlayer& player, const Framebuffer& framebuffer)
{
	const Bool culling = renderer.getCulling();
	player.camera.renderOnlyRot(renderer);
	renderer.uniforms().setCameraPos(Vec4(Vec3(0, 0, (static_cast<Float>(player.chunkLocation.z) + player.camera.getLocation().z) / static_cast<Float>(ULLONG_MAX)), 1));
	renderer.setCulling(false);
	renderer.useShader(resourceManager, "atmosphere");
	framebuffer.setAsTexture(0); // [TODO]might not work if draw doesn't bind; check

	// renderer.setAlphaBlending(true);
	// glBlendFunc(GL_ONE, GL_ONE);
	renderer.setDepthTest(false);

	renderer.renderMesh(resourceManager, "centeredCube");
	renderer.setCulling(culling);

	// renderer.setAlphaBlending(false);
	renderer.setDepthTest(true);
}

void renderPlanet(ResourceManager& resourceManager, Renderer& r, PlanetSystem& planetSystem, const PlanetSystemPlayer& player, const Framebuffer& framebuffer)
{
	// set modes
	r.setAlphaBlending(false);
	r.setDepthTest(false);

	// set uniforms
	r.uniforms().setCustomInt1(r.planetRenderSystem.worldDistortion);
	r.uniforms().setMMatrix(Matrix(1));
	player.camera.renderOnlyRot(r);

	// render meshes
	for(UShort level = 0; level < MAX_CHUNK_LEVEL; level++) r.planetRenderSystem.renderLevel(resourceManager, planetSystem, r, player, level, framebuffer);
}
