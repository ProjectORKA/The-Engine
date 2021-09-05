#version 450
#extension GL_ARB_shading_language_include : require
#include "/uniforms.glsl" //! #include "uniforms.glsl" 

in vec2 textureCoordinate;
out vec4 color;

void main()
{
  color = vec4(texture(texture0, gl_FragCoord.xy / 4).rgb,1.0f);
}