//! #include "uniforms.glsl"

layout(location = 0) out vec4 fragmentColor;

#define iChannel0 texture0

const vec3 LUMA = vec3(0.2126, 0.7152, 0.0722);

vec4 statsFromColor(vec3 c)
{
	float l = max(dot(c, LUMA), 1e-6);
	return vec4(l, l, log(l), 1.0);
}

void main()
{
	ivec2 outCoord = ivec2(gl_FragCoord.xy);
	ivec2 srcSize  = textureSize(iChannel0, 0);
	ivec2 base     = outCoord * 2;

	bool sourceIsStats = (customInt1 != 0);

	vec4 accum = vec4(1e20, -1e20, 0.0, 0.0); // min, max, logSum, weight

	for (int dy = 0; dy < 2; ++dy)
	{
		for (int dx = 0; dx < 2; ++dx)
		{
			ivec2 coord = base + ivec2(dx, dy);
			if (coord.x >= srcSize.x || coord.y >= srcSize.y) continue;

			vec4 s = sourceIsStats ? texelFetch(iChannel0, coord, 0) : statsFromColor(texelFetch(iChannel0, coord, 0).rgb);

			accum.x = min(accum.x, s.x); // min
			accum.y = max(accum.y, s.y); // max
			accum.z += s.z;              // log sum
			accum.w += s.w;              // weight
		}
	}

	if (accum.w == 0.0) accum = vec4(1.0, 1.0, 0.0, 1.0);

	fragmentColor = accum;
}

