
//! #include "uniforms.glsl"

out vec3 position;

void main() {
	gl_Position = pMatrix* vMatrix * vec4(vertex, 1);
	position = vertex;
};