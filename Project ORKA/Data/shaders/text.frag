#version 450
layout(location = 0) out vec4 color;
in vec2 textureCoordinate;

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
	float customColor = texture(texture0,textureCoordinate).r;
	if(customColor < 0.9) discard;
	color = vec4(vec3(1),customColor);
}