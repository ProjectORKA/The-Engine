
//! #include "uniforms.glsl"

layout(location = 0) out vec4 fragmentColor;
in vec2 textureCoordinate;

in vec3 normal;
in vec4 worldPosition;
in vec4 vertexColor;


vec2 dirToUV(vec3 dir){
  float rho = length(dir);
  float theta = atan(dir.y, dir.x);
  float phi = acos(dir.z / rho);

  // Normalize the values
  vec2 result;
  result.y = -phi / 3.14159265358979323846;  // Normalized latitude (range 0 to 1)
  result.x = theta / (2.0 * 3.14159265358979323846);  // Normalized longitude (range 0 to 1)
  return result;
}

float fresnel(vec3 normal, vec3 viewDir) {
  // Calculate the dot product of the normal and view direction
  float cosTheta = dot(normal, -viewDir);

  // Calculate the Fresnel term using Schlick's approximation
  float fresnelTerm = pow(1.0 - cosTheta, 3.0);

  return fresnelTerm;
}

void main(){

    vec3 incomingDir = normalize(worldPosition.xyz - cameraPosition.xyz);
    vec3 reflectingDir = reflect(incomingDir, normalize(normal));

    float f = fresnel(normal,incomingDir);

	vec4 color = vec4(1.2 * f * texture(texture0, dirToUV(reflectingDir)).rgb, 1.0);
	
    fragmentColor = color;
};