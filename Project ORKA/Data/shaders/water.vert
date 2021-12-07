
//! #include "uniforms.glsl"

//input
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 uvs;
layout(location = 2) in vec3 normals;

//output
out vec3 position;
out vec2 textureCoordinate;

void main(){
	position = vertex;
	textureCoordinate = uvs;
	gl_Position = vpMatrix * vec4(vertex,1);
};