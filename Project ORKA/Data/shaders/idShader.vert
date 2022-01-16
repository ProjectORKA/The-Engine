
//! #include "uniforms.glsl"

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 uvs;
layout(location = 2) in vec3 normals;

flat out uint instanceID;

void main() {
	instanceID = gl_InstanceID;
	gl_Position  = pMatrix * vMatrix * mMatrix * vec4(vertex,1);
};