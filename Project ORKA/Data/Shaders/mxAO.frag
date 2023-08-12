
#ifndef MXAO_MIPLEVEL_AO
 #define MXAO_MIPLEVEL_AO		0	// [0 to 2]      Miplevel of AO texture. 0 = fullscreen, 1 = 1/2 screen width/height, 2 = 1/4 screen width/height and so forth. Best results: IL MipLevel = AO MipLevel + 2
#endif

#ifndef MXAO_MIPLEVEL_IL
 #define MXAO_MIPLEVEL_IL		2	// [0 to 4]      Miplevel of IL texture. 0 = fullscreen, 1 = 1/2 screen width/height, 2 = 1/4 screen width/height and so forth.
#endif

#ifndef MXAO_ENABLE_IL
 #define MXAO_ENABLE_IL			0	// [0 or 1]	    Enables Indirect Lighting calculation. Will cause a major fps hit.
#endif

#ifndef MXAO_SMOOTHNORMALS
 #define MXAO_SMOOTHNORMALS     0   // [0 or 1]      This feature makes low poly surfaces smoother, especially useful on older games.
#endif

#ifndef MXAO_TWO_LAYER
 #define MXAO_TWO_LAYER         0   // [0 or 1]      Splits MXAO into two separate layers that allow for both large and fine AO.
#endif

#ifndef MXAO_HQ
 #define MXAO_HQ                0   // [0 or 1]      Enables a different, more physically accurate but slower SSAO mode. Based on Ground Truth Ambient Occlusion by Activision. No IL yet.
#endif

/*=============================================================================
	UI Uniforms
=============================================================================*/

// Sample Quality
// Very Low  (4 samples)
// Low (8 samples)
// Medium (16 samples)
// High (24 samples)
// Very High (32 samples)
// Ultra (64 samples)
// Maximum (255 samples)
// Global quality control, main performance knob. Higher radii might require higher quality.
int MXAO_GLOBAL_SAMPLE_QUALITY_PRESET = 2;

// Sample Radius
// min = 0.5
// max = 20.0
// Sample radius of MXAO, higher means more large-scale occlusion with less fine-scale details.
float MXAO_SAMPLE_RADIUS = 2.5;


#if (MXAO_HQ==0)
    // Normal Bias
    // min 0.0
    // max 0.8
    // Occlusion Cone bias to reduce self-occlusion of surfaces that have a low angle to each other.
    float MXAO_SAMPLE_NORMAL_BIAS = 0.2;
#else
    float MXAO_SAMPLE_NORMAL_BIAS = 0       // don't break PS which needs this, cleaner this way
#endif

// Render Size Scale
// min 0.5
// max 1.0
// Factor of MXAO resolution, lower values greatly reduce performance overhead but decrease quality.
// 1.0 = MXAO is computed in original resolution
// 0.5 = MXAO is computed in 1/2 width 1/2 height of original resolution
// ...
float MXAO_GLOBAL_RENDER_SCALE = 1.0;

// Ambient Occlusion Amount
// min 0.0
// max 4.0
// Intensity of AO effect. Can cause pitch black clipping if set too high.
float MXAO_SSAO_AMOUNT = 1.00;


// Indirect Lighting Amount
// min 0.00
// max 12.0
// Intensity of IL effect. Can cause overexposured white spots if set too high.
float MXAO_SSIL_AMOUNT = 4.0f;

// Indirect Lighting Saturation
// min 0.0f
// max 3.0f
// Controls color saturation of IL effect.
float MXAO_SSIL_SATURATION = 1.0f;


#if (MXAO_TWO_LAYER != 0)
    float MXAO_SAMPLE_RADIUS_SECONDARY = 0.2;
//        ui_type = "drag";
//        ui_min = 0.1; ui_max = 1.00;
//        ui_label = "Fine AO Scale";
//        ui_tooltip = "Multiplier of Sample Radius for fine geometry. A setting of 0.5 scans the geometry at half the radius of the main AO.";
//        ui_category = "Double Layer";
//    > 

    float MXAO_AMOUNT_FINE = 1.0f;
//        ui_type = "drag";
//        ui_min = 0.00; ui_max = 1.00;
//        ui_label = "Fine AO intensity multiplier";
//        ui_tooltip = "Intensity of small scale AO / IL.";
//        ui_category = "Double Layer";
//    > = 1.0;

    float MXAO_AMOUNT_COARSE = 1.0;
//        ui_type = "drag";
//        ui_min = 0.00; ui_max = 1.00;
//        ui_label = "Coarse AO intensity multiplier";
//        ui_tooltip = "Intensity of large scale AO / IL.";
//        ui_category = "Double Layer";
//    > = 1.0;
#endif

int MXAO_BLEND_TYPE = 0;
//	ui_type = "slider";
//	ui_min = 0; ui_max = 3;
//    ui_label = "Blending Mode";
//	ui_tooltip = "Different blending modes for merging AO/IL with original color.\0Blending mode 0 matches formula of MXAO 2.0 and older.";
//    ui_category = "Blending";
// > = 0;

float MXAO_FADE_DEPTH_START = 0.05;
//	ui_type = "drag";
//    ui_label = "Fade Out Start";
//	ui_min = 0.00; ui_max = 1.00;
//	ui_tooltip = "Distance where MXAO starts to fade out. 0.0 = camera, 1.0 = sky. Must be less than Fade Out End.";
//    ui_category = "Blending";
// > = 0.05;

float MXAO_FADE_DEPTH_END = 0.4;
//	ui_type = "drag";
//    ui_label = "Fade Out End";
//	ui_min = 0.00; ui_max = 1.00;
//	ui_tooltip = "Distance where MXAO completely fades out. 0.0 = camera, 1.0 = sky. Must be greater than Fade Out Start.";
//    ui_category = "Blending";
// > = 0.4;

int MXAO_DEBUG_VIEW_ENABLE = 0;
//	ui_type = "combo";
//    ui_label = "Enable Debug View";
//	ui_items = "None\0AO/IL channel\0Normal vectors\0";
//	ui_tooltip = "Different debug outputs";
//    ui_category = "Debug";
// > = 0;

/*=============================================================================
	Textures, Samplers, Globals
=============================================================================*/

#define QUINT_COMMON_VERSION_REQUIRE 200

#ifndef QUINT_COMMON_VERSION
 #define QUINT_COMMON_VERSION 200
#endif

#if QUINT_COMMON_VERSION_REQUIRE > QUINT_COMMON_VERSION
 #error "qUINT_common.fxh outdated."
 #error "Please download update from github.com/martymcmodding/qUINT"
#endif

#if !defined(QUINT_COMMON_VERSION_REQUIRE)
 #error "Incompatible qUINT_common.fxh and shaders."
 #error "Do not mix different file versions."
#endif

/*=============================================================================
	Define defaults
=============================================================================*/

float saturate(in float value){
    return clamp(value,0.0,1.0);
}

vec4 saturate(in vec4 value){
    return clamp(value,0.0,1.0);
}

vec2 saturate(in vec2 value){
    return clamp(value,0.0,1.0);
}

vec3 saturate(in vec3 value){
    return clamp(value,0.0,1.0);
}

const bool RESHADE_DEPTH_INPUT_IS_UPSIDE_DOWN = false;
const bool RESHADE_DEPTH_INPUT_IS_REVERSED = true;
const bool RESHADE_DEPTH_INPUT_IS_LOGARITHMIC = false;
const float RESHADE_DEPTH_LINEARIZATION_FAR_PLANE = 1000.0;


/*=============================================================================
	Uniforms
=============================================================================*/

uniform int BUFFER_WIDTH;
uniform int BUFFER_HEIGHT;
uniform int BUFFER_RCP_HEIGHT;
uniform int BUFFER_RCP_WIDTH;

// namespace qUINT_
// {
//    float FRAME_TIME = 0.0f; // [TODO] add frametime
//	int FRAME_COUNT = 0; // [TODO] add framecount

    // vec2 qUINT_ASPECT_RATIO = vec2(1.0, BUFFER_WIDTH * BUFFER_RCP_HEIGHT);
	// vec2 qUINT_PIXEL_SIZE   = vec2(BUFFER_RCP_WIDTH, BUFFER_RCP_HEIGHT);
	// vec2 SCREEN_SIZE        = vec2(BUFFER_WIDTH, BUFFER_HEIGHT);

	// Global textures and samplers
    // textures
	uniform sampler2D qUINT_BackBufferTex;   // color texture
	uniform sampler2D qUINT_DepthBufferTex;  // depth texture
    // sampler
	uniform sampler2D qUINT_sBackBufferTex;  // color sampler
	uniform sampler2D qUINT_sDepthBufferTex; // depth sampler

    // Helper functions
	float qUINT_linear_depth(vec2 uv)
	{

        if(RESHADE_DEPTH_INPUT_IS_UPSIDE_DOWN) uv.y = 1.0 - uv.y;

		float depth = texture(qUINT_sDepthBufferTex, uv).x;

        if(RESHADE_DEPTH_INPUT_IS_LOGARITHMIC){
            const float C = 0.01;
            depth = (exp(depth * log(C + 1.0)) - 1.0) / C;
        }
        
        if(RESHADE_DEPTH_INPUT_IS_REVERSED){
            depth = 1.0 - depth;
        }
		
        const float N = 1.0;
		depth /= RESHADE_DEPTH_LINEARIZATION_FAR_PLANE - depth * (RESHADE_DEPTH_LINEARIZATION_FAR_PLANE - N);

		return saturate(depth);
	}
// }

// Vertex shader generating a triangle covering the entire screen
// void PostProcessVS(in uint id : SV_VertexID, out vec4 vpos : SV_Position, out vec2 uv : TEXCOORD)
// {
//	uv.x = (id == 2) ? 2.0 : 0.0;
//	uv.y = (id == 1) ? 2.0 : 0.0;
//	vpos = vec4(uv * vec2(2.0, -2.0) + vec2(-1.0, 1.0), 0.0, 1.0);
// }



// texture2D MXAO_ColorTex 	{ Width = BUFFER_WIDTH;   Height = BUFFER_HEIGHT;   Format = RGBA8; MipLevels = 3+MXAO_MIPLEVEL_IL;};
// texture2D MXAO_DepthTex 	{ Width = BUFFER_WIDTH;   Height = BUFFER_HEIGHT;   Format = R16F;  MipLevels = 3+MXAO_MIPLEVEL_AO;};
// texture2D MXAO_NormalTex	{ Width = BUFFER_WIDTH;   Height = BUFFER_HEIGHT;   Format = RGBA8; MipLevels = 3+MXAO_MIPLEVEL_IL;};

// { Texture = MXAO_ColorTex;	};
uniform sampler2D sMXAO_ColorTex;
// { Texture = MXAO_DepthTex;	};
uniform sampler2D sMXAO_DepthTex;
// { Texture = MXAO_NormalTex;	};
uniform sampler2D sMXAO_NormalTex;

// texture2D CommonTex0 	{ Width = BUFFER_WIDTH;   Height = BUFFER_HEIGHT;   Format = RGBA8; };
// { Texture = CommonTex0;	};
uniform sampler2D sCommonTex0;

//texture2D CommonTex1 	{ Width = BUFFER_WIDTH;   Height = BUFFER_HEIGHT;   Format = RGBA8; };
//{ Texture = CommonTex1;	};
uniform sampler2D sCommonTex1;

/*=============================================================================
	Vertex Shader
=============================================================================*/

//struct MXAO_VSOUT
//{
//	vec4                  vpos        : SV_Position;
//    vec4                  uv          : TEXCOORD0;
//    nointerpolation float   samples     : TEXCOORD1;
//    nointerpolation vec3  uvtoviewADD : TEXCOORD4;
//    nointerpolation vec3  uvtoviewMUL : TEXCOORD5;
//};

struct MXAO_VSOUT
{
	vec4 vpos;
    vec4 uv;
    float samples;
    vec3 uvtoviewADD;
    vec3 uvtoviewMUL;
};

struct BlurData
{
	vec4 key;
	vec4 mask;
};

int samples_per_preset[8] = int[8](4, 8, 16, 24, 32, 64, 255, 8 /*overridden*/);

MXAO_VSOUT VS_MXAO(in uint id)
{
    MXAO_VSOUT MXAO;

    MXAO.uv.x = (id == uint(2)) ? 2.0 : 0.0;
    MXAO.uv.y = (id == uint(1)) ? 2.0 : 0.0;
    MXAO.uv.zw = MXAO.uv.xy / MXAO_GLOBAL_RENDER_SCALE;
    MXAO.vpos = vec4(MXAO.uv.xy * vec2(2.0, -2.0) + vec2(-1.0, 1.0), 0.0, 1.0);

    
    MXAO.samples   = float(samples_per_preset[MXAO_GLOBAL_SAMPLE_QUALITY_PRESET]);
    
    MXAO.uvtoviewADD = vec3(-1.0,-1.0,1.0);
    MXAO.uvtoviewMUL = vec3(2.0,2.0,0.0);

#if 0
    static const float FOV = 75; //vertical FoV
    MXAO.uvtoviewADD = vec3(-tan(radians(FOV * 0.5)).xx,1.0) * qUINT::ASPECT_RATIO.yxx;
   	MXAO.uvtoviewMUL = vec3(-2.0 * MXAO.uvtoviewADD.xy,0.0);
#endif

    return MXAO;
}

/*=============================================================================
	Functions
=============================================================================*/

vec3 get_position_from_uv(in vec2 uv, in MXAO_VSOUT MXAO)
{
    return (uv.xyx * MXAO.uvtoviewMUL + MXAO.uvtoviewADD) * qUINT_linear_depth(uv) * RESHADE_DEPTH_LINEARIZATION_FAR_PLANE;
}

vec3 get_position_from_uv_mipmapped(in vec2 uv, in MXAO_VSOUT MXAO, in float miplevel)
{
    return (uv.xyx * MXAO.uvtoviewMUL + MXAO.uvtoviewADD) * textureLod(sMXAO_DepthTex, uv, miplevel).x;
}

void spatial_blur_data(inout BlurData o, in sampler2D inputsampler, in float inputscale, in vec2 uv, in float miplevel)
{
	o.key = textureLod(inputsampler, uv * vec2(inputscale), miplevel * inputscale);
	o.mask = textureLod(sMXAO_NormalTex, uv, miplevel);
	o.mask.xyz = o.mask.xyz * vec3(2) - vec3(1);
}

float compute_spatial_tap_weight(in BlurData center, in BlurData tap)
{
	float depth_term = saturate(1.0f - abs(tap.mask.w - center.mask.w));
	float normal_term = saturate(dot(tap.mask.xyz, center.mask.xyz) * 16.0f - 15.0f);
	return depth_term * normal_term;
}

vec4 blur_filter(in MXAO_VSOUT MXAO, in sampler2D inputsampler, in float inputscale, in float radius, in int blursteps)
{
	vec2 blur_uv = MXAO.uv.xy;

    BlurData center, tap;
	spatial_blur_data(center, inputsampler, inputscale, blur_uv,0.0f);

	vec4 blursum 			= center.key;
	vec4 blursum_noweight = center.key;
	float blurweight = 1.0f;

    const vec2 offsets[8] = 
    vec2[8](
    	vec2(1.5,0.5),vec2(-1.5,-0.5),vec2(-0.5,1.5),vec2(0.5,-1.5),
        vec2(1.5,2.5),vec2(-1.5,-2.5),vec2(-2.5,1.5),vec2(2.5,-1.5)
    );

    vec2 blur_offsetscale = vec2(BUFFER_RCP_WIDTH, BUFFER_RCP_HEIGHT) / inputscale * radius;


	for(int i = 0; i < blursteps; i++) 
	{
		blur_uv.xy = MXAO.uv.xy + offsets[i] * blur_offsetscale;
		spatial_blur_data(tap, inputsampler, inputscale, blur_uv, 0.0);

		float tap_weight = compute_spatial_tap_weight(center, tap);

		blurweight += tap_weight;
		blursum.xyzw += tap.key.xyzw * tap_weight;
		blursum_noweight.xyzw += tap.key.xyzw;
	}

	blursum.xyzw /= blurweight;
	blursum_noweight.xyzw /= vec4(1 + blursteps);
    
    if(blurweight < 2.0f){
        return vec4(blursum_noweight.xyzw);
    } else {
        return vec4(blursum.xyzw);
    }
}

void sample_parameter_setup(in MXAO_VSOUT MXAO, in float scaled_depth, in float layer_id, out float scaled_radius, out float falloff_factor)
{
    scaled_radius  = 0.25 * MXAO_SAMPLE_RADIUS / (MXAO.samples * (scaled_depth + 2.0));
    falloff_factor = -1.0/(MXAO_SAMPLE_RADIUS * MXAO_SAMPLE_RADIUS);

    #if(MXAO_TWO_LAYER != 0)
        scaled_radius  *= mix(1.0, MXAO_SAMPLE_RADIUS_SECONDARY + 1e-6, layer_id);
        falloff_factor *= mix(1.0, 1.0 / (MXAO_SAMPLE_RADIUS_SECONDARY * MXAO_SAMPLE_RADIUS_SECONDARY + 1e-6), layer_id);
    #endif
}

void sincos(in float value, out float sinresult, out float cosresult){
    sinresult = sin(value);
    cosresult = cos(value);
}

void smooth_normals(inout vec3 normal, in vec3 position, in MXAO_VSOUT MXAO)
{
    vec2 scaled_radius = 0.018 / position.z * vec2(1.0, BUFFER_WIDTH * BUFFER_RCP_HEIGHT);
    vec3 neighbour_normal[4] = vec3[4](normal, normal, normal, normal);

    for(int i = 0; i < 4; i++)
    {
        vec2 direction;
        sincos(6.28318548 * 0.25 * float(i), direction.y, direction.x);

        for(int direction_step = 1; direction_step <= 5; direction_step++)
        {
            float search_radius = pow(2.0f,float(direction_step));
            vec2 sample_uv = MXAO.uv.zw + direction * search_radius * scaled_radius;

            vec3 temp_normal = textureLod(sMXAO_NormalTex, sample_uv * 2.0f - 1.0f, -1.0f).xyz;
            vec3 temp_position = get_position_from_uv_mipmapped(sample_uv, MXAO, 0.0f);

            vec3 position_delta = temp_position - position;
            float distance_weight = saturate(1.0 - dot(position_delta, position_delta) * 20.0 / search_radius);
            float normal_angle = dot(normal, temp_normal);
            float angle_weight = smoothstep(0.3, 0.98, normal_angle) * smoothstep(1.0, 0.98, normal_angle); //only take normals into account that are NOT equal to the current normal.

            float total_weight = saturate(3.0 * distance_weight * angle_weight / search_radius);

            neighbour_normal[i] = mix(neighbour_normal[i], temp_normal, total_weight);
        }
    }

    normal = normalize(neighbour_normal[0] + neighbour_normal[1] + neighbour_normal[2] + neighbour_normal[3]);
}

/*=============================================================================
	Pixel Shaders
=============================================================================*/

void PS_InputBufferSetup(in MXAO_VSOUT MXAO, out vec4 color, out vec4 depth, out vec4 normal)
{
    vec3 single_pixel_offset = vec3(vec2(BUFFER_RCP_WIDTH, BUFFER_RCP_HEIGHT).xy, 0);

	vec3 position          =              get_position_from_uv(MXAO.uv.xy, MXAO);
	vec3 position_delta_x1 = - position + get_position_from_uv(MXAO.uv.xy + single_pixel_offset.xz, MXAO);
	vec3 position_delta_x2 =   position - get_position_from_uv(MXAO.uv.xy - single_pixel_offset.xz, MXAO);
	vec3 position_delta_y1 = - position + get_position_from_uv(MXAO.uv.xy + single_pixel_offset.zy, MXAO);
	vec3 position_delta_y2 =   position - get_position_from_uv(MXAO.uv.xy - single_pixel_offset.zy, MXAO);

	position_delta_x1 = mix(position_delta_x1, position_delta_x2, float(abs(position_delta_x1.z) > abs(position_delta_x2.z)));
	position_delta_y1 = mix(position_delta_y1, position_delta_y2, float(abs(position_delta_y1.z) > abs(position_delta_y2.z)));

	float deltaz = abs(position_delta_x1.z * position_delta_x1.z - position_delta_x2.z * position_delta_x2.z)
				 + abs(position_delta_y1.z * position_delta_y1.z - position_delta_y2.z * position_delta_y2.z);

	normal  = vec4(normalize(cross(position_delta_y1, position_delta_x1)) * 0.5 + 0.5, deltaz);
    color 	= texture(qUINT_sBackBufferTex, MXAO.uv.xy);
	depth 	= vec4(qUINT_linear_depth(MXAO.uv.xy) * RESHADE_DEPTH_LINEARIZATION_FAR_PLANE);
}

void PS_StencilSetup(in MXAO_VSOUT MXAO, out vec4 color)
{        
    if(    qUINT_linear_depth(MXAO.uv.zw) >= MXAO_FADE_DEPTH_END
        || 0.25 * 0.5 * float(MXAO_SAMPLE_RADIUS) / (texture(sMXAO_DepthTex, MXAO.uv.zw).x + 2.0) * float(BUFFER_HEIGHT) < 1.0
        || MXAO.uv.z > 1.0
        || MXAO.uv.w > 1.0
        ) discard;

    color = vec4(1.0);
}

void PS_AmbientObscurance(in MXAO_VSOUT MXAO, out vec4 color)
{
	vec3 position = get_position_from_uv_mipmapped(MXAO.uv.zw, MXAO, 0.0f);
    vec3 normal = texture(sMXAO_NormalTex, MXAO.uv.zw).xyz * 2.0 - 1.0;

    float sample_jitter = dot(floor(mod(MXAO.vpos.xy, 4.0f) + 0.1), vec2(0.0625, 0.25)) + 0.0625;

    float  layer_id = mod((MXAO.vpos.x + MXAO.vpos.y) , 2.0);

#if(MXAO_SMOOTHNORMALS != 0)
    smooth_normals(normal, position, MXAO);
#endif
    float linear_depth = position.z / RESHADE_DEPTH_LINEARIZATION_FAR_PLANE;        
    position += normal * linear_depth;

    if(MXAO_GLOBAL_SAMPLE_QUALITY_PRESET == 7) MXAO.samples = 2.0f + floor(0.05 * MXAO_SAMPLE_RADIUS / linear_depth);

    float scaled_radius;
    float falloff_factor;
    sample_parameter_setup(MXAO, position.z, layer_id, scaled_radius, falloff_factor);

    vec2 sample_uv, sample_direction;
    sincos(2.3999632 * 16.0f * sample_jitter, sample_direction.x, sample_direction.y); //2.3999632 * 16
    sample_direction *= scaled_radius;   

    color = vec4(0.0);

    for(int i = 0; i < int(MXAO.samples); i++)
    {
        sample_uv = MXAO.uv.zw + sample_direction.xy * vec2(1.0, BUFFER_WIDTH * BUFFER_RCP_HEIGHT) * (float(i) + sample_jitter);   
        sample_direction.xy = sample_direction.xy * mat2x2(0.76465, -0.64444, 0.64444, 0.76465); //cos/sin 2.3999632 * 16            

        float sample_mip = saturate(scaled_radius * float(i) * 20.0) * 3.0;
           
    	vec3 occlusion_vector = -position + get_position_from_uv_mipmapped(sample_uv, MXAO, sample_mip + float(MXAO_MIPLEVEL_AO));                
        float  occlusion_distance_squared = dot(occlusion_vector, occlusion_vector);
        float  occlusion_normal_angle = dot(occlusion_vector, normal) * inversesqrt(occlusion_distance_squared);

        float sample_occlusion = saturate(1.0 + falloff_factor * occlusion_distance_squared) * saturate(occlusion_normal_angle - MXAO_SAMPLE_NORMAL_BIAS);
        if(MXAO_ENABLE_IL != 0){
            if(sample_occlusion > 0.1)
            {
                vec3 sample_indirect_lighting = textureLod(sMXAO_ColorTex, sample_uv,sample_mip + float(MXAO_MIPLEVEL_IL)).xyz;
                vec3 sample_normal = textureLod(sMXAO_NormalTex, sample_uv, sample_mip + float(MXAO_MIPLEVEL_IL)).xyz * 2.0 - 1.0;
                sample_indirect_lighting *= saturate(dot(-sample_normal, occlusion_vector) * inversesqrt(occlusion_distance_squared) * 4.0) * saturate(1.0 + falloff_factor * occlusion_distance_squared * 0.25);
                color += vec4(sample_indirect_lighting, sample_occlusion);
            }
        }else{
            color.w += sample_occlusion;
        }
    }

    color = saturate(color / vec4(((vec4(1.0) - vec4(MXAO_SAMPLE_NORMAL_BIAS)) * vec4(MXAO.samples))) * vec4(2.0));
    color = vec4(color.xyzw);

#if(MXAO_TWO_LAYER != 0)
    color *= mix(MXAO_AMOUNT_COARSE, MXAO_AMOUNT_FINE, layer_id); 
#endif
}

void PS_AmbientObscuranceHQ(in MXAO_VSOUT MXAO, out vec4 color)
{
	vec3 position = get_position_from_uv_mipmapped(MXAO.uv.zw, MXAO, 0.0);
	vec3 normal 	= texture(sMXAO_NormalTex, MXAO.uv.zw).xyz * 2.0 - 1.0;	

#if(MXAO_SMOOTHNORMALS != 0)
    smooth_normals(normal, position, MXAO);
#endif

	vec3 viewdir 	= normalize(-position);

	int directions = int(2.0 + floor(MXAO.samples / 32.0) * 2.0);
	int stepshalf = int(MXAO.samples / float(directions * 2));
	
	float angle_correct = 1.0 - viewdir.z * viewdir.z; 
	float scaled_radius = MXAO_SAMPLE_RADIUS / position.z / float(stepshalf) * RESHADE_DEPTH_LINEARIZATION_FAR_PLANE;
	float falloff_factor = 0.25 / (MXAO_SAMPLE_RADIUS * MXAO_SAMPLE_RADIUS);	

	float sample_jitter = dot(floor(mod(MXAO.vpos.xy , 4.0) + 0.1), vec2(0.0625, 0.25)) + 0.0625;

	float dir_phi = 3.14159265 / float(directions);
	vec2 sample_direction; sincos(dir_phi * sample_jitter * 6.0, sample_direction.y, sample_direction.x);
	mat2x2 rot_dir = mat2x2(cos(dir_phi),-sin(dir_phi),
                                sin(dir_phi),cos(dir_phi));

	color = vec4(0);

	for(int dirID = 0; dirID < directions; dirID++)
	{
		sample_direction = sample_direction * rot_dir;
		vec2 start = sample_direction * sample_jitter;

		vec3 sliceDir = vec3(sample_direction, 0);
		vec2 h = vec2(-1.0);

		for(int j = 0; j < stepshalf; j++)
		{
			vec4 sample_uv = MXAO.uv.zwzw + scaled_radius * vec2(BUFFER_RCP_WIDTH, BUFFER_RCP_HEIGHT).xyxy * start.xyxy * vec4(1,1,-1,-1);
			float sample_mip = saturate(scaled_radius * float(j) * 0.01) * 3.0;

			vec3 occlusion_vector[2];
			occlusion_vector[0] = -position + get_position_from_uv_mipmapped(sample_uv.xy, MXAO, sample_mip + float(MXAO_MIPLEVEL_AO));  
			occlusion_vector[1] = -position + get_position_from_uv_mipmapped(sample_uv.zw, MXAO, sample_mip + float(MXAO_MIPLEVEL_AO)); 

			vec2  occlusion_distance_squared = vec2(dot(occlusion_vector[0], occlusion_vector[0]), 
														dot(occlusion_vector[1], occlusion_vector[1]));

            vec2 inv_distance = inversesqrt(occlusion_distance_squared);

			vec2 sample_h = vec2(dot(occlusion_vector[0], viewdir), 
								     dot(occlusion_vector[1], viewdir)) * inv_distance;
            
            sample_h = mix(sample_h, h, saturate( occlusion_distance_squared * falloff_factor));


            if(length(sample_h) > length(h)){
                h.xy = sample_h;
            }else{
                h.xy = mix(sample_h, h, 0.75);
            }
       
			start += sample_direction;
		}

		vec3 normal_slice_plane = normalize(cross(sliceDir, viewdir));
		vec3 tangent = cross(viewdir, normal_slice_plane);
		vec3 proj_normal = normal - normal_slice_plane * dot(normal, normal_slice_plane); 

		float proj_length = length(proj_normal);
		float cos_gamma = clamp(dot(proj_normal, viewdir) * (1.0/(proj_length)), -1.0, 1.0);
		float gamma = -sign(dot(proj_normal, tangent)) * acos(cos_gamma);

		h = acos(min(h, 1.0));

		h.x = gamma + max(-h.x - gamma, -1.5707963);
		h.y = gamma + min( h.y - gamma,  1.5707963);

        h *= 2.0;		

		vec2 sample_occlusion = cos_gamma + h * sin(gamma) - cos(h - gamma);
		color.w += proj_length * dot(sample_occlusion, vec2(0.25)); 
	}

    color /= vec4(directions);
    color.w = 1.0 - color.w;
	color = vec4(color.xyzw);
}

void PS_SpatialFilter1(in MXAO_VSOUT MXAO, out vec4 color)
{
    color = blur_filter(MXAO, sCommonTex0, MXAO_GLOBAL_RENDER_SCALE, 0.75, 4);
}

const vec3 lumcoeff = vec3(0.2126, 0.7152, 0.0722);

void PS_SpatialFilter2(MXAO_VSOUT MXAO, out vec4 color)
{
    vec4 ssil_ssao = vec4(blur_filter(MXAO, sCommonTex1, 1.0, 1.0 / MXAO_GLOBAL_RENDER_SCALE, 8));

	color = texture(sMXAO_ColorTex, MXAO.uv.xy);

    float scenedepth = qUINT_linear_depth(MXAO.uv.xy);      
    float colorgray = dot(color.rgb, lumcoeff);
    float blendfact = 1.0 - colorgray;

    if(MXAO_ENABLE_IL != 0){
	    ssil_ssao.xyz  = mix(vec3(dot(ssil_ssao.xyz, lumcoeff)), ssil_ssao.xyz, MXAO_SSIL_SATURATION) * MXAO_SSIL_AMOUNT * 2.0;
    }else{
        ssil_ssao.xyz = vec3(0.0);
    }

	ssil_ssao = saturate(ssil_ssao); //compiler..
#if(MXAO_HQ == 0)
	ssil_ssao.w  = 1.0 - pow(1.0 - ssil_ssao.w, MXAO_SSAO_AMOUNT * 2.0);
#else
    ssil_ssao.w  = 1.0 - pow(1.0 - ssil_ssao.w, MXAO_SSAO_AMOUNT);
#endif
    ssil_ssao    *= 1.0 - smoothstep(MXAO_FADE_DEPTH_START, MXAO_FADE_DEPTH_END, scenedepth * vec4(2.0, 2.0, 2.0, 1.0));

    if(MXAO_BLEND_TYPE == 0)
    {
        color.rgb -= (ssil_ssao.www - ssil_ssao.xyz) * blendfact * color.rgb;
    }
    else if(MXAO_BLEND_TYPE == 1)
    {
        color.rgb = color.rgb * saturate(1.0 - ssil_ssao.www * blendfact * 1.2) + ssil_ssao.xyz * blendfact * colorgray * 2.0;
    }
    else if(MXAO_BLEND_TYPE == 2)
    {
        float colordiff = saturate(2.0 * distance(normalize(color.rgb + 1e-6),normalize(ssil_ssao.rgb + 1e-6)));
        color.rgb = color.rgb + ssil_ssao.rgb * mix(color.rgb, vec3(dot(color.rgb, vec3(0.3333))), colordiff) * blendfact * blendfact * 4.0;
        color.rgb = color.rgb * (1.0 - ssil_ssao.www * (1.0 - dot(color.rgb, lumcoeff)));
    }
    else if(MXAO_BLEND_TYPE == 3)
    {
        color.rgb *= color.rgb;
        color.rgb -= (ssil_ssao.www - ssil_ssao.xyz) * color.rgb;
        color.rgb = sqrt(color.rgb);
    }

    if(MXAO_DEBUG_VIEW_ENABLE == 1)
    {
        color.rgb = max(vec3(0.0), 1.0 - ssil_ssao.www + ssil_ssao.xyz);
        color.rgb *= (MXAO_ENABLE_IL != 0) ? 0.5 : 1.0;
    }
    else if(MXAO_DEBUG_VIEW_ENABLE == 2)
    {      
        color.rgb = texture(sMXAO_NormalTex, MXAO.uv.xy).xyz;
        color.b = 1.0-color.b; //looks nicer
    }
       
    color.a = 1.0;        
}

/*=============================================================================
	Techniques
=============================================================================*/

//technique MXAO 
//< ui_tooltip = "                     >> qUINT::MXAO <<\n\n"
//			   "MXAO is a screen-space ambient occlusion shader.\n"
//               "It adds diffuse shading to object corners to give more depth\n"
//               "and detail to the scene. Check out the preprocessor options to\n"
//               "get access to more functionality.\n"
//               "\nMake sure to move MXAO to the very top of your shader list for\n"
//               "maximum compatibility with other shaders.\n"
//               "\nMXAO is written by Marty McFly / Pascal Gilcher"; >
//{
//    pass
//	{
//		VertexShader = VS_MXAO;
//		PixelShader  = PS_InputBufferSetup;
//		RenderTarget0 = MXAO_ColorTex;
//		RenderTarget1 = MXAO_DepthTex;
//		RenderTarget2 = MXAO_NormalTex;
//	}
//    pass
//    {
//        VertexShader = VS_MXAO;
//		PixelShader  = PS_StencilSetup;
//        /*Render Target is Backbuffer*/
//        ClearRenderTargets = true;
//		StencilEnable = true;
//		StencilPass = REPLACE;
//        StencilRef = 1;
//    }
//#if(MXAO_HQ != 0)
//    pass
//    {
//        VertexShader = VS_MXAO;
//        PixelShader  = PS_AmbientObscuranceHQ;
//        RenderTarget = CommonTex0;
//        ClearRenderTargets = true;
//        StencilEnable = true;
//        StencilPass = KEEP;
//        StencilFunc = EQUAL;
//        StencilRef = 1;
//    }
//#else
//    pass
//    {
//        VertexShader = VS_MXAO;
//        PixelShader  = PS_AmbientObscurance;
//        RenderTarget = CommonTex0;
//        ClearRenderTargets = true;
//        StencilEnable = true;
//        StencilPass = KEEP;
//        StencilFunc = EQUAL;
//        StencilRef = 1;
//    }
//#endif
//    pass
//	{
//		VertexShader = VS_MXAO;
//		PixelShader  = PS_SpatialFilter1;
//        RenderTarget = CommonTex1;
//	}
//	pass
//	{
//		VertexShader = VS_MXAO;
//		PixelShader  = PS_SpatialFilter2;
//        /*Render Target is Backbuffer*/
//	}
//}