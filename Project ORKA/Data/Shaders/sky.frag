
//! #include "uniforms.glsl"

layout(location = 0) out vec4 color;
in vec2 textureCoordinate;
in vec3 normal;

void main(){
	color = vec4(pow(texture(texture0,textureCoordinate).xyz,vec3(2)),1.0);
};