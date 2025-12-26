out vec3 vWorldPos;

void main()
{
	vec4 world = mMatrix * vec4(vertex, 1.0);
	vWorldPos  = world.xyz;
	gl_Position = pMatrix * vMatrix * world;
}

