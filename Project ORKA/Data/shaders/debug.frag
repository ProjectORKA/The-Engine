
//! #include "uniforms.glsl"
in vec3 normal;
in vec4 vertexColor;
in vec4 worldPosition;
in vec2 textureCoordinate; 
layout(location = 0) out vec4 color;


void main(){
	vec3 col = vertexColor.xyz;

	col = normalize(col);

	color = vec4(mod(vertexColor.xyz,vec3(1)),1);
	//color = vec4(worldPosition.xyz,1);
};