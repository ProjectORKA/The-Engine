
#pragma once

#include "Basics.hpp"

struct Renderer;

struct NeuralNet {
	Vector<UInt> structure;
	Vector<Vector<Vector<Float>>> weights;
	Vector<Vector<Vector<Float>>> contribution;

	Vector<Vector<Float>> neuronActivation;
	
	UInt layerCount();
	Vector<Float> output();
	void propagateForward();
	void input(Vector<Float> input);
	UInt neuronCountAtLayer(UInt layerID);
	void render(Vec2 area, Renderer& renderer);
	void propagateBackward(Vector<Float> target);
	Vec2 getNeuronPosition(Vec2 area, UInt layerID, UInt neuronID);

	NeuralNet() {};
	NeuralNet(Vector<UInt> structure);
};