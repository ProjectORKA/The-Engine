//! #include "uniforms.glsl"

void main()
{
	gl_Position = pMatrix * vMatrix * mMatrix * vec4(vertex, 1.0);
}

