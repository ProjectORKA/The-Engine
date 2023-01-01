
#pragma once

#include "Basics.hpp"
#include "Engine.hpp"

struct Renderer;

struct NeuralNet {
	Vector<UInt> structure;
	Vector<Vector<Vector<Float>>> weights;
	Vector<Vector<Float>> neuronActivation;
	Vector<Vector<Vector<Float>>> contribution;
	
	UInt layerCount();
	Vector<Float> output();
	void propagateForward();
	void input(Vector<Float> input);
	UInt neuronCountAtLayer(UInt layerID);
	void propagateBackward(Vector<Float> target);
	void render(Engine& engine, Vec2 area, Renderer& renderer);
	Vec2 getNeuronPosition(Vec2 area, UInt layerID, UInt neuronID);

	NeuralNet() {};
	NeuralNet(Vector<UInt> structure);
};