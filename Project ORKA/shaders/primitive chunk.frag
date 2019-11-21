#version 330 core

out vec4 color;

in vec4 vertexColor;

void main(){

	color = vec4(vertexColor);

};