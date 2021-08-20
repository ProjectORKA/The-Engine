#version 450
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 uvs;

//uniforms
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

//output
out vec2 textureCoordinate;

void main(){
	vec4 worldPosition = mMatrix * vec4(vertex, 1);
	gl_Position  = vpMatrix * worldPosition;
	textureCoordinate = uvs;
};