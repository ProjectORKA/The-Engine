
//! #include "uniforms.glsl"

layout(location = 0) out vec4 fragmentColor;

#define iResolution vec3(float(framebufferWidth), float(framebufferHeight), 1.0)
#define iCameraPos cameraPosition
#define iCameraDir cameraVector
#define iCameraRight worldOffset
#define iCameraUp customColor
#define iCameraFov customColor.w

#define PI 3.141592653589793
#define EPSILON 1e-4
#define MAX_FLOAT 1e30
#define MAX_BOUNCES 8
#define NUM_BOXES 10
#define NUM_SPHERES 3

const int MAT_DIFFUSE    = 0;
const int MAT_METAL      = 1;
const int MAT_DIELECTRIC = 2;
const int MAT_EMISSIVE   = 3;

struct Material
{
	int   type;
	vec3  albedo;
	vec3  emission;
	float roughness;
	float metallic;
	float ior;
};

struct HitRecord
{
	bool  hit;
	float t;
	vec3  point;
	vec3  normal;
	int   materialId;
	bool  frontFace;
};

struct Ray
{
	vec3 origin;
	vec3 direction;
};

Material getMaterial(int id)
{
	if (id == 1)
		return Material(MAT_DIFFUSE, vec3(0.65, 0.05, 0.05), vec3(0.0), 1.0, 0.0, 1.5);
	if (id == 2)
		return Material(MAT_DIFFUSE, vec3(0.12, 0.45, 0.15), vec3(0.0), 1.0, 0.0, 1.5);
	if (id == 3)
		return Material(MAT_EMISSIVE, vec3(0.0), vec3(12.0), 0.0, 0.0, 1.5);
	if (id == 4)
		return Material(MAT_DIELECTRIC, vec3(1.0), vec3(0.0), 0.02, 0.0, 1.5);
	if (id == 5)
		return Material(MAT_METAL, vec3(0.85, 0.65, 0.15), vec3(0.0), 0.15, 1.0, 1.5);
	if (id == 6)
		return Material(MAT_METAL, vec3(0.85, 0.35, 0.20), vec3(0.0), 0.25, 1.0, 1.5);
	return Material(MAT_DIFFUSE, vec3(0.73), vec3(0.0), 1.0, 0.0, 1.5);
}

void getBox(int index, out vec3 boxMin, out vec3 boxMax, out int materialId)
{
	const float wall = 0.01;
	if (index == 0) { boxMin = vec3(-1.0, 0.0, 0.0); boxMax = vec3(1.0, 2.0, wall); materialId = 0; return; }
	if (index == 1) { boxMin = vec3(-1.0, 0.0, 2.0 - wall); boxMax = vec3(-0.30, 2.0, 2.0); materialId = 0; return; }
	if (index == 2) { boxMin = vec3(0.30, 0.0, 2.0 - wall); boxMax = vec3(1.0, 2.0, 2.0); materialId = 0; return; }
	if (index == 3) { boxMin = vec3(-0.30, 0.0, 2.0 - wall); boxMax = vec3(0.30, 0.78, 2.0); materialId = 0; return; }
	if (index == 4) { boxMin = vec3(-0.30, 1.22, 2.0 - wall); boxMax = vec3(0.30, 2.0, 2.0); materialId = 0; return; }
	if (index == 5) { boxMin = vec3(-1.0, 2.0 - wall, 0.0); boxMax = vec3(1.0, 2.0, 2.0); materialId = 0; return; }
	if (index == 6) { boxMin = vec3(-1.0, 0.0, 0.0); boxMax = vec3(-1.0 + wall, 2.0, 2.0); materialId = 1; return; }
	if (index == 7) { boxMin = vec3(1.0 - wall, 0.0, 0.0); boxMax = vec3(1.0, 2.0, 2.0); materialId = 2; return; }
	if (index == 8) { boxMin = vec3(-0.28, 0.80, 1.97); boxMax = vec3(0.28, 1.20, 2.0); materialId = 3; return; }
	boxMin = vec3(0.15, 0.55, 0.0); boxMax = vec3(0.85, 1.35, 0.95); materialId = 0;
}

void getSphere(int index, out vec3 center, out float radius, out int materialId)
{
	if (index == 0) { center = vec3(-0.20, 1.00, 0.70); radius = 0.15; materialId = 4; return; }
	if (index == 1) { center = vec3(0.65, 1.15, 0.65); radius = 0.10; materialId = 5; return; }
	center = vec3(-0.50, 0.275, 0.375); radius = 0.275; materialId = 6;
}

uint hashU32(uint x)
{
	x ^= x >> 16;
	x *= 0x7feb352du;
	x ^= x >> 15;
	x *= 0x846ca68bu;
	x ^= x >> 16;
	return x;
}

float rand01(inout uint state)
{
	state = hashU32(state);
	return float(state) / 4294967295.0;
}

vec2 rand22(inout uint state)
{
	return vec2(rand01(state), rand01(state));
}

vec3 rand33(inout uint state)
{
	return vec3(rand01(state), rand01(state), rand01(state));
}

uint pixelRandomSeed(vec2 fragCoord)
{
	uint x = uint(fragCoord.x);
	uint y = uint(fragCoord.y);
	return hashU32(hashU32(x * 747796405u) ^ hashU32(y * 2891336453u));
}

uint frameRandomSeed()
{
	return hashU32(hashU32(uint(f1)) ^ hashU32(uint(f2) * 1597334677u));
}

vec3 randomUnitVector(inout uint state)
{
	for (int i = 0; i < 8; ++i)
	{
		vec3 p = rand33(state) * 2.0 - 1.0;
		if (dot(p, p) < 1.0) return p;
	}
	return vec3(0.0, 0.0, 1.0);
}

bool hitBox(vec3 boxMin, vec3 boxMax, Ray r, float tMin, float tMax, inout HitRecord rec, int materialId)
{
	float t0 = tMin;
	float t1 = tMax;

	for (int axis = 0; axis < 3; ++axis)
	{
		float originComponent    = r.origin[axis];
		float directionComponent = r.direction[axis];

		if (abs(directionComponent) < EPSILON)
		{
			if (originComponent < boxMin[axis] || originComponent > boxMax[axis]) return false;
			continue;
		}

		float invDirection = 1.0 / directionComponent;
		float tNear          = (boxMin[axis] - originComponent) * invDirection;
		float tFar           = (boxMax[axis] - originComponent) * invDirection;
		if (invDirection < 0.0) { float tmp = tNear; tNear = tFar; tFar = tmp; }

		t0 = max(t0, tNear);
		t1 = min(t1, tFar);
		if (t1 <= t0) return false;
	}

	float hitT = t0;
	if (hitT <= tMin || hitT >= tMax) return false;

	vec3 point  = r.origin + r.direction * hitT;
	vec3 center = (boxMin + boxMax) * 0.5;
	vec3 extent = (boxMax - boxMin) * 0.5;
	vec3 local  = (point - center) / max(extent, vec3(EPSILON));

	vec3 normal = vec3(0.0);
	float absX = abs(local.x);
	float absY = abs(local.y);
	float absZ = abs(local.z);
	if (absX > absY && absX > absZ) normal = vec3(sign(local.x), 0.0, 0.0);
	else if (absY > absZ) normal = vec3(0.0, sign(local.y), 0.0);
	else normal = vec3(0.0, 0.0, sign(local.z));

	rec.t          = hitT;
	rec.point      = point;
	rec.materialId = materialId;
	rec.frontFace  = dot(r.direction, normal) < 0.0;
	rec.normal     = rec.frontFace ? normal : -normal;
	rec.hit        = true;
	return true;
}

bool hitSphere(vec3 center, float radius, Ray r, float tMin, float tMax, inout HitRecord rec, int materialId)
{
	vec3 oc           = r.origin - center;
	float a           = dot(r.direction, r.direction);
	float halfB       = dot(oc, r.direction);
	float c           = dot(oc, oc) - radius * radius;
	float discriminant = halfB * halfB - a * c;
	if (discriminant < 0.0) return false;

	float sqrtDisc = sqrt(discriminant);
	float root     = (-halfB - sqrtDisc) / a;
	if (root <= tMin || root >= tMax)
	{
		root = (-halfB + sqrtDisc) / a;
		if (root <= tMin || root >= tMax) return false;
	}

	vec3 outwardNormal = (r.origin + root * r.direction - center) / radius;
	rec.t          = root;
	rec.point      = r.origin + root * r.direction;
	rec.materialId = materialId;
	rec.frontFace  = dot(r.direction, outwardNormal) < 0.0;
	rec.normal     = rec.frontFace ? outwardNormal : -outwardNormal;
	rec.hit        = true;
	return true;
}

bool hitScene(Ray r, float tMin, float tMax, inout HitRecord rec)
{
	HitRecord temp;
	bool      hitAnything = false;
	float     closest     = tMax;

	for (int i = 0; i < NUM_BOXES; ++i)
	{
		vec3 boxMin, boxMax;
		int  materialId;
		getBox(i, boxMin, boxMax, materialId);
		temp.hit = false;
		if (hitBox(boxMin, boxMax, r, tMin, closest, temp, materialId))
		{
			hitAnything = true;
			closest     = temp.t;
			rec         = temp;
		}
	}

	for (int i = 0; i < NUM_SPHERES; ++i)
	{
		vec3  center;
		float radius;
		int   materialId;
		getSphere(i, center, radius, materialId);
		temp.hit = false;
		if (hitSphere(center, radius, r, tMin, closest, temp, materialId))
		{
			hitAnything = true;
			closest     = temp.t;
			rec         = temp;
		}
	}

	return hitAnything;
}

bool shadowHit(Ray r, float tMin, float tMax)
{
	HitRecord rec;
	return hitScene(r, tMin, tMax, rec);
}

float schlick(float cosine, float refIdx)
{
	float r0 = (1.0 - refIdx) / (1.0 + refIdx);
	r0       = r0 * r0;
	return r0 + (1.0 - r0) * pow(1.0 - cosine, 5.0);
}

vec3 reflectVec(vec3 v, vec3 n)
{
	return v - 2.0 * dot(v, n) * n;
}

bool refractVec(vec3 v, vec3 n, float eta, out vec3 refracted)
{
	float cosTheta = min(-dot(v, n), 1.0);
	float sinTheta = sqrt(max(0.0, 1.0 - cosTheta * cosTheta));
	float sinT     = eta * sinTheta;
	if (sinT > 1.0) return false;
	refracted = eta * v + (eta * cosTheta - sqrt(max(0.0, 1.0 - sinT * sinT))) * n;
	return true;
}

vec3 cosineSampleHemisphere(vec3 normal, inout uint state)
{
	vec3 unit = randomUnitVector(state);
	if (dot(unit, normal) > 0.0) return normalize(normal + unit);
	return normalize(normal - unit);
}

vec3 ggxSampleReflection(vec3 normal, vec3 viewDir, float roughness, inout uint state)
{
	float alpha    = max(roughness * roughness, 0.001);
	vec2  xi       = rand22(state);
	float phi      = 2.0 * PI * xi.x;
	float cosTheta = sqrt((1.0 - xi.y) / (1.0 + (alpha * alpha - 1.0) * xi.y));
	float sinTheta = sqrt(max(0.0, 1.0 - cosTheta * cosTheta));

	vec3 localH    = vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
	vec3 up        = abs(normal.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
	vec3 tangent   = normalize(cross(up, normal));
	vec3 bitangent = cross(normal, tangent);
	vec3 halfVec   = normalize(tangent * localH.x + bitangent * localH.y + normal * localH.z);

	return normalize(reflectVec(-viewDir, halfVec));
}

vec3 sampleAreaLight(vec3 point, inout uint state)
{
	vec3 lightMin    = vec3(-0.28, 0.80, 1.97);
	vec3 lightMax    = vec3(0.28, 1.20, 2.0);
	vec3 samplePoint = mix(lightMin, lightMax, rand33(state));
	return samplePoint;
}

vec3 directLighting(vec3 point, vec3 normal, vec3 throughput, Material mat, inout uint state)
{
	if (mat.type != MAT_DIFFUSE) return vec3(0.0);

	vec3 lightPoint = sampleAreaLight(point, state);
	vec3 toLight    = lightPoint - point;
	float distSq    = dot(toLight, toLight);
	float dist      = sqrt(distSq);
	vec3 lightDir   = toLight / dist;

	if (dot(normal, lightDir) <= 0.0) return vec3(0.0);

	Ray shadowRay;
	shadowRay.origin    = point + normal * 0.001;
	shadowRay.direction = lightDir;
	if (shadowHit(shadowRay, 0.001, dist - 0.001)) return vec3(0.0);

	const vec3 lightEmission = vec3(12.0);
	const float lightArea    = (0.56 * 0.40) * 2.0;
	float cosTheta           = dot(normal, lightDir);
	float pdf                = distSq / (lightArea * max(cosTheta, 1e-4));
	return throughput * lightEmission * cosTheta / max(pdf, 1e-4);
}

bool scatter(Material mat, Ray rIn, HitRecord rec, inout vec3 attenuation, inout Ray scattered, inout uint state)
{
	if (mat.type == MAT_EMISSIVE) return false;

	vec3 normal = rec.normal;

	if (mat.type == MAT_DIFFUSE)
	{
		vec3 scatterDir = cosineSampleHemisphere(normal, state);
		scattered.origin    = rec.point + normal * 0.001;
		scattered.direction = scatterDir;
		attenuation         = mat.albedo;
		return true;
	}

	if (mat.type == MAT_METAL)
	{
		vec3 reflected = ggxSampleReflection(normal, normalize(rIn.direction), mat.roughness, state);
		scattered.origin    = rec.point + normal * 0.001;
		scattered.direction = reflected;
		attenuation         = mat.albedo;
		return dot(scattered.direction, normal) > 0.0;
	}

	attenuation        = vec3(1.0);
	scattered.origin   = rec.point;
	float refRatio     = rec.frontFace ? 1.0 / mat.ior : mat.ior;
	vec3 unitDirection = normalize(rIn.direction);
	float cosTheta     = min(-dot(unitDirection, normal), 1.0);
	float sinTheta     = sqrt(max(0.0, 1.0 - cosTheta * cosTheta));
	bool cannotRefract = refRatio * sinTheta > 1.0;

	if (cannotRefract || schlick(cosTheta, refRatio) > rand01(state))
		scattered.direction = reflectVec(unitDirection, normal);
	else
	{
		vec3 refracted;
		if (!refractVec(unitDirection, normal, refRatio, refracted)) return false;
		scattered.direction = refracted;
	}

	scattered.origin = rec.frontFace ? rec.point - normal * 0.001 : rec.point + normal * 0.001;
	return true;
}

vec3 emitted(Material mat, HitRecord rec, Ray rIn)
{
	if (mat.type != MAT_EMISSIVE) return vec3(0.0);
	if (dot(rIn.direction, rec.normal) < 0.0) return mat.emission;
	return vec3(0.0);
}

vec3 pathTrace(Ray r, inout uint state)
{
	vec3 throughput = vec3(1.0);
	vec3 radiance   = vec3(0.0);

	for (int bounce = 0; bounce < MAX_BOUNCES; ++bounce)
	{
		HitRecord rec;
		rec.hit = false;
		if (!hitScene(r, 0.001, MAX_FLOAT, rec)) break;

		Material mat = getMaterial(rec.materialId);
		radiance += throughput * emitted(mat, rec, r);

		if (mat.type == MAT_EMISSIVE) break;

		if (bounce < 4)
			radiance += directLighting(rec.point, rec.normal, throughput, mat, state);

		if (bounce >= 3)
		{
			float survival = max(max(throughput.x, throughput.y), throughput.z);
			if (rand01(state) > survival) break;
			throughput /= max(survival, 1e-4);
		}

		vec3 attenuation;
		Ray  scattered;
		if (!scatter(mat, r, rec, attenuation, scattered, state)) break;

		throughput *= attenuation;
		r = scattered;
	}

	return radiance;
}

vec3 cameraRay(vec2 fragCoord, uint jitterSeed)
{
	uint jitterState = hashU32(jitterSeed ^ 0xA511E9B3u);
	vec2 jitter      = rand22(jitterState) - 0.5;
	vec2 ndc         = (fragCoord + jitter) / iResolution.xy * 2.0 - 1.0;
	ndc.x           *= iResolution.x / iResolution.y;

	float fovTan = tan(radians(iCameraFov * 0.5));
	return normalize(iCameraDir.xyz + iCameraRight.xyz * ndc.x * fovTan + iCameraUp.xyz * ndc.y * fovTan);
}

void main()
{
	vec2 uv = gl_FragCoord.xy / iResolution.xy;

	uint state = pixelRandomSeed(gl_FragCoord.xy) ^ frameRandomSeed();

	vec3 ro = iCameraPos.xyz;
	vec3 rd = cameraRay(gl_FragCoord.xy, state);

	Ray r;
	r.origin    = ro;
	r.direction = rd;

	vec3 sampleColor = pathTrace(r, state);

	vec3 accum = sampleColor;
	if (customInt1 == 0u)
		accum += texture(texture0, uv).rgb;

	fragmentColor = vec4(accum, 1.0);
}
