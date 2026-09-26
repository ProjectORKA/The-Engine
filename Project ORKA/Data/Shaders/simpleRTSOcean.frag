
//! #include "uniforms.glsl"

layout(location = 0) out vec4 gBufferColor;
layout (location = 1) out vec3 gBufferNormal;
layout (location = 2) out vec3 gBufferPosition;
layout (location = 3) out uint gBufferMaterialID;

in vec3 normal;
in vec4 worldPosition;
in vec3 vertexColor;

float fogFunction(in float range){
    return 1.0f - 1.0f / pow(abs(range) + 1,2);
};

void main(){
    vec3 skyColor = vec3(0.207143f, 0.722031f, 1.0f);
    vec3 sunColor = vec3(1.0f, 0.9f, 0.6f) * 2;

    vec3 viewPosition = normalize(worldPosition.xyz - cameraPosition.xyz);

	vec3 ambient = skyColor / 4;
	float sunlight = max(dot(normal, normalize(vec3(sunDir))), 0);
    vec3 light = vec3(sunColor * sunlight * 0.35 + ambient);

	float fres = pow(1.0 - dot(-viewPosition, normal), 5.0) / 12.0;
    vec3 refl = skyColor * fres;

    float fog = clamp(fogFunction(distance(worldPosition.xyz, cameraPosition.xyz) / 10000.0), 0, 1);

    vec3 water = vertexColor * (light + 0.25) + refl;

	gBufferColor = vec4(mix(water, skyColor, fog), 1);
    gBufferPosition = worldPosition.xyz;
	gBufferNormal = normal;
};
