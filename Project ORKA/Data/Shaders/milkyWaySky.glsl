// Procedural Milky Way sky (Shadertoy-style galaxy). ORKA Z-up rays are remapped to Y-up.
// explanation=false: full compilation only, no overlay text.

#define MW_CAMERA_ZOOM 0.35
#define MW_ITERATIONS 4
#define MW_SEED 107.0

vec2 mwResolution()
{
	return vec2(float(framebufferWidth), float(framebufferHeight));
}

vec3 mwHash33(vec3 p, float seed)
{
	p += vec3(seed * vec3(-395.23, 705.23, 966.37));
	uvec3 q = floatBitsToUint(p);
	q = ((q >> 16u) ^ q.yzx) * 1111111111u;
	q = ((q >> 16u) ^ q.yzx) * 1111111111u;
	q = ((q >> 16u) ^ q.yzx) * 1111111111u;
	return vec3(q) / float(0xffffffffu);
}

float mwSphere(vec3 p, float seed, float entropyPos, float rad, float entropyR, float sm)
{
	vec3 uvLoc = fract(p) - 0.5;
	vec3 offset;
	float c = 0.0;
	float r = 0.0;
	float l = 0.0;
	float id;
	for (float dz = -1.0; dz <= 1.0; dz++)
	{
		for (float dy = -1.0; dy <= 1.0; dy++)
		{
			for (float dx = -1.0; dx <= 1.0; dx++)
			{
				offset = mwHash33(floor(p) + vec3(dx, dy, dz), seed * 2.0 + 0.15) - 0.5;
				id = mwHash33(offset, 13.17 + seed).x;
				r = (mwHash33(floor(p) + vec3(dx, dy, dz), seed + 13.17) - 0.5).y * entropyR + rad;
				l = length(vec3(dx, dy, dz) + offset * entropyPos - uvLoc);
				c = max(c, smoothstep(r + sm, r - sm, l) * id);
			}
		}
	}
	return c;
}

float mwSphereNoise(vec3 direction, float seed, float softness)
{
	return mwSphere(direction, seed, 1.0, 0.5, 0.5, softness);
}

float mwStars(vec3 dir, float seed, float curve)
{
	vec2 res = mwResolution();
	float f;
	float ff = 0.0;
	float s = 5e2 / MW_CAMERA_ZOOM / res.x;
	float a = 2e-3 * res.x * MW_CAMERA_ZOOM * MW_CAMERA_ZOOM;
	f = mwSphere(dir * 350.0, seed + 254.564, 1.0, 0.45 * s, 0.025, 0.1);
	ff += pow(f, 1000.0 * curve) * 1.5;
	f = mwSphere(dir * 350.0, seed + 26.274, 1.0, 0.25 * s, 0.25, 0.05);
	ff += pow(f, 4.0 * curve) * 0.5;
	f = mwSphere(dir * 450.0, seed + 656.344, 1.0, 0.25 * s, 0.25, 0.3);
	ff += pow(f, 1.0 * curve) * 0.25;
	return ff * a;
}

float mwNebula(vec3 dir, float seed, float steps, float softness, float ratioA, float ratioF)
{
	float aa = 1.0;
	float f = 1.0;
	float n = 0.0;
	float d = 0.0;
	for (float i = 1.0; i <= steps; i++)
	{
		n += pow(mwSphereNoise(dir * f, seed + i * 7.343, softness * 0.9 + 0.1), softness * 0.9 + 0.1) * aa;
		d += aa;
		aa *= ratioA;
		f *= ratioF;
	}
	return n;
}

vec3 mwProjection(vec3 axis, vec3 vector)
{
	return normalize(cross(axis, cross(vector, axis)));
}

vec3 mwProjectionTo(vec3 axis, vec3 orient, vec3 direction)
{
	vec3 x = normalize(cross(orient, axis));
	vec3 y = normalize(cross(x, orient));
	vec3 z = normalize(cross(y, x));
	vec3 d = normalize(direction);
	return vec3(dot(x, d), dot(y, d), dot(z, d));
}

vec3 mwOrkaToSkyDir(vec3 orkaDirection)
{
	return normalize(vec3(orkaDirection.x, orkaDirection.z, orkaDirection.y));
}

vec3 mwGetFar(vec3 direction)
{
	const float iterations = float(MW_ITERATIONS);
	float core = 0.0;
	float periphery = 0.0;
	float neb = 0.0;
	vec3 galaxyAxis = normalize(vec3(0.4, 1.0, -0.2));
	vec3 galaxyDirection = vec3(0.0, 0.0, 1.0);
	vec3 dir = mwProjectionTo(galaxyAxis, vec3(0.0, 0.0, 1.0), direction);
	vec3 nebulaAbs = vec3(0.7, 0.85, 0.95);
	vec3 proj = mwProjection(galaxyAxis, galaxyDirection);
	vec3 col = vec3(0.0);

	col += vec3(mwStars(direction * 0.5, 435.34, 1.0)) * 0.13;

	vec3 coreColor = vec3(0.99, 0.95, 0.9);
	float coreWide = 0.22;
	float coreHeight = 0.7;
	float coreSmooth = 0.7;
	float coreMask =
		smoothstep(0.0, coreSmooth, dot(direction, -galaxyAxis + proj * coreHeight) + coreWide) *
		smoothstep(0.0, coreSmooth, dot(direction, +galaxyAxis + proj * coreHeight) + coreWide);
	if (coreMask > 0.0)
		core = max(0.0, pow(coreMask, 3.0) * 5.0 - 0.5 * mwNebula(dir * 5.0, 291.432 + MW_SEED, iterations, 0.15, 0.7, 1.8));

	vec3 glowColor = vec3(0.99, 0.95, 0.9);
	float glowWide = 0.47;
	float glowHeight = 0.3;
	float glowSmooth = 0.7;
	float glowMask =
		smoothstep(0.0, glowSmooth, dot(direction, -galaxyAxis + proj * glowHeight) + glowWide) *
		smoothstep(0.0, glowSmooth, dot(direction, +galaxyAxis + proj * glowHeight) + glowWide);

	vec3 peripheryColor = vec3(0.1, 0.3, 0.99);
	float peripheryWide = 0.22;
	float peripheryHeight = 0.19;
	float peripherySmooth = 0.9;
	float peripheryMask =
		smoothstep(0.0, peripherySmooth, dot(direction, -galaxyAxis + proj * peripheryHeight) + peripheryWide) *
		smoothstep(0.0, peripherySmooth, dot(direction, +galaxyAxis + proj * peripheryHeight) + peripheryWide);
	peripheryMask = pow(peripheryMask, 0.4);
	if (peripheryMask > 0.0)
		periphery = max(0.0, peripheryMask * 4.0 - 1.0 * mwNebula(dir * 5.0, 583.457 + MW_SEED, iterations, 0.1, 0.6, 2.1));

	float nebulaWide = 0.75;
	float nebulaHeight = 0.13;
	float nebulaSmooth = 0.8;
	float nebulaMask =
		smoothstep(0.0, nebulaSmooth, dot(direction, -galaxyAxis + proj * nebulaHeight) + nebulaWide) *
		smoothstep(0.0, nebulaSmooth, dot(direction, +galaxyAxis + proj * nebulaHeight) + nebulaWide);
	if (nebulaMask > 0.0)
		neb = max(0.0, nebulaMask * 9.0 - 1.5 * mwNebula(dir * 4.0, 1276.12 + MW_SEED, iterations, 0.2, 0.6, 2.1));

	col += pow(core, 3.0) * coreColor * 0.002;
	col += pow(glowMask, 2.0) * glowColor * 0.07;
	col *= exp(-periphery * nebulaAbs * 0.2);
	col += periphery * peripheryColor * 0.05;
	col *= exp(-pow(neb * 0.7, 5.1) * nebulaAbs * 0.0015);
	col += vec3(mwStars(direction * 0.5, 968.148, 2.0)) * 0.46;

	return col;
}

vec3 milkyWaySky(vec3 orkaDirection)
{
	vec3 direction = mwOrkaToSkyDir(orkaDirection);
	vec3 c = mwGetFar(direction) * 7.0;
	return 1.0 - exp(-c * 1.5);
}
