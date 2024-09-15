#pragma once

#include "Basics.hpp"
#include "Random.hpp"
#include "Renderer.hpp"


struct Renderer;

#define INT_NEURAL_ONE 100000

namespace ML{
inline Double leakyRELU(const Double value, const Double reduction = 0.01)
{
	if(value >= 0.0) return value;
	return value * reduction;
}
}

struct Neuron
{
	Vector<ULL> input = {};
	Vector<ULL> output = {};
	Vector<Double> weights = {};
	Vec3        position = Vec3(0);
	Double      error = 0;
	Double      activation = 0.0;
	ULL         errorSources = 0;

	void calculate(const Vector<Neuron>& brain)
	{
		//calculate activation
		for(ULL i = 0; i < input.size(); i++) activation += brain[input[i]].activation * weights[i];
		//apply activation function
		activation = max(0, activation);
	}

	void initializeWeights()
	{
		weights.resize(input.size());
		for(Double& w : weights) w = randomFloatSlow();
	}
};

struct NeuralNet
{
	Vector<Neuron> neurons;
	Vector<UInt>   structure;
	Vector<Int>    inputValues;
	Bool           training = true;
	Vector<Int>    desiredOutputValues;
	Float          learningRate = 0.1f;

	void addNeuron(const Vec3 position, const Vector<ULL>& input, const Vector<ULL>& output)
	{
		Neuron n;
		n.position   = position;
		n.activation = 0.0;
		n.input      = input;
		n.output     = output;
		n.initializeWeights();
		n.error        = 0.0;
		n.errorSources = 0.0;
		neurons.push_back(n);
	}

	void setOutputs(const ULL count)
	{
		structure.push_back(count);

		const ULL   previousLayerRangeStart = 0;
		const ULL   previousLayerRangeEnd   = neurons.size();
		const Float lastNeuronXposition     = neurons.back().position.x;

		for(ULL i = 0; i < count; i++)
		{
			Neuron n;
			n.position.x = lastNeuronXposition + 1;
			n.position.y = -static_cast<Float>(i);
			n.position.z = 0.0f;
			n.activation = 1;
			neurons.push_back(n);

			//connect them after
			for(ULL j = 0; j < previousLayerRangeEnd; j++) connectNeurons(j, previousLayerRangeEnd + i);
		}
	}

	void connectNeurons(const ULL in, const ULL out)
	{
		neurons[out].input.push_back(in);
		neurons[in].output.push_back(out);
		neurons[out].weights.push_back(randomFloatFast());
	}

	[[nodiscard]] Color colorFromActivation(const Float activation) const
	{
		return lerp(Color(1, 0.5, 0, 1), Color(0, 0.5, 1, 1), activation);
	}

	void loadInput()
	{
		inputValues.resize(1000);
		for(Int i = 0; i < 1000; i++) inputValues[i] = randomIntSlow(INT_NEURAL_ONE);
	}

	void loadOutput()
	{
		desiredOutputValues.resize(inputValues.size());
		for(Int i = 0; i < inputValues.size(); i++) desiredOutputValues[i] = inputValues[i] * 2;
	}

	void update()
	{
		loadInput();

		if(training) loadOutput();

		for(Neuron& n : neurons) n.calculate(neurons);

		//output

		//compare

		//for(ULL i = neurons.size(); i > 0; i--)
		//{
		//	neurons[i].calculateError();
		//}
	}

	void render(Renderer& r) const
	{
		r.fill(Color(0.5, 0.5, 0.5, 1));
		r.setDepthTest(false);
		// render connections
		Vec3Vector lines;
		for(const Neuron& n : neurons)
		{
			for(const ULL i : n.input)
			{
				lines.push_back(neurons[i].position);
				lines.push_back(n.position);
			}
		}
		r.lines(lines);

		r.fill(Color(1));
		// render neurons
		Vec3Vector positions;
		for(const Neuron& n : neurons) positions.push_back(n.position);
		r.renderMeshInstanced("centeredCube", positions, 0.5);
	}
};
