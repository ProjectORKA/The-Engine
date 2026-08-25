
//! #include "uniforms.glsl"

layout(location = 0) out vec4 fragmentColor;
in vec2 textureCoordinate;

void main(){
	float db = texture(texture0, textureCoordinate).r;

	float normalized = clamp((db + 100.0) / 100.0, 0.0, 1.0);

	float binT       = textureCoordinate.y;
	float brightness = mix(customColor.x, customColor.y, binT);
	float contrast   = mix(customColor.z, customColor.w, binT);

	float t = clamp(normalized * brightness, 0.0, 1.0);
	if(contrast != 1.0) t = pow(t, contrast);

	fragmentColor = texture(texture1, vec2(t, 0.5));
};
