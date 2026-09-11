//! #include "uniforms.glsl"

in vec4 pointColor;
layout(location = 0) out vec4 fragmentColor;

void main()
{
	if (pointColor.a < 0.5) discard;
	fragmentColor = pointColor;
}
