#include "Snake.hpp"
#include "Random.hpp"

void SnakeSnake::addSegment()
{
	bodySegments.emplace_back(0, 0);
}

void SnakeFood::update()
{
	foodPosition += randomVec2Fast(-0.02f, 0.02f);
	foodColor = randomVec4Fast(0.0f, 1.0f);
}

void SnakeRenderer::destroy(Window& window) {}

void SnakeRenderer::connect(GameSimulation& simulation) {}

void SnakeSnake::dumbAI(const SnakeFood& f)
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

void SnakeSnake::inputEvent(Window& window, const InputEvent input)
{
	if(input == forward) direction = Vec2(0, speed);
	if(input == left) direction = Vec2(-speed, 0);
	if(input == back) direction = Vec2(0, -speed);
	if(input == right) direction = Vec2(speed, 0);
}

void SnakeSnake::update(SnakeFood& snakeFood, const Float deltaTime)
{
	headPosition += direction * deltaTime;
	if(length(snakeFood.foodPosition - headPosition) < 0.06f)
	{
		snakeFood.foodPosition = randomVec2Fast(-1, 1);
		addSegment();
	}

	for(Int i = 0; i < bodySegments.size(); i++)
	{
		if(i == 0) bodySegments[i] = normalize(bodySegments[i] - headPosition) * Vec2(2 * segmentRadius) + headPosition;
		else bodySegments[i]       = normalize(bodySegments[i] - bodySegments[i - 1]) * Vec2(2 * segmentRadius) + bodySegments[i - 1];
	}
}

void SnakeSnake::render(Window& window)
{
	window.renderer.fill(Color(snakeColor));
	window.renderer.useShader("color");
	window.renderer.uniforms().setMMatrix(matrixFromPositionAndSize(headPosition, segmentRadius));
	window.renderer.renderMesh("circle");
	for(const auto& bodySegment : bodySegments)
	{
		window.renderer.uniforms().setMMatrix(matrixFromPositionAndSize(bodySegment, segmentRadius));
		window.renderer.renderMesh("circle");
	}
}

void SnakeRenderer::create(Window& window) {}

void SnakeRenderer::inputEvent(Window& window, const InputEvent input)
{
	snake.inputEvent(window, input);
}

void SnakeFood::render(Window& window)
{
	window.renderer.fill(Color(foodColor));
	window.renderer.useShader("color");
	window.renderer.uniforms().setMMatrix(matrixFromPositionAndSize(foodPosition, 0.03f));
	window.renderer.renderMesh("circle");
}

void SnakeRenderer::render(Window& window, TiledRectangle area)
{
	Renderer& renderer = window.renderer;

	renderer.clearBackground(Color(0.0f, 0.2f, 0.2f, 1.0f));
	renderer.normalizedSpaceWithAspectRatio(16.0f / 9.0f);

	snake.render(window);
	snakefood.render(window);
}

void SnakeRenderer::renderInteractive(Window& window, TiledRectangle area) {}
