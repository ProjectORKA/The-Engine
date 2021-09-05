#version 450
#extension GL_ARB_shading_language_include : require
#include "/uniforms.glsl" //! #include "uniforms.glsl" 

//input
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 uvs;
layout(location = 2) in vec3 normals;

//output
out vec3 normal;
out float depth;
out float slope;
out vec4 vertexColor;
out vec3 vertexPosition;
out vec3 worldCoordinate;
out vec2 textureCoordinate; 

void main(){

	vec3 position = vertex + vec3(0,0,0);//texture(texture1,uvs));

	vec3 positionInChunk = (mMatrix * vec4(position, 1)).xyz;
	vec3 cameraRelativePosition = chunkOffsetVector.xyz + positionInChunk;
	vec3 customNormal = normals;

	slope = customNormal.z;

	if(distortion){
		if(worldOffset.w < 15){

			float radius = pow(2,worldOffset.w-2)*1.2f;
			
			float height = cameraRelativePosition.z;

			float leveledCameraHeight = f1 / pow(2, 64 - worldOffset.w);
			
			float leveledCameraHeightFromCenter = radius+leveledCameraHeight;

			float terrainHeightfromCenter = height + leveledCameraHeightFromCenter;

			float dist = length(vec2(cameraRelativePosition.xy));

			float planetDist = dist / radius;

			vec2 direction = normalize(cameraRelativePosition.xy);
			cameraRelativePosition.xy = direction * (terrainHeightfromCenter) * sin(min(planetDist,3.141));
							
			vec3 newZ = normalize(vec3(direction * sin(planetDist),cos(planetDist)));
			vec3 newX = normalize(cross(vec3(0,1,0),newZ));
			vec3 newY = normalize(cross(newZ,newX));
			customNormal = (customNormal.x * newX) + (customNormal.y * newY) + (customNormal.z * newZ);
			
			cameraRelativePosition.z = (terrainHeightfromCenter) * cos(planetDist)-leveledCameraHeightFromCenter;
		}
	}

	vec3 worldColor = ((position) + vec3(0.5,0.5,0.0) + worldOffset.xyz)/vec3(pow(2,worldOffset.w),pow(2,worldOffset.w),pow(2,worldOffset.w-1));

	vec4 screenSpacePosition = vpMatrix * vec4(cameraRelativePosition.xyz,1);
	worldCoordinate = ((position.xyz*pow(2,64-worldOffset.w)) + worldOffset.xyz) / pow(2,64);
	gl_Position  = screenSpacePosition;

	vertexPosition = cameraRelativePosition.xyz;
	depth = screenSpacePosition.w;
	normal = customNormal;
	textureCoordinate = uvs;
};