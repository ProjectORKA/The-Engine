#include "AIPlayground.hpp"
#include "Window.hpp"

AIPlayground::AIPlayground()
{
	const Vector<UInt> structure = { 4, 5, 5, 2, 1 };
	network = NeuralNet(structure);
}

void AIPlayground::render(ResourceManager& resourceManager, Window& window, const TiledRectangle area)
{
	Renderer& r = window.renderer;

	r.screenSpace();
	r.useShader(resourceManager, "color");
	r.setAlphaBlending(true);

	Vector<Float> inputVector;
	const Vector<Float> targetVector;

	for(UInt i = 0; i < network.structure[0]; i++)
	{
		inputVector.push_back(randomFloat());
	}

	network.input(inputVector);
	network.propagateForward();
	network.render(resourceManager, r, area.size);
	network.propagateBackward(targetVector);

	r.textRenderSystem.render(resourceManager, r, String(toString(1.0f / r.time.delta)), Vec2(50));
}