#include "MoonCrash.hpp"
#include "Window.hpp"



MoonCrashPlayer::MoonCrashPlayer()
{
	chunkPosition.z = ULLONG_MAX / 2;
}

void MoonCrashSimulation::destroy()
{
	planetSystem.destroy();
}

void MoonCrashRenderer::destroy(Window& window)
{
	mainFramebuffer.destroy(window.renderer);
}

void MoonCrashRenderer::connect(GameSimulation& simulation)
{
	this->simulation = static_cast<MoonCrashSimulation*>(&simulation);
}

void MoonCrashPlayer::update(Window& window)
{
	// get frequently used info
	Float delta = window.renderer.deltaTime();

	// set up temporary data
	auto  movementVector = Vec3(0);
	Float desiredSpeed   = 0;

	// process input
	if(window.capturing) camera.rotate(window.mouseDelta * DVec2(mouseSensitivity));
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
		camera.setPosition(camera.getPosition() + movementVector);	// add it to cameras position
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

	// apply sub chunk position
	if(floor(camera.getPosition().x) != 0.0f)
	{
		if(floor(camera.getPosition().x) > 0.0f)
		{
			ULL chunkDelta = floor(camera.getPosition().x);
			camera.setPosition(camera.getPosition() - Vec3(chunkDelta, 0, 0));
			chunkPosition.x += chunkDelta;
		}
		else
		{
			ULL chunkDelta = -floor(camera.getPosition().x);
			camera.setPosition(camera.getPosition() + Vec3(chunkDelta, 0, 0));
			chunkPosition.x -= chunkDelta;
		}
	}
	if(floor(camera.getPosition().y) != 0)
	{
		if(floor(camera.getPosition().y) > 0)
		{
			ULL chunkDelta = floor(camera.getPosition().y);
			camera.setPosition(camera.getPosition() - Vec3(0, chunkDelta, 0));
			chunkPosition.y += chunkDelta;
		}
		else
		{
			ULL chunkDelta = -floor(camera.getPosition().y);
			camera.setPosition(camera.getPosition() + Vec3(0, chunkDelta, 0));
			chunkPosition.y -= chunkDelta;
		}
	}
	if(floor(camera.getPosition().z) != 0)
	{
		if(floor(camera.getPosition().z) > 0)
		{
			ULL chunkDelta = floor(camera.getPosition().z);
			if(chunkPosition.z + chunkDelta > chunkPosition.z)
			{
				camera.setPosition(camera.getPosition() - Vec3(0, 0, chunkDelta));
				chunkPosition.z += chunkDelta;
			}
			else
			{
				// camera is outside world bounds, but we will render anyway
				chunkDelta      = ULLONG_MAX - chunkPosition.z;
				chunkPosition.z = ULLONG_MAX;
				camera.setPosition(camera.getPosition() + Vec3(0, 0, chunkDelta));
			}
		}
		else
		{
			ULL chunkDelta = -floor(camera.getPosition().z);
			if(chunkDelta > chunkPosition.z)
			{
				chunkPosition.z = 0;
				camera.setPosition(camera.getPosition() * Vec3(1, 1, 0));
			}
			else
			{
				camera.setPosition(camera.getPosition() + Vec3(0, 0, chunkDelta));
				chunkPosition.z -= chunkDelta;
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
	window.renderer.planetRenderSystem.update(window.renderer, simulation->planetSystem, player);
}

void MoonCrashSimulation::create()
{
	planetSystem.create();
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
	if(input == exit) window.releaseCursor();

	player.inputEvent(window, input);
}

void MoonCrashPlayer::render(Window& window)
{
	Renderer& renderer = window.renderer;

	camera.render(renderer);
}

void MoonCrashRenderer::create(Window& window)
{
	player.speedExponent   = 200;
	player.chunkPosition.z = 16000000000000000000;

	mainFramebuffer.create("MainFramebuffer", Area(1920, 1080));
	mainFramebuffer.add(window.renderer, WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
	mainFramebuffer.add(window.renderer, WritePixelsFormat::Depth, DataType::Float, FramebufferAttachment::Depth, false, Wrapping::Clamped);
	mainFramebuffer.checkComplete();
}

void MoonCrashRenderer::render(Window& window, TiledRectangle area)
{
	Renderer& r = window.renderer;

	mainFramebuffer.resize(r, area.size);
	mainFramebuffer.bindDraw(r);
	mainFramebuffer.clear();

	player.render(window);

	r.uniforms().setSunDir(player.skyRotation * Vec4(0, 0, 1, 0));

	r.setWireframeMode(r.wireframeMode);

	r.uniforms().setMMatrix(player.skyRotation);
	r.renderSky(player.camera);

	// renderMoonCrashAtmosphere(rm, r, player, mainFramebuffer);

	renderPlanet(r, simulation->planetSystem, player, mainFramebuffer);

	// render UI
	r.setWireframeMode(false);
	r.setDepthTest(false);
	r.screenSpace();
	r.textRenderSystem.setSize(16.0f);
	r.textRenderSystem.setLetterSpacing(0.6f);
	constexpr Float textSpacing = 20;
	r.textRenderSystem.alignText(Alignment::left, Alignment::bottom);

	r.textRenderSystem.render(r, "Speed: " + toString(player.speedExponent), Vec2(textSpacing, 6 * textSpacing));
	r.textRenderSystem.render(r, "FPS: " + toString(1.0f / r.time.getDelta()), Vec2(textSpacing, 5 * textSpacing));
	r.textRenderSystem.render(r, "FrameTime: " + toString(r.time.getDelta()), Vec2(textSpacing, 4 * textSpacing));
	r.textRenderSystem.render(r, "Camera height float: " + toString(player.camera.getPosition().z), Vec2(textSpacing, 3 * textSpacing));
	r.textRenderSystem.render(r, "Camera height ULL: " + toString(player.chunkPosition.z), Vec2(textSpacing, 2 * textSpacing));
	r.textRenderSystem.render(r, "SunDir: " + toString(r.uniforms().getSunDir()), Vec2(textSpacing, 1 * textSpacing));

	mainFramebuffer.setAsTexture(0);
	r.drawToWindow();
	r.fullScreenShader("final");
}

void renderMoonCrashAtmosphere(Renderer& renderer, const MoonCrashPlayer& player, const Framebuffer& framebuffer)
{
	const Bool culling = renderer.getCulling();
	player.camera.renderOnlyRot(renderer);
	renderer.uniforms().setCameraPos(Vec4(Vec3(0, 0, (static_cast<Float>(player.chunkPosition.z) + player.camera.getPosition().z) / static_cast<Float>(ULLONG_MAX)), 1));
	renderer.setCulling(false);
	renderer.useShader("atmosphere");
	framebuffer.setAsTexture(0); // [TODO]might not work if draw doesn't bind; check

	// renderer.setAlphaBlending(true);
	// glBlendFunc(GL_ONE, GL_ONE);
	renderer.setDepthTest(false);

	renderer.renderMesh("centeredCube");
	renderer.setCulling(culling);

	// renderer.setAlphaBlending(false);
	renderer.setDepthTest(true);
}

void renderPlanet(Renderer& r, PlanetSystem& planetSystem, const PlanetSystemPlayer& player, const Framebuffer& framebuffer)
{
	// set modes
	r.setAlphaBlending(false);
	r.setDepthTest(false);

	// set uniforms
	r.uniforms().setCustomInt1(r.planetRenderSystem.worldDistortion);
	r.uniforms().setMMatrix(Matrix(1));
	player.camera.renderOnlyRot(r);

	// render meshes
	for(UShort level = 0; level < MAX_CHUNK_LEVEL; level++) r.planetRenderSystem.renderLevel(planetSystem, r, player, level, framebuffer);
}
