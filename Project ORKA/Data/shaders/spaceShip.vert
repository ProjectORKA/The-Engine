#version 330 core

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 uvs;
//layout(location = 1) in vec3 positions;

out vec4 vertexColor;
out vec2 textureCoordinate; 


uniform float time;
uniform float cameraHeight;

uniform mat4 mMatrix;
uniform mat4 vpMatrix;

void main() {
	gl_Position  = vpMatrix * mMatrix * vec4(vertex,1);
	vertexColor = vec4(1);
	textureCoordinate = uvs;
};