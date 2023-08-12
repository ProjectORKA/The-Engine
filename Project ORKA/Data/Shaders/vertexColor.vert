
//! #include "uniforms.glsl"

out vec4 col;

void main() {

	vec3 wPos;
	if(instanced){
		wPos = (transform * vec4(vertex,1)).xyz;
	} else {
		wPos = (mMatrix * vec4(vertex, 1)).xyz;
	}

	vec4 worldPosition = vec4(wPos,1);
	gl_Position = pMatrix * vMatrix * worldPosition;

	col = vec4(colors,1);
};