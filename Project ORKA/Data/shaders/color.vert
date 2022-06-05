
//! #include "uniforms.glsl"

void main() {

	vec3 wPos;
	if(instanced){
		wPos = (transform * vec4(vertex,1)).xyz;
	} else {
		wPos = (mMatrix * vec4(vertex, 1)).xyz;
	}

	vec4 worldPosition = vec4(wPos,1);
	gl_Position = pMatrix* vMatrix * worldPosition;
};