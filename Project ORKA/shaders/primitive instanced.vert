#version 330 core

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 positions;
layout(location = 2) in vec2 uvs;

out vec4 vertexColor; 

uniform mat4 vpMatrix;

void main() {
	vec4 position = vpMatrix * vec4(vertex + positions, 1.0f);
    gl_Position = position;
	gl_PointSize = 10.0f / position.z;
	vertexColor = vec4(uvs, 1, 1);
//	vertexColor = vec4(position.xyz, 1);
};
