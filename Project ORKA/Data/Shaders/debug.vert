
//! #include "uniforms.glsl"

out vec3 normal;
out vec4 vertexColor;
out vec4 worldPosition;
out vec2 textureCoordinate;

void main() {
	
	vec3 wPos;
	
	if(instanced){
		wPos = (transform * vec4(vertex,1)).xyz;
	} else {
		wPos = (mMatrix * vec4(vertex, 1)).xyz;
	}

	worldPosition = vec4(wPos,1);
	normal = normals;
	textureCoordinate = uvs;
	gl_Position  = pMatrix * vMatrix * worldPosition;
	vertexColor = vec4(vec3(vertex),1.0f);
};