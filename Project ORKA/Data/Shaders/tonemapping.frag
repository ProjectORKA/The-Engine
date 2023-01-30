
//! #include "uniforms.glsl"

layout(location = 0) out vec4 fragmentColor;
in vec2 textureCoordinate;

vec3 ACESFilm(vec3 col)
{
	vec3 x = col;
	float a = 2.51;
	float b = 0.03;
	float c = 2.43;
	float d = 0.59;
	float e = 0.14;
	return (x*(a*x+b))/(x*(c*x+d)+e);
}

void main(){
    vec3 orig = vec4(texture(texture0,textureCoordinate)).xyz;
	vec3 color1 = ACESFilm(orig);
	vec3 color2 = 1.2 * pow(ACESFilm(orig),vec3(1.8));
	fragmentColor = vec4(color2, 1);
};