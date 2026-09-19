#include "MoonCrash.hpp"
#include "Renderer.hpp"

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
	framebuffer.destroy(window.renderer);
}

void MoonCrashRenderer::connect(GameSimulation& simulation)
{
	this->simulation = dynamic_cast<MoonCrashSimulation*>(&simulation);
}

void MoonCrashPlayer::update(Window& window)
{
	// get frequently used info
	Float delta = window.renderer.deltaTime();

	// set up temporary data
	auto movementVector = Vec3(0);

	// process input
	if (ui.capturing) camera.rotate(window.mouseDelta * DVec2(mouseSensitivity));
	if (window.pressed(forward)) movementVector += camera.getForwardVector();
	if (window.pressed(backward)) movementVector -= camera.getForwardVector();
	if (window.pressed(right)) movementVector += camera.getRightVector();
	if (window.pressed(left)) movementVector -= camera.getRightVector();
	if (window.pressed(up)) movementVector += camera.getUpVector();
	if (window.pressed(down)) movementVector -= camera.getUpVector();

	// calculate movement
	if (length(movementVector) > 0)
	{
		Float desiredSpeed = 0;
		// if there is movement input
		desiredSpeed   = powf(baseNumber, speedExponent);          // calculate speed
		movementVector = normalize(movementVector);                // get direction of movement (just direction)
		movementVector *= desiredSpeed * delta;                    // add speed to direction
		camera.setPosition(camera.getPosition() + movementVector); // add it to cameras position
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
	if (floor(camera.getPosition().x) != 0.0f)
	{
		if (floor(camera.getPosition().x) > 0.0f)
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
	if (floor(camera.getPosition().y) != 0)
	{
		if (floor(camera.getPosition().y) > 0)
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
	if (floor(camera.getPosition().z) != 0)
	{
		if (floor(camera.getPosition().z) > 0)
		{
			if (ULL chunkDelta = floor(camera.getPosition().z); chunkPosition.z + chunkDelta > chunkPosition.z)
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
			if (ULL chunkDelta = -floor(camera.getPosition().z); chunkDelta > chunkPosition.z)
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
	planetRenderSystem.update(window.renderer, player);
}

void MoonCrashSimulation::create()
{
	planetSystem.create();
	loaded = true;
}

void MoonCrashRenderer::inputEvent(Window& window, InputEvent& input)
{
	if (input == wireFrameToggle) window.renderer.wireframeMode = !window.renderer.wireframeMode;
	if (input == countNodesButton) planetRenderSystem.count();
	if (input == worldDistortion) planetRenderSystem.worldDistortion = !planetRenderSystem.worldDistortion;
	if (input == vertexColorToggle) planetRenderSystem.vertexColors = !planetRenderSystem.vertexColors;
	if (input == chunkBoundsToggle) planetRenderSystem.chunkBoundingBoxes = !planetRenderSystem.chunkBoundingBoxes;
	if (input == rebuildShaders)
	{
		window.renderer.mutex.lock();
		window.renderer.shaderSystem.rebuild(window.renderer);
		window.renderer.mutex.unlock();
	}
	if (input == enter) ui.capture(*this);
	if (input == exit) ui.releaseCapture();

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

	// Make sure we only create the framebuffer once and with a valid size.
	if (framebufferInitialized) framebuffer.destroy(window.renderer);

	IVec2 fbSize = getSize();
	if (fbSize.x <= 0 || fbSize.y <= 0) fbSize = window.getContentSize();

	framebuffer.create(window.renderer, "MooncrashFramebuffer", fbSize);
	framebuffer.add(window.renderer, WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
	framebuffer.add(window.renderer, WritePixelsFormat::Depth, DataType::Float, FramebufferAttachment::Depth, false, Wrapping::Clamped);
	framebuffer.checkComplete(window.renderer);
	framebufferInitialized = true;

	planetRenderSystem.create(simulation->planetSystem);
}

void MoonCrashRenderer::renderScene(Window& window)
{
	Renderer& r = window.renderer;

	framebuffer.resize(r, getSize());
	framebuffer.bindDraw(r);
	framebuffer.clear(r);

	player.render(window);

	r.uniforms().setSunDir(player.skyRotation * Vec4(0, 0, 1, 0));

	r.setWireframeMode(r.wireframeMode);

	r.uniforms().setMMatrix(player.skyRotation);
	r.renderSky(player.camera);

	// renderMoonCrashAtmosphere(rm, r, player, mainFramebuffer);

	renderPlanet(r);

	// render UI
	r.setWireframeMode(false);
	r.setDepthTest(false);
	r.screenSpaceFromBottomLeft();
	r.fontSize(16.0f);
	r.letterSpacing(0.6f);
	constexpr Float textSpacing = 20;
	r.textRenderSystem.alignText(Alignment::left, Alignment::bottom);
	r.text("Active chunks: " + toString(lifeTimerCounter), Vec2(textSpacing, 7 * textSpacing));
	r.text("Speed: " + toString(player.speedExponent), Vec2(textSpacing, 6 * textSpacing));
	r.text("FPS: " + toString(1.0f / r.time.getDelta()), Vec2(textSpacing, 5 * textSpacing));
	r.text("FrameTime: " + toString(r.time.getDelta()), Vec2(textSpacing, 4 * textSpacing));
	r.text("Camera height float: " + toString(player.camera.getPosition().z), Vec2(textSpacing, 3 * textSpacing));
	r.text("Camera height ULL: " + toString(player.chunkPosition.z), Vec2(textSpacing, 2 * textSpacing));
	r.text("SunDir: " + toString(r.uniforms().getSunDir()), Vec2(textSpacing, 1 * textSpacing));
}

void MoonCrashRenderer::renderMoonCrashAtmosphere(Renderer& r) const
{
	const Bool culling = r.getCulling();
	player.camera.renderOnlyRot(r);
	r.uniforms().setCameraPos(Vec4(Vec3(0, 0, (static_cast<Float>(player.chunkPosition.z) + player.camera.getPosition().z) / static_cast<Float>(ULLONG_MAX)), 1));
	r.setCulling(false);
	r.useShader("atmosphere");
	framebuffer.setAsTexture(r, 0); // [TODO]might not work if draw doesn't bind; check

	// renderer.setAlphaBlending(true);
	// glBlendFunc(GL_ONE, GL_ONE);
	r.setDepthTest(false);

	r.renderMesh("centeredCube");
	r.setCulling(culling);

	// renderer.setAlphaBlending(false);
	r.setDepthTest(true);
}

void MoonCrashRenderer::renderPlanet(Renderer& r)
{
	// set modes
	r.setAlphaBlending(false);
	r.setDepthTest(false);

	// set uniforms
	r.uniforms().setCustomInt1(planetRenderSystem.worldDistortion);
	r.uniforms().setMMatrix(Matrix(1));
	player.camera.renderOnlyRot(r);

	// render meshes
	for (UShort level = 0; level < MAX_CHUNK_LEVEL; level++) planetRenderSystem.renderLevel(r, player, level, framebuffer);
}
