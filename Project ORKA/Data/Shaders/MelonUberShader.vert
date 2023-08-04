
//! #include "uniforms.glsl"

out vec3 normal;
out vec3 viewPosition;
out vec3 vertexPosition;

void main() {
	vec3 worldPosition;
	
	if(instanced){
		worldPosition = (transform * vec4(vertex, 1.0f)).xyz;
		normal = normalize((transform * vec4(normals, 0.0f)).xyz);
	} else {
		worldPosition = (mMatrix * vec4(vertex, 1.0f)).xyz;
		normal = normalize((mMatrix * vec4(normals, 0.0f)).xyz);
	}

	viewPosition = cameraPosition.xyz - worldPosition;
	vertexPosition = worldPosition;
	gl_Position  = pMatrix * vMatrix * vec4(worldPosition, 1.0f);
};