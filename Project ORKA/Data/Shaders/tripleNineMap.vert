
//! #include "uniforms.glsl"

out float depth;
out vec3 normal;
out vec3 worldPos;
out vec2 textureCoordinate; 
out vec3 viewDir;

void main() {
	if(instanced){
		worldPos = (transform * vec4(vertex,1)).xyz;
		normal = normalize((transform * vec4(normals,0)).xyz);
	} else {
		worldPos = (mMatrix * vec4(vertex, 1)).xyz;
		normal = normalize((mMatrix * vec4(normals,0)).xyz);
	}
	// if normal bug occurs, try this
	// normal = mat3(transpose(inverse(mMatrix))) * normals;

	viewDir = worldPos - cameraPosition.xyz;
	gl_Position  = pMatrix * vMatrix * vec4(worldPos,1);
	depth = gl_Position.w;
	textureCoordinate = uvs;
};