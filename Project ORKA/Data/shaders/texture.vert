
//! #include "uniforms.glsl"

//output
out vec2 textureCoordinate; 

void main(){
	vec4 worldPosition = mMatrix * vec4(vertex, 1);
	gl_Position  = pMatrix * vMatrix * worldPosition;
	textureCoordinate = uvs;
};