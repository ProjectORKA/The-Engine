#version 450
#extension GL_ARB_shading_language_include : require
#include "/uniforms.glsl" //! #include "uniforms.glsl" 

layout(location = 0) out vec4 fragmentColor;
in vec2 textureCoordinate;

void main(){
	vec4 color = texture(texture0,textureCoordinate);
	fragmentColor = color * customColor;
};