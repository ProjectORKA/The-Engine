
#pragma once

#include "Renderer.hpp"


#define NEURON_COUNT 6

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


Float activationFunction(Float a);

struct NeuralNetwork {
	
	Vector<Neuron> neurons;
	Vector<Connection> connections;

	void calculate();
	void initialize();
	Vec2 IndexToPosition(Index id);
	void render(Renderer& renderer);
	void input(Vector<Float> input);
};