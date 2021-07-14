#version 450
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 uvs;
layout(location = 2) in vec3 normals;
out vec2 textureCoordinate; 
out vec3 normal;

layout(std140, binding = 0) uniform GlobalUniforms
{
	mat4 mMatrix;
	mat4 vpMatrix;
	
	vec4 worldOffset;
	vec4 cameraVector;		 //its vec3 but treated as vec4 in memory
	vec4 chunkOffsetVector;	 //its vec3 but treated as vec4 in memory
	vec4 customColor;

	float time;
	float custom1;
	float custom2;
	float custom3;

	bool distortion;
};

uniform sampler2D texture0;

void main() {

	vec3 positionInChunk = (mMatrix * vec4(vertex, 1)).xyz;

	gl_Position  = vpMatrix * vec4(positionInChunk,1);
	textureCoordinate = uvs;
	normal = normals;
};