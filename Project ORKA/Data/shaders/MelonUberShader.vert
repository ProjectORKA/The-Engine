
//! #include "uniforms.glsl"

//output
out vec3 normal;
out vec3 viewPosition;
out vec3 vertexPosition;
void main() {
	vec3 worldPosition;
	if(instanced){
		worldPosition = transform.xyz + transform.w * vertex;
		normal = normals;
	} else {
		worldPosition = (mMatrix * vec4(vertex, 1)).xyz;
		normal = normalize((mMatrix * vec4(normals, 0)).xyz);
	}
	viewPosition = cameraPosition.xyz - worldPosition;
	vertexPosition = worldPosition;
	gl_Position  = pMatrix * vMatrix * vec4(worldPosition, 1);
};