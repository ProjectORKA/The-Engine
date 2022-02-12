
//! #include "uniforms.glsl"

layout(location = 0) out vec4 fragmentColor;
in vec2 textureCoordinate;

void main(){
	vec4 color = texture(texture0,textureCoordinate);
	fragmentColor = color;//color * customColor;
};