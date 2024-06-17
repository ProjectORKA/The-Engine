
//! #include "uniforms.glsl"

layout(location = 0) out vec4 fragmentColor;

in vec2 textureCoordinate;
in vec3 position;

float sdfPlane(vec3 p){
	return p.z - 0.1;
}

void main(){

//	vec3 rayOrigin = chunkOffsetVector.xyz;
//	vec3 rayDirection = normalize(position);
//	vec3 samplePosition = rayOrigin;
//
//
//	int  sampleCount = 0;
//
//	while(sampleCount < 100){
//		float dist = sdfPlane(samplePosition)/2;
//		samplePosition += rayDirection * dist;
//
//		if(dist < 0.001) break;
//
//		sampleCount++;
//	}

	// if(distance(rayOrigin, samplePosition) > 10) discard;

	fragmentColor = vec4(vec3(texelFetch(texture0,ivec2(gl_FragCoord.xy),0).xyz * 100000000),1);
};