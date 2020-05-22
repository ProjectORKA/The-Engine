#version 330 core

out vec4 color;

in vec4 vertexColor;
in vec2 textureCoordinate; 

uniform sampler2D texture0;

void main(){
	
	if(texture(texture0,textureCoordinate).a != 1) discard;

	color = vec4(texture(texture0, textureCoordinate).rgba);
};