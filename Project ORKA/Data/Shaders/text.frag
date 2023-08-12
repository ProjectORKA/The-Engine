
//! #include "uniforms.glsl"

layout(location = 0) out vec4 gBufferColor;
layout (location = 1) out vec3 gBufferNormal;
layout (location = 2) out vec3 gBufferPosition;
layout (location = 3) out uint gBufferMaterialID;
layout (location = 4) out uint gBufferObjectID;

in vec2 textureCoordinate;

void main(){
	gBufferColor = vec4(customColor.xyz,texture(texture0,textureCoordinate).x > 0.5);
	gBufferNormal = vec3(0,0,1);
	gBufferPosition = vec3(textureCoordinate,0);
	gBufferObjectID = objectID;
}