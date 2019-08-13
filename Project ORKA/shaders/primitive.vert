#version 330 core

layout(location = 0) in vec3 vertex;

out vec4 vertexColor; 

uniform mat4 vpMatrix;

void main() {

    gl_Position = vpMatrix * vec4(vertex, 1.0);

	vertexColor = vec4(vertex,1.0f);
};
