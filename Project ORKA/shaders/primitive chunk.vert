#version 400 core

layout(location = 0) in vec3 vertex;



out vec4 vertexColor; 

uniform vec4 worldOffset;
uniform mat4 mMatrix;
uniform mat4 vpMatrix;

void main() {

	vec3 worldPos = (vertex + vec3(0.5,0.5,0.0) + worldOffset.xyz)/vec3(pow(2,worldOffset.w),pow(2,worldOffset.w),pow(2,worldOffset.w-1));
	worldPos.z = vertex.z;

	dvec3 relativePosition = (mMatrix * vec4(vertex, 1.0)).xyz;
	
	if(worldOffset.w < 10){
		double radius = pow(4,worldOffset.w)/10;
		relativePosition.z = relativePosition.z + sqrt(radius - relativePosition.x * relativePosition.x - relativePosition.y * relativePosition.y) - sqrt(radius);
	}


	gl_Position = vpMatrix * vec4(relativePosition, 1.0);


	vec3 levelColor = vec3(mod(worldOffset.w / 5.0f,0.9) + 0.1);
	vertexColor = vec4(vec3(worldPos * levelColor),1.0f - vertex.z*2);

};