
//! #include "uniforms.glsl"

out vec2 textureCoordinate; 
out vec3 normal;
out vec3 viewPosition;
out vec4 worldPosition;
out vec3 vertexColor;

void main() {

	vec3 wPos;
	vertexColor = colors;

	if(instanced){
		wPos = (transform * vec4(vertex,1)).xyz;
		normal = normalize((transform * vec4(normals,0)).xyz);
	} else {
		wPos = (mMatrix * vec4(vertex, 1)).xyz;
		normal = normalize((mMatrix * vec4(normals,0)).xyz);
	}

	worldPosition = vec4(wPos,1);
	gl_Position  = pMatrix * vMatrix * worldPosition;

	textureCoordinate = uvs;
	
};