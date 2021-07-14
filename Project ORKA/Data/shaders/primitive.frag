#version 450
layout(location = 0) out vec4 fragmentColor;
in vec3 vertexPosition;
in vec2 textureCoordinate;
in vec3 normal;
in float depth;
in vec3 worldCoordinate;

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

void main(){
	if(texture(texture0,textureCoordinate).a != 1) discard;
	if(normal.z<0)discard;

	vec3 fragmentViewVector = normalize(vertexPosition);
	vec3 sunDir = normalize(vec3(1,1,1));
	vec3 reflection = normalize(reflect(fragmentViewVector,normal));



	float diffuse = clamp(dot(normalize(normal),sunDir),0,1);
	float specular = clamp(1 * pow(max(dot(reflection, sunDir), 0.0f), 8.0f),0,1);
	float fresnel = clamp(0.1f * (1-dot(-fragmentViewVector,normalize(normal))),0,1);
	float ambient = 0.05f;


	float coloredLight = fresnel + ambient + diffuse;
	float externalLight = specular;
	
	//vec3 color = (worldCoordinate + texture(texture0, textureCoordinate).rgb)/2;
	//vec3 color = texture(texture0, textureCoordinate).rgb;
	//vec3 color = vec3(textureCoordinate,1);

	//
	fragmentColor = vec4((vec3(worldCoordinate.xy,0) + vertexPosition)/2,1.0f);

	//draw phong lighting
	//fragmentColor = vec4(color * vec3(coloredLight) + vec3(externalLight),1);

	//draw lighting only
	//fragmentColor = vec4(vec3(coloredLight) + vec3(externalLight),1);

	//draw worldLocation
	//fragmentColor = vec4(worldCoordinate,1.0);

	//draw normals
	//fragmentColor  = vec4(normalize(normal),1.0f);

	//draw vertexColor
	//fragmentColor  = vec4(vertexColor,1.0f);

	//draw uvs
	//fragmentColor  = vec4(textureCoordinate,0.0f,1.0f);

	//draw lighting
	//fragmentColor = vec4(vec3(lighting),1);
	
	//draw depth
	//fragmentColor = vec4(vec3(depth/pow(2,worldOffset.w)),1);
	
	//fragmentColor = vec4(vec3(sqrt(vertexColor.z / pow(2,worldOffset.w))),1);
};