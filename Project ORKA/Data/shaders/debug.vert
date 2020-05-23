#version 330 core

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 uvs;
//layout(location = 1) in vec3 positions;

out vec4 vertexColor;
out vec2 textureCoordinate; 

uniform float time;
uniform mat4 mMatrix;
uniform mat4 vpMatrix;
uniform int distortion;
uniform vec4 worldOffset;
uniform float cameraHeight;
uniform vec3 chunkOffsetVector;

void main() {

	vec3 positionInChunk = (mMatrix * vec4(vertex, 1)).xyz;
	vec3 cameraRelativePosition = chunkOffsetVector + positionInChunk;

	//planet curvature

	if(distortion > 0){
		if(worldOffset.w < 13){

			float radius = pow(2,worldOffset.w-2);
			
			float height = cameraRelativePosition.z;

			float dist = length(vec2(cameraRelativePosition.xy));
			
			if(cameraRelativePosition.xy != vec2(0)){
				vec2 direction = normalize(cameraRelativePosition.xy);
				cameraRelativePosition.xy = direction * (height + radius + cameraHeight) * sin(dist/radius);
			}
			cameraRelativePosition.z = (height + radius + cameraHeight) * cos(dist/radius)-radius-cameraHeight;
		}
	}

	vec3 levelColor = vec3(mod(worldOffset.w / 5.0f,0.9) + 0.1);
	vec3 worldColor = ((vertex) + worldOffset.xyz)/vec3(pow(2,worldOffset.w));
	//outputs
	gl_Position  = vpMatrix * vec4(cameraRelativePosition.xyz,1);

	vertexColor = vec4(vec3((worldColor+0.1*vec3(uvs,1))/1.1),1.0f);
	textureCoordinate = uvs;
};