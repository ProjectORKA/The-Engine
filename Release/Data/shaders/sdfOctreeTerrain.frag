
//! #include "uniforms.glsl"

layout(location = 0) out vec4 color;
in vec3 pos;

struct Ray{
	vec3 origin;
	vec3 direction;
	float distance;
	vec3 lastPos;
	void calc(){
		if(distance > 10) discard;
		lastPos = origin + direction * distance;
	}
};

vec3 sphereSDF(inout Ray r, vec3 pos, float radius){
	vec3 delta = r.lastPos - pos;
	vec3 normal = normalize(delta);
	r.distance += length(delta)-radius;
	r.calc();
	return normal;
};

void main(){
	float distance = distance(cameraPosition.xyz,pos);
	vec3 direction = normalize(pos - cameraPosition.xyz);

	vec3 col = vec3(0);

	Ray r = Ray(cameraPosition.xyz,direction,distance,vec3(0));
	r.calc();
	for(int i = 0; i < 16; i++){
		col = sphereSDF(r,vec3(1.5),0.5);
	}

	color = vec4(col.xyz,1);
};