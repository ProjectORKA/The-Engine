
//! #include "uniforms.glsl"

out vec4 worldPos;

void main() {
	worldPos = mMatrix * vec4(vertex, 1);

	gl_Position = pMatrix* vMatrix * worldPos;
};