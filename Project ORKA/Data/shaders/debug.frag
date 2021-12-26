
//! #include "uniforms.glsl"

in vec4 vertexColor;
in vec2 textureCoordinate; 
layout(location = 0) out vec4 color;

void main(){

	vec3 col = vertexColor.xyz;

	if(col.x > col.y){
		if(col.x > col.z){
			col /= col.x;
		}else{
			col /= col.z;
		}
	} else{
		if(col.y > col.z){
			col /= col.y;
		}else{
			col /= col.z;
		}
	}

	color = vec4(col,1);
};