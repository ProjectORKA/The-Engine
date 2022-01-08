
//! #include "uniforms.glsl"

layout(location = 0) out vec4 color;
in vec2 textureCoordinate;
in vec3 normal;

void main(){
	//if(texture(texture0,textureCoordinate).a != 1) discard;

	color = vec4(texture(texture0,textureCoordinate).xyz,1.0);

};