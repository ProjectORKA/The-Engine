#pragma once
#include "Basics.hpp"

struct GlobalUniformData
{
	// values have to be laid out in blocks of 16 bytes in memory
	// data has to match up with "uniforms.glsl"
	Matrix mMatrix           = Matrix(1);        // 16 * 4
	Matrix vMatrix           = Matrix(1);        // 16 * 4
	Matrix pMatrix           = Matrix(1);        // 16 * 4
	Vec4   worldOffset       = Vec4(0, 0, 0, 0); // 16
	Vec4   cameraVector      = Vec4(0, 0, 0, 0); // 16
	Vec4   cameraPosition    = Vec4(0, 0, 0, 0); // 16
	Vec4   customColor       = Vec4(0, 0, 0, 1); // 16
	Vec4   sunDir            = Vec4(0, 0, 1, 1); // 16
	Float  time              = 0.0f;             // 16
	Float  custom1           = 0.0f;
	Float  custom2           = 0.0f;
	Float  custom3           = 0.0f;
	Int    windowWidth       = 0; // 16
	Int    windowHeight      = 0;
	Int    framebufferWidth  = 0;
	Int    framebufferHeight = 0;
	Int    materialId        = 0; // 16
	Int    objectId          = 0;
	Int    instanced         = 0;
	Int    customInt1        = 0;
};
