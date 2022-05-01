
#version 450

layout(std140, binding = 0) uniform GlobalUniforms
{
	mat4 mMatrix;
	mat4 vMatrix;
	mat4 pMatrix;

	vec4 worldOffset;
	vec4 cameraVector;
	vec4 cameraPosition;
	vec4 customColor;
	vec4 sunDir;

	float time;
	float f1;
	float f2;
	float f3;

	uint width;
	uint height;
	uint posX;
	uint posY;

	uint materialID;
	uint objectID;
	bool instanced;
	uint customInt1;
};

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;
uniform sampler2D texture4;

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 uvs;
layout(location = 2) in vec3 normals;
layout(location = 3) in vec3 colors;
layout(location = 4) in mat4 transform;