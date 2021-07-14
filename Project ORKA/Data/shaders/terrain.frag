#version 450
layout(location = 0) out vec4 fragmentColor;
in vec4 vertexColor;
in vec3 vertexPosition;
in vec2 textureCoordinate;
in vec3 normal;
in float depth;
in vec3 worldCoordinate;
in float slope;

layout(std140, binding = 0) uniform GlobalUniforms
{
	mat4 mMatrix;
	mat4 vpMatrix;
	
	vec4 worldOffset;
	vec4 cameraVector;		 //its vec3 but treated as vec4 in memory
	vec4 chunkOffsetVector;	 //its vec3 but treated as vec4 in memory
	vec4 customColor;

	float time;
	float custom1;
	float custom2;
	float custom3;

	bool distortion;
};

uniform sampler2D texture0;
uniform sampler2D texture1;

////////////////////////////////////////////
const vec2 zOffset = vec2(37.0,17.0);
const vec2 wOffset = vec2(59.0,83.0);

vec4 cubic(float v){
    vec4 n = vec4(1.0, 2.0, 3.0, 4.0) - v;
    vec4 s = n * n * n;
    float x = s.x;
    float y = s.y - 4.0 * s.x;
    float z = s.z - 4.0 * s.y + 6.0 * s.x;
    float w = 6.0 - x - y - z;
    return vec4(x, y, z, w) * (1.0/6.0);
}

vec4 textureBicubic(sampler2D inputTexture, vec2 texCoords){

   vec2 texSize = textureSize(inputTexture, 0);
   vec2 invTexSize = 1.0 / texSize;

   texCoords = texCoords * texSize - 0.5;


    vec2 fxy = fract(texCoords);
    texCoords -= fxy;

    vec4 xcubic = cubic(fxy.x);
    vec4 ycubic = cubic(fxy.y);

    vec4 c = texCoords.xxyy + vec2 (-0.5, +1.5).xyxy;

    vec4 s = vec4(xcubic.xz + xcubic.yw, ycubic.xz + ycubic.yw);
    vec4 offset = c + vec4 (xcubic.yw, ycubic.yw) / s;

    offset *= invTexSize.xxyy;

    vec4 sample0 = texture(inputTexture, offset.xz);
    vec4 sample1 = texture(inputTexture, offset.yz);
    vec4 sample2 = texture(inputTexture, offset.xw);
    vec4 sample3 = texture(inputTexture, offset.yw);

    float sx = s.x / (s.x + s.y);
    float sy = s.z / (s.z + s.w);

    return mix(
       mix(sample3, sample2, sx), mix(sample1, sample0, sx)
    , sy);
}

vec4 noise( in vec2 uv ){
	return textureBicubic(texture0,uv);
}

vec4 fractalNoise(in vec2 uv) {
	vec4 f;
	float impact = 1;

	for(int i = 0; i < 16; i++){
		impact /= 2;
		f  += impact*noise(uv); uv = (uv + vec2(-0.682,+0.053)) * 2.01;
	}
	return f;
}

/////////////////////////////////////////////


void main(){

	vec3 customNormal = normalize(normal + 0.5 * fractalNoise(worldCoordinate.xy).xyz);

	vec3 fragmentViewVector = normalize(vertexPosition);
	vec3 sunDir = normalize(vec3(1,1,1));
	vec3 reflection = normalize(reflect(fragmentViewVector,customNormal));

	float diffuse = clamp(dot(normalize(customNormal),sunDir),0,1);
	float specular = clamp(pow(0.25*dot(reflection, sunDir), 2.0f),0,1);
	float fresnel = clamp(0.1f * (1-dot(-fragmentViewVector,normalize(customNormal))),0,1);
	float ambient = 0.05f;

	float coloredLight = fresnel + ambient + diffuse;
	float externalLight = specular;
	
	//color = pow(slope,16) > 0.2 ? color : vec3(0.8,0.8,0.8);
	
	int uvLevel = 16;

	vec2 newtextureCorrdinate = mod((textureCoordinate / pow(2,worldOffset.w-uvLevel)) + worldOffset.xy / pow(2,64-uvLevel),1);
	//vec2 newtextureCorrdinate = vec2(worldOffset.x / pow(2,64-uvLevel));//vec2(mod(worldOffset.x,16),mod(worldOffset.y,16));

	vec3 color = texture(texture0, newtextureCorrdinate).rgb;
	//fragmentColor = texture(texture0, textureCoordinate).rgba;
	//vec3 color = vec3(newtextureCorrdinate,0);

	//draw phong lighting
	fragmentColor = vec4(color * vec3(coloredLight) + vec3(externalLight),1);

	//noise
	//fragmentColor = vec4(normalize(vec3(fractalNoise(worldCoordinate.xy))),1.0);

	//draw color only
	//fragmentColor = vec4(color,1);

	//draw heightmap
	//fragmentColor = vec4(vec3(worldCoordinate.z)*100, 1);

	//draw lighting only
	//fragmentColor = vec4(vec3(coloredLight) + vec3(externalLight),1);

	//draw worldLocation
	//fragmentColor = vec4(worldCoordinate,1.0);

	//draw normals
	//fragmentColor  = vec4(customNormal,1.0f);

	//draw vertexColor
	//fragmentColor  = vec4(vertexColor,1.0f);

	//draw uvs
	//fragmentColor  = vec4(textureCoordinate,0.0f,1.0f);

	//draw lighting
	//fragmentColor = vec4(vec3(lighting),1);
	
	//draw depth
	//fragmentColor = vec4(vec3(depth/pow(2,worldOffset.w)),1);
	
	//draw texture
	//fragmentColor = texture(texture1,textureCoordinate);
};