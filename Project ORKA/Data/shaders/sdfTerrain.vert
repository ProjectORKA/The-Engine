#version 450

//input
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 uvs;
layout(location = 2) in vec3 normals;

layout(std140, binding = 0) uniform GlobalUniforms
{
	mat4 mMatrix;
	mat4 vpMatrix;
	
	vec4 worldOffset;
	vec4 cameraVector;		 //its vec3 but treated as vec4 in memory
	vec4 chunkOffsetVector;	 //its vec3 but treated as vec4 in memory
	vec4 customColor;

	float time;
	float custom1; //<- ths will be the camera height
	float custom2;
	float custom3;

	bool distortion;
};

uniform sampler2D texture0;
uniform sampler2D texture1;
//uniform sampler2D texture2;

//output
out vec3 rayOrigin;
out highp vec3 rayDirection;

void main(){
	rayOrigin = chunkOffsetVector.xyz;
	rayDirection = normalize(vertex);

	vec4 screenSpacePos = vpMatrix * vec4(vertex,1);

	gl_Position = screenSpacePos;
};