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
	float colorFringe = 0.0005;
	float width = 0.0;
	float contrast = 1;

	float red = texture(texture0,textureCoordinate).r;
	float green = texture(texture0,textureCoordinate - vec2(colorFringe,0)).r;
	float blue = texture(texture0,textureCoordinate - vec2(colorFringe * 2,0)).r;
	
	red = pow(red + width,contrast);
	green = pow(green + width,contrast);
	blue = pow(blue + width,contrast);
	
	vec4 textColor = vec4(red,green,blue, (red+green+blue) / 3);

	color = textColor;
}