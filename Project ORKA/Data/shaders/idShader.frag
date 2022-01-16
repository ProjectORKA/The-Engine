
//! #include "uniforms.glsl"

layout(location = 0) out uvec3 idBuffer;
//x = objectID
//y = instanceID
//z = primitiveID

flat in uint instanceID;

void main(){
	idBuffer = uvec3(objectID,instanceID,gl_PrimitiveID);
};