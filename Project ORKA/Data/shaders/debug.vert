
//! #include "uniforms.glsl"

out vec4 vertexColor;

void main() {
	
	vec3 worldPosition;
	
	if(instanced){
		worldPosition = transform.xyz + transform.w * vertex;
	} else {
		worldPosition = (mMatrix * vec4(vertex, 1)).xyz;
	}

	gl_Position  = pMatrix * vMatrix * vec4(worldPosition,1);
	vertexColor = vec4(vec3(vertex),1.0f);
};