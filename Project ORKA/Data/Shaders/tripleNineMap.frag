
//! #include "uniforms.glsl"

layout(location = 0) out vec4 gBufferColor;
layout (location = 2) out vec3 gBufferPosition;

in float depth;
in vec3 normal;
in vec3 viewDir;
in vec3 worldPos;
in vec2 textureCoordinate; 

vec2 dirToUV(vec3 dir){
  float rho = length(dir);
  float theta = atan(dir.y, -dir.x);
  float phi = acos(dir.z / rho);

  // Normalize the values
  vec2 result;
  result.y = -phi / 3.14159265358979323846;  // Normalized latitude (range 0 to 1)
  result.x = theta / (2.0 * 3.14159265358979323846);  // Normalized longitude (range 0 to 1)
  return result;
}

void main(){
	vec3 fragmentViewVector = normalize(viewDir);
	vec3 reflection = normalize(reflect(fragmentViewVector,normal));

	float diffuse =  clamp(dot(normalize(normal),sunDir.xyz),0,1);
	float specular = 0; //clamp(1 * pow(max(dot(reflection, sunDir.xyz), 0.0f), 4.0f),0,1);
	float fresnel = clamp(0.1f * (1-dot(-fragmentViewVector,normalize(normal))),0,1);
	float ambient = 0.05f;


	float coloredLight = diffuse;
	float externalLight = specular + fresnel + ambient;
	vec3 skyReflection = texture(texture0,dirToUV(reflection)).xyz;

	vec3 outCol = customColor.xyz * coloredLight + externalLight * skyReflection;


	gBufferColor = vec4(vec3(outCol),1);
	gBufferPosition = worldPos;
};