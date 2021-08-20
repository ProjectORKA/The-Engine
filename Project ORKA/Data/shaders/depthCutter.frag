#version 450
in vec2 textureCoordinate;


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

layout (location = 0) out vec4 fragmentColor;

void main(){
	float depth = texture(texture0,textureCoordinate).r;
//	float ndc = depth * 2.0 - 1.0;
	float near = 0.001f;
	float far = 1000.0f;
//	float linearDepth = (2.0 * near * far) / (far + near - (depth * 2.0 - 1.0) * (far - near));
//	if(linearDepth > 1.0)linearDepth = 0;

	float halfDepth = -0.5*far/(near-far)+0.5*depth;
	//float halfDepth = depth/2;
	
	fragmentColor = vec4(vec3(halfDepth/10.0),1);
	gl_FragDepth = halfDepth;
};