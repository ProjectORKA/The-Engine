
//! #include "uniforms.glsl"
in vec3 normal;
in vec4 vertexColor;
in vec4 worldPosition;
in vec2 textureCoordinate;

layout(location = 0) out vec4 color;

void main(){
	vec3 col1 = vertexColor.xyz;
	vec3 col2 = vec3(textureCoordinate, 0);
	vec3 col3 = mod(worldPosition.xyz, vec3(1));
	color = vec4(mix(col1, mix(col2, col3, 0.5), 0.5), 1);
};