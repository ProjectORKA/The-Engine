
#include "AIPlayground.hpp"
#include "Window.hpp"

AIPlayground::AIPlayground() {
	Vector<UInt> structure = { 4,5,5,2,1 };
	network = NeuralNet(structure);
}

void AIPlayground::render(ResourceManager& resourceManager, Window& window, TiledRectangle area)
{
	Renderer& r = window.renderer;

	r.screenSpace();
	r.useShader(resourceManager, "color");
	r.setAlphaBlending(true);

	Vector<Float> inputVector;
	Vector<Float> targetVector;

	for (UInt i = 0; i < network.structure[0]; i++) {
		inputVector.pushBack(randomFloat());
	}

	network.input(inputVector);
	network.propagateForward();
	network.render(resourceManager, r, area.size);
	network.propagateBackward(targetVector);

	r.textRenderSystem.render(resourceManager, r, String(toString(1.0f / r.time.delta)), Vec2(50));
}
