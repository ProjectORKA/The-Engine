
//! #include "uniforms.glsl"

in vec3 vertex;
in vec2 uvs;

out vec2 textureCoordinate;
void main()
{
	gl_Position = vpMatrix * mMatrix * vec4(vertex, 1);
	textureCoordinate = uvs; 
}