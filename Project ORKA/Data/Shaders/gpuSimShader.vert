
//! #include "uniforms.glsl"

//output
out vec2 textureCoordinate; 

void main(){
	gl_Position  = pMatrix * vec4(vertex,1);
	textureCoordinate = uvs;
};