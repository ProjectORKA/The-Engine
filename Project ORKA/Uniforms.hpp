#pragma once

#include "Basics.hpp"
#include "GraphicsAPI.hpp"
#include "ResourceManager.hpp"

struct GlobalUniformData {
	//values have to be layed out in blocks of 16 bytes in memory
	//data has to match up with "uniforms.glsl"
	Matrix mMatrix = Matrix(1); //16 * 4
	Matrix vMatrix = Matrix(1); //16 * 4
	Matrix pMatrix = Matrix(1); //16 * 4

	Vec4 worldOffset = Vec4(0, 0, 0, 0); //16
	Vec4 cameraVector = Vec4(0, 0, 0, 0); //16
	Vec4 cameraPosition = Vec4(0, 0, 0, 0); //16
	Vec4 customColor = Vec4(0, 0, 0, 1); //16
	Vec4 sunDir = Vec4(0, 0, 1, 1); //16

	float time = 0.0f; //16
	float custom1 = 0.0f;
	float custom2 = 0.0f;
	float custom3 = 0.0f;

	Int windowWidth = 0; //16
	Int windowHeight = 0;
	Int framebufferWidth = 0;
	Int framebufferHeight = 0;

	Int materialID = 0; //16
	Int objectID = 0;
	Int instanced = 0;
	Int customInt1 = 0;
};

struct Uniforms {
	Index id = -1;
	String uniformBlockShaderCode;

	Bool update = true;

	void reset();
	void upload() const;
	void destroy() const;
	void resetMatrices();
	void create(ResourceManager& resourceManager);

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
	Int& windowWidth();
	Int& windowHeight();
	Int& framebufferWidth();
	Int& framebufferHeight();
	Vec4& customColor();
	Vec4& worldOffset();

	Float& time(Float value);
	Vec4& sunDir(Vec4 value);
	Int& objectID(UInt value);
	Int& instanced(Int value);
	Int& customInt1(Int value);
	Float& custom1(Float value);
	Vec4& cameraPos(Vec4 value);
	Vec4& cameraVec(Vec4 value);
	Int& windowWidth(Int value);
	Int& windowHeight(Int value);
	Int& framebufferWidth(Int value);
	Int& framebufferHeight(Int value);
	Vec4& customColor(Vec4 value);
	Vec4& worldOffset(Vec4 value);
	Matrix& mMatrix(const Matrix& value);
	Matrix& vMatrix(const Matrix& value);
	Matrix& pMatrix(const Matrix& value);

	Map<Name, Index> sampler2Ds;
	Map<Name, Index> sampler2DMS;

private:
	GlobalUniformData data;
};
