
//! #include "uniforms.glsl"

layout(location = 0) out vec4 fragmentColor;

#define iResolution vec3(float(framebufferWidth), float(framebufferHeight), 1.0)
#define iMouse customColor
#define iChannel0 texture0
#define iChannel1 texture1
#define iChannel2 texture2

#define BUFFER_SIZE iResolution.xy

vec2 hash(in vec2 x)
{
	const vec2 k = vec2(0.3183099, 0.3678794);
	x = x * k + k.yx;
	return -1.0 + 2.0 * fract(16.0 * k * fract(x.x * x.y * (x.x + x.y)));
}

// from https://www.shadertoy.com/view/XdXBRH
vec3 noised(in vec2 p)
{
	vec2 i = floor(p);
	vec2 f = fract(p);

	vec2 u  = f * f * f * (f * (f * 6.0 - 15.0) + 10.0);
	vec2 du = 30.0 * f * f * (f * (f - 2.0) + 1.0);

	vec2 ga = hash(i + vec2(0.0, 0.0));
	vec2 gb = hash(i + vec2(1.0, 0.0));
	vec2 gc = hash(i + vec2(0.0, 1.0));
	vec2 gd = hash(i + vec2(1.0, 1.0));

	float va = dot(ga, f - vec2(0.0, 0.0));
	float vb = dot(gb, f - vec2(1.0, 0.0));
	float vc = dot(gc, f - vec2(0.0, 1.0));
	float vd = dot(gd, f - vec2(1.0, 1.0));

	return vec3(va + u.x * (vb - va) + u.y * (vc - va) + u.x * u.y * (va - vb - vc + vd),
	            ga + u.x * (gb - ga) + u.y * (gc - ga) + u.x * u.y * (ga - gb - gc + gd) +
	                du * (u.yx * (va - vb - vc + vd) + vec2(vb, vc) - va));
}

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
	if (fragCoord.x >= BUFFER_SIZE.x || fragCoord.y >= BUFFER_SIZE.y) return;

	vec2 uv = fragCoord / BUFFER_SIZE;

	vec3 color = vec3(0.0);

	float a = 0.5;
	float f = 2.0;
	for (int i = 0; i < 8; i++)
	{
		color += noised(uv * f) * a;
		a *= 0.95;
		f *= 2.0;
	}

	fragColor = vec4(color, 1.0);
}

void main()
{
	mainImage(fragmentColor, gl_FragCoord.xy);
}

