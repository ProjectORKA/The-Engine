#version 450
#extension GL_ARB_shading_language_include : require
#include "/uniforms.glsl" //! #include "uniforms.glsl" 

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 uvs;
layout(location = 2) in vec3 normals;
out vec2 textureCoordinate; 
out vec3 normal;

void main() {
	gl_Position  = vpMatrix * vec4(vertex, 1);
	textureCoordinate = uvs;
	normal = normals;
};