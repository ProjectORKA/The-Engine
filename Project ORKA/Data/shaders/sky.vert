
//! #include "uniforms.glsl"

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 uvs;
layout(location = 2) in vec3 normals;
out vec2 textureCoordinate; 
out vec3 normal;

void main() {
	gl_Position = pMatrix* vMatrix * vec4(vertex, 1);
	textureCoordinate = uvs;
	normal = normals;
};