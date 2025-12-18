#pragma once
#include "Prototype.hpp"
#include "Random.hpp"

struct Generation
{
	Float bias = 0.0;

	Vector<Float> inputs;
	Float         fitness;
};

struct SineWaveTerrainRegressor final : Prototype
{
	GPUMesh      terrain;
	GPUMesh      terrain2;
	const Double gamma = 0.9;
	Double       loss  = 0.5;
	Vec3Vector   sampleData;
	Vec3Vector   terrainData;

	Vector<Float> inputMin;
	Vector<Float> inputMax;
	Int           valueCount = 33;

	Vector<Generation> gens;
	Int                generationCount = 256;

	Float bestFitness = std::numeric_limits<Float>::max();

	void action() override
	{
		for (Int i = 0; i < generationCount; i++)
		{
			gens[i].fitness = 0;

			for (const auto& data : sampleData)
			{
				gens[i].fitness += abs(data.z - terrainFunction(data.x, data.y, gens[i]));
			}
		}

		std::ranges::sort(gens, [](const Generation& a, const Generation& b)
		{
			return a.fitness < b.fitness;
		});

		for (Int j = 0; j < valueCount; j++)
		{
			inputMin[j] = std::numeric_limits<Float>::max();
			inputMax[j] = std::numeric_limits<Float>::min();
		}

		for (Int i = 0; i < generationCount; i++)
		{
			if (i < generationCount / 4)
			{

				for (Int j = 0; j < valueCount; j++)
				{
					inputMin[j] = std::min(inputMin[j], gens[i].inputs[j]);
					inputMax[j] = std::max(inputMax[j], gens[i].inputs[j]);
				}
			} else
			{
				for (Int j = 0; j < valueCount; j++)
				{
					gens[i].inputs[j] = randomFloatFast(inputMin[j], inputMax[j]);
				}
			}
		}

		if (gens[0].fitness < bestFitness)
		{
			bestFitness = gens[0].fitness;
			for (auto& data : terrainData)
			{
				data.z = terrainFunction(data.x, data.y, gens[0]);
			}
			logDebug(gens[0].fitness);
		}
	}

	[[nodiscard]] Float terrainFunction(const Float x, const Float y, const Generation& gen) const
	{
		const Double xPi = x * PI;
		const Double yPi = y * PI;
		const Double height = gen.bias + 
		gen.inputs[0]  * sin((xPi * 1.0) + gen.inputs[1] ) + gen.inputs[2]  * sin((yPi * 1.0) + gen.inputs[3] ) +
		gen.inputs[4]  * sin((xPi * 2.0) + gen.inputs[5] ) + gen.inputs[6]  * sin((yPi * 2.0) + gen.inputs[7] ) +
		gen.inputs[8]  * sin((xPi * 3.0) + gen.inputs[9] ) + gen.inputs[10] * sin((yPi * 3.0) + gen.inputs[11] ) +
		gen.inputs[12] * sin((xPi * 4.0) + gen.inputs[13] ) + gen.inputs[14] * sin((yPi * 4.0) + gen.inputs[15] );
		return height;
	}

	void save() const {}

	void create(Renderer& r) override
	{
		CpuMesh sampleMesh;
		CpuMesh terrainMesh;
		sampleMesh.load("sampleTerrain");
		terrainMesh.load("terrain");
		terrain.upload(r, sampleMesh);
		terrain2.upload(r, terrainMesh);

		sampleData  = sampleMesh.positions;
		terrainData = terrainMesh.positions;

		for (auto& data : terrainData) data.z = 0.0;

		//set up min and max values
		inputMin.push_back(0.00428783	- 0.5);
		inputMin.push_back(-0.007868321	- 0.5);
		inputMin.push_back(-27.6026688	- 0.5);
		inputMin.push_back(-0.005611666	- 0.5);
		inputMin.push_back(27.0999203	- 0.5);
		inputMin.push_back(0.004152854	- 0.5);
		inputMin.push_back(24.4238682	- 0.5);
		inputMin.push_back(-0.000797887	- 0.5);
		inputMin.push_back(12.1230631	- 0.5);
		inputMin.push_back(-0.002588993	- 0.5);
		inputMin.push_back(-16.5764027	- 0.5);
		inputMin.push_back(-0.002391485	- 0.5);
		inputMin.push_back(-1.29954374	- 0.5);
		inputMin.push_back(-0.001960422	- 0.5);
		inputMin.push_back(32.2843323	- 0.5);
		inputMin.push_back(0.004498377	- 0.5);
		inputMin.push_back(9.88891029	- 0.5);

		inputMax.push_back(0.00428783	+ 0.5);
		inputMax.push_back(-0.007868321	+ 0.5);
		inputMax.push_back(-27.6026688	+ 0.5);
		inputMax.push_back(-0.005611666	+ 0.5);
		inputMax.push_back(27.0999203	+ 0.5);
		inputMax.push_back(0.004152854	+ 0.5);
		inputMax.push_back(24.4238682	+ 0.5);
		inputMax.push_back(-0.000797887	+ 0.5);
		inputMax.push_back(12.1230631	+ 0.5);
		inputMax.push_back(-0.002588993	+ 0.5);
		inputMax.push_back(-16.5764027	+ 0.5);
		inputMax.push_back(-0.002391485	+ 0.5);
		inputMax.push_back(-1.29954374	+ 0.5);
		inputMax.push_back(-0.001960422	+ 0.5);
		inputMax.push_back(32.2843323	+ 0.5);
		inputMax.push_back(0.004498377	+ 0.5);
		inputMax.push_back(9.88891029	+ 0.5);


		for (Int j = inputMax.size() - 1; j < valueCount; j++)
		{
			inputMin.push_back(-10.0);
			inputMax.push_back(10.0);
		}

		for (Int i = 0; i < generationCount; i++)
		{
			Generation gen;
			gen.fitness = std::numeric_limits<Float>::max();
			for (Int j = 0; j < valueCount; j++)
			{
				gen.inputs.push_back(randomFloatFast(inputMin[j], inputMax[j]));
			}
			gens.push_back(gen);
		}

		action();
	}

	void destroy() override {}

	void render(Renderer& r, const Player& player) override
	{
		terrain.updatePositionBuffer(r, sampleData);
		terrain2.updatePositionBuffer(r, terrainData);

		r.uniforms().setMMatrix(matrixFromPosition(Vec3(0, 0, 0.0)));
		r.textRenderSystem.alignText(Alignment::center, Alignment::center);
		r.fontSize(0.5f);

		terrain.render(r);
		r.uniforms().setMMatrix(matrixFromPosition(0, 0));
		terrain2.render(r);
		r.uniforms().setMMatrix(matrixFromPosition(0, 0));

		r.useShader("color");
		r.fill(255, 0, 0);
		r.points(sampleData);

		r.screenSpaceFromBottomLeft();
		r.uniforms().setMMatrix(Matrix(1));
		r.fontSize(16.0f);
		r.letterSpacing(0.6f);
		r.textRenderSystem.alignText(Alignment::left, Alignment::bottom);
		r.text("FPS: " + toString(static_cast<Int>(1.0f / r.time.getDelta())), Vec2(50));
	}

	void inputEvent(Window& window, InputEvent input) override {}
};
