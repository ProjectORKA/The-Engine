#version 420
in vec2 textureCoordinate;
out vec4 color;

layout(std140, binding = 0) uniform GlobalUniforms
{
	mat4 mMatrix;
	mat4 vpMatrix;
	
	vec4 worldOffset;
	vec4 cameraVector;		 //its vec3 but treated as vec4 in memory
	vec4 chunkOffsetVector;	 //its vec3 but treated as vec4 in memory
	vec4 customColor;

	float time;
	float custom1;
	float custom2;
	float custom3;

	bool distortion;
};

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;
uniform sampler2D texture4;

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