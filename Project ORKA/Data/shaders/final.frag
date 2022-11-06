
//! #include "uniforms.glsl"

layout(location = 0) out vec4 fragmentColor;
in vec2 textureCoordinate;

uniform usampler2D utexture0;

vec3 hash3( uint n ) 
{
    if(n == 0) return vec3(0);
    // integer hash copied from Hugo Elias
	n = (n << 13U) ^ n;
    n = n * (n * n * 15731U + 789221U) + 1376312589U;
    uvec3 k = n * uvec3(n,n*16807U,n*48271U);
    return vec3( k & uvec3(0x7fffffffU))/float(0x7fffffff);
}

void main(){
	//uvec4 color = texture(utexture0,textureCoordinate);
    //vec3 col = hash3(color.z);
    //col = texture(texture0,textureCoordinate).xyz;
	//fragmentColor = vec4((hash3(color.x) + hash3(color.z)) / 2.0f, 1.0f);
    //fragmentColor = vec4(hash3(texture(utexture0,textureCoordinate).x),1);
    vec4 color = vec4(texture(texture0,textureCoordinate));
    
    //gamma correction
    //color = vec4(pow(color.rgb, vec3(1.0/2.2)),1);
    fragmentColor = color;
};