
//! #include "uniforms.glsl"

layout(location = 0) out vec4 fragmentColor;
in vec2 textureCoordinate;

void main(){
	fragmentColor = texture(texture0,textureCoordinate);
};