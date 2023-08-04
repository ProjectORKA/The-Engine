
//! #include "uniforms.glsl"

out float depth;
out vec3 normal;
out vec3 worldPos;
out vec2 textureCoordinate; 
out vec3 viewDir;
out vec3 vertexColor;

void main() {
	worldPos = (mMatrix * vec4(vertex,1)).xyz;
	viewDir = worldPos - cameraPosition.xyz;

	gl_Position  = pMatrix * vMatrix * vec4(worldPos,1);

	depth = gl_Position.w;
	normal = mat3(transpose(inverse(mMatrix))) * normals;
	textureCoordinate = uvs;
	vertexColor = colors;
};