
//! #include "uniforms.glsl"

layout(location = 0) out vec4 gBufferColor;
layout (location = 1) out vec3 gBufferNormal;
layout (location = 2) out vec3 gBufferPosition;
layout (location = 3) out uint gBufferMaterialID;

in vec4 vertexColor;
in vec3 vertexPosition;
in vec2 textureCoordinate;
in vec3 normal;
in float depth;
in vec3 worldCoordinate;
in float slope;

////////////////////////////////////////////
const vec2 zOffset = vec2(37.0,17.0);
const vec2 wOffset = vec2(59.0,83.0);
/////////////////////////////////////////////

vec2 uvCoordsAtLevel(uint level){
	return (textureCoordinate / pow(2,worldOffset.w - level)) + mod(worldOffset.xy/pow(2,64 - level),1);
}

vec3 mix(vec3 a, vec3 b, float alpha){
	return a * (1- alpha) + b * alpha;
}

void main(){

	vec3 fragmentViewVector = normalize(vertexPosition);
	//vec3 sunDir = normalize(vec3(1,1,1));
	vec3 reflection = normalize(reflect(fragmentViewVector,normal));

	float diffuse = clamp(dot(normalize(normal),sunDir.xyz),0,1);
	float specular = pow(clamp(dot(reflection, sunDir.xyz),0,1), 4.0f) * 0.1;
	float fresnel = pow(clamp(1-dot(-fragmentViewVector,normalize(normal)),0,1),8) * 0.01f;
	float ambient = 0.0001f;

	float coloredLight = fresnel + ambient + diffuse;

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
	color = pow(color,vec3(2.0));

	float slopeImpact =clamp(1-slope,0,1);

	color = mix(color,vec3(0.2), slopeImpact);
	
	vec3 deepWater = vec3(0.0f, 31.0f, 84.0f) / 400.0f;
	vec3 beachWater = vec3(0.0f, 157.0f, 255.0f) / 100.0f;
	vec3 belowWaterline = mix(beachWater,deepWater,clamp(pow(4*(0.5 - worldCoordinate.z),0.01),0,1));
	if(worldCoordinate.z < 0.5) color = mix(color,belowWaterline,0.75);

	//	if(distortion) color = texture(texture2, textureCoordinate).xyz;
	//	else color = normal;
	//vec3 color = vec3(newtextureCorrdinate,0);

	//draw phong lighting
	gBufferColor = vec4(pow(color,vec3(2)) * vec3(coloredLight * 10),1);

	//gBufferColor = vec4(vec3(color),1);

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