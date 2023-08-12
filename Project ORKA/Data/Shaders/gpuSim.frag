
//! #include "uniforms.glsl"

layout(location = 0) out vec4 fragmentColor;
in vec2 textureCoordinate;

void main(){
    vec2 size = textureSize(texture0,0);
    
    vec3 orig = texture(texture0,textureCoordinate).xyz;
    vec3 n = texture(texture0, textureCoordinate + (vec2(0,+1)/size)).xyz;
    vec3 s = texture(texture0, textureCoordinate + (vec2(0,-1)/size)).xyz;
    vec3 e = texture(texture0, textureCoordinate + (vec2(-1,0)/size)).xyz;
    vec3 w = texture(texture0, textureCoordinate + (vec2(+1,0)/size)).xyz;
    float change = 0.001f;
    vec3 final = (orig + n + s + e + w) / 5;

    fragmentColor = vec4(final.xyz,1);
};