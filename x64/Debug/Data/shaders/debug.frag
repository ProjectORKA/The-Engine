
//! #include "uniforms.glsl"

in vec4 vertexColor;
in vec2 textureCoordinate; 
layout(location = 0) out vec4 color;

void main(){

	vec3 col = vertexColor.xyz;

	col = normalize(col);

	color = vec4(col,1);
};