
//! #include "uniforms.glsl"

layout(location = 0) out vec4 color;
in vec4 worldPos;

layout(std430, binding = 0) buffer sphereBuffer
{
    vec4 spheres[];
};

float raySphereIntersect(vec3 r0, vec3 rd, vec4 sphere) {
    // - r0: ray origin
    // - rd: normalized ray direction
    // - s0: sphere center
    // - sr: sphere radius
    // - Returns distance from r0 to first intersecion with sphere,
    //   or -1.0 if no intersection.
    float a = dot(rd, rd);
    vec3 s0_r0 = r0 - sphere.xyz;
    float b = 2.0 * dot(rd, s0_r0);
    float c = dot(s0_r0, s0_r0) - (sphere.w*sphere.w);
    if (b*b - 4.0*a*c < 0.0) {
        return 1e30;
    }
    return (-b - sqrt((b*b) - 4.0*a*c))/(2.0*a);
}


bool intersect(vec3 pos, vec3 dir, out float t0, out float t1, vec4 sphere)
{
    float r = sphere.w/10;
    
    vec3 l = sphere.xyz - pos; 
    float tca = dot(l,dir); 
    if (tca < 0) return false; 
    float d2 = dot(l,l) - tca * tca; 
    if (d2 > r) return false;
    float thc = sqrt(r - d2); 
    t0 = tca - thc; 
    t1 = tca + thc; 
    return true;
}

void main(){
	vec3 pos = cameraPosition.xyz;
	vec3 maxDelta = worldPos.xyz - pos;
	vec3 dir = normalize(maxDelta);
	float dis = length(maxDelta);

    bool hit = false;

	for(int i = 0; i < spheres.length(); i++){
        float t1 = 0;
        float t2 = 0;
        bool result = intersect(pos,dir,t1,t2,spheres[i]);
        hit = hit || result;
        if(result) dis = min(dis,min(t1,t2));
	}
	pos = pos + (dis * dir);
	
    if(!hit) discard;
    
    //if(distance(cameraPosition.xyz,pos)>distance(cameraPosition.xyz,worldPos.xyz)) discard;

	color = vec4(pos,1);
};