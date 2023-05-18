#include "NeuralNetwork.hpp"
#include "Math.hpp"
#include "Renderer.hpp"
#include "Random.hpp"

UInt NeuralNet::layerCount() const { return structure.size(); }
Vector<Float> NeuralNet::output() { return neuronActivation[layerCount() - 1]; }

void NeuralNet::propagateForward() {
	for (UInt layerID = 0; layerID < layerCount() - 1; layerID++) {
		for (UInt neuronAID = 0; neuronAID < neuronCountAtLayer(layerID); neuronAID++) {
			for (UInt neuronBID = 0; neuronBID < neuronCountAtLayer(layerID + 1); neuronBID++) {
				//reset the neuron
				if (neuronAID == 0) neuronActivation[layerID + 1][neuronBID] = 0;

				//calculate contribution
				contribution[layerID][neuronAID][neuronBID] = neuronActivation[layerID][neuronAID] * weights[layerID][
					neuronAID][neuronBID];

				//accumulate inputs
				neuronActivation[layerID + 1][neuronBID] += contribution[layerID][neuronAID][neuronBID];
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

	for (UInt layerID = 0; layerID < layerCount() - 1; layerID++) {
		weights[layerID].resize(neuronCountAtLayer(layerID));
		contribution[layerID].resize(neuronCountAtLayer(layerID));

		for (UInt neuronAID = 0; neuronAID < neuronCountAtLayer(layerID); neuronAID++) {
			weights[layerID][neuronAID].resize(neuronCountAtLayer(layerID + 1));
			contribution[layerID][neuronAID].resize(neuronCountAtLayer(layerID + 1));

			for (UInt neuronBID = 0; neuronBID < neuronCountAtLayer(layerID + 1); neuronBID++) {
				weights[layerID][neuronAID][neuronBID] = randomFloat();
				contribution[layerID][neuronAID][neuronBID] = 0;
			}
		}
	}
}

UInt NeuralNet::neuronCountAtLayer(const UInt layerID) const { return structure[layerID]; }
void NeuralNet::propagateBackward(Vector<Float> target) {}

void NeuralNet::render(ResourceManager& resourceManager, Renderer& renderer, const Vec2 area) {
	renderer.fill(Color(1, 1, 1, 0.1));
	//render connections
	for (UInt layerID = 0; layerID < layerCount() - 1; layerID++) {
		for (UInt neuronAID = 0; neuronAID < neuronCountAtLayer(layerID); neuronAID++) {
			for (UInt neuronBID = 0; neuronBID < neuronCountAtLayer(layerID + 1); neuronBID++) {
				renderer.fill(Color(1, 1, 1, contribution[layerID][neuronAID][neuronBID]));
				renderer.lineRenderer.renderLine(renderer, getNeuronPosition(area, layerID, neuronAID),
				                                 getNeuronPosition(area, layerID + 1, neuronBID),
				                                 weights[layerID][neuronAID][neuronBID] * 3);
			}
		}
	}

	//render neurons
	for (Int layerID = 0; layerID < layerCount(); layerID++) {
		for (Int neuronID = 0; neuronID < structure[layerID]; neuronID++) {
			renderer.uniforms().mMatrix() = matrixFromLocationAndSize(getNeuronPosition(area, layerID, neuronID), 10);
			renderer.fill(Color(1, 1, 1, neuronActivation[layerID][neuronID]));
			renderer.renderMesh(resourceManager, "sphere");
		}
	}
}

Vec2 NeuralNet::getNeuronPosition(const Vec2 area, const UInt layerID, const UInt neuronID) const {
	return Vec2((layerID + 1) * area.x / static_cast<Float>(layerCount() + 1),
	            (neuronID + 1) * area.y / static_cast<Float>(structure[layerID] + 1));
}
