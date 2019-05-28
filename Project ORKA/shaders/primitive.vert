#version 330 core

layout(location = 0) in vec3 vertex;

out vec3 vertexColor; 

void main() {

    gl_Position = vec4(vertex, 1.0);

	vertexColor = vertex;
};
