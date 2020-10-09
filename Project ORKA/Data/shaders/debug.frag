#version 330 core

layout(location = 0) out vec4 color;

in vec4 vertexColor;
in vec2 textureCoordinate; 

uniform sampler2D texture0;

void main(){
	color = vertexColor;//vec4(texture(texture0, textureCoordinate).rgb,1.0);
};