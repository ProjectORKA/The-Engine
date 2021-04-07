
#pragma once

#include "Basics.hpp"

struct NeuronBridge {
	Neuron* inputNeuron = nullptr;
	Float weight = 0.0f;
};

struct Neuron {
	Vector<NeuronBridge> inputs;
	Float value = 0.0f;
	Float confidence = 0.0f;
};