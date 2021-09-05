#pragma once

#include "Basics.hpp"
#include "GraphicsAPI.hpp"

struct GlobalUniformData {
	//values have to be layed out in blocks of 16 bytes in memory
	//data has to match up with "uniforms.glsl"
	Matrix mMatrix = Matrix(1);						//16 * 4
	Matrix vpMatrix = Matrix(1);					//16 * 4
	Vec4 worldOffset = Vec4(0, 0, 0, 0);			//16
	Vec4 cameraVector = Vec4(0, 0, 0, 0);			//16
	Vec4 chunkOffsetVector = Vec4(0, 0, 0, 0);		//16
	Vec4 customColor = Vec4(0, 0, 0, 1);
	float time = 0.0f;								//16
	float custom1 = 0.0f;
	float custom2 = 0.0f;
	float custom3 = 0.0f;
	Int distortion = 0;								//16
	Int placeholder1 = 0.0f;
	Int placeholder2 = 0.0f;
	Int placeholder3 = 0.0f;
};

struct Uniforms {
	Index id = -1;
	GlobalUniformData data;

	void create();
	void reset();
	void update();
	void destroy();

	Map<Name, Index> sampler2Ds;
	Map<Name, Index> sampler2DMS;
};