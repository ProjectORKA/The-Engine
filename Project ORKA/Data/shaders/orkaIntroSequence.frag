
//! #include "uniforms.glsl"

layout(location = 0) out vec4 fragmentColor;
in vec2 textureCoordinate;

void main(){
    vec4 color = vec4(texture(texture0,textureCoordinate));
    fragmentColor = color * customColor;
};