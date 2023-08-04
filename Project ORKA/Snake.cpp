#include "Snake.hpp"
#include "Random.hpp"

void SnakeGame::run()
{
	resourceManager.create();
	ui.create();
	ui.window("ORKA Sandbox", Area(settings.defaultWindowWidth, settings.defaultWindowHeight), true, true, WindowState::Windowed, renderer, resourceManager);
	ui.run();
}

void Snake::addSegment()
{
	bodySegments.push_back(Vec2(0, 0));
}

void SnakeFood::update()
{
	foodPosition += randomVec2(-0.02f, 0.02f);
	foodColor = randomVec4(0.0f, 1.0f);
}

void SnakeRenderer::destroy(Window& window) {}

void Snake::dumbAI(const SnakeFood& f)
{
	direction = speed * normalize(f.foodPosition - headPosition);
}

void SnakeRenderer::update(Window& window)
{
	const Renderer& renderer = window.renderer;

	totalTime += renderer.deltaTime();
	if(totalTime > 0.0016666f)
	{
		snake.dumbAI(snakefood);
		snake.update(snakefood, renderer.deltaTime());
		snakefood.update();
		totalTime = 0;
	}
}

void Snake::inputEvent(Window& window, const InputEvent input)
{
	if(input == forward) direction = Vec2(0, speed);
	if(input == left) direction = Vec2(-speed, 0);
	if(input == back) direction = Vec2(0, -speed);
	if(input == right) direction = Vec2(speed, 0);
}

void Snake::update(SnakeFood& snakeFood, const Float deltaTime)
{
	headPosition += direction * deltaTime;
	if(length(snakeFood.foodPosition - headPosition) < 0.06f)
	{
		snakeFood.foodPosition = randomVec2(-1, 1);
		addSegment();
	}

	for(Int i = 0; i < bodySegments.size(); i++)
	{
		if(i == 0) bodySegments[i] = normalize(bodySegments[i] - headPosition) * Vec2(2 * segmentRadius) + headPosition;
		else bodySegments[i]       = normalize(bodySegments[i] - bodySegments[i - 1]) * Vec2(2 * segmentRadius) + bodySegments[i - 1];
	}
}

void Snake::render(ResourceManager& resourceManager, Window& window)
{
	window.renderer.fill(Color(snakeColor));
	window.renderer.useShader(resourceManager, "color");
	window.renderer.uniforms().setMMatrix(matrixFromLocationAndSize(headPosition, segmentRadius));
	window.renderer.renderMesh(resourceManager, "circle");
	for(const auto& bodySegment : bodySegments)
	{
		window.renderer.uniforms().setMMatrix(matrixFromLocationAndSize(bodySegment, segmentRadius));
		window.renderer.renderMesh(resourceManager, "circle");
	}
}

void SnakeRenderer::create(ResourceManager& resourceManager, Window& window) {}

void SnakeRenderer::inputEvent(Window& window, const InputEvent input)
{
	snake.inputEvent(window, input);
}

void SnakeFood::render(ResourceManager& resourceManager, Window& window)
{
	window.renderer.fill(Color(foodColor));
	window.renderer.useShader(resourceManager, "color");
	window.renderer.uniforms().setMMatrix(matrixFromLocationAndSize(foodPosition, 0.03f));
	window.renderer.renderMesh(resourceManager, "circle");
}

void SnakeRenderer::render(ResourceManager& resourceManager, Window& window, TiledRectangle area)
{
	Renderer& renderer = window.renderer;

	renderer.clearBackground(Color(0.0f, 0.2f, 0.2f, 1.0f));
	renderer.normalizedSpaceWithAspectRatio(16.0f / 9.0f);

	snake.render(resourceManager, window);
	snakefood.render(resourceManager, window);
}

void SnakeRenderer::renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) {}
