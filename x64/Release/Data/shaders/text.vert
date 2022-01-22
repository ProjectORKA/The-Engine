
//! #include "uniforms.glsl"

//output
out vec2 textureCoordinate;

void main(){
	vec3 worldPosition;
	
	if(instanced){
		worldPosition = transform.xyz + transform.w * vertex;
	} else {
		worldPosition = (mMatrix * vec4(vertex, 1)).xyz;
	}

	gl_Position  = pMatrix * vMatrix * vec4(worldPosition,1);
	textureCoordinate = uvs;
};