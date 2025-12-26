layout(binding = 16) uniform sampler3D voxelTex;

in vec3 vWorldPos;

layout(location = 0) out vec4 outColor;

vec2 intersectAabb(vec3 ro, vec3 rd, vec3 bmin, vec3 bmax)
{
	vec3 invDir = 1.0 / rd;
	vec3 t0 = (bmin - ro) * invDir;
	vec3 t1 = (bmax - ro) * invDir;
	vec3 tMin = min(t0, t1);
	vec3 tMax = max(t0, t1);
	float tEnter = max(max(tMin.x, tMin.y), tMin.z);
	float tExit  = min(min(tMax.x, tMax.y), tMax.z);
	return vec2(tEnter, tExit);
}

vec3 estimateNormal(vec3 uvw, vec3 texelSize, vec3 fallbackDir)
{
	float dx = texture(voxelTex, uvw + vec3(texelSize.x, 0.0, 0.0)).r - texture(voxelTex, uvw - vec3(texelSize.x, 0.0, 0.0)).r;
	float dy = texture(voxelTex, uvw + vec3(0.0, texelSize.y, 0.0)).r - texture(voxelTex, uvw - vec3(0.0, texelSize.y, 0.0)).r;
	float dz = texture(voxelTex, uvw + vec3(0.0, 0.0, texelSize.z)).r - texture(voxelTex, uvw - vec3(0.0, 0.0, texelSize.z)).r;
	vec3 n = vec3(dx, dy, dz);
	float lenSq = dot(n, n);
	if (lenSq < 1e-5) return normalize(-fallbackDir);
	return normalize(n);
}

void main()
{
	vec3 chunkMin  = worldOffset.xyz;
	vec3 chunkSize = customColor.xyz;
	vec3 chunkMax  = chunkMin + chunkSize;

	vec3 rayOrigin = cameraPosition.xyz;
	vec3 rawDir    = vWorldPos - rayOrigin;
	float dirLenSq = dot(rawDir, rawDir);
	if (dirLenSq < 1e-8) discard;
	vec3 rayDir = normalize(rawDir);

	// Axis-aligned box intersection (explicitly keep tMin/tMax for entry face determination)
	vec3 invDir = 1.0 / rayDir;
	vec3 t0 = (chunkMin - rayOrigin) * invDir;
	vec3 t1 = (chunkMax - rayOrigin) * invDir;
	vec3 tMin3 = min(t0, t1);
	vec3 tMax3 = max(t0, t1);
	float tEnter = max(max(tMin3.x, tMin3.y), tMin3.z);
	float tExit  = min(min(tMax3.x, tMax3.y), tMax3.z);
	if (tExit <= tEnter) discard;
	tEnter = max(tEnter, 0.0);

	// Set up 3D DDA traversal through the voxel grid
	vec3 originLocal = rayOrigin - chunkMin;
	vec3 startLocal  = originLocal + rayDir * tEnter;
	float tCurrent   = tEnter;

	const float epsilon = 1e-4;
	startLocal += rayDir * epsilon;
	tCurrent   += epsilon;

	ivec3 dims = ivec3(chunkSize + vec3(0.5));
	ivec3 voxel = ivec3(clamp(floor(startLocal), vec3(0.0), vec3(dims - ivec3(1))));
	ivec3 step = ivec3(sign(rayDir));

	vec3 tDelta = vec3(1e30);
	tDelta.x = rayDir.x != 0.0 ? abs(1.0 / rayDir.x) : 1e30;
	tDelta.y = rayDir.y != 0.0 ? abs(1.0 / rayDir.y) : 1e30;
	tDelta.z = rayDir.z != 0.0 ? abs(1.0 / rayDir.z) : 1e30;

	vec3 tMax = vec3(1e30);
	if (rayDir.x != 0.0)
	{
		float boundary = float(voxel.x) + (step.x > 0 ? 1.0 : 0.0);
		tMax.x = tCurrent + (boundary - startLocal.x) / rayDir.x;
	}
	if (rayDir.y != 0.0)
	{
		float boundary = float(voxel.y) + (step.y > 0 ? 1.0 : 0.0);
		tMax.y = tCurrent + (boundary - startLocal.y) / rayDir.y;
	}
	if (rayDir.z != 0.0)
	{
		float boundary = float(voxel.z) + (step.z > 0 ? 1.0 : 0.0);
		tMax.z = tCurrent + (boundary - startLocal.z) / rayDir.z;
	}

	// Track which face we entered through for crisp normals
	vec3 entryNormal = vec3(0.0);
	if (tEnter > 0.0)
	{
		float maxTMin = tMin3.x;
		int axis = 0;
		if (tMin3.y > maxTMin) { maxTMin = tMin3.y; axis = 1; }
		if (tMin3.z > maxTMin) { axis = 2; }
		if (axis == 0) entryNormal = vec3(rayDir.x > 0.0 ? -1.0 : 1.0, 0.0, 0.0);
		else if (axis == 1) entryNormal = vec3(0.0, rayDir.y > 0.0 ? -1.0 : 1.0, 0.0);
		else entryNormal = vec3(0.0, 0.0, rayDir.z > 0.0 ? -1.0 : 1.0);
	}
	else
	{
		vec3 absDir = abs(rayDir);
		if (absDir.x > absDir.y && absDir.x > absDir.z) entryNormal = vec3(-sign(rayDir.x), 0.0, 0.0);
		else if (absDir.y > absDir.z) entryNormal = vec3(0.0, -sign(rayDir.y), 0.0);
		else entryNormal = vec3(0.0, 0.0, -sign(rayDir.z));
	}

	vec3 lastNormal = entryNormal;
	float n = chunkSize.x; // chunk is cubic; use edge length as n
	int maxSteps = int(max(1.0, 3.0 * (n * n) - 3.0 * n + 1.0));
	vec3 texelSize = vec3(1.0) / chunkSize;

	for (int i = 0; i < maxSteps; ++i)
	{
		if (any(lessThan(voxel, ivec3(0))) || any(greaterThanEqual(voxel, dims)) || tCurrent > tExit) break;

		vec3 uvw = (vec3(voxel) + 0.5) / chunkSize;
		float density = texture(voxelTex, uvw).r;
		if (density > 0.5)
		{
			vec3 hitLocal = originLocal + rayDir * tCurrent;
			vec3 rel = hitLocal - vec3(voxel);
			vec2 faceUv;
			if (abs(lastNormal.x) > 0.5)      faceUv = rel.yz;
			else if (abs(lastNormal.y) > 0.5) faceUv = rel.xz;
			else                              faceUv = rel.xy;

			vec3 normal = normalize(lastNormal);
			if (dot(normal, normal) < 1e-5) normal = estimateNormal(uvw, texelSize, rayDir);

			vec3 lightDir = normalize(vec3(0.5, 0.7, 1.0));
			float diffuse = max(dot(normal, lightDir), 0.0);
			float ambient = 0.25;

			vec2 centeredUv = abs(faceUv - 0.5);
			float faceFactor = 0.9 + 0.1 * (1.0 - max(centeredUv.x, centeredUv.y) * 2.0);

			vec3 baseColor = mix(vec3(0.12, 0.35, 0.8), vec3(0.85, 0.92, 1.0), clamp(density, 0.0, 1.0));
			outColor = vec4(baseColor * faceFactor * (ambient + diffuse), 1.0);
			return;
		}

		// advance to the next voxel boundary along the smallest tMax
		if (tMax.x < tMax.y)
		{
			if (tMax.x < tMax.z)
			{
				tCurrent = tMax.x;
				tMax.x += tDelta.x;
				voxel.x += step.x;
				lastNormal = vec3(-step.x, 0.0, 0.0);
			}
			else
			{
				tCurrent = tMax.z;
				tMax.z += tDelta.z;
				voxel.z += step.z;
				lastNormal = vec3(0.0, 0.0, -step.z);
			}
		}
		else
		{
			if (tMax.y < tMax.z)
			{
				tCurrent = tMax.y;
				tMax.y += tDelta.y;
				voxel.y += step.y;
				lastNormal = vec3(0.0, -step.y, 0.0);
			}
			else
			{
				tCurrent = tMax.z;
				tMax.z += tDelta.z;
				voxel.z += step.z;
				lastNormal = vec3(0.0, 0.0, -step.z);
			}
		}
	}

	discard;
}

