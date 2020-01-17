#version 330 core

layout(location = 0) in vec3 vertex;
//layout(location = 1) in vec3 positions;
layout(location = 2) in vec2 uvs;

out vec4 vertexColor; 

uniform int distortion;

uniform float time;

uniform vec4 worldOffset;
uniform vec3 chunkOffsetVector;
uniform mat4 mMatrix;
uniform mat4 vpMatrix;

void main() {
	
	vec3 chunkPosition = (mMatrix * vec4(vertex, 1)).xyz;
	vec3 cameraRelativePosition = chunkOffsetVector + chunkPosition;

	if(distortion > 0){
		if(worldOffset.w < 13){
			float r = pow(2,worldOffset.w-2);
			
			float h = chunkPosition.z + 0.5;
			
			vec2 dir = normalize(cameraRelativePosition.xy);
			float dist = length(vec2(cameraRelativePosition.xy));
			
			cameraRelativePosition.xy = dir * sin(dist/r) * (r + h);
			cameraRelativePosition.z = cos(dist/r) * (r + h) - r + chunkOffsetVector.z - 0.5;
		}
	}

	gl_Position  = vpMatrix * vec4(cameraRelativePosition.xyz,1);
	
	vec3 levelColor = vec3(mod(worldOffset.w / 5.0f,0.9) + 0.1);
	vec3 worldColor = ((vertex) + vec3(0.5,0.5,0.0) + worldOffset.xyz)/vec3(pow(2,worldOffset.w),pow(2,worldOffset.w),pow(2,worldOffset.w-1));
	vertexColor = vec4(vec3((worldColor+vec3(uvs,1))/2),1.0f);
};