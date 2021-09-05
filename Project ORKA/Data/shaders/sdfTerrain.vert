#version 450
#extension GL_ARB_shading_language_include : require
#include "/uniforms.glsl" //! #include "uniforms.glsl" 

//input
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 uvs;
layout(location = 2) in vec3 normals;

//output
out vec3 rayOrigin;
out highp vec3 rayDirection;

void main(){
	rayOrigin = chunkOffsetVector.xyz;
	rayDirection = normalize(vertex);

	vec4 screenSpacePos = vpMatrix * vec4(vertex,1);

	gl_Position = screenSpacePos;
};