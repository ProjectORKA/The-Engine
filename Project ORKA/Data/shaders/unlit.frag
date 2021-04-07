#version 450

layout(location = 0) out vec4 fragmentColor;

in vec2 textureCoordinate;

uniform sampler2D texture0;
uniform float tint;
uniform float fade;

void main(){
	vec4 color = texture(texture0,textureCoordinate);

	if(color.a == 0) discard;
	else{
		fragmentColor = vec4(color.xyz*vec3(tint),color.a * fade);
	}
};