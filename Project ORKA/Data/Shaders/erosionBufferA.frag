// Terrain erosion demo: a port of third-party Shadertoy code, not original ORKA code.
//  - Based on "Terrain Erosion Noise" by Fewes, https://www.shadertoy.com/view/7ljcRW,
//    a fork of "Eroded Terrain Noise" by clayjohn, https://www.shadertoy.com/view/MtGcWh
//    (MIT License, Copyright 2020 Clay John).
//  - noised(): "Noise - Gradient - 2D - Deriv" by Inigo Quilez, https://www.shadertoy.com/view/XdXBRH (MIT License).
//  - Ridges() / Erosion(): adapted from guil's "Gavoronoise", https://www.shadertoy.com/view/llsGWl,
//    licensed CC BY-NC-SA 3.0 (https://creativecommons.org/licenses/by-nc-sa/3.0/).
// Because of the Gavoronoise part, this file as a whole is shared under CC BY-NC-SA 3.0.
// Full license texts and notices: THIRD_PARTY.md in the repository root.

//! #include "uniforms.glsl"

layout(location = 0) out vec4 fragmentColor;

// Shadertoy compatibility ----------------------------------------------------
#define iResolution vec3(float(framebufferWidth), float(framebufferHeight), 1.0)
#define iMouse customColor
#define iChannel0 texture0
#define iChannel1 texture1
#define iChannel2 texture2

// ----------------------------------------------------------------------------
#define EROSION_TILES 4.0
#define EROSION_OCTAVES 8
#define EROSION_GAIN 0.5
#define EROSION_LACUNARITY 2.0
#define EROSION_SLOPE_STRENGTH 3.0
#define EROSION_SLOPE_SENSITIVITY 0.3
#define EROSION_BRANCH_STRENGTH 3.0
#define EROSION_STRENGTH 0.04

#define PI 3.14159265358979
#define saturate(x) clamp(x, 0.0, 1.0)
#define sq(x) (x*x)
#define BUFFER_SIZE iResolution.xy
#define HEIGHT_TILES 3.0
#define HEIGHT_OCTAVES 8
#define HEIGHT_AMP 0.25
#define HEIGHT_GAIN 0.1
#define HEIGHT_LACUNARITY 2.0
#define WATER_HEIGHT 0.45

vec2 hash(in vec2 x)
{
	const vec2 k = vec2(0.3183099, 0.3678794);
	x = x * k + k.yx;
	return -1.0 + 2.0 * fract(16.0 * k * fract(x.x * x.y * (x.x + x.y)));
}

// from https://www.shadertoy.com/view/XdXBRH
vec3 noised(in vec2 p)
{
	vec2 i = floor(p);
	vec2 f = fract(p);

	vec2 u  = f * f * f * (f * (f * 6.0 - 15.0) + 10.0);
	vec2 du = 30.0 * f * f * (f * (f - 2.0) + 1.0);

	vec2 ga = hash(i + vec2(0.0, 0.0));
	vec2 gb = hash(i + vec2(1.0, 0.0));
	vec2 gc = hash(i + vec2(0.0, 1.0));
	vec2 gd = hash(i + vec2(1.0, 1.0));

	float va = dot(ga, f - vec2(0.0, 0.0));
	float vb = dot(gb, f - vec2(1.0, 0.0));
	float vc = dot(gc, f - vec2(0.0, 1.0));
	float vd = dot(gd, f - vec2(1.0, 1.0));

	return vec3(va + u.x * (vb - va) + u.y * (vc - va) + u.x * u.y * (va - vb - vc + vd),
	            ga + u.x * (gb - ga) + u.y * (gc - ga) + u.x * u.y * (ga - gb - gc + gd) +
	                du * (u.yx * (va - vb - vc + vd) + vec2(vb, vc) - va));
}

// Produce ridges orthogonal to the input slope
vec3 Ridges(in vec2 p, vec2 slope)
{
	vec2 sideDir = slope.yx * vec2(-1.0, 1.0);
	vec2 ip      = floor(p);
	vec2 fp      = fract(p);
	float revolution = 2.0 * PI;
	vec3 va          = vec3(0.0);
	float weightSum  = 0.0;
	for (int i = -2; i <= 1; i++)
	{
		for (int j = -2; j <= 1; j++)
		{
			vec2 gridOffset        = vec2(i, j);
			vec2 gridPoint         = ip - gridOffset;
			vec2 randomOffset      = hash(gridPoint) * 0.5;
			vec2 vectorToCellPoint = randomOffset - gridOffset - fp;

			float sqrDist = dot(vectorToCellPoint, vectorToCellPoint);
			float weight  = exp(-sqrDist * 2.0);

			weightSum += weight;

			float waveInput = dot(vectorToCellPoint, sideDir) * revolution;

			va += vec3(cos(waveInput), sin(waveInput) * sideDir) * weight;
		}
	}
	return va / weightSum;
}

vec3 Erosion(in vec2 p, vec3 heightAndSlope, float a, int octaves)
{
	float slopeMag = length(heightAndSlope.yz);
	heightAndSlope.yz /= mix(slopeMag, 1.0 / EROSION_SLOPE_STRENGTH, EROSION_SLOPE_SENSITIVITY);

	float f = 1.0;
	for (int i = 0; i < octaves; i++)
	{
		heightAndSlope += Ridges(p * EROSION_TILES * f, heightAndSlope.yz) *
		                  a * vec3(1.0, f * EROSION_BRANCH_STRENGTH, f * EROSION_BRANCH_STRENGTH);
		a *= EROSION_GAIN;
		f *= EROSION_LACUNARITY;
	}

	return heightAndSlope;
}

vec2 Heightmap(vec2 uv)
{
	vec2 p = uv * HEIGHT_TILES;

	vec3 n = vec3(0.0);
	float nf = 1.0;
	float na = HEIGHT_AMP;
	for (int i = 0; i < HEIGHT_OCTAVES; i++)
	{
		n += noised(p * nf) * na * vec3(1.0, nf, nf);
		na *= HEIGHT_GAIN;
		nf *= HEIGHT_LACUNARITY;
	}

	n.x = n.x * 0.5 + 0.5;

	vec3 h = n;
	float a = 0.5;
	a *= smoothstep(WATER_HEIGHT - 0.07, WATER_HEIGHT + 0.2, n.x);

	int octaves = EROSION_OCTAVES;

	h = Erosion(p, h, a, octaves);

	return vec2(n.x + (h.x - n.x - 0.5) * EROSION_STRENGTH, h.x - n.x);
}

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
	if (fragCoord.x >= BUFFER_SIZE.x || fragCoord.y >= BUFFER_SIZE.y) return;

	vec2 uv = fragCoord / BUFFER_SIZE;

	vec2 h = Heightmap(uv);

	vec2 uv1 = uv + vec2(1.0, 0.0) / 512.0;
	vec2 uv2 = uv + vec2(0.0, 1.0) / 512.0;
	vec2 h1  = Heightmap(uv1);
	vec2 h2  = Heightmap(uv2);
	vec3 v1  = vec3(uv1 - uv, (h1.x - h.x));
	vec3 v2  = vec3(uv2 - uv, (h2.x - h.x));
	vec3 normal = normalize(cross(v1, v2)).xzy;

	fragColor = vec4(h.x, normal.xz, h.y);
}

void main()
{
	mainImage(fragmentColor, gl_FragCoord.xy);
}

