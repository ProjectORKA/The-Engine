
//! #include "uniforms.glsl"

layout(location = 0) out vec4 color;
in vec3 col;
in vec3 norm;

void main(){
	vec3 skyColor = vec3(0.3,0.8,1);
	vec3 sunDirection = vec3(-0.2,1,1);
	vec3 sunColor = vec3(1,0.8,0.7);
	vec3 backlights = customColor.xyz * vec3(clamp(dot(normalize(vec3(0.7,-0.7,-0.6)),norm),0,1));
	float fresnel = (1.1 - dot(norm,vec3(0,0,1)))  * 0.7;
	vec3 ambientlight = customColor.xyz * skyColor * fresnel;
	float fillLight = pow((1 + dot(normalize(sunDirection),norm))/2.0f,2);
	color = vec4(customColor.xyz * col * fillLight + ambientlight + sunColor * backlights,0.3);
};