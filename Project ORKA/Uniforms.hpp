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
	
	float time = 0.0f;								//16
	float custom1 = 0.0f;
	float custom2 = 0.0f;
	float custom3 = 0.0f;
	
	Int width = 0;									//16
	Int height = 0;
	Int posX = 0;
	Int posY = 0;
	
	Int materialID = 0;								//16
	Int objectID = 0;
	Int instanced = 0;
	Int customInt1 = 0;
};

struct Uniforms {
	Index id = -1;
	String uniformBlockShaderCode;

	Bool update = true;

	void create();
	void reset();
	void upload();
	void destroy();

	Int& width();
	Int& height();
	Float& time();
	Vec4& sunDir();
	Int& objectID();
	Int& instanced();
	Float& custom1();
	Int& customInt1();
	Vec4& cameraPos();
	Vec4& cameraVec();
	Matrix& mMatrix();
	Matrix& vMatrix();
	Matrix& pMatrix();
	Vec4& customColor();
	Vec4& worldOffset();

	Int& width(Int value);
	Int& height(Int value);
	Float& time(Float value);
	Vec4& sunDir(Vec4 value);
	Int& objectID(UInt value);
	Int& instanced(Int value);
	Int& customInt1(Int value);
	Float& custom1(Float value);
	Vec4& cameraPos(Vec4 value);
	Vec4& cameraVec(Vec4 value);
	Vec4& customColor(Vec4 value);
	Vec4& worldOffset(Vec4 value);
	Matrix& mMatrix(Matrix value);
	Matrix& vMatrix(Matrix value);
	Matrix& pMatrix(Matrix value);

	Map<Name, Index> sampler2Ds;
	Map<Name, Index> sampler2DMS;
private:
	GlobalUniformData data;
};