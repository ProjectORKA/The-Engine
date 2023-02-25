
//! #include "uniforms.glsl"

//output
out vec3 dir; 

void main(){
	dir = normalize(vertex);
	gl_Position  = pMatrix * vMatrix * vec4(vertex, 1);
};