
//! #include "uniforms.glsl"

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