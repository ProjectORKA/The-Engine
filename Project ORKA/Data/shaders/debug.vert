
//! #include "uniforms.glsl"

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 uvs;
layout(location = 2) in vec3 normals;
out vec4 vertexColor;

void main() {
	gl_Position  = pMatrix * vMatrix * mMatrix * vec4(vertex,1);
	vertexColor = vec4(vec3(vertex),1.0f);
};