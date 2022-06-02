
#include "NeuralNetwork.hpp"
#include "Math.hpp"
#include "Renderer.hpp"
#include "Random.hpp"

Vec2 NeuralNet::getNeuronPosition(Vec2 area, UInt layerID, UInt neuronID) {
	return Vec2((layerID + 1) * area.x / Float(layerCount() + 1), (neuronID + 1) * area.y / Float(structure[layerID] + 1));
}

void NeuralNet::input(Vector<Float> input) {
	for (UInt i = 0; i < structure[0]; i++) {
		neuronActivation[0][i] = input[i];
	}
}

UInt NeuralNet::layerCount() {
	return structure.size();
}

UInt NeuralNet::neuronCountAtLayer(UInt layerID) {
	return structure[layerID];
}

Vector<Float> NeuralNet::output() {
	return neuronActivation[layerCount() - 1];
}

void NeuralNet::propagateForward() {
	for (UInt layerID = 0; layerID < layerCount() - 1; layerID++) {
		for (UInt neuronAID = 0; neuronAID < neuronCountAtLayer(layerID); neuronAID++) {
			for (UInt neuronBID = 0; neuronBID < neuronCountAtLayer(layerID + 1); neuronBID++) {
				//reset the neuron
				if (neuronAID == 0) neuronActivation[layerID + 1][neuronBID] = 0;
				
				//calculate contribution
				contribution[layerID][neuronAID][neuronBID] = neuronActivation[layerID][neuronAID] * weights[layerID][neuronAID][neuronBID];

				//accumulate inputs
				neuronActivation[layerID + 1][neuronBID] += contribution[layerID][neuronAID][neuronBID];
			}
		}
	}
}

void NeuralNet::propagateBackward(Vector<Float> target)
{
}

void NeuralNet::render(Vec2 area, Renderer& renderer) {
	renderer.fill(Color(1, 1, 1, 0.1));
	//render connections
	for (UInt layerID = 0; layerID < layerCount()-1; layerID++) {
		for (UInt neuronAID = 0; neuronAID < neuronCountAtLayer(layerID); neuronAID++) {
			for (UInt neuronBID = 0; neuronBID < neuronCountAtLayer(layerID+1); neuronBID++) {
				renderer.fill(Color(1, 1, 1, contribution[layerID][neuronAID][neuronBID]));
				renderer.lineRenderer.renderLine(renderer, getNeuronPosition(area, layerID, neuronAID), getNeuronPosition(area, layerID+1, neuronBID), weights[layerID][neuronAID][neuronBID]*3);
			}
		}
	}

	//render neurons
	for (Int layerID = 0; layerID < layerCount(); layerID++) {
		for (Int neuronID = 0; neuronID < structure[layerID]; neuronID++) {
			renderer.uniforms().mMatrix() = matrixFromLocationAndSize(getNeuronPosition(area, layerID, neuronID), 10);
			renderer.fill(Color(1,1,1,neuronActivation[layerID][neuronID]));
			renderer.renderMesh("sphere");
		}
	}
}

NeuralNet::NeuralNet(Vector<UInt> structure) {

	this->structure = structure;

	neuronActivation.clear();
	for (auto neuronCount : structure) {
		Vector<Float> layer;
		layer.resize(neuronCount + 1);
		neuronActivation.push_back(layer);
	}

	weights.resize(layerCount());
	contribution.resize(layerCount());

	for (UInt layerID = 0; layerID < layerCount()-1; layerID++) {
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