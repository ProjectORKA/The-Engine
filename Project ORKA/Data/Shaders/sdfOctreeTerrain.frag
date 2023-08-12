
//! #include "uniforms.glsl"

layout(location = 0) out vec4 color;
in vec3 pos;


struct Ray{
	vec3 origin;
	vec3 direction;
	float dist;
	vec3 lastPos;
};

void calc(inout Ray r){
	if(r.dist > 10) discard;
	r.lastPos = r.origin + r.direction * r.dist;
}


vec3 sphereSDF(inout Ray r, vec3 pos, float radius){
	vec3 delta = r.lastPos - pos;
	vec3 normal = normalize(delta);
	r.dist += length(delta)-radius;
	calc(r);
	return normal;
};

void main(){
	float distance = distance(cameraPosition.xyz,pos);
	vec3 direction = normalize(pos - cameraPosition.xyz);

	vec3 col = vec3(0);

	Ray r = Ray(cameraPosition.xyz,direction,distance,vec3(0));
	calc(r);
	for(int i = 0; i < 16; i++){
		col = sphereSDF(r,vec3(1.5),0.5);
	}

	color = vec4(col.xyz,1);
};