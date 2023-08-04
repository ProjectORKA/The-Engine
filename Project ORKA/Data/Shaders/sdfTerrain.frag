
//! #include "uniforms.glsl"

in vec3 rayOrigin;
in vec3 rayDirection;

layout(location = 0) out vec4 fragmentColor;

float eps = 0.001f;

// vec3 getNormal( inout vec3 p )
// {
//    return normalize( vec3( f(p.x-eps,p.z) - f(p.x+eps,p.z),
//                            2.0f*eps,
//                            f(p.x,p.z-eps) - f(p.x,p.z+eps) ) );
// }

float noise(vec2 uv){
	return texture(texture1,uv).r;
}

void main(){
	
	vec3 dir = normalize(rayDirection);


	fragmentColor = vec4(vec3(noise(dir.xy)),1);
	
	gl_FragDepth = 0.5f;
};