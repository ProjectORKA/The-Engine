
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

	double time;
	float f1;
	float f2;

	uint windowWidth;
	uint windowHeight;
	uint framebufferWidth;
	uint framebufferHeight;

	uint materialID;
	uint objectID;
	bool instanced;
	uint customInt1;
};
