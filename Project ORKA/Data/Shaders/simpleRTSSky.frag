
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
    vec3 skyColor = vec3(0.207143f, 0.722031f, 1.0f);
    vec3 worldPos = worldPosition.xyz;

    if(worldPosition.z < 0.0){
        worldPos = vec3((normalize(worldPosition) / abs(normalize(worldPosition).z)) * cameraPosition.z);
    }

    // calculate fog
    float fog = clamp(fogFunction(distance(worldPos.xyz,cameraPosition.xyz)/10000),0,1);

    vec3 color = texture2D(texture0, textureCoordinate).xyz;

	gBufferColor = vec4(mix(color, skyColor, fog), 1);
    gBufferPosition  = worldPosition.xyz;
	gBufferNormal = normal;
};