
//! #include "uniforms.glsl"

in vec2 textureCoordinate;

layout (location = 0) out vec4 fragmentColor;

void main(){
	float depth = texture(texture0,textureCoordinate).r;
	float near = 0.001f;
	float far = 1000.0f;
//	float linearDepth = (2.0 * near * far) / (far + near - (depth * 2.0 - 1.0) * (far - near));
//	if(linearDepth > 1.0)linearDepth = 0;

	float halfDepth = -0.5*far/(near-far)+0.5*depth;
	
	fragmentColor = vec4(vec3(halfDepth/10.0),1);
	gl_FragDepth = halfDepth;
};