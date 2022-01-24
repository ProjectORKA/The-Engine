
//! #include "uniforms.glsl"

out vec2 textureCoordinate; 

void main() {
	textureCoordinate = uvs;
	gl_Position  = vec4(vertex,1);
};