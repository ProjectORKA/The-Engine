
//! #include "uniforms.glsl"

out vec2 textureCoordinate; 
out vec3 normal;

void main() {
	gl_Position = pMatrix* vMatrix * vec4(vertex, 1);
	textureCoordinate = uvs;
	normal = normals;
};