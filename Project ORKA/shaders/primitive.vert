#version 330 core

layout(location = 0) in vec3 vertex;

out vec3 vertexColor; 

//uniform mat4 modelMatrix;
//uniform mat4 viewMatrix;
//uniform mat4 projectionMatrix;
uniform mat4 mvpMatrix;

void main() {

//	mat4 mvp = projectionMatrix * viewMatrix * modelMatrix;

    gl_Position = mvpMatrix * vec4(vertex, 1.0);

	vertexColor = vertex;
};
