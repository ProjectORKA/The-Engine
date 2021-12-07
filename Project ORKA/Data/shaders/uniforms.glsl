
#version 450
layout(std140, binding = 0) uniform GlobalUniforms
{
	mat4 mMatrix;
	mat4 vpMatrix;
	
	vec4 worldOffset;
	vec4 cameraVector;
	vec4 chunkOffsetVector;
	vec4 customColor;

	float time;
	float f1;
	float f2;
	float f3;

	bool distortion;
	int i1;
	int i2;
	int i3;
};

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;
uniform sampler2D texture4;