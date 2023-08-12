
//! #include "uniforms.glsl"

out float depth;
out vec3 normal;
out vec3 vertexPosition;
out vec3 worldCoordinate;
out vec2 textureCoordinate; 

void main() {

	vec3 positionInChunk = (mMatrix * vec4(vertex, 1)).xyz;
	vec3 cameraRelativePosition = positionInChunk - cameraPosition.xyz;
	vec3 customNormal = normals;

	if(bool(customInt1)){
		if(worldOffset.w < 13){

			float radius = pow(2,worldOffset.w-2)*1.2f;
			
			float height = cameraRelativePosition.z;

			float relativeCameraHeight = f1 / pow(2, 64 - worldOffset.w);

			float dist = length(vec2(cameraRelativePosition.xy));

			if(cameraRelativePosition.xy != vec2(0)){
				vec2 direction = normalize(cameraRelativePosition.xy);
				cameraRelativePosition.xy = direction * (height + radius + relativeCameraHeight) * sin(min(dist/radius,3.141));
	
				vec3 newZ = normalize(vec3(direction * sin(dist/radius),cos(dist/radius)));
				vec3 newX = normalize(cross(vec3(0,1,0),newZ));
				vec3 newY = normalize(cross(newZ,newX));
				
				customNormal = (normals.x * newX) + (normals.y * newY) + (normals.z * newZ);
			}
			cameraRelativePosition.z = (height + radius + relativeCameraHeight) * cos(dist/radius)-radius-relativeCameraHeight;
		}
	}

	vec3 worldColor = ((vertex) + vec3(0.5,0.5,0.0) + worldOffset.xyz)/vec3(pow(2,worldOffset.w),pow(2,worldOffset.w),pow(2,worldOffset.w-1));

	gl_Position  = pMatrix * vMatrix * vec4(cameraRelativePosition.xyz,1);

	// gl_Position = vpMatrix * (vec4(chunkOffsetVector,0) + (mMatrix * vec4(vertex,1)));

	vertexPosition = vertex;
	depth = gl_Position.w;
	normal = customNormal;
	textureCoordinate = uvs;
	worldCoordinate = vec3(worldOffset.xyz + vertex) / pow(2,64);
};