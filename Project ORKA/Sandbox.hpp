
#pragma once

#include "Math.hpp"
#include "Game.hpp"
#include "NeuralNetwork.hpp"
#include "MathDNA.hpp"

struct Sandbox : public Game {
	Mutex mutex;
	Plot targetPlot;
	NeuralNetwork brain;

	Sandbox() {
		targetPlot.add(0.000000, 0.000000);
		targetPlot.add(0.002390, 0.059189);
		targetPlot.add(0.006407, 0.117294);
		targetPlot.add(0.012321, 0.160215);
		targetPlot.add(0.017965, 0.203678);
		targetPlot.add(0.023201, 0.216359);
		targetPlot.add(0.031421, 0.228497);
		targetPlot.add(0.045605, 0.239551);
		targetPlot.add(0.062500, 0.247351);
		targetPlot.add(0.091491, 0.259311);
		targetPlot.add(0.111854, 0.265424);
		targetPlot.add(0.128609, 0.269184);
		targetPlot.add(0.185617, 0.275301);
		targetPlot.add(0.250000, 0.280602);
		targetPlot.add(0.311199, 0.289214);
		targetPlot.add(0.374349, 0.302705);
		targetPlot.add(0.438801, 0.321023);
		targetPlot.add(0.500108, 0.345955);
		targetPlot.add(0.532660, 0.362713);
		targetPlot.add(0.569007, 0.385436);
		targetPlot.add(0.600582, 0.412714);
		targetPlot.add(0.625000, 0.443246);
		targetPlot.add(0.648279, 0.469191);
		targetPlot.add(0.661717, 0.480554);
		targetPlot.add(0.677414, 0.489657);
		targetPlot.add(0.693772, 0.495333);
		targetPlot.add(0.711259, 0.496302);
		targetPlot.add(0.749812, 0.497111);
		targetPlot.add(0.781627, 0.498638);
		targetPlot.add(0.813630, 0.501483);
		targetPlot.add(0.844315, 0.507153);
		targetPlot.add(0.875000, 0.514328);
		targetPlot.add(0.908658, 0.527308);
		targetPlot.add(0.937647, 0.544430);
		targetPlot.add(0.964112, 0.569648);
		targetPlot.add(0.983484, 0.598933);
		targetPlot.add(0.991034, 0.625152);
		targetPlot.add(0.993705, 0.648660);
		targetPlot.add(0.994707, 0.697844);
		targetPlot.add(0.997436, 0.800822);
		targetPlot.add(1.000000, 1.000000);

		brain.initialize();




	};

	void buttonIsPressed(Window& window, Int keyID, Int action, Int modifiers) {
		mutex.lock();

		brain.initialize();
		mutex.unlock();
	};



	void render(Renderer& renderer) override {
		mutex.lock();
		renderer.shaderSystem.use("color");
		
		renderer.apectCorrectNormalizedSpace();
		renderer.uniforms().data.customColor = Vec4(1);
		renderer.uniforms().data.mMatrix = translate(Matrix(1),Vec3(-0.5,-0.5,0));
		renderer.uniforms().update();

		CPUMesh cpuGraphMesh(targetPlot);
		
		GPUMesh graphMesh(cpuGraphMesh);
		graphMesh.render();

		//static Vector<Float> input;
		//static Float time = 0;
		//time += renderer.renderTime.delta;
		////if (time > 0.1) {
		////	time -= 0.1;
		//	input.clear();
		//	input.push_back(randomFloat(0, 1));
		//	input.push_back(randomFloat(0, 1));
		////}

		//brain.input(input);
		//brain.calculate();
		//brain.render(renderer);


		DNAFunction f;
		f.type = f.DNApow;
		DNAResult x;
		x.type = x.DNAinput;

		DNAResult b;
		b.type = b.DNArandom;
		b.variable = 2;

		DNAFunction e;
		e.type = e.DNAmul;
		e.a = &b;
		e.b = &x;

		dnaInput = 1;

		f.a = &x;
		f.b = &e;


		f.print();

		Plot plot;
		for (int i = 0; i < 1000; i++) {
			dnaInput = Float(i) / 1000.0f;
			plot.add(dnaInput, f.calculate());
		}
		CPUMesh cpuPlot(plot);

		GPUMesh gpuPlot(cpuPlot);
		gpuPlot.render();


		mutex.unlock();
	};
};