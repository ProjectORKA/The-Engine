

//? #version 450

layout(std140, binding = 0) uniform GlobalUniforms
{
	mat4 mMatrix;
	mat4 vpMatrix;
	
	vec4 worldOffset;
	vec4 cameraVector;
	vec4 chunkOffsetVector;
	vec4 customColor;

	float time;
	float custom1;
	float custom2;
	float custom3;

	bool distortion;
};