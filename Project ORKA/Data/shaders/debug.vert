
//! #include "uniforms.glsl"

out vec4 vertexColor;
out vec4 worldPosition;

void main() {
	
	vec3 wPos;
	
	if(instanced){
		wPos = transform.xyz + transform.w * vertex;
	} else {
		wPos = (mMatrix * vec4(vertex, 1)).xyz;
	}

	worldPosition = vec4(wPos,1);

	gl_Position  = pMatrix * vMatrix * worldPosition;
	vertexColor = vec4(vec3(vertex),1.0f);
};