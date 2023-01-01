
#include "AIPlayground.hpp"
#include "Window.hpp"

AIPlayground::AIPlayground() {
	Vector<UInt> structure = { 4,5,5,2,1 };
	network = NeuralNet(structure);
}

void AIPlayground::render(Engine& engine, Window& window, TiledRectangle area)
{
	Renderer& renderer = window.renderer;

	renderer.screenSpace();
	renderer.useShader(engine, "color");
	renderer.setAlphaBlending(true);

	Vector<Float> inputVector;
	Vector<Float> targetVector;

	for (UInt i = 0; i < network.structure[0]; i++) {
		inputVector.push_back(randomFloat());
	}

	network.input(inputVector);
	network.propagateForward();
	network.render(engine, area.size, renderer);
	network.propagateBackward(targetVector);

	renderer.renderText(engine, String(toString(1.0f / renderer.time.delta)), Vec2(50), fonts.heading);
}
