#version 450
in vec2 textureCoordinate;
out vec4 color;

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

uniform vec2 blurSamples[48] = {
vec2(0.0,0.0),
vec2(0.14298186,0.019736316),
vec2(0.055298414,-0.1964911),
vec2(-0.22913039,-0.09999633),
vec2(-0.1505588,0.24630342),
vec2(0.24977213,0.20440288),
vec2(0.2592219,-0.2404246),
vec2(-0.21896382,-0.31287086),
vec2(-0.3633392,0.18614861),
vec2(0.14289021,0.40875712),
vec2(0.44741613,-0.09028913),
vec2(-0.029642105,-0.47779498),
vec2(-0.49858665,-0.037568044),
vec2(-0.109693944,0.50872445),
vec2(0.5074042,0.18495302),
vec2(0.2614626,-0.49410254),
vec2(-0.4685896,-0.337279),
vec2(-0.41043934,0.43093637),
vec2(0.3815155,0.47900516),
vec2(0.54110545,-0.32099566),
vec2(-0.2503299,-0.5949803),
vec2(-0.6390217,0.17073745),
vec2(0.08367965,0.67181176),
vec2(0.6921579,0.009169547),
vec2(0.10595811,-0.6991229),
vec2(-0.6920501,-0.20469469),
vec2(-0.30329078,0.6705829),
vec2(0.63467735,0.39960563),
vec2(0.49149272,-0.58460945),
vec2(-0.52097416,-0.5768471),
vec2(-0.6536521,0.44467852),
vec2(0.35692734,0.7200251),
vec2(0.77426136,-0.2592028),
vec2(-0.15323749,-0.8148732),
vec2(-0.840627,0.040981866),
vec2(-0.07543349,0.8505742),
vec2(0.84407634,0.19373989),
vec2(0.31157616,-0.820825),
vec2(-0.7808541,-0.4265367),
vec2(-0.5362215,0.7245457),
vec2(0.6526284,0.6382864),
vec2(0.73049325,-0.5661681),
vec2(-0.46655235,-0.81075823),
vec2(-0.87719804,0.35546696),
vec2(0.23486686,0.9281725),
vec2(0.9623221,-0.10694001),
vec2(0.02593312,-0.97860146),
vec2(-0.9763059,-0.16122517),
};
void main()
{
	int samples = 48;
	vec2 resolution = textureSize(texture0,0).xy;
	float radius = 0;//min(resolution.x, resolution.y)*custom1;
	vec2 pos = vec2(0);
	vec4 blurredColor = vec4(0);
	
	for(int i = 0; i < samples; i++){
		pos = (radius * blurSamples[i] + gl_FragCoord.xy) / resolution;
		blurredColor += texture(texture0,pos) * texture(texture0,pos);
	}
	color = sqrt(blurredColor/samples);
}