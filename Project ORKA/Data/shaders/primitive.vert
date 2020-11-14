#version 400

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 uvs;
layout(location = 2) in vec3 normals;

out vec4 vertexColor;
out vec3 vertexPosition;
out vec2 textureCoordinate; 
out vec3 worldCoordinate;
out vec3 normal;
out float depth;

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
	vec3 customNormal = normals;

	if(distortion > 0){
		if(worldOffset.w < 13){

			float radius = pow(2,worldOffset.w-2)*1.5f ;
			
			float height = cameraRelativePosition.z;

			float relativeCameraHeight = cameraHeight / pow(2, 64 - worldOffset.w);

			float dist = length(vec2(cameraRelativePosition.xy));

			if(cameraRelativePosition.xy != vec2(0)){
				vec2 direction = normalize(cameraRelativePosition.xy);
				cameraRelativePosition.xy = direction * (height + radius + relativeCameraHeight) * sin(min(dist/radius,3.141));
	
				vec3 newZ = normalize(vec3(direction * sin(dist/radius),cos(dist/radius)));
				vec3 newX = normalize(cross(vec3(0,1,0),newZ));
				vec3 newY = normalize(cross(newZ,newX));
				
				customNormal = (normals.x * newX) + (normals.y * newY) + (normals.z * newZ);
			}
			cameraRelativePosition.z = (height + radius + relativeCameraHeight) * cos(dist/radius)-radius-relativeCameraHeight;
		}
	}

	vec3 worldColor = ((vertex) + vec3(0.5,0.5,0.0) + worldOffset.xyz)/vec3(pow(2,worldOffset.w),pow(2,worldOffset.w),pow(2,worldOffset.w-1));

	//outputs
	vec4 screenSpacePosition = vpMatrix * vec4(cameraRelativePosition.xyz,1);
	gl_Position  = screenSpacePosition;

	vertexPosition = cameraRelativePosition.xyz;
	depth = screenSpacePosition.w;
	vertexColor = vec4(vec3((worldColor+vec3(uvs,1))/2),1.0f);
	normal = customNormal;
	textureCoordinate = uvs;
	worldCoordinate = vec3(worldOffset.xyz + vertex.xyz)/pow(2,worldOffset.w);
};