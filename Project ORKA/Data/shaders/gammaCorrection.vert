
//! #include "uniforms.glsl"

//output
out vec2 textureCoordinate; 

void main(){
	textureCoordinate = uvs;
	gl_Position  = pMatrix * vMatrix * mMatrix * vec4(vertex, 1);;
};