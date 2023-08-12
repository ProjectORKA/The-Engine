
#version 460

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

	uint windowWidth;
	uint windowHeight;
	uint framebufferWidth;
	uint framebufferHeight;

	uint materialID;
	uint objectID;
	bool instanced;
	uint customInt1;
};

layout(binding = 0) uniform sampler2D texture0;
layout(binding = 1) uniform sampler2D texture1;
layout(binding = 2) uniform sampler2D texture2;
layout(binding = 3) uniform sampler2D texture3;
layout(binding = 4) uniform sampler2D texture4;
layout(binding = 5) uniform sampler2D texture5;
layout(binding = 6) uniform sampler2D texture6;
layout(binding = 7) uniform sampler2D texture7;
layout(binding = 8) uniform sampler2D texture8;
layout(binding = 9) uniform sampler2D texture9;
layout(binding = 10) uniform sampler2D texture10;
layout(binding = 11) uniform sampler2D texture11;
layout(binding = 12) uniform sampler2D texture12;
layout(binding = 13) uniform sampler2D texture13;
layout(binding = 14) uniform sampler2D texture14;
layout(binding = 15) uniform sampler2D texture15;

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 uvs;
layout(location = 2) in vec3 normals;
layout(location = 3) in vec3 tangents;
layout(location = 4) in vec3 bitangents;
layout(location = 5) in vec3 colors;
layout(location = 6) in mat4 transform;