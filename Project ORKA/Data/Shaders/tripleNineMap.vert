
//! #include "uniforms.glsl"

//output
out vec2 textureCoordinate; 
out vec4 worldPosition;

void main(){
	textureCoordinate = uvs;
	worldPosition = mMatrix * vec4(vertex, 1);
	gl_Position  = pMatrix * vMatrix * worldPosition;
};