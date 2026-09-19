struct UiQuad
{
	vec4 originAxisX;
	vec4 axisYSize;
	vec4 cornerRadii;
	vec4 colors[4];
	vec4 uvRect;
	vec4 gradient;
	vec4 parameters;
};

layout(std430, binding = 3) readonly buffer UiQuadStream { UiQuad uiQuads[]; };

out vec2 uiUv;
out vec2 uiLocal;
out vec2 uiSize;
out vec4 uiRadii;
out vec4 uiColor;

void main()
{
	UiQuad quad = uiQuads[gl_InstanceID + gl_BaseInstance];
	vec2 pixel = quad.originAxisX.xy +
		quad.originAxisX.zw * vertex.x + quad.axisYSize.xy * vertex.y;
	if ((customInt1 & 2u) != 0u)
		gl_Position = pMatrix * vMatrix * mMatrix * vec4(pixel, 0.0, 1.0);
	else
		gl_Position = vec4(pixel.x * 2.0 / float(windowWidth) - 1.0,
			1.0 - pixel.y * 2.0 / float(windowHeight), 0.0, 1.0);
	uiUv = mix(quad.uvRect.xy, quad.uvRect.zw, vertex.xy);
	uiLocal = vertex.xy * quad.axisYSize.zw;
	uiSize = quad.axisYSize.zw;
	uiRadii = quad.cornerRadii;
	uiColor = mix(mix(quad.colors[0], quad.colors[1], vertex.x),
		mix(quad.colors[3], quad.colors[2], vertex.x), vertex.y);
}
