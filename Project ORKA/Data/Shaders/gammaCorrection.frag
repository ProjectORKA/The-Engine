
//! #include "uniforms.glsl"

layout(location = 0) out vec4 fragmentColor;
in vec2 textureCoordinate;

void main(){
    fragmentColor = pow(texture(texture0, textureCoordinate), vec4(1.0/2.4));
}