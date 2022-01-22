
//! #include "uniforms.glsl"

out vec2 textureCoordinate;
void main()
{
	gl_Position = pMatrix* vMatrix * mMatrix * vec4(vertex, 1);
	textureCoordinate = uvs; 
}