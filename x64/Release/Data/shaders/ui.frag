
//! #include "uniforms.glsl"

in vec2 textureCoordinate;
out vec4 color;

void main()
{
  color = vec4(texture(texture0, gl_FragCoord.xy / 4).rgb,1.0f);
}