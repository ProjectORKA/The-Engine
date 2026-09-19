in vec2 uiUv;
in vec4 uiColor;
out vec4 color;
void main()
{
	if ((customInt1 & 1u) != 0u)
	{
		vec4 image = texture(texture0, uiUv);
		image.rgb *= image.a;
		color = image * uiColor;
	}
	else
	{
		float check = mod(floor(gl_FragCoord.x / 8.0) + floor(gl_FragCoord.y / 8.0), 2.0);
		color = mix(vec4(0.12, 0.0, 0.12, 1.0), vec4(0.8, 0.0, 0.8, 1.0), check) * uiColor.a;
	}
}
