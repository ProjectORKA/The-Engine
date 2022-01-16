
//! #include "uniforms.glsl"

layout(location = 0) out vec4 fragmentColor;
in vec3 vertexPosition;
in vec2 textureCoordinate;
in vec3 normal;
in float depth;
in vec3 worldCoordinate;

void main(){
	if(texture(texture0,textureCoordinate).a != 1) discard;
	if(normal.z<0)discard;

	vec3 fragmentViewVector = normalize(vertexPosition);
	vec3 sunDir = normalize(vec3(1,1,1));
	vec3 reflection = normalize(reflect(fragmentViewVector,normal));

	float diffuse = clamp(dot(normalize(normal),sunDir),0,1);
	float specular = clamp(1 * pow(max(dot(reflection, sunDir), 0.0f), 8.0f),0,1);
	float fresnel = clamp(0.1f * (1-dot(-fragmentViewVector,normalize(normal))),0,1);
	float ambient = 0.05f;


	float coloredLight = fresnel + ambient + diffuse;
	float externalLight = specular;

	fragmentColor = vec4(customColor.xyz * vec3(coloredLight) + vec3(externalLight),1);
};