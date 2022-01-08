
#pragma once

#include"Basics.hpp"

struct Renderer;

#define NEURON_COUNT 16

struct Neuron {
	Float value = 0;
	Float error = 0;
};

struct Connection {
	Index inputNeuron = 0;
	Index outputNeuron = 0;
	Float weight = 0.0f;

	void calculate(Vector<Neuron>& neurons);
};

struct NeuralNetwork {
	Vector<Neuron> neurons;
	Vector<Connection> connections;

	void calculate();
	void initialize();
	Vec3 IndexToPosition(Index id);
	void render(Renderer& renderer);
	void input(Vector<Float> input);
};

Float activationFunction(Float a);