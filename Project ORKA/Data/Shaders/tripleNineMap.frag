
//! #include "uniforms.glsl"

layout(location = 0) out vec4 fragmentColor;
in vec2 textureCoordinate;
in vec4 worldPosition;

//float grid(in vec2 uv){
//	float gridSize = 0.00075;
//	float threshold = 0.01;
//	float grid = 0;
//	grid += float(mod(uv.x,gridSize) > gridSize * threshold);
//	grid += float(mod(uv.y,gridSize) > gridSize * threshold);
////	grid += float(mod(pos.z,gridSize) > gridSize * threshold);
//
//	return float(grid > 1);
//}


void main(){
	fragmentColor = texture(texture0,textureCoordinate);// * ((grid(textureCoordinate)+1)/2);
};