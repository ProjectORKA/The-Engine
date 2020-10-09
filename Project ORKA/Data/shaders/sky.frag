#version 400

layout(location = 0) out vec4 color;

in vec2 textureCoordinate;
in vec3 normal;

uniform sampler2D baseColor;
//uniform sampler2D normal;

void main(){
	if(texture(baseColor,textureCoordinate).a != 1) discard;

	color = vec4(texture(baseColor,textureCoordinate).xyz,1.0);

};