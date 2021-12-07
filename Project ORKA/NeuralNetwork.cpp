
#include "NeuralNetwork.hpp"

Float activationFunction(Float a) {
	//return 1.0f / (1.0f + pow(100.0f, (-a)));
	//return max(a, 0.0f);
	return clamp(a, 0.0f, 10.0f);
	//return -2.0f / (pow(10.0f, a) + 1.0f) + 1.0f;
}

void NeuralNetwork::initialize() {
	neurons.clear();
	connections.clear();
	for (Int i = 0; i < NEURON_COUNT; i++) {
		neurons.emplace_back();
		neurons.back().value = 0;

		for (Int c = i; c < NEURON_COUNT; c++) {
			connections.emplace_back();
			connections.back().inputNeuron = i;
			connections.back().outputNeuron = c;
			connections.back().weight = randomFloat(0.01, 0.1);
		}
	}
}

void NeuralNetwork::input(Vector<Float> input) {
	for (Int i = 0; i < input.size(); i++) {
		neurons[i].value = input[i];
	}

	for (Int i = input.size(); i < NEURON_COUNT; i++) {
		neurons[i].value = 0;
	}
}

void NeuralNetwork::calculate() {
	Index currentNeuron = 0;
	
	for (Connection & c : connections) {
		if (c.inputNeuron != currentNeuron) {
			currentNeuron = c.inputNeuron;
			neurons[currentNeuron].value = activationFunction(neurons[currentNeuron].value);
		}

		c.calculate(neurons);
	}
}

Vec2 NeuralNetwork::IndexToPosition(Index id) {
	Float posOnCircle = 2 * PI * Float(id) / Float(NEURON_COUNT);
	return Vec2(0.9) * Vec2(-sin(posOnCircle), -cos(posOnCircle));
}

void NeuralNetwork::render(Renderer& renderer) {

	Float overallSize = 1.0f / NEURON_COUNT;

	for (Int i = 0; i < NEURON_COUNT; i++) {

		//render circle
		renderer.shaderSystem.use("color");

		renderer.uniforms().data.customColor = Color(Vec3(neurons[i].value), 0.75);

		renderer.uniforms().data.mMatrix = scale(translate(Matrix(1), Vec3(IndexToPosition(i), 0)), Vec3(overallSize));
		renderer.uniforms().update();
		renderer.renderMesh("circle");

		//render connections
		renderer.uniforms().data.mMatrix = Matrix(1);
		renderer.uniforms().update();
		for (Connection & c : connections) {

			if (c.weight >= 0) {
				renderer.uniforms().data.customColor = Color(Vec3(0, neurons[i].value, 0), 0.75);
			}
			else {
				renderer.uniforms().data.customColor = Color(Vec3(neurons[i].value, 0, 0), 0.75);
			}
			renderer.uniforms().update();

			renderer.arrow2D(IndexToPosition(c.inputNeuron), IndexToPosition(c.outputNeuron), abs(c.weight) * overallSize * 0.5);
		}
	}
}

void Connection::calculate(Vector<Neuron>& neurons) {
	if (inputNeuron == outputNeuron) neurons[outputNeuron].value += 1 * weight;
	else neurons[outputNeuron].value += neurons[inputNeuron].value * weight;
}
