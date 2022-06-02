
//! #include "uniforms.glsl"

layout(location = 0) out float fragmentColor;
in vec2 textureCoordinate;

void main(){
    float a = texture(texture0,textureCoordinate).x;// + vec2(0,1/512)).x;

    fragmentColor = a;
};