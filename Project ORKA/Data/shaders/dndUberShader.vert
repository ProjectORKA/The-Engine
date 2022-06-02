
//! #include "uniforms.glsl"

out vec2 textureCoordinate; 
out vec3 normal;
out vec3 viewPosition;
out vec4 worldPosition;
out vec3 vertexColor;

void main() {
	worldPosition = mMatrix * vec4(vertex, 1);
	viewPosition = (cameraPosition - worldPosition).xyz;
	gl_Position  = pMatrix * vMatrix * worldPosition;
	vertexColor = colors.xyz;
	textureCoordinate = uvs;
	normal = normalize(normals);
};