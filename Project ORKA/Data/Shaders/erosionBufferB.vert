
//! #include "uniforms.glsl"

out vec2 textureCoordinate;

void main(){
	// Flip vertically when customInt1 is set (used for framebuffer blit).
	textureCoordinate = bool(customInt1) ? vec2(uvs.x, 1.0 - uvs.y) : uvs;
	gl_Position  = pMatrix * vMatrix * mMatrix * vec4(vertex, 1);
};

