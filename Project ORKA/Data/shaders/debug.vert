#version 450
#extension GL_ARB_shading_language_include : require
#include "/uniforms.glsl" //! #include "uniforms.glsl" 

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 uvs;
layout(location = 2) in vec3 normals;
out vec4 vertexColor;
out vec2 textureCoordinate; 

void main() {

	vec3 positionInChunk = (mMatrix * vec4(vertex, 1)).xyz;
	vec3 cameraRelativePosition = chunkOffsetVector.xyz + positionInChunk;

	//planet curvature

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
			
			cameraRelativePosition.z = (terrainHeightfromCenter) * cos(planetDist)-leveledCameraHeightFromCenter;
		}
	}

	vec3 levelColor = vec3(mod(worldOffset.w / 5.0f,0.9) + 0.1);
	vec3 worldColor = ((vertex) + worldOffset.xyz)/vec3(pow(2,worldOffset.w));
	//outputs
	gl_Position  = vpMatrix * vec4(cameraRelativePosition.xyz,1);

	vertexColor = vec4(vec3((worldColor+0.5*vec3(uvs,1))/1.5),1.0f);
	textureCoordinate = uvs;
};