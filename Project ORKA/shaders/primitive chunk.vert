#version 330 core

layout(location = 0) in vec3 vertex;

out vec4 vertexColor; 

uniform mat4 vpMatrix;
uniform vec4 worldOffset;

void main() {

    gl_Position = vpMatrix * vec4(vertex, 1.0);
	
	vec3 worldPos = (vertex + vec3(0.5,0.5,0.0) + worldOffset.xyz)/vec3(pow(2,worldOffset.w),pow(2,worldOffset.w),pow(2,worldOffset.w-1));
	vec3 levelColor = vec3(mod(worldOffset.w / 5.0f,1.0));
	vertexColor = vec4(vec3(1 * (1 - levelColor)),1.0f - vertex.z*2);

};