
//! #include "uniforms.glsl"

//output
out vec3 normal;
out vec4 worldPosition;
out vec2 textureCoordinate; 
out vec4 vertexColor;

void main(){
	//normal = normalize(mat3(transpose(inverse(mMatrix))) * normals);
	
	vec4 wPos;
	if(instanced){
		worldPosition = transform * vec4(vertex,1);
		normal = normalize((transform * vec4(normals,0)).xyz);
	} else {
		worldPosition = mMatrix * vec4(vertex,1);
		normal = normalize((mMatrix * vec4(normals,0)).xyz);
	}

	textureCoordinate = uvs;
	vertexColor = vec4(colors,1);
	gl_Position  = pMatrix * vMatrix * worldPosition;
};