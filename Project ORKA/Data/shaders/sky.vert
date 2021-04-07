#version 450

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 uvs;
layout(location = 2) in vec3 normals;

out vec2 textureCoordinate; 
out vec3 normal;

uniform float time;
uniform mat4 mMatrix;
uniform mat4 vpMatrix;

void main() {

	vec3 positionInChunk = (mMatrix * vec4(vertex, 1)).xyz;

	gl_Position  = vpMatrix * vec4(positionInChunk,1);
	textureCoordinate = uvs;
	normal = normals;
};