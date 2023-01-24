
//! #include "uniforms.glsl"

layout(location = 0) out vec4 fragmentColor;
in vec2 textureCoordinate;


vec4 downsampleBox13Tap(sampler2D sampler, vec2 uv, vec2 texelSize)
{
    vec4 A = texture(sampler, uv + texelSize * vec2(-1.0, -1.0));
    vec4 B = texture(sampler, uv + texelSize * vec2( 0.0, -1.0));
    vec4 C = texture(sampler, uv + texelSize * vec2( 1.0, -1.0));
    vec4 D = texture(sampler, uv + texelSize * vec2(-0.5, -0.5));
    vec4 E = texture(sampler, uv + texelSize * vec2( 0.5, -0.5));
    vec4 F = texture(sampler, uv + texelSize * vec2(-1.0,  0.0));
    vec4 G = texture(sampler, uv);
    vec4 H = texture(sampler, uv + texelSize * vec2( 1.0,  0.0));
    vec4 I = texture(sampler, uv + texelSize * vec2(-0.5,  0.5));
    vec4 J = texture(sampler, uv + texelSize * vec2( 0.5,  0.5));
    vec4 K = texture(sampler, uv + texelSize * vec2(-1.0,  1.0));
    vec4 L = texture(sampler, uv + texelSize * vec2( 0.0,  1.0));
    vec4 M = texture(sampler, uv + texelSize * vec2( 1.0,  1.0));

    vec2 div = (1.0 / 4.0) * vec2(0.5, 0.125);

    vec4 o = (D + E + I + J) * div.x;
    o += (A + B + G + F) * div.y;
    o += (B + C + H + G) * div.y;
    o += (F + G + L + K) * div.y;
    o += (G + H + M + L) * div.y;

    return o;
}


void main(){
    vec2 texelSize = 1/vec2(framebufferWidth,framebufferHeight);
    fragmentColor = downsampleBox13Tap(texture0,textureCoordinate,texelSize);
};