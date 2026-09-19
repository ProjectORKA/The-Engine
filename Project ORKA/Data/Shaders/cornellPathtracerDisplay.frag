
//! #include "uniforms.glsl"

layout(location = 0) out vec4 fragmentColor;
in vec2 textureCoordinate;

vec3 acesToneMap(vec3 color)
{
	color *= 0.6;
	const float a = 2.51;
	const float b = 0.03;
	const float c = 2.43;
	const float d = 0.59;
	const float e = 0.14;
	return clamp((color * (a * color + b)) / (color * (c * color + d) + e), 0.0, 1.0);
}

void main()
{
	vec3 accum   = texture(texture0, textureCoordinate).rgb;
	float samples = max(f1, 1.0);
	vec3 color   = accum / samples;
	color        = acesToneMap(color);
	color        = sqrt(color);
	fragmentColor = vec4(color, 1.0);
}
