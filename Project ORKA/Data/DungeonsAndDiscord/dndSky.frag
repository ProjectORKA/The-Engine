
//! #include "uniforms.glsl"

layout(location = 0) out vec4 fragmentColor;
in vec3 dir;

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
	fragmentColor = vec4(texture(texture0, dirToUV(dir)).xyz,1);
};