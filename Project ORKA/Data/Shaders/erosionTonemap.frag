//! #include "uniforms.glsl"

layout(location = 0) out vec4 fragmentColor;

#define iResolution vec3(float(framebufferWidth), float(framebufferHeight), 1.0)
#define iChannel0 texture0 // HDR color
#define iChannel1 texture1 // dither

// Same ACES fit as scene shader
vec3 Tonemap_ACES(vec3 x)
{
	const float a = 2.51;
	const float b = 0.03;
	const float c = 2.43;
	const float d = 0.59;
	const float e = 0.14;
	return (x * (a * x + b)) / (x * (c * x + d) + e);
}

void main()
{
	vec2 uv  = gl_FragCoord.xy / iResolution.xy;
	vec3 hdr = texture(iChannel0, uv).rgb;

	// Min / max luminance passed via customColor.x/y
	float minLum = customColor.x;
	float maxLum = max(customColor.y, minLum + 1e-6);

	// Naive remap: lowest -> black, highest -> white
	vec3 color = clamp((hdr - minLum) / (maxLum - minLum), 0.0, 1.0);

	// Simple display gamma
	color = pow(color, vec3(1.0 / 2.2));

	// Small blue-noise style dithering to fight 8-bit banding
	vec3 dither = texture(iChannel1, uv * iResolution.xy * 0.5).rgb - 0.5;
	color += dither * (1.0 / 255.0);

	fragmentColor = vec4(clamp(color, 0.0, 1.0), 1.0);
}

