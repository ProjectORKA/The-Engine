#version 450
#extension GL_ARB_shading_language_include : require
#include "/uniforms.glsl" //! #include "uniforms.glsl" 

in vec2 textureCoordinate;
out vec4 color;

vec4 lerp(vec4 a, vec4 b, float alpha){
	vec4 c = vec4(0);
	c += b * alpha;
	c += a * (1-alpha);
	return c;
}

void main()
{
	vec4 image = texture(texture0,gl_FragCoord.xy / textureSize(texture0,0));
	vec4 mask = texture(texture1,gl_FragCoord.xy / textureSize(texture1,0));
	vec4 dropShadow = texture(texture2,gl_FragCoord.xy / textureSize(texture2,0));
	vec4 blur = texture(texture3,gl_FragCoord.xy / textureSize(texture3,0));
	vec4 noise = texture(texture4,gl_FragCoord.xy / textureSize(texture4,0));

	vec4 customColor = lerp(blur * vec4(vec3(0.25),1) + noise * vec4(vec3(0.015),1),image * dropShadow,mask.r);

	color = customColor;
}