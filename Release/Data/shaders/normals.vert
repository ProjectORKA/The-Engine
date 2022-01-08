
//! #include "uniforms.glsl"

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 uvs;
layout(location = 2) in vec3 normals;

out vec4 vertexColor;

void main() {
	vec3 worldPosition = (mMatrix * vec4(vertex, 1)).xyz;
	
	worldPosition -= vec3(cameraPosition.xy,0);
	vec3 normal = normals;

	//calculateDistortion(worldPosition,normal);

	worldPosition += vec3(cameraPosition.xy,0);

	gl_Position  = pMatrix * vMatrix * vec4(worldPosition,1);

	vertexColor = vec4(normalize(normalize(normal) + normalize(vertex)),1);
};