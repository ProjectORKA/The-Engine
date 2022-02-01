
//! #include "uniforms.glsl"

flat out uint instanceID;

void main() {
	instanceID = gl_InstanceID;
	gl_Position  = pMatrix * vMatrix * mMatrix * vec4(vertex,1);
};