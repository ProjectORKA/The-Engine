
//! #include "uniforms.glsl"

layout(location = 0) out vec4 fragmentColor;
in vec2 textureCoordinate;

void main(){
    fragmentColor = vec4(vec3(pow(texture(texture0,textureCoordinate).xyz,vec3(1.5))),1);
};