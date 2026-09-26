
//! #include "uniforms.glsl"

out vec3 normal;
out vec4 worldPosition;
out vec3 vertexColor;

void main() {
	vertexColor = colors;
	vec3 wPos = (mMatrix * vec4(vertex, 1)).xyz;
	normal = normalize((mMatrix * vec4(normals, 0)).xyz);
	worldPosition = vec4(wPos, 1);
	gl_Position = pMatrix * vMatrix * worldPosition;
}
