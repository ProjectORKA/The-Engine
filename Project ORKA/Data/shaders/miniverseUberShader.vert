
//! #include "uniforms.glsl"

out vec4 col;

void main() {
	gl_Position = pMatrix* vMatrix * mMatrix * vec4(vertex, 1);

	col = customColor;
};