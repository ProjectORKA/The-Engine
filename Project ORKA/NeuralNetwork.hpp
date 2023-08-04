#pragma once

#include "Basics.hpp"
#include "ResourceManager.hpp"

struct Renderer;

struct NeuralNet
{
	Vector<Vector<Vector<Float>>> weights;
	Vector<UInt>                  structure;
	Vector<Vector<Vector<Float>>> contribution;
	Vector<Vector<Float>>         neuronActivation;

	NeuralNet() = default;
	NeuralNet(const Vector<UInt>& structure);

	void propagateForward();
	void input(const Vector<Float>& input);
	void propagateBackward(Vector<Float> target) const;
	void render(ResourceManager& resourceManager, Renderer& renderer, Vec2 area) const;

	[[nodiscard]] Vector<Float> output();
	[[nodiscard]] UInt          layerCount() const;
	[[nodiscard]] UInt          neuronCountAtLayer(UInt layerId) const;
	[[nodiscard]] Vec2          getNeuronPosition(Vec2 area, UInt layerId, UInt neuronId) const;
};
