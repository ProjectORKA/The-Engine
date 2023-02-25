
//! #include "uniforms.glsl"

in vec3 rayOrigin;
in vec3 rayDirection;

layout(location = 0) out vec4 fragmentColor;

float eps = 0.001f;
float maxDist = 1000.0f;
int steps = 512;

float plane(vec3 p){
	return p.z;
}

float sphere( vec3 p, vec3 l, float s )
{
  return length(p-l)-s;
}

float scene(vec3 p){
	return min(
		min(
		sphere(p, vec3(0,3,1), 1),
		sphere(p, vec3(1,3,0.5), 1)
		),
		plane(p)
	);
}

vec3 getNormal(vec3 p)
{
    vec2 e = vec2(eps, 0.);
    vec3 n = scene(p) - vec3(
        scene(p - e.xyy),
        scene(p - e.yxy),
        scene(p - e.yyx)
    );
    return normalize(n);
}



void main(){
	vec3 dir = normalize(rayDirection);
	vec3 orig = cameraPosition.xyz;
	vec3 curr = orig;

	float dist = 2 * eps;

	int i = 0;

	while(dist < maxDist && dist > eps){
		dist = scene(curr);
		curr += dir * dist;
		if(i++ > steps) break;
	}

	if(dist > maxDist) discard;

	vec3 normal = normalize(getNormal(curr));

	vec3 surfacePos = curr;

	vec3 col = vec3(dot(normal, sunDir.xyz));

	i = 0;
	while(true){
		dist = max(eps,scene(curr));
		curr += sunDir.xyz * dist;
		if(i++ > steps || dist > maxDist) break;
	}

//	col *= clamp((100*length(curr - surfacePos)) / target, 0 , 1);

	if(dist < maxDist) col = vec3(0);

	fragmentColor = vec4(col,1);
};