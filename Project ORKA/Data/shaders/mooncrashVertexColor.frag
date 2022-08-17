
//! #include "uniforms.glsl"

layout(location = 0) out vec4 gBufferColor;
layout (location = 1) out vec3 gBufferNormal;
layout (location = 2) out vec3 gBufferPosition;
layout (location = 3) out uint gBufferMaterialID;

in vec3 vertexColor;

void main(){
	gBufferColor = vec4(vertexColor,1);
};