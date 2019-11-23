#version 330 core

layout(location = 0) in vec3 vertex;



out vec4 vertexColor; 

uniform vec4 worldOffset;
uniform mat4 mMatrix;
uniform mat4 vpMatrix;

void main() {

    gl_Position = vpMatrix * mMatrix * vec4(vertex, 1.0);

	vertexColor = vec4(vertex,1.0f);
};
