
//! #include "uniforms.glsl"

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 uvs;
layout(location = 2) in vec3 normals;

//output
out float depth;
out vec3 normal;
out vec3 vertexPosition;
out vec3 worldCoordinate;
out vec2 textureCoordinate; 

void main() {
	gl_Position  = pMatrix * vMatrix * mMatrix * vec4(vertex, 1);

	vertexPosition = vertex;
	depth = gl_Position.w;
	normal = normals;
	textureCoordinate = uvs;
	worldCoordinate = vec3(worldOffset.xyz + vertex) / pow(2,64);
};