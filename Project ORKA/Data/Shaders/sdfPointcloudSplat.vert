//! #include "uniforms.glsl"

// Same curvature as Mooncrash terrain.vert
void distortPosition(inout vec3 location)
{
	float R    = pow(2.0, worldOffset.w);
	float dist = length(vec2(location.xy * 1.5)) / R;
	if (dist > 1.0)
	{
		location = vec3(0.0, 0.0, -5.0);
		return;
	}
	float func = sqrt(1.0 - pow(clamp(dist, -1.0, 1.0), 2.0));
	location.z += (func - 1.0) * R;
}

struct Point
{
	vec4 worldPos; // planet: xyz=flat/world pos, w=1; sky: xyz=view dir, w=0
	vec4 color;
};

layout(std430, binding = 1) readonly buffer PointCloud
{
	Point points[];
};

out vec4 pointColor;

void main()
{
	Point p = points[gl_VertexID];
	if (p.color.a < 0.5)
	{
		gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
		pointColor  = vec4(0.0);
		gl_PointSize = 1.0;
		return;
	}

	vec3 rel;
	bool isPlanet = p.worldPos.w > 0.5;
	if (isPlanet)
	{
		rel = p.worldPos.xyz - cameraPosition.xyz;
		// sunDir.w = splat distortion toggle
		if (sunDir.w > 0.5) distortPosition(rel);

		float nearClip = 0.001;
		float d = length(rel);
		if (d > 1e-8 && d < nearClip) rel *= nearClip / d;
	}
	else
	{
		// Sky is directional (infinity): translation must not move stars — only rotation via vMatrix.
		float skyDist = pow(2.0, worldOffset.w); // planet radius as a stable far depth
		rel = normalize(p.worldPos.xyz) * skyDist;
	}

	gl_Position  = pMatrix * vMatrix * vec4(rel, 1.0);
	pointColor   = vec4(p.color.rgb, 1.0);
	gl_PointSize = 1.0;
}
