#pragma once

#include "Basics.hpp"
#include "GraphicsAPI.hpp"
#include "FileSystem.hpp"

struct GlobalUniformData {
	//values have to be layed out in blocks of 16 bytes in memory
	//data has to match up with "uniforms.glsl"
	Matrix mMatrix = Matrix(1);						//16 * 4
	Matrix vMatrix = Matrix(1);						//16 * 4
	Matrix pMatrix = Matrix(1);						//16 * 4
	Vec4 worldOffset = Vec4(0, 0, 0, 0);			//16
	Vec4 cameraVector = Vec4(0, 0, 0, 0);			//16
	Vec4 cameraPosition = Vec4(0, 0, 0, 0);			//16
	Vec4 customColor = Vec4(0, 0, 0, 1);			//16
	Vec4 sunDir = Vec4(0, 0, 1, 1);					//16
	Vec4 screenDimensions = Vec4(1, 1, 0, 0);		//16
	float time = 0.0f;								//16
	float custom1 = 0.0f;
	float custom2 = 0.0f;
	float custom3 = 0.0f;
	Int distortion = 0;								//16
	Int width = 0;
	Int height = 0;
	Int placeholder1 = 0;
};

struct Uniforms {
	Index id = -1;
	GlobalUniformData data;
	String uniformBlockShaderCode;

	void create();
	void reset();
	void update();
	void destroy();

	Map<Name, Index> sampler2Ds;
	Map<Name, Index> sampler2DMS;
};