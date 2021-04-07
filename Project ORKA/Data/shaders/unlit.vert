#version 450

//input
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 uvs;

//uniforms
uniform mat4 mMatrix;
uniform mat4 vpMatrix;

//output
out vec2 textureCoordinate; 

void main(){
	vec4 worldPosition = mMatrix * vec4(vertex, 1);
	gl_Position  = vpMatrix * worldPosition;
	textureCoordinate = uvs;
};