#include "FlappyBird.hpp"

void FlappyBird::run()
{
	resourceManager.create();
	ui.create();
	ui.window("ORKA Sandbox", Area(settings.defaultWindowWidth, settings.defaultWindowHeight), true, true, WindowState::Windowed, flappyBirdRenderer, resourceManager);
	ui.run();
}

void FlappyBirdBird::jump()
{
	if(heldInPlace) heldInPlace = false;
	else if(alive && !isColliding) velocity.y = 2.0f;
}

void FlappyBirdBird::reset()
{
	alive       = true;
	heldInPlace = true;
	transform.setY(0.0f);
	velocity = Vec3(0);
}

void FlappyBirdPipes::randomize()
{
	gap  = randomFloat(minGap, maxGap);
	posY = randomFloat(-0.9f + gap / 2.0f, 0.9f - gap / 2.0f);
}

void FlappyBirdRenderer::destroy(Window& window) {}

FlappyBirdPipes::FlappyBirdPipes(const Int id)
{
	this->posX = static_cast<Float>(id) * spacing;
	randomize();
}

void FlappyBirdPipes::update(const Float delta)
{
	posX -= speed * delta;

	if(posX < -5.0f)
	{
		while(posX < 5.0f) posX += spacing;

		randomize();
	}

	transform1.setRotation(Vec3(-90, 90, 0));
	transform2.setRotation(Vec3(-90, 90, 180));
	transform1.setLocation(Vec3(posX, posY - gap / 2.0f, 0.0f));
	transform2.setLocation(Vec3(posX, posY + gap / 2.0f, 0.0f));
}

void FlappyBirdRenderer::update(Window& window)
{
	// check if any bird is dead
	Bool anyBirdDead = false;
	for(const auto& bird : birds) if(!bird.alive) anyBirdDead = true;

	// if it is slow the game
	Float delta = window.renderer.deltaTime() * gameSpeed * slowMotionSpeed;
	if(!anyBirdDead) delta = window.renderer.deltaTime() * gameSpeed;

	// update the pipes
	for(auto& pipes : pipeColumns) pipes.update(delta);

	// if ai is enabled process that
	if(enableAI) for(auto& bird : birds) bird.updateAI(pipeColumns, delta);

	// update the birds
	for(auto& bird : birds) bird.update(pipeColumns, delta);
}

void FlappyBirdBird::inputEvent(const InputEvent input)
{
	if(input == jumpButton) jump();
}

void FlappyBirdBird::render(ResourceManager& rm, Renderer& r) const
{
	if(alive) r.fill(Color(1));
	else r.fill(Color(1, 0, 0, 1));

	transform.render(r);
	r.renderMesh(rm, "flappyBirdBird");
}

void FlappyBirdPipes::render(ResourceManager& rm, Renderer& r) const
{
	transform1.render(r);
	r.renderMesh(rm, "flappyBirdPipe");

	transform2.render(r);
	r.renderMesh(rm, "flappyBirdPipe");
}

void FlappyBirdRenderer::inputEvent(Window& window, const InputEvent input)
{
	if(input == reloadShaders) window.renderer.shaderSystem.rebuild();
	if(input == toggleWireFrame) window.renderer.wireframeMode = !window.renderer.wireframeMode;

	for(auto& bird : birds) bird.inputEvent(input);
}

void FlappyBirdRenderer::create(ResourceManager& resourceManager, Window& window)
{
	camera.setLocation(Vec3(0, 0, 5));
	camera.setRotation(DVec3(PI, 0, 0));
	camera.setFieldOfView(30);

	randomizeSeed();

	for(Int i = 0; i < flappyBirdCount; i++)
	{
		birds.emplace_back();
		birds.back().transform.setY(-1.0f + static_cast<Float>(i) / static_cast<Float>(flappyBirdCount) * 2.0f);
		birds.back().transform.setX(-1.0f + static_cast<Float>(i) / static_cast<Float>(flappyBirdCount) * 2.0f);
	}

	pipeColumns.clear();

	const FlappyBirdPipes p(0);

	for(Int i = 0; static_cast<Float>(i) < 10.0f / p.spacing; i++) pipeColumns.push_back(FlappyBirdPipes(3 + i));

}

void FlappyBirdRenderer::render(ResourceManager& rm, Window& window, TiledRectangle area)
{
	Renderer& r = window.renderer;

	camera.render(r);
	// r.aspectCorrectNormalizedSpace();

	r.clearBackground(Color(0.1, 0.75, 1.0, 1));

	r.setDepthTest(true);
	r.setCulling(true);
	r.setWireframeMode();

	r.useShader(rm, "flappyBird");

	for(auto& bird : birds) bird.render(rm, r);
	r.fill(Color(1));

	for(auto& pipe : pipeColumns) pipe.render(rm, r);

	Transform t;
	t.setSize(Vec3(0.1f, 1, 0.1f));
	// t.setSize(Vec3(0.1f));
	t.setRotation(-90, 90, 0);
	t.setLocation(Vec3(0, -1, 0));
	t.render(r);
	r.renderMesh(rm, "flappyBirdGround");
}

void FlappyBirdBird::update(const Vector<FlappyBirdPipes>& pipeColumns, const Float delta)
{
	// rotation based on up/down speed
	transform.setSize(size);
	transform.setRotation(Vec3(-90, 90, 0));
	transform.rotate(Vec3(0, 0, 1), tan(clamp(velocity.y * 0.7f, -PI / 4.0f,PI / 4.0f)) * 75.0f);

	if(!heldInPlace)
	{
		// check collisions
		isColliding = false;
		// check pipe collisions
		for(const FlappyBirdPipes pipes : pipeColumns)
		{
			const Vec3 location = transform.getLocation();

			if(location.x < pipes.posX + pipes.width / 2.0f)
				if(location.x > pipes.posX - pipes.width / 2.0f)
				{
					if(location.y > pipes.posY + pipes.gap / 2.0f) isColliding = true;
					if(location.y < pipes.posY - pipes.gap / 2.0f) isColliding = true;
				}
		}
		// check ground collisions
		if(transform.getLocation().y < -1.0f) reset();

		if(isColliding) alive = false;

		// move the bird
		velocity += delta * Vec3(0, -5, 0);
		transform.move(velocity * delta);
	}
}

void FlappyBirdBird::updateAI(const Vector<FlappyBirdPipes>& pipeColumns, const Float delta)
{
	if(heldInPlace) jump();

	const FlappyBirdPipes closest = getClosestPipeColumn(pipeColumns);

	const Float lowestJumpPoint  = closest.posY - closest.gap / 2.0f + 0.05f;
	const Float highestJumpPoint = closest.posY + closest.gap / 2.0f - 0.44f;

	if(lowestJumpPoint > highestJumpPoint) logError("Cant calculate jump point!");

	if(transform.getLocation().y + velocity.y * delta <= randomFloat(lowestJumpPoint, highestJumpPoint)) jump();
}

FlappyBirdPipes FlappyBirdBird::getClosestPipeColumn(const Vector<FlappyBirdPipes>& pipeColumns) const
{
	FlappyBirdPipes closest(1000.0f);

	for(auto& pipes : pipeColumns)
	{
		if(pipes.posX >= transform.getLocation().x - pipes.width / 2.0f)
		{
			closest = pipes;

			for(auto& pipes2 : pipeColumns)
			{
				Float posX = transform.getLocation().x;
				if(pipes2.posX >= posX - pipes.width / 2.0f && distance(closest.posX, posX) > distance(pipes2.posX, posX)) closest = pipes2;
			}

			break;
		}
	}
	return closest;
}

void FlappyBirdRenderer::renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) {}
