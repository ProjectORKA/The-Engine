#version 450
#extension GL_ARB_shading_language_include : require
#include "/uniforms.glsl" //! #include "uniforms.glsl" 

layout(location = 0) out vec4 color;
in vec2 textureCoordinate;

void main( void )
{
    color = texture(texture0, textureCoordinate);
}