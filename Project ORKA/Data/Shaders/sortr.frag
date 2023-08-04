
//! #include "uniforms.glsl"

layout(location = 0) out vec4 fragmentColor;
in vec2 textureCoordinate;

void main(){
	vec4 col = texture(texture0,textureCoordinate);
	col.rgb = col.rgb * col.a;
	fragmentColor = col;
};