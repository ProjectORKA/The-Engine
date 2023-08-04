
//! #include "uniforms.glsl"

in vec3 rayOrigin;
in vec3 rayDirection;

layout(location = 0) out vec4 fragmentColor;

float eps = 0.0001f;
float maxDist = 1000.0f;
int steps = 512;

float plane(vec3 p){
	return p.z;
}

float sphere( vec3 p, vec3 l, float s )
{
  return length(p-l)-s;
}

float box( vec3 p, vec3 b )
{
  vec3 q = abs(p) - b;
  return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

float scene(vec3 p){
	return
	min(
		min(
			min(
				box(p,vec3(1)),
				sphere(p, vec3(0,3,1), 1)
			),
			sphere(p, vec3(1,3,0.5), 1)
		),
		plane(p)
	) - 0.01;
}

vec3 getNormal(vec3 p)
{
    vec2 e = vec2(eps, 0.);
    vec3 n = scene(p) - vec3(
        scene(p - e.xyy),
        scene(p - e.yxy),
        scene(p - e.yyx)
    );
    return normalize(n);
}

float calcSoftshadow( in vec3 ro, in vec3 rd, in float mint, in float tmax, int technique )
{
	float res = 1.0;
    float t = mint;
    float ph = 1e10; // big, such that y = 0 on the first iteration
    
    for( int i=0; i<32; i++ )
    {
		float h = scene( ro + rd*t );

        // traditional technique
        if( technique==0 )
        {
        	res = min( res, 10.0*h/t );
        }
        // improved technique
        else
        {
            // use this if you are getting artifact on the first iteration, or unroll the
            // first iteration out of the loop
            // float y = (i==0) ? 0.0 : h*h/(2.0*ph); 

            float y = h*h/(2.0*ph);
            float d = sqrt(h*h-y*y);
            res = min( res, 10.0*d/max(0.0,t-y) );
            ph = h;
        }
        
        t += h;
        
        if( res<eps || t>tmax ) break;
        
    }
    res = clamp( res, 0.0, 1.0 );
    return res*res*(3.0-2.0*res);
}

float ao(vec3 p, vec3 n){
    
    float target = eps;
    
    float ao = 0;
    float aoMaxDistance = 11;
    float exponentialsteps = 1.5;
    int aoSamples = 16;

    for(int i = 0; i < aoSamples; i++){
        vec3 samplePoint = p + n * pow(exponentialsteps,-i) * aoMaxDistance;
        float distanceTosamplePoint = distance(p,samplePoint);
        ao += scene(samplePoint) / distanceTosamplePoint;
    }

    return ao / aoSamples;
}

void main(){


	vec3 dir = normalize(rayDirection);
	vec3 orig = cameraPosition.xyz;
	vec3 curr = orig;

	eps = scene(orig) / 10000;
    maxDist = 1000;


	float dist = 4 * eps;


	int i = 0;

	while(dist < maxDist && dist > eps){
		dist = scene(curr);
		curr += dir * dist;
		if(i++ > steps) break;
	}

	vec3 normal = normalize(getNormal(curr));

	vec3 surfacePos = curr;

	vec3 col = vec3(dot(normal, sunDir.xyz));

	float shadow = (1+calcSoftshadow(surfacePos,sunDir.xyz,eps * 100, 10.0, 1))/2; 
	
	float ambientOcclusion = (1 + ao(surfacePos, normal))/2;

	fragmentColor = vec4(col * ambientOcclusion * shadow,1);
};