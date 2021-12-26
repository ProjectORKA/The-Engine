
//! #include "uniforms.glsl"

layout(location = 0) out vec4 color;
in vec2 textureCoordinate;
in vec3 normal;
in vec3 viewPosition;
in vec4 worldPosition;
void main(){

	float fresnel = 1-dot(normalize(viewPosition),normal);
	float lighting = max(fresnel/10,dot(normal,vec3(0,0,1)));


	color = vec4(vec3(lighting),1);//vec4(vec3(lighting),1) * customColor;
};