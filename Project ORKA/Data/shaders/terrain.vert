#version 450
#extension GL_ARB_shading_language_include : require
#include "/uniforms.glsl" //! #include "uniforms.glsl" 

#define PI 3.14159265359

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

mat2x3 calculateDistortion1(vec3 location, vec3 normal){
	if(worldOffset.w < 15){
		float radius = pow(2,worldOffset.w-2)*1.2f;
		float height = location.z;
		float leveledCameraHeight = f1 / pow(2, 64 - worldOffset.w);
		float leveledCameraHeightFromCenter = radius+leveledCameraHeight;
		float terrainHeightfromCenter = height + leveledCameraHeightFromCenter;
		float dist = length(vec2(location.xy));
		float planetDist = dist / radius;
		vec2 direction = normalize(location.xy);
		location.xy = direction * (terrainHeightfromCenter) * sin(min(planetDist,PI));
		vec3 newZ = normalize(vec3(direction * sin(planetDist),cos(planetDist)));
		vec3 newX = normalize(cross(vec3(0,1,0),newZ));
		vec3 newY = normalize(cross(newZ,newX));
		normal = (normal.x * newX) + (normal.y * newY) + (normal.z * newZ);
		location.z = (terrainHeightfromCenter) * cos(planetDist)-leveledCameraHeightFromCenter;
	}
	return mat2x3(vec3(location),
	vec3(normal));
}

mat2x3 calculateDistortion2(vec3 location, vec3 normal){

	float dist = length(vec2(location.xy * 2)) / pow(2,worldOffset.w);
	if(dist > 1){
		location.xy = normalize(location.xy) * pow(2,worldOffset.w-1);
		dist = 1;
	}

	float func = sqrt(1 - pow(clamp(dist,-1,1),2));
	vec3 newZ = normalize(vec3(location.xy * 2,  func * pow(2,worldOffset.w)));
	vec3 newX = normalize(cross(vec3(0,1,0),newZ));
	vec3 newY = normalize(cross(newZ,newX));
	normal = (normal.x * newX) + (normal.y * newY) + (normal.z * newZ);
	//location.z -= (dist * dist) * pow(2,worldOffset.w);
	location.z += (func - 1) * pow(2,worldOffset.w);
	
	return mat2x3(vec3(location),
	vec3(normal));
}

void main(){

	

	vec3 position = vertex + vec3(0,0,0);//texture(texture1,uvs));

	vec3 positionInChunk = (mMatrix * vec4(position, 1)).xyz;
	vec3 cameraRelativePosition = chunkOffsetVector.xyz + positionInChunk;
	vec3 customNormal = normals;

	slope = customNormal.z;


	if(distortion){
		mat2x3 squish = calculateDistortion2(cameraRelativePosition, customNormal);
		cameraRelativePosition = squish[0];
		customNormal = squish[1];
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