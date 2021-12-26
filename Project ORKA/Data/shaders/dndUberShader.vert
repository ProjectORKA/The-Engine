
//! #include "uniforms.glsl"

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 uvs;
layout(location = 2) in vec3 normals;

out vec2 textureCoordinate; 
out vec3 normal;
out vec3 viewPosition;
out vec4 worldPosition;
void main() {
	worldPosition = mMatrix * vec4(vertex, 1);
	viewPosition = (cameraPosition - worldPosition).xyz;
	gl_Position  = pMatrix * vMatrix * worldPosition;

	textureCoordinate = uvs;
	normal = normals;
};