layout(location = 8) in vec4 uiVertexColor;
out vec2 uiUv;
out vec4 uiColor;
void main()
{
	vec2 pixel = vertex.xy;
	gl_Position = (customInt1 & 2u) != 0u
		? pMatrix * vMatrix * mMatrix * vec4(pixel, 0.0, 1.0)
		: vec4(pixel.x * 2.0 / float(windowWidth) - 1.0, 1.0 - pixel.y * 2.0 / float(windowHeight), 0.0, 1.0);
	uiUv = uvs;
	uiColor = uiVertexColor;
}
