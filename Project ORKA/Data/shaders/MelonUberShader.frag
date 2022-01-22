
//! #include "uniforms.glsl"

layout(location = 0) out vec4 fragmentColor;

in vec3 normal;
in vec3 viewPosition;
in vec3 vertexPosition;

void main(){
	float fresnel = (1-dot(normalize(viewPosition),normal))/20;
	float ambient = 0.1;
	float diffuse = clamp(dot(normal,sunDir.xyz),0,1);
	float lighting = fresnel + ambient + diffuse;

	fragmentColor = vec4(customColor.xyz * lighting,1);
	
	//debug normal
	//fragmentColor = vec4(normal,1);
};