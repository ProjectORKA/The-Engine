
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

vec3 Unreal3(vec3 col){
	return col / (col + 0.155) * 1.019;
}

vec3 Unreal3Inverse(vec3 col){
	return col / (col + 0.155) * 1.019;
}


vec3 filmicToneMappingWithGamma(vec3 color)
{
	color = max(vec3(0.), color - vec3(0.004));
	color = (color * (6.2 * color + .5)) / (color * (6.2 * color + 1.7) + 0.06);
	return color;
}

void main(){
    vec3 orig = vec4(texture(texture0,textureCoordinate)).xyz;

	// color correction
	float exposure = 1;
	float brightness = 0.0;
	float contrast = 1.05;
	float saturation = 1.4;
	vec3 colorCorrected = max(contrast * (exposure * orig - 0.5) + 0.5 + brightness,vec3(0));
	
	colorCorrected = mix(
						vec3(
							dot(
								colorCorrected,
								vec3(
									0.299,
									0.587,
									0.114
								)
							)
						),
						colorCorrected,
						saturation);

	vec3 color1 = pow(ACESFilm(colorCorrected),vec3(2.2));
	vec3 color2 = pow(Unreal3(colorCorrected),vec3(2.2));
	vec3 color3 = filmicToneMappingWithGamma(colorCorrected);
//	vec3 color4 = 1.2 * pow(ACESFilm(colorCorrected),vec3(1.8));
//	vec3 color5 = 1.2 * pow(Unreal3(colorCorrected),vec3(1.8));
	vec3 color6 = pow(filmicToneMappingWithGamma(colorCorrected),vec3(1.6f));
	// split screen
//	if(textureCoordinate.x < 0.25)	fragmentColor = vec4(color1, 1);
//	else if(textureCoordinate.x < 0.5) fragmentColor = vec4(color2, 1);
//	else if(textureCoordinate.x < 0.75) fragmentColor = vec4(color3, 1);
//	else fragmentColor = vec4(color4, 1);

	vec3 outColor = clamp(color6,0,1);

	fragmentColor = vec4(outColor, 1);
};