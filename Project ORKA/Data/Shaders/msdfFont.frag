
//! #include "uniforms.glsl"

layout(location = 0) out vec4 fragmentColor;
in vec2 textureCoordinate;

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

float pxRange = 1;

float screenPxRange() {
    vec2 unitRange = vec2(pxRange)/vec2(textureSize(texture0, 0));
    vec2 screenTexSize = vec2(1.0)/fwidth(textureCoordinate);
    return max(0.5*dot(unitRange, screenTexSize), 1.0);
}

void main() {
    vec4 msd = texture(texture0, textureCoordinate);
    float sd = median(msd.r, msd.g, msd.b);
    float screenPxDistance = screenPxRange()*(sd - 0.5);
    float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);
    float alpha = float(msd.a > 0);
    fragmentColor = mix(vec4(0,0,0,1), vec4(alpha), opacity);
}