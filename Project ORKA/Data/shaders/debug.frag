
//! #include "uniforms.glsl"

in vec4 vertexColor;
in vec4 worldPosition;
in vec2 textureCoordinate; 
layout(location = 0) out vec4 color;


void main(){

	vec3 col = vertexColor.xyz;

	col = normalize(col);

	color = vec4(worldPosition.xyz,1);
};