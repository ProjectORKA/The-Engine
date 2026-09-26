
//! #include "uniforms.glsl"

out vec3 normal;
out vec4 worldPosition;
out vec3 vertexColor;

void main() {
	vertexColor = colors;
	vec3 wPos = (mMatrix * vec4(vertex, 1)).xyz;
	wPos.z += sin(wPos.x * 0.04 + time * 1.2) * 0.08 + cos(wPos.y * 0.035 - time) * 0.06;
	normal = vec3(0.0, 0.0, 1.0);
	worldPosition = vec4(wPos, 1);
	gl_Position = pMatrix * vMatrix * worldPosition;
}
