in vec2 uiUv;
in vec2 uiLocal;
in vec2 uiSize;
in vec4 uiRadii;
in vec4 uiColor;
out vec4 color;

float roundedBoxCoverage(vec2 local, vec2 size, vec4 radii)
{
	vec2 halfSize = size * 0.5;
	vec2 p = local - halfSize;
	float radius = max(max(radii.x, radii.y), max(radii.z, radii.w));
	radius = min(radius, min(halfSize.x, halfSize.y));
	vec2 q = abs(p) - halfSize + radius;
	float dist = min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - radius;
	float aa = max(fwidth(dist), 1e-4);
	return 1.0 - smoothstep(-aa, aa, dist);
}

void main()
{
	float radius = max(max(uiRadii.x, uiRadii.y), max(uiRadii.z, uiRadii.w));
	if (radius > 0.0)
	{
		float coverage = roundedBoxCoverage(uiLocal, uiSize, uiRadii);
		if (coverage <= 0.0) discard;
		color = uiColor * coverage;
	}
	else color = uiColor;
}
