#include "NeuralNetwork.hpp"
#include "Math.hpp"
#include "Renderer.hpp"
#include "Random.hpp"

Vector<Float> NeuralNet::output()
{
	return neuronActivation[layerCount() - 1];
}

UInt NeuralNet::layerCount() const
{
	return structure.size();
}

void NeuralNet::propagateForward()
{
	for(UInt layerId = 0; layerId < layerCount() - 1; layerId++)
	{
		for(UInt neuronAId = 0; neuronAId < neuronCountAtLayer(layerId); neuronAId++)
		{
			for(UInt neuronBId = 0; neuronBId < neuronCountAtLayer(layerId + 1); neuronBId++)
			{
				// reset the neuron
				if(neuronAId == 0) neuronActivation[layerId + 1][neuronBId] = 0;

				// calculate contribution
				contribution[layerId][neuronAId][neuronBId] = neuronActivation[layerId][neuronAId] * weights[layerId][neuronAId][neuronBId];

				// accumulate inputs
				neuronActivation[layerId + 1][neuronBId] += contribution[layerId][neuronAId][neuronBId];
			}
		}
	}
}

void NeuralNet::input(const Vector<Float>& input)
{
	for(UInt i = 0; i < structure[0]; i++) neuronActivation[0][i] = input[i];
}

NeuralNet::NeuralNet(const Vector<UInt>& structure)
{
	this->structure = structure;

	neuronActivation.clear();
	for(const auto neuronCount : structure)
	{
		Vector<Float> layer;
		layer.resize(neuronCount + 1);
		neuronActivation.push_back(layer);
	}

	weights.resize(layerCount());
	contribution.resize(layerCount());

	for(UInt layerId = 0; layerId < layerCount() - 1; layerId++)
	{
		weights[layerId].resize(neuronCountAtLayer(layerId));
		contribution[layerId].resize(neuronCountAtLayer(layerId));

		for(UInt neuronAId = 0; neuronAId < neuronCountAtLayer(layerId); neuronAId++)
		{
			weights[layerId][neuronAId].resize(neuronCountAtLayer(layerId + 1));
			contribution[layerId][neuronAId].resize(neuronCountAtLayer(layerId + 1));

			for(UInt neuronBId = 0; neuronBId < neuronCountAtLayer(layerId + 1); neuronBId++)
			{
				weights[layerId][neuronAId][neuronBId]      = randomFloatFast(-1.0f, 1.0f);
				contribution[layerId][neuronAId][neuronBId] = 0;
			}
		}
	}
}

void NeuralNet::propagateBackward(Vector<Float> target) const {}

UInt NeuralNet::neuronCountAtLayer(const UInt layerId) const
{
	return structure[layerId];
}

Vec2 NeuralNet::getNeuronPosition(const Vec2 area, const UInt layerId, const UInt neuronId) const
{
	return Vec2((layerId + 1) * area.x / static_cast<Float>(layerCount() + 1), (neuronId + 1) * area.y / static_cast<Float>(structure[layerId] + 1));
}

void NeuralNet::render(ResourceManager& resourceManager, Renderer& renderer, const Vec2 area) const
{
	renderer.fill(Color(1, 1, 1, 0.1));
	renderer.setDepthTest(false);
	// render connections
	for(UInt layerId = 0; layerId < layerCount() - 1; layerId++)
	{
		for(UInt neuronAId = 0; neuronAId < neuronCountAtLayer(layerId); neuronAId++)
		{
			for(UInt neuronBId = 0; neuronBId < neuronCountAtLayer(layerId + 1); neuronBId++)
			{
				const Color color = lerp(Color(1, 0, 0, 1), Color(0, 1, 0, 1), contribution[layerId][neuronAId][neuronBId]);
				renderer.fill(color);
				renderer.lineRenderer.renderLineAdvanced(renderer, getNeuronPosition(area, layerId, neuronAId), getNeuronPosition(area, layerId + 1, neuronBId), weights[layerId][neuronAId][neuronBId] * 3);
			}
		}
	}

	// render neurons
	for(Int layerId = 0; layerId < layerCount(); layerId++)
	{
		for(Int neuronId = 0; neuronId < structure[layerId]; neuronId++)
		{
			const Color color = lerp(Color(1, 0, 0, 1), Color(0, 1, 0, 1), neuronActivation[layerId][neuronId]);
			renderer.fill(color);
			renderer.circle(resourceManager, getNeuronPosition(area, layerId, neuronId), 10);
		}
	}
}
