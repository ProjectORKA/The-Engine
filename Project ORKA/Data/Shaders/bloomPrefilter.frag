
//! #include "uniforms.glsl"

layout(location = 0) out vec4 fragmentColor;
in vec2 textureCoordinate;

float brightness(in vec3 color){
    return color.x + color.y + color.z;
}

void main(){
    vec2 texelSize = 1/vec2(framebufferWidth,framebufferHeight);

    vec3 c = texture(texture0,textureCoordinate).xyz;
    vec3 n = texture(texture0,textureCoordinate + texelSize * vec2(0,1)).xyz;
    vec3 s = texture(texture0,textureCoordinate + texelSize * vec2(0,-1)).xyz;
    vec3 e = texture(texture0,textureCoordinate + texelSize * vec2(1,0)).xyz;
    vec3 w = texture(texture0,textureCoordinate + texelSize * vec2(-1,0)).xyz;

    vec3 g = max(max(n,s),max(e,w));

    if(brightness(c) > brightness(g)) c = c / brightness(c) * brightness(g);

    fragmentColor = vec4(c,1);
};