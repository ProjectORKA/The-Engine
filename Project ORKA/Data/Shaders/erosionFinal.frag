
//! #include "uniforms.glsl"

layout(location = 0) out vec4 fragmentColor;

#define iResolution vec3(float(framebufferWidth), float(framebufferHeight), 1.0)
#define iCameraPos cameraPosition
#define iCameraDir cameraVector
#define iCameraRight worldOffset
#define iCameraUp customColor
#define iCameraFov customColor.w
#define iChannel0 texture0
#define iChannel1 texture1
#define iChannel2 texture2

#define EROSION_TILES 4.0
#define EROSION_OCTAVES 5
#define EROSION_GAIN 0.5
#define EROSION_LACUNARITY 2.0
#define EROSION_SLOPE_STRENGTH 3.0
#define EROSION_SLOPE_SENSITIVITY 0.3
#define EROSION_BRANCH_STRENGTH 3.0
#define EROSION_STRENGTH 0.04

#define PI 3.14159265358979
#define DEG_TO_RAD (PI / 180.0)
#define saturate(x) clamp(x, 0.0, 1.0)
#define sq(x) (x*x)

#define HEIGHTMAP_SIZE 4096.0
#define HEIGHT_TILES 3.0
#define HEIGHT_OCTAVES 3
#define HEIGHT_AMP 0.25
#define HEIGHT_GAIN 0.1
#define HEIGHT_LACUNARITY 2.0

#define SHADOWS
#define WATER
#define WATER_HEIGHT 0.45
#define DETAIL_TEXTURE

#define M_GROUND 0
#define M_STRATA 1
#define M_WATER  2

#define CLIFF_COLOR    vec3(0.22, 0.2, 0.2)
#define DIRT_COLOR     vec3(0.6, 0.5, 0.4)
#define GRASS_COLOR1   vec3(0.15, 0.3, 0.1)
#define GRASS_COLOR2   vec3(0.4, 0.5, 0.2)
#define SAND_COLOR     vec3(0.8, 0.7, 0.6)

#define WATER_COLOR vec3(0.0, 0.05, 0.1)
#define WATER_SHORE_COLOR vec3(0.0, 0.25, 0.25)

#define SUN_COLOR vec3(1.0, 0.98, 0.95)
#define SUN_LUMINANCE_NITS 1600000000.0   // solar disc luminance approximation
#define STARFIELD_PEAK_NITS 200.0         // provided texture peak in nits

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

vec3 RNM(vec3 n1, vec3 n2)
{
	n1 += vec3(0.0, 0.0, 1.0);
	n2 *= vec3(-1.0, -1.0, 1.0);
	return n1 * dot(n1, n2) / n1.z - n2;
}

// https://iquilezles.org/articles/intersectors
vec2 boxIntersection(in vec3 ro, in vec3 rd, vec3 boxSize, out vec3 outNormal)
{
	vec3 m = 1.0 / rd;
	vec3 n = m * ro;
	vec3 k = abs(m) * boxSize;
	vec3 t1 = -n - k;
	vec3 t2 = -n + k;
	float tN = max(max(t1.x, t1.y), t1.z);
	float tF = min(min(t2.x, t2.y), t2.z);
	if (tN > tF || tF < 0.0) return vec2(-1.0);
	outNormal = -sign(rd) * step(t1.yzx, t1.xyz) * step(t1.zxy, t1.xyz);
	return vec2(tN, tF);
}

vec3 CameraRay(vec2 fragCoord)
{
	// Screen space [-1,1] with Y up
	vec2 ndc = fragCoord / iResolution.xy * 2.0 - 1.0;
	ndc.x *= iResolution.x / iResolution.y; // aspect correction

	float fov = iCameraFov;
	float z = 1.0 / tan(radians(fov * 0.5));

	vec3 dir = normalize(iCameraDir.xyz * z + iCameraRight.xyz * ndc.x + iCameraUp.xyz * ndc.y);
	return dir;
}

vec3 SampleMilkyWay(vec3 rd)
{
	vec2 uv = vec2(atan(rd.z, rd.x) / (2.0 * PI) + 0.5, acos(clamp(rd.y, -1.0, 1.0)) / PI);
	vec3 srgb = texture(iChannel2, uv).rgb;
	vec3 linear = pow(srgb, vec3(2.2));
	return linear * STARFIELD_PEAK_NITS;
}



vec3 SkyBackground(vec3 rd, vec3 sun, bool includeSun)
{
	float sunDot = saturate(dot(rd, sun));
	// Real-life sun apparent radius ~0.27 degrees; outer edge widened slightly for AA
	float sunDisc = smoothstep(cos(radians(0.55)), cos(radians(0.27)), sunDot);

	vec3 base = SampleMilkyWay(rd);
	base += vec3(0.2, 0.25, 0.3); // faint airglow
	if (includeSun) base += SUN_COLOR * (sunDisc * SUN_LUMINANCE_NITS);

	return base;
}

vec3 Tonemap_ACES(vec3 x)
{
	const float a = 2.51;
	const float b = 0.03;
	const float c = 2.43;
	const float d = 0.59;
	const float e = 0.14;
	return (x * (a * x + b)) / (x * (c * x + d) + e);
}

// BRDF -------------------------------------------------------------
float pow5(float x)
{
	float x2 = x * x;
	return x2 * x2 * x;
}

float D_GGX(float linearRoughness, float NoH, const vec3 h)
{
	float oneMinusNoHSquared = 1.0 - NoH * NoH;
	float a = NoH * linearRoughness;
	float k = linearRoughness / (oneMinusNoHSquared + a * a);
	float d = k * k * (1.0 / PI);
	return d;
}

float V_SmithGGXCorrelated(float linearRoughness, float NoV, float NoL)
{
	float a2 = linearRoughness * linearRoughness;
	float GGXV = NoL * sqrt((NoV - a2 * NoV) * NoV + a2);
	float GGXL = NoV * sqrt((NoL - a2 * NoL) * NoL + a2);
	return 0.5 / (GGXV + GGXL);
}

vec3 F_Schlick(const vec3 f0, float VoH)
{
	return f0 + (vec3(1.0) - f0) * pow5(1.0 - VoH);
}

float F_Schlick(float f0, float f90, float VoH)
{
	return f0 + (f90 - f0) * pow5(1.0 - VoH);
}

float Fd_Burley(float linearRoughness, float NoV, float NoL, float LoH)
{
	float f90 = 0.5 + 2.0 * linearRoughness * LoH * LoH;
	float lightScatter = F_Schlick(1.0, f90, NoL);
	float viewScatter = F_Schlick(1.0, f90, NoV);
	return lightScatter * viewScatter * (1.0 / PI);
}

float Fd_Lambert()
{
	return 1.0 / PI;
}

vec3 Shade(vec3 diffuse, vec3 f0, float smoothness, vec3 n, vec3 v, vec3 l, vec3 lc)
{
	vec3 h = normalize(v + l);

	float NoV = abs(dot(n, v)) + 1e-5;
	float NoL = saturate(dot(n, l));
	float NoH = saturate(dot(n, h));
	float LoH = saturate(dot(l, h));

	float roughness = 1.0 - smoothness;
	float linearRoughness = roughness * roughness;
	float D = D_GGX(linearRoughness, NoH, h);
	float V = V_SmithGGXCorrelated(linearRoughness, NoV, NoL);
	vec3 F = F_Schlick(f0, LoH);
	vec3 Fr = (D * V) * F;

	vec3 Fd = diffuse * Fd_Burley(linearRoughness, NoV, NoL, LoH);

	return (Fd + Fr) * lc * NoL;
}

vec4 GetChannel0(vec2 uv)
{
	uv = clamp(uv, vec2(0.001), vec2(0.999));
	// UV is in world space (0-1), texture is HEIGHTMAP_SIZE x HEIGHTMAP_SIZE
	// No scaling needed - world space 0-1 maps directly to texture 0-1
	return texture(iChannel0, uv);
}

vec4 GetChannel1(vec2 uv)
{
	uv = fract(uv);
	// UV is in world space (0-1), texture is HEIGHTMAP_SIZE x HEIGHTMAP_SIZE  
	// No scaling needed - world space 0-1 maps directly to texture 0-1
	return texture(iChannel1, uv);
}

vec4 map(vec3 p, out float erosion)
{
	vec2 uv = p.xz + vec2(0.5) - vec2(0.5) / HEIGHTMAP_SIZE;

	vec4 tex = GetChannel0(uv);
	float height = tex.x;
	vec3 normal = tex.yzz;
	normal.y = sqrt(1.0 - dot(normal.xz, normal.xz));

	erosion = tex.w;

	return vec4(height, normal);
}

float march(vec3 ro, vec3 rd, out vec3 normal, out int material, out float s_t)
{
	s_t = 9999.0;

	vec3 boxNormal;
	vec2 box = boxIntersection(ro, rd, vec3(0.5, 1.0, 0.5), boxNormal);

	if (box.y < 0.0)
	{
		return -1.0;
	}

	float tStart = max(0.0, box.x) + 1e-2;
	float tEnd = box.y - 1e-2;

	material = M_GROUND;

	float stepSize = 0.0;
	float stepScale = 1.0;
	float t = tStart;
	float altitude = 0.0;
	for (int i = 0; i < 32; i++)
	{
		vec3 pos = ro + rd * t;

		float foo;
		vec4 tex = map(pos, foo);
		float h = tex.x;
		normal = tex.yzw;

		altitude = pos.y - h;

		s_t = max(0.0, min(s_t, altitude / t));

		if (altitude < 0.0)
		{
			if (i < 1)
			{
				if (pos.y < 0.35)
				{
					s_t = 9999.0;
					return -1.0;
				}
				normal = boxNormal;
				material = M_STRATA;
				break;
			}
		}

		if (altitude < 0.0)
		{
			stepScale *= 0.5;
			t -= stepSize * stepScale;
		}
		else
		{
			stepSize = abs(altitude) + 1e-2;
			t += stepSize * stepScale;
		}
	}

	if (t > tEnd)
	{
		s_t = 9999.0;
		return -1.0;
	}

#ifdef WATER
	vec3 waterNormal;
	vec2 water = boxIntersection(ro, rd, vec3(0.5, WATER_HEIGHT, 0.5), waterNormal);
	if ((water.y > 0.0 && (water.x < t || t < 0.0)) && material != M_STRATA)
	{
		t = max(0.0, water.x);
		normal = waterNormal;
		material = M_WATER;
	}
	else
#endif

	if (box.y < 0.0)
	{
		return -1.0;
	}

	return t;
}

vec3 GetReflection(vec3 p, vec3 r, vec3 sun, float smoothness)
{
	// Use sky lighting only; direct sun is already accounted for in the BRDF term.
	vec3 refl = SkyBackground(r, sun, false);

	vec3 foo;
	float r_t;
	int r_material;
	march(p, r, foo, r_material, r_t);
	return refl * (1.0 - exp(-r_t * 10.0 * sq(smoothness)));
}

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
	float debugWidth = 0.0;

	vec3 roW = iCameraPos.xyz;
	vec3 rdW = CameraRay(fragCoord.xy - vec2(debugWidth / 2.0, 0.0));
	// Transform from engine space (X right, Y forward, Z up) to shader space (X right, Z forward, Y up)
	vec3 ro = vec3(roW.x, roW.z, roW.y);
	vec3 rd = normalize(vec3(rdW.x, rdW.z, rdW.y));

	vec4 foo;
	vec3 normal;
	int material;
	float t = march(ro, rd, normal, material, foo.w);

	vec3 sunW = normalize(vec3(-1.0, 0.1, 0.25));
	vec3 sun = normalize(vec3(sunW.x, sunW.z, sunW.y));

	vec3 sky = SkyBackground(rd, sun, false);

	vec3 color;

	if (t < 0.0)
	{
		color = sky;
	}
	else
	{
		vec3 pos = ro + rd * t;

		float erosion;
		float diff = pos.y - map(pos, erosion).x;

		vec4 breakupTex = vec4(0.0);

#ifdef DETAIL_TEXTURE
		breakupTex = GetChannel1(pos.xz + 0.5);
		vec3 breakupNormal = breakupTex.zyw;
		if (material != M_STRATA)
		{
			normal = normalize(normal + breakupNormal.xzy * 0.1);
		}
#endif
		float breakup = breakupTex.x;

		vec3 f0 = vec3(0.04);
		float smoothness = 0.0;
		float reflAmount = 0.0;
		float occlusion = 1.0;

		vec3 r = reflect(rd, normal);

		vec3 diffuseColor = vec3(0.5);
		if (material == M_GROUND)
		{
			occlusion = sq(saturate(erosion + 0.5));

			diffuseColor = CLIFF_COLOR * smoothstep(0.4, 0.52, pos.y);
			diffuseColor = mix(diffuseColor, DIRT_COLOR, smoothstep(0.3, 0.0, occlusion + breakup * 1.0));

			vec3 grassMix = mix(GRASS_COLOR1, GRASS_COLOR2, smoothstep(0.4, 0.6, pos.y - erosion * 0.05 + breakup * 0.3));
			diffuseColor = mix(diffuseColor, grassMix, smoothstep(WATER_HEIGHT + 0.05, WATER_HEIGHT + 0.02, pos.y - breakup * 0.02) * smoothstep(0.8, 1.0, normal.y + breakup * 0.1));

			diffuseColor = mix(diffuseColor, vec3(1.0), smoothstep(0.53, 0.6, pos.y + breakup * 0.1));
#ifdef WATER
			diffuseColor = mix(diffuseColor, SAND_COLOR, smoothstep(WATER_HEIGHT + 0.005, WATER_HEIGHT, pos.y + breakup * 0.01));
#endif
			diffuseColor *= 1.0 + breakup * 0.5;
		}
		else if (material == M_STRATA)
		{
			vec3 strata = smoothstep(0.0, 1.0, cos(diff * vec3(130.0, 190.0, 250.0)));
			diffuseColor = vec3(0.3);
			diffuseColor = mix(diffuseColor, vec3(0.50), strata.x);
			diffuseColor = mix(diffuseColor, vec3(0.55), strata.y);
			diffuseColor = mix(diffuseColor, vec3(0.60), strata.z);

			diffuseColor *= exp(diff * 10.0) * vec3(1.0, 0.9, 0.7);
		}
		else if (material == M_WATER)
		{
			float shore = normal.y > 1e-2 ? exp(-diff * 60.0) : 0.0;
			float foam = normal.y > 1e-2 ? smoothstep(0.005, 0.0, diff + breakup * 0.005) : 0.0;

			diffuseColor = mix(WATER_COLOR, WATER_SHORE_COLOR, shore);

			diffuseColor = mix(diffuseColor, vec3(1.0), foam);

			// Disable sun specular highlights on water
			smoothness = 0.0;
			f0 = vec3(0.0);
		}

		float shadow = 1.0;

#ifdef SHADOWS
		if (material != M_STRATA)
		{
			float s_t;
			int s_material;
			march(pos + vec3(0.0, 1.0, 0.0) * 1e-4, sun, foo.xyz, s_material, s_t);
			shadow = 1.0 - exp(-s_t * 20.0);
		}
#endif
		color = Shade(diffuseColor, f0, smoothness, normal, -rd, sun, SUN_COLOR * SUN_LUMINANCE_NITS * shadow);
		if (material != M_WATER)
		{
			color += GetReflection(pos, r, sun, smoothness) * F_Schlick(f0, dot(-rd, normal));
		}
	}

	fragColor = vec4(color, 1.0);
}

void main()
{
	mainImage(fragmentColor, gl_FragCoord.xy);
}

