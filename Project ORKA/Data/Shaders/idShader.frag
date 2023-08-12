
//! #include "uniforms.glsl"

layout(location = 0) out uvec4 idBuffer;
// x = objectID
// y = instanceID
// z = primitiveID
// w = unknown

flat in uint instanceID;

void main(){
	idBuffer = uvec4(objectID,instanceID,gl_PrimitiveID,0);
};