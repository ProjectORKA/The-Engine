
//! #include "uniforms.glsl"

layout(location = 0) out vec4 color;
in vec2 textureCoordinate;

void main( void )
{
    color = texture(texture0, textureCoordinate);
}