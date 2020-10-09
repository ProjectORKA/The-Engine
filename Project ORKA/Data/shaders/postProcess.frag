#version 400

out vec4 color;

in vec2 textureCoordinate;

uniform sampler2D baseColor;

void main(){

	color = vec4(texture(baseColor, textureCoordinate).rgb,1);

};