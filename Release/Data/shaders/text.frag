
//! #include "uniforms.glsl"

layout(location = 0) out vec4 color;
in vec2 textureCoordinate;

void main(){
	float colorFringe = 0.0005;
	float width = 0.0;
	float contrast = 1;

	float red = texture(texture0,textureCoordinate).r;
	float green = texture(texture0,textureCoordinate - vec2(colorFringe,0)).r;
	float blue = texture(texture0,textureCoordinate - vec2(colorFringe * 2,0)).r;
	
	red = pow(red + width,contrast);
	green = pow(green + width,contrast);
	blue = pow(blue + width,contrast);
	
	vec4 textColor = vec4(red,green,blue, (red+green+blue) / 3);

	color = textColor;
}