in vec2 uiUv;
in vec4 uiColor;
out vec4 color;
void main()
{
	float coverage = (customInt1 & 1u) != 0u ? texture(texture0, uiUv).r : 1.0;
	color = uiColor * coverage;
}
