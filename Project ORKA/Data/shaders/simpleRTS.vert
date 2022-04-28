
//! #include "uniforms.glsl"

out vec2 textureCoordinate; 
out vec3 normal;
out vec3 viewPosition;
out vec4 worldPosition;

void main() {

	vec3 wPos;
	
	if(instanced){
		wPos = (transform * vec4(vertex,1)).xyz;
		normal = (transform * vec4(normals,0)).xyz;
	} else {
		wPos = (mMatrix * vec4(vertex, 1)).xyz;
		normal = (mMatrix * vec4(normals,0)).xyz;
	}

	worldPosition = vec4(wPos,1);
	viewPosition = (cameraPosition - worldPosition).xyz;
	gl_Position  = pMatrix * vMatrix * worldPosition;

	textureCoordinate = uvs;
	
};