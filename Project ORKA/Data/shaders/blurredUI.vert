#version 420
in vec3 vertex;
in vec2 uvs;

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

out vec2 textureCoordinate;
void main()
{
	gl_Position = vpMatrix * mMatrix * vec4(vertex, 1);
	textureCoordinate = uvs; 
}