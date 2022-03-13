
//! #include "uniforms.glsl"

out vec4 col;

void main() {
	vec3 wPos;
	if(instanced){
		wPos = transform.xyz + transform.w * vertex;
	} else {
		wPos = (mMatrix * vec4(vertex, 1)).xyz;
	}
	
	gl_Position = pMatrix* vMatrix * vec4(wPos, 1);

	col = customColor;
};