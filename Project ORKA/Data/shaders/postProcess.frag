#version 450

layout(location = 0) out vec4 color;

in vec2 textureCoordinate;

uniform sampler2D texture0;

void main( void )
{
    color = texture(texture0, textureCoordinate);
}