
//! #include "uniforms.glsl"

layout(location = 0) out vec4 fragmentColor;
in vec2 textureCoordinate;

void main(){
	fragmentColor = vec4(texture(texture0,textureCoordinate).rgb,1);
};