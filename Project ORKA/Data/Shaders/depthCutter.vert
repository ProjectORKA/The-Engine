
//! #include "uniforms.glsl"

out vec2 textureCoordinate;


void main() {
	gl_Position = vec4(vertex,1);
	textureCoordinate = uvs;
};