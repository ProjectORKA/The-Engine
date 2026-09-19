struct UiQuad { vec4 originAxisX; vec4 axisYSize; vec4 cornerRadii; vec4 colors[4]; vec4 uvRect; vec4 gradient; vec4 parameters; };
layout(std430, binding = 3) readonly buffer UiQuadStream { UiQuad uiQuads[]; };
out vec2 uiUv;
out vec4 uiColor;
void main()
{
	UiQuad q = uiQuads[gl_InstanceID + gl_BaseInstance];
	vec2 pixel = q.originAxisX.xy + q.originAxisX.zw * vertex.x + q.axisYSize.xy * vertex.y;
	gl_Position = (customInt1 & 2u) != 0u
		? pMatrix * vMatrix * mMatrix * vec4(pixel, 0.0, 1.0)
		: vec4(pixel.x * 2.0 / float(windowWidth) - 1.0, 1.0 - pixel.y * 2.0 / float(windowHeight), 0.0, 1.0);
	uiUv = mix(q.uvRect.xy, q.uvRect.zw, vertex.xy);
	uiColor = q.colors[0];
}
