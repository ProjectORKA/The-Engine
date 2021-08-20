#version 450
in vec3 rayOrigin;
in vec3 rayDirection;

layout(std140, binding = 0) uniform GlobalUniforms
{
	mat4 mMatrix;
	mat4 vpMatrix;
	
	vec4 worldOffset;
	vec4 cameraVector;		 //its vec3 but treated as vec4 in memory
	vec4 chunkOffsetVector;	 //its vec3 but treated as vec4 in memory
	vec4 customColor;

	float time;
	float custom1;
	float custom2;
	float custom3;

	bool distortion;
};

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;

layout(location = 0) out vec4 fragmentColor;

////////////////////////////////////////////
const vec2 zOffset = vec2(37.0,17.0);
const vec2 wOffset = vec2(59.0,83.0);


/////////////////////////////////////////////


#define NUM_NOISE_OCTAVES 5

highp float  epsilon =  1;
const float pi = 3.14159265359;

// Precision-adjusted variations of https://www.shadertoy.com/view/4djSRW
float hash(float p) { p = fract(p * 0.011); p *= p + 7.5; p *= p + p; return fract(p); }
float hash(vec2 p) {vec3 p3 = fract(vec3(p.xyx) * 0.13); p3 += dot(p3, p3.yzx + 3.333); return fract((p3.x + p3.y) * p3.z); }

float noise(float x) {
    float i = floor(x);
    float f = fract(x);
    float u = f * f * (3.0 - 2.0 * f);
    return mix(hash(i), hash(i + 1.0), u);
}

float noise(vec2 x) {
    vec2 i = floor(x);
    vec2 f = fract(x);
	float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));
    vec2 u = f * f * (3.0 - 2.0 * f);
	return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

vec4 bicubicTexture(sampler2D iChannel0, vec2 uv ){
	vec2 textureResolution = textureSize(iChannel0,0);
	uv = uv*textureResolution + 0.5;
	vec2 iuv = floor( uv );
	vec2 fuv = fract( uv );
	uv = iuv + fuv*fuv*(3.0-2.0*fuv);
	uv = (uv - 0.5)/textureResolution;
	vec4 colB = texture( iChannel0, uv );
	return colB;
}

float fbm(vec2 x) {
	float v = 0.0;
	float a = 0.5;
	vec2 shift = vec2(100);
	// Rotate to reduce axial bias
    mat2 rot = mat2(cos(0.5), sin(0.5), -sin(0.5), cos(0.50));
	for (int i = 0; i < NUM_NOISE_OCTAVES; ++i) {
		v += a * noise(x);
		x = rot * x * 2.0 + shift;
		a *= 0.5;
	}
	return v;
}

float terrain(vec2 p){
	//float f = bicubicTexture(texture1,p.xy).r;
	float f = texture(texture1,p.xy).r;
	//if(epsilon < 0.01) f += f * fbm(p.xy*3000)*0.1;
	return  pow(f,8) / 2500;
}

float sdfTerrain(vec3 p){
	return p.z - terrain(p.xy);
}

float sdfSphere(vec3 p, vec4 sphere){
	return distance(p,sphere.xyz) - sphere.w;
}

float sdfAdd(float a, float b){
	return max(a,b);
}

float sdfPlane(vec3 p){
	return p.z;
}

vec3 sphereCoords(vec3 p){
	return vec3(
	p.x,
	p.y,
	length(p + vec3(rayOrigin.x,rayOrigin.y,0.5)));
}

float sdfPlanet(vec3 p){
	float radius = 3;
	vec3 sphereCoords = sphereCoords(p);
	
	vec4 sphere = vec4(rayOrigin.xy,-0.5,0.5);
	return (sdfSphere(p,sphere) - min( terrain(sphereCoords.xy) , sdfSphere(sphereCoords,vec4(0.5,0.5,0,0.2))))/3.0;
}

float sceneSDF(vec3 p){
	return sdfPlanet(p);
}

float oscillator(float a){
	return (sin(a * time) + 1) / 2;
}

vec3 estimateNormal(vec3 p) {
    return normalize(vec3(
        sceneSDF(vec3(p.x + epsilon, p.y, p.z)) - sceneSDF(vec3(p.x - epsilon, p.y, p.z)),
        sceneSDF(vec3(p.x, p.y + epsilon, p.z)) - sceneSDF(vec3(p.x, p.y - epsilon, p.z)),
        sceneSDF(vec3(p.x, p.y, p.z  + epsilon)) - sceneSDF(vec3(p.x, p.y, p.z - epsilon))
    ));
}
///1.5 +0.000001;

void main(){

	highp vec3 sampleLocation = rayOrigin;

	int sampleCount = 0;

	while(sampleCount < 4096){
		epsilon = sceneSDF(sampleLocation);
		sampleLocation += rayDirection * epsilon;
		if(
		epsilon < 0.000001 ||
		distance(sampleLocation,rayOrigin) > 10) break;
		sampleCount++;
	}

	//epsilon = 0.00001;

	if(distance(rayOrigin,sampleLocation) > 10)discard;

	//vec3 normal = bicubicTexture(texture0,sampleLocation.xy).xyz*2 - 1;
	vec3 normal = estimateNormal(sampleLocation);
	float ao = 1;//-(sampleCount / 512.0);
	float light = dot(normalize(vec3(0.5,0.5,1)),normal) * ao;
//	vec3 color = texture(
//		texture0,
//		vec2(
//		clamp(
//			(distance(
//				sampleLocation.xyz,
//				vec3(rayOrigin.xy,-0.5)
//			) - 0.5) * 2500,
//			0.001,
//			0.9999),
//		0
//		)
//	).xyz;
	vec3 color = bicubicTexture(texture2,sphereCoords(sampleLocation).xy).xyz;
	fragmentColor = vec4(color * light,1);
	
	//fragmentColor = vec4(rayDirection,1);
	
	
	//fragmentColor = vec4(vec3(vec3(epsilon*100000)),1);

};