
//! #include "uniforms.glsl"

out vec3 pos;

void main() {
	gl_Position = pMatrix* vMatrix * mMatrix * vec4(vertex, 1);
	pos = (mMatrix * vec4(vertex, 1)).xyz;
}