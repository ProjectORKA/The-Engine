
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

void calculateDistortion(inout vec3 location, inout vec3 normal) {

	float radius = 256;

	float dist = length(vec2(location.xy)) / radius;
	if (dist > 1) {
		location.xy = normalize(location.xy) * radius;
		dist = 1;
	}

	float func = sqrt(pow((location.z / radius + 1), 2) - pow(clamp(dist, 0, 1), 2));
	vec3 newZ = normalize(vec3(location.xy * 2 / radius, func));
	vec3 newX = normalize(cross(vec3(0, 1, 0), newZ));
	vec3 newY = normalize(cross(newZ, newX));
	normal = (normal.x * newX) + (normal.y * newY) + (normal.z * newZ);
	location.z = func * radius;
}