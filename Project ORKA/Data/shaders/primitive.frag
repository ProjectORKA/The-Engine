#version 400

layout(location = 0) out vec4 color;

in vec4 vertexColor;
in vec3 vertexPosition;
in vec2 textureCoordinate;
in vec3 normal;
in float depth;
in vec3 worldCoordinate;

uniform vec3 cameraVector;
uniform vec4 worldOffset;

uniform sampler2D baseColor;
//uniform sampler2D normalMap;


void main(){
	if(texture(baseColor,textureCoordinate).a != 1) discard;
	if(normal.z<0)discard;

	vec3 fragmentViewVector = normalize(vertexPosition);
	vec3 sunDir = normalize(vec3(1,1,1));
	vec3 reflection = normalize(reflect(fragmentViewVector,normal));



	float diffuse = clamp(dot(normalize(normal),sunDir),0,1);
	float specular = clamp(1 * pow(max(dot(reflection, sunDir), 0.0f), 8.0f),0,1);
	float fresnel = clamp(0.2f * (1-dot(-fragmentViewVector,normalize(normal))),0,1);
	float ambient = 0.05f;

	float coloredLight = fresnel + ambient + diffuse;
	float externelLight = specular;
	
	vec3 fragmentColor = (worldCoordinate.xyz + texture(baseColor, textureCoordinate).rgb) / 2;
	//vec3 fragmentColor = vec3(textureCoordinate,1);

	//draw phong lighting
	color = vec4(fragmentColor * vec3(coloredLight) + vec3(externelLight),1);

	//draw worldLocation
	//color = vec4(worldCoordinate,1.0);

	//draw normals
	//color  = vec4(normalize(normal),1.0f);

	//draw vertexColor
	//color  = vec4(vertexColor,1.0f);

	//draw uvs
	//color  = vec4(textureCoordinate,0.0f,1.0f);

	//draw lighting
	//color = vec4(vec3(lighting),1);
	
	//draw depth
	//color = vec4(vec3(depth/pow(2,worldOffset.w)),1);
	
	//color = vec4(vec3(sqrt(vertexColor.z / pow(2,worldOffset.w))),1);
};