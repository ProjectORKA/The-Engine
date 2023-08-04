
//! #include "uniforms.glsl"

out vec3 rayOrigin;
out highp vec3 rayDirection;

void main(){
	rayOrigin = -cameraPosition.xyz;
	rayDirection = normalize(vertex);

	vec4 screenSpacePos = pMatrix* vMatrix * mMatrix * vec4(vertex, 1);
	gl_Position = screenSpacePos;
};