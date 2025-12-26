out vec2 vUv;
out vec3 vWorldPos;
out vec3 vObjectPos;
out vec3 vFaceNormal;
out vec3 vObjNormal;

void main()
{
	vec4 world = mMatrix * vec4(vertex, 1.0);
	vWorldPos  = world.xyz;
	vObjectPos = vertex;
	vUv        = uvs;

	vec3 nObj   = normals;
	vec3 nWorld = normalize((mMatrix * vec4(normals, 0.0)).xyz);
	vec3 toCam  = normalize(cameraPosition.xyz - world.xyz);

	// gl_FrontFacing is only available in the fragment stage; pass raw normals and flip in the fragment shader.
	vFaceNormal = nWorld;
	vObjNormal  = normals;
	gl_Position = pMatrix * vMatrix * world;
}

