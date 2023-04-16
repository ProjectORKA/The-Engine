#include "Snake.hpp"

void Snake::addSegment() {
	bodySegments.pushBack(Vec2(0, 0));
}
void Snake::dumbAI(SnakeFood& f) {
	direction = speed * normalize(f.foodPosition - headPosition);
}
void Snake::render(ResourceManager& resourceManager, Window& window) {
	window.renderer.fill(Color(snakeColor));
	window.renderer.useShader(resourceManager, "color");
	window.renderer.uniforms().mMatrix(matrixFromLocationAndSize(headPosition, segmentRadius));
	window.renderer.renderMesh(resourceManager, "circle");
	for (Int i = 0; i < bodySegments.size(); i++) {
		window.renderer.uniforms().mMatrix(matrixFromLocationAndSize(bodySegments[i], segmentRadius));
		window.renderer.renderMesh(resourceManager, "circle");
	}
}
void Snake::inputEvent(Window& window, InputEvent input) {
	if (input == forward) {
		direction = Vec2(0, speed);
	}
	if (input == left) {
		direction = Vec2(-speed, 0);
	}
	if (input == back) {
		direction = Vec2(0, -speed);
	}
	if (input == right) {
		direction = Vec2(speed, 0);
	}
}
void Snake::update(SnakeFood& snakefood, Float deltaTime) {

	headPosition += direction * deltaTime;
	if (length(snakefood.foodPosition - headPosition) < 0.06) {
		snakefood.foodPosition = randomVec2(-1, 1);
		addSegment();
	}

	for (Int i = 0; i < bodySegments.size(); i++) {
		if (i == 0) {
			bodySegments[i] = normalize(bodySegments[i] - headPosition) * Vec2(2*segmentRadius) + headPosition;
		}
		else{
			bodySegments[i] = normalize(bodySegments[i] - bodySegments[i-1]) * Vec2(2* segmentRadius) + bodySegments[i-1];
		}
	}
}

void SnakeFood::update() {
	foodPosition += randomVec2(-0.02, 0.02);
	foodColor = randomVec4(0, 1);
}
void SnakeFood::render(ResourceManager& resourceManager, Window& window) {
	window.renderer.fill(Color(foodColor));
	window.renderer.useShader(resourceManager, "color");
	window.renderer.uniforms().mMatrix(matrixFromLocationAndSize(foodPosition, 0.03));
	window.renderer.renderMesh(resourceManager, "circle");

}
void SnakeGame::update(Window& window) {
	Renderer& renderer = window.renderer;

	totalTime += renderer.deltaTime();
	if (totalTime > 0.0016666) {
		snake.dumbAI(snakefood);
		snake.update(snakefood, renderer.deltaTime());
		snakefood.update();
		totalTime = 0;
	}
}

void SnakeGame::render(ResourceManager& resourceManager, Window& window, TiledRectangle area) {
	Renderer& renderer = window.renderer;

	renderer.clearColor(Color(0, 0.2, 0.2, 0.7));
	renderer.normalizedSpaceWithAspectRatio(16.0 / 9.0);

	snake.render(resourceManager, window);
	snakefood.render(resourceManager, window);
}
void SnakeGame::inputEvent(Window& window, InputEvent input) {
	snake.inputEvent(window, input);
}