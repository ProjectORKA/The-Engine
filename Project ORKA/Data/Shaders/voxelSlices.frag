layout(binding = 16) uniform sampler3D voxelTex;

in vec2 vUv;
in vec3 vWorldPos;
in vec3 vFaceNormal;
in vec3 vObjNormal;
in vec3 vObjectPos;

layout(location = 0) out vec4 outColor;

void main()
{
	vec3 toCamera = normalize(cameraPosition.xyz - vWorldPos);
	float flip = dot(toCamera, vFaceNormal) < 0.0 ? -1.0 : 1.0;

	vec3 objNormal  = vObjNormal;
	if(!gl_FrontFacing){
		objNormal = -objNormal;
	}

	vec3 chunkMin  = worldOffset.xyz;
	vec3 chunkSize = customColor.xyz;

	// Offset sampling half a voxel back along the camera-facing normal (object-space)
	vec3 faceOffsetObj = -objNormal * 0.5;
	vec3 uvw = (vObjectPos + faceOffsetObj - chunkMin) / chunkSize;
	// if (any(lessThan(uvw, vec3(0.0))) || any(greaterThan(uvw, vec3(1.0)))) discard;

	float density = texture(voxelTex, uvw).r;
	if (density < 0.5) discard;

	vec3 lightDir = normalize(vec3(0.5, 0.7, 1.0));
	float diffuse = max(dot(objNormal, lightDir), 0.0);
	float ambient = 0.25;

	vec3 baseColor = mix(vec3(0.12, 0.35, 0.8), vec3(0.85, 0.92, 1.0), clamp(density, 0.0, 1.0));
	outColor = vec4(baseColor * (ambient + diffuse), 1.0);
    // outColor = vec4(objNormal, 1.0);
}

