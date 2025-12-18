
//! #include "uniforms.glsl"

#define PI 3.14159265359

out vec3 vertexColor;

void calculateDistortion2(inout vec3 location, inout vec3 normal){
	float dist = length(vec2(location.xy * 1.5)) / pow(2,worldOffset.w);
	if(dist > 1){
		location = vec3(0,0,-5);
		
		return;
	}
	float func = sqrt(1 - pow(clamp(dist,-1,1),2));
	vec3 newZ = normalize(vec3(location.xy * 2,  func * pow(2,worldOffset.w)));
	vec3 newX = normalize(cross(vec3(0,1,0),newZ));
	vec3 newY = normalize(cross(newZ,newX));
	normal = (normal.x * newX) + (normal.y * newY) + (normal.z * newZ);
	location.z += (func - 1) * pow(2,worldOffset.w);
}

void main(){

	vec3 positionInChunk = (mMatrix * vec4(vertex, 1)).xyz;
	vec3 cameraRelativePosition = positionInChunk - cameraPosition.xyz;
	vec3 customNormal = normalize(mat3(transpose(inverse(mMatrix))) * normals);

	if(bool(customInt1)){
		calculateDistortion2(cameraRelativePosition, customNormal);
	}

	vec4 screenSpacePosition = pMatrix * vMatrix * vec4(cameraRelativePosition.xyz,1);

	vertexColor = colors;
	gl_Position  = screenSpacePosition;
};