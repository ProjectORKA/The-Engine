in vec2 uiUv;
in vec4 uiColor;
in float uiDistanceRange;
out vec4 color;
float median(float a, float b, float c) { return max(min(a, b), min(max(a, b), c)); }
void main()
{
	if ((customInt1 & 1u) == 0u) { color = uiColor; return; }
	vec3 sampleValue = texture(texture0, uiUv).rgb;
	float distance = median(sampleValue.r, sampleValue.g, sampleValue.b) - 0.5;
	float range = max(uiDistanceRange, 1.0);
	float coverage = clamp(distance * range / max(fwidth(distance), 0.0001) + 0.5, 0.0, 1.0);
	color = uiColor * coverage;
}
