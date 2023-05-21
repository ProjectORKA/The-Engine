#include "NeuralNetwork.hpp"
#include "Math.hpp"
#include "Renderer.hpp"
#include "Random.hpp"

UInt NeuralNet::layerCount() const { return structure.size(); }
Vector<Float> NeuralNet::output() { return neuronActivation[layerCount() - 1]; }

void NeuralNet::propagateForward() {
	for (UInt layerId = 0; layerId < layerCount() - 1; layerId++) {
		for (UInt neuronAId = 0; neuronAId < neuronCountAtLayer(layerId); neuronAId++) {
			for (UInt neuronBId = 0; neuronBId < neuronCountAtLayer(layerId + 1); neuronBId++) {
				//reset the neuron
				if (neuronAId == 0) neuronActivation[layerId + 1][neuronBId] = 0;

				//calculate contribution
				contribution[layerId][neuronAId][neuronBId] = neuronActivation[layerId][neuronAId] * weights[layerId][
					neuronAId][neuronBId];

				//accumulate inputs
				neuronActivation[layerId + 1][neuronBId] += contribution[layerId][neuronAId][neuronBId];
			}
		}
	}
}

void NeuralNet::input(const Vector<Float>& input) {
	for (UInt i = 0; i < structure[0]; i++) { neuronActivation[0][i] = input[i]; }
}

NeuralNet::NeuralNet(const Vector<UInt>& structure) {
	this->structure = structure;

	neuronActivation.clear();
	for (const auto neuronCount : structure) {
		Vector<Float> layer;
		layer.resize(neuronCount + 1);
		neuronActivation.push_back(layer);
	}

	weights.resize(layerCount());
	contribution.resize(layerCount());

	for (UInt layerId = 0; layerId < layerCount() - 1; layerId++) {
		weights[layerId].resize(neuronCountAtLayer(layerId));
		contribution[layerId].resize(neuronCountAtLayer(layerId));

		for (UInt neuronAId = 0; neuronAId < neuronCountAtLayer(layerId); neuronAId++) {
			weights[layerId][neuronAId].resize(neuronCountAtLayer(layerId + 1));
			contribution[layerId][neuronAId].resize(neuronCountAtLayer(layerId + 1));

			for (UInt neuronBId = 0; neuronBId < neuronCountAtLayer(layerId + 1); neuronBId++) {
				weights[layerId][neuronAId][neuronBId] = randomFloat();
				contribution[layerId][neuronAId][neuronBId] = 0;
			}
		}
	}
}

UInt NeuralNet::neuronCountAtLayer(const UInt layerId) const { return structure[layerId]; }
void NeuralNet::propagateBackward(Vector<Float> target) {}

void NeuralNet::render(ResourceManager& resourceManager, Renderer& renderer, const Vec2 area) {
	renderer.fill(Color(1, 1, 1, 0.1));
	//render connections
	for (UInt layerId = 0; layerId < layerCount() - 1; layerId++) {
		for (UInt neuronAId = 0; neuronAId < neuronCountAtLayer(layerId); neuronAId++) {
			for (UInt neuronBId = 0; neuronBId < neuronCountAtLayer(layerId + 1); neuronBId++) {
				renderer.fill(Color(1, 1, 1, contribution[layerId][neuronAId][neuronBId]));
				renderer.lineRenderer.renderLine(renderer, getNeuronPosition(area, layerId, neuronAId),
				                                 getNeuronPosition(area, layerId + 1, neuronBId),
				                                 weights[layerId][neuronAId][neuronBId] * 3);
			}
		}
	}

	//render neurons
	for (Int layerId = 0; layerId < layerCount(); layerId++) {
		for (Int neuronId = 0; neuronId < structure[layerId]; neuronId++) {
			renderer.uniforms().mMatrix() = matrixFromLocationAndSize(getNeuronPosition(area, layerId, neuronId), 10);
			renderer.fill(Color(1, 1, 1, neuronActivation[layerId][neuronId]));
			renderer.renderMesh(resourceManager, "sphere");
		}
	}
}

Vec2 NeuralNet::getNeuronPosition(const Vec2 area, const UInt layerId, const UInt neuronId) const {
	return Vec2((layerId + 1) * area.x / static_cast<Float>(layerCount() + 1),
	            (neuronId + 1) * area.y / static_cast<Float>(structure[layerId] + 1));
}
