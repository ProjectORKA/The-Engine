
//! #include "uniforms.glsl"

in vec4 vertexColor;
in vec2 textureCoordinate; 
layout(location = 0) out vec4 color;

void main(){
	color = vertexColor;//vec4(texture(texture0, textureCoordinate).rgb,1.0);
};