#version 450
layout(location = 0) out vec4 color;
in vec2 textureCoordinate;
in vec3 normal;

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

void main(){
	//if(texture(texture0,textureCoordinate).a != 1) discard;

	color = vec4(texture(texture0,textureCoordinate).xyz,1.0);

};