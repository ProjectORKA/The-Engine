
//! #include "uniforms.glsl"

//output
out vec3 position;
out vec2 textureCoordinate;

void main(){
	position = vertex;
	textureCoordinate = uvs;
	gl_Position = pMatrix * vMatrix * vec4(vertex,1);
};