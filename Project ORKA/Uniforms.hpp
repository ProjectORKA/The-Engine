#pragma once

#include "Basics.hpp"
#include "GraphicsAPI.hpp"

struct GlobalUniformData {
	//values have to be layed out in blocks of 16 bytes in memory
	Matrix  mMatrix;			//16 * 4
	Matrix vpMatrix;			//16 * 4
	Vec4 worldOffset;			//16
	Vec4 cameraVector;			//16
	Vec4 chunkOffsetVector;		//16
	Vec4 customColor;
	float time;					//16
	float custom1;
	float custom2;
	float custom3;
	Int distortion;				//16
	float placeholder1;
	float placeholder2;
	float placeholder3;
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