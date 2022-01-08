
//! #include "uniforms.glsl"

layout(location = 0) out vec4 gBufferColor;
layout (location = 1) out vec3 gBufferNormal;
layout (location = 2) out vec3 gBufferPosition;
layout (location = 3) out uint gBufferMaterialID;
layout (location = 4) out uint gBufferObjectID;

in vec2 textureCoordinate;
in vec3 normal;
in vec3 viewPosition;
in vec4 worldPosition;
void main(){

	float fresnel = 1-dot(normalize(viewPosition),normal);
	float lighting = max(fresnel/10,dot(normal,vec3(0,0,1)));

	gBufferColor = vec4(vec3(lighting),1);
	gBufferPosition  = vec3(1);//worldPosition.xyz;
	gBufferNormal = vec3(1);
	gBufferMaterialID = 1;
	gBufferObjectID = objectID;
};