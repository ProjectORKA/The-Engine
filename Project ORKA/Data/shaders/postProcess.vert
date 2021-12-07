
//! #include "uniforms.glsl"

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 uvs;
out vec2 textureCoordinate; 

void main() {
	textureCoordinate = uvs;
	gl_Position  = vec4(vertex,1);
};