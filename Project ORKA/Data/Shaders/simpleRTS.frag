
//! #include "uniforms.glsl"

layout(location = 0) out vec4 gBufferColor;
layout (location = 1) out vec3 gBufferNormal;
layout (location = 2) out vec3 gBufferPosition;
layout (location = 3) out uint gBufferMaterialID;

in vec2 textureCoordinate;
in vec3 normal;
in vec4 worldPosition;
in vec3 vertexColor;


float fresnel(in vec3 I, in vec3 N, in float ior) 
{ 
    float cosi = clamp(-1, 1, dot(I, N)); 
    float etai, etat;
    if (cosi > 0) {
        etai = ior;
        etat = 1;
    }else{
        etat = ior;
        etai = 1;
    }
    // Compute sini using Snell's law
    float sint = etai / etat * sqrt(max(0.f, 1 - cosi * cosi)); 
    // Total internal reflection
    
    float kr;
    if (sint >= 1) { 
        kr = 1; 
    } 
    else { 
        float cost = sqrt(max(0.f, 1 - sint * sint)); 
        cosi = abs(cosi); 
        float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost)); 
        float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost)); 
        kr = (Rs * Rs + Rp * Rp) / 2; 
    }
    return cosi / 5;
    // As a consequence of the conservation of energy, transmittance is given by:
    // kt = 1 - kr;
};

float fogFunction(in float range){
    return 1.0f - 1.0f / pow(abs(range) + 1,2);
};

void main(){
    // define the sky color so we can fake some sky light
    vec3 skyColor = vec3(0.207143f, 0.722031f, 1.0f);
    vec3 sunColor = vec3(1.0f, 0.9f, 0.6f) * 3;

    // calculate the correct view position
    // this needs to happen in the fragment shader
    vec3 viewPosition = normalize(worldPosition.xyz - cameraPosition.xyz);

    // calculate the incoming light
    // get the ambient light
	vec3 ambient = skyColor / 5;
	float sunlight = max(dot(normal,normalize(vec3(sunDir))),0);
    vec3 light = vec3(sunColor * sunlight + ambient);

    // calculate reflection
	float fres = pow(1-dot(-viewPosition,normal),8) / 50;
    vec3 refl = skyColor * fres;
    
    // calculate fog
    float fog = clamp(fogFunction(distance(worldPosition.xyz,cameraPosition.xyz)/10000),0,1);


	gBufferColor = vec4(mix(vertexColor * customColor.xyz * light + refl, skyColor, fog), 1);
    gBufferPosition  = worldPosition.xyz;
	gBufferNormal = normal;
};