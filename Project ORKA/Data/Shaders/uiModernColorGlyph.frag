in vec2 uiUv;
in vec4 uiColor;
out vec4 color;
void main()
{
	vec4 glyph = (customInt1 & 1u) != 0u ? texture(texture0, uiUv) : vec4(1.0);
	glyph.rgb *= glyph.a;
	color = glyph * uiColor;
}
