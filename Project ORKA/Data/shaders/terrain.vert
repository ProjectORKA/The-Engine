#version 450

//input
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 uvs;
layout(location = 2) in vec3 normals;

layout(std140, binding = 0) uniform GlobalUniforms
{
	mat4 mMatrix;
	mat4 vpMatrix;
	
	vec4 worldOffset;
	vec4 cameraVector;		 //its vec3 but treated as vec4 in memory
	vec4 chunkOffsetVector;	 //its vec3 but treated as vec4 in memory
	vec4 customColor;

	float time;
	float custom1; //<- ths will be the camera height
	float custom2;
	float custom3;

	bool distortion;
};

uniform sampler2D texture0;
uniform sampler2D texture1;

//output
out vec3 normal;
out float depth;
out float slope;
out vec4 vertexColor;
out vec3 vertexPosition;
out vec3 worldCoordinate;
out vec2 textureCoordinate; 

void main(){

	vec3 positionInChunk = (mMatrix * vec4(vertex, 1)).xyz;
	vec3 cameraRelativePosition = chunkOffsetVector.xyz + positionInChunk;
	vec3 customNormal = normals;

	slope = normals.z;

	if(distortion){
		if(worldOffset.w < 15){

			float radius = pow(2,worldOffset.w-2)*1.2f;
			
			float height = cameraRelativePosition.z;

			float leveledCameraHeight = custom1 / pow(2, 64 - worldOffset.w);
			
			float leveledCameraHeightFromCenter = radius+leveledCameraHeight;

			float terrainHeightfromCenter = height + leveledCameraHeightFromCenter;

			float dist = length(vec2(cameraRelativePosition.xy));

			float planetDist = dist / radius;

			vec2 direction = normalize(cameraRelativePosition.xy);
			cameraRelativePosition.xy = direction * (terrainHeightfromCenter) * sin(min(planetDist,3.141));
							
			vec3 newZ = normalize(vec3(direction * sin(planetDist),cos(planetDist)));
			vec3 newX = normalize(cross(vec3(0,1,0),newZ));
			vec3 newY = normalize(cross(newZ,newX));
			customNormal = (normals.x * newX) + (normals.y * newY) + (normals.z * newZ);
			
			cameraRelativePosition.z = (terrainHeightfromCenter) * cos(planetDist)-leveledCameraHeightFromCenter;
		}
	}

	vec3 worldColor = ((vertex) + vec3(0.5,0.5,0.0) + worldOffset.xyz)/vec3(pow(2,worldOffset.w),pow(2,worldOffset.w),pow(2,worldOffset.w-1));

	//outputs
	vec4 screenSpacePosition = vpMatrix * vec4(cameraRelativePosition.xyz,1);
	worldCoordinate = ((vertex.xyz*pow(2,64-worldOffset.w)) + worldOffset.xyz) / pow(2,64);
	gl_Position  = screenSpacePosition;

	vertexPosition = cameraRelativePosition.xyz;
	depth = screenSpacePosition.w;
	normal = customNormal;
	textureCoordinate = uvs;
};