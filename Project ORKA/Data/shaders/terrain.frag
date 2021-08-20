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
uniform sampler2D texture2;

////////////////////////////////////////////
const vec2 zOffset = vec2(37.0,17.0);
const vec2 wOffset = vec2(59.0,83.0);


/////////////////////////////////////////////

vec2 uvCoordsAtLevel(uint level){
	return (textureCoordinate / pow(2,worldOffset.w - level)) + mod(worldOffset.xy/pow(2,64 - level),1);;
}


void main(){

	vec3 fragmentViewVector = normalize(vertexPosition);
	vec3 sunDir = normalize(vec3(1,1,1));
	vec3 reflection = normalize(reflect(fragmentViewVector,normal));

	float diffuse = clamp(dot(normalize(normal),sunDir),0,1);
	float specular = clamp(pow(0.25*dot(reflection, sunDir), 2.0f),0,1);
	float fresnel = clamp(0.1f * (1-dot(-fragmentViewVector,normalize(normal))),0,1);
	float ambient = 0.05f;

	float coloredLight = fresnel + ambient + diffuse;
	float externalLight = specular;
	
	//color = pow(slope,16) > 0.2 ? color : vec3(0.8,0.8,0.8);
	

	//vec2 newtextureCorrdinate = mod((textureCoordinate / pow(2,worldOffset.w-uvLevel)) + worldOffset.xy / pow(2,64-uvLevel),1);
	//vec2 newtextureCorrdinate = vec2(worldOffset.x / pow(2,64-uvLevel));//vec2(mod(worldOffset.x,16),mod(worldOffset.y,16));

	vec3 color = texture(texture0, uvCoordsAtLevel(0)).rgb +
	texture(texture0, uvCoordsAtLevel(4)).rgb +
	texture(texture0, uvCoordsAtLevel(8)).rgb +
	texture(texture0, uvCoordsAtLevel(12)).rgb +
	texture(texture0, uvCoordsAtLevel(16)).rgb +
	texture(texture0, uvCoordsAtLevel(20)).rgb +
	texture(texture0, uvCoordsAtLevel(24)).rgb;

	color /= 4;
	color.r = pow(color.r,2.0);
	color.g = pow(color.g,2.0);
	color.b = pow(color.b,2.0);

	//	if(distortion) color = texture(texture2, textureCoordinate).xyz;
	//	else color = normal;
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
	//fragmentColor  = vec4(vec3(abs(normal.x)),1.0f);

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