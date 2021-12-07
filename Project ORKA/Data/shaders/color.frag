
//! #include "uniforms.glsl"

layout(location = 0) out vec4 color;
in vec2 textureCoordinate;
in vec3 normal;

void main(){
	color = customColor;
};