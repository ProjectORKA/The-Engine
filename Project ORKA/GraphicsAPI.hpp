#pragma once

#include "Basics.hpp"
#include "Debug.hpp"
#include "GlobalUniformData.hpp"

namespace External
{
	#include "GL/glew.h"
}

// #define DEBUG_OPENGL
// #define TRACE_OPENGL
// #define CHECK_OPENGL

enum class Filter : Int
{
	Nearest   = GL_NEAREST,
	NearestMm = GL_NEAREST_MIPMAP_NEAREST,
	Linear    = GL_LINEAR,
	LinearMm  = GL_LINEAR_MIPMAP_LINEAR
};

enum class Wrapping : Int
{
	Repeat   = GL_REPEAT,
	Mirrored = GL_MIRRORED_REPEAT,
	Clamped  = GL_CLAMP_TO_EDGE,
	Border   = GL_CLAMP_TO_BORDER
};

enum class DataType : UInt
{
	Byte      = GL_UNSIGNED_BYTE,
	SByte     = GL_BYTE,
	UShort    = GL_UNSIGNED_SHORT,
	Short     = GL_SHORT,
	UInt      = GL_UNSIGNED_INT,
	Int       = GL_INT,
	HalfFloat = GL_HALF_FLOAT,
	Float     = GL_FLOAT,
};

enum class ShaderType : UInt
{
	Vertex                 = GL_VERTEX_SHADER,
	Compute                = GL_COMPUTE_SHADER,
	Geometry               = GL_GEOMETRY_SHADER,
	Fragment               = GL_FRAGMENT_SHADER,
	TessellationControl    = GL_TESS_CONTROL_SHADER,
	TessellationEvaluation = GL_TESS_EVALUATION_SHADER
};

enum class BufferUsage : UInt
{
	StreamDraw  = GL_STREAM_DRAW,
	StreamRead  = GL_STREAM_READ,
	StreamCopy  = GL_STREAM_COPY,
	StaticDraw  = GL_STATIC_DRAW,
	StaticRead  = GL_STATIC_READ,
	StaticCopy  = GL_STATIC_COPY,
	DynamicDraw = GL_DYNAMIC_DRAW,
	DynamicRead = GL_DYNAMIC_READ,
	DynamicCopy = GL_DYNAMIC_COPY,
};

enum class BufferTarget : UInt
{
	// for vertex data
	Array = GL_ARRAY_BUFFER,
	// for vertex indices
	ElementArray = GL_ELEMENT_ARRAY_BUFFER,
	// for shader uniform variables
	Uniform           = GL_UNIFORM_BUFFER,
	ShaderStorage     = GL_SHADER_STORAGE_BUFFER,
	AtomicCounter     = GL_ATOMIC_COUNTER_BUFFER,
	CopyRead          = GL_COPY_READ_BUFFER,
	CopyWrite         = GL_COPY_WRITE_BUFFER,
	DispatchIndirect  = GL_DISPATCH_INDIRECT_BUFFER,
	DrawIndirect      = GL_DRAW_INDIRECT_BUFFER,
	PixelPack         = GL_PIXEL_PACK_BUFFER,
	PixelUnpack       = GL_PIXEL_UNPACK_BUFFER,
	Query             = GL_QUERY_BUFFER,
	Texture           = GL_TEXTURE_BUFFER,
	TransformFeedback = GL_TRANSFORM_FEEDBACK_BUFFER,
};

enum class BlendEquation : UInt
{
	Add             = GL_FUNC_ADD,
	Subtract        = GL_FUNC_SUBTRACT,
	ReverseSubtract = GL_FUNC_REVERSE_SUBTRACT,
	Minimum         = GL_MIN,
	Maximum         = GL_MAX,
};

enum class PrimitiveMode : UInt
{
	Lines         = GL_LINES,
	Points        = GL_POINTS,
	Triangles     = GL_TRIANGLES,
	LineStrip     = GL_LINE_STRIP,
	TriangleFan   = GL_TRIANGLE_FAN,
	TriangleStrip = GL_TRIANGLE_STRIP
};

enum class TextureTarget : UInt
{
	Texture1D                 = GL_TEXTURE_1D,
	Texture2D                 = GL_TEXTURE_2D,
	Texture3D                 = GL_TEXTURE_3D,
	Texture1DArray            = GL_TEXTURE_1D_ARRAY,
	Texture2DArray            = GL_TEXTURE_2D_ARRAY,
	Rectangle                 = GL_TEXTURE_RECTANGLE,
	CubeMap                   = GL_TEXTURE_CUBE_MAP,
	CubeMapArray              = GL_TEXTURE_CUBE_MAP_ARRAY,
	Buffer                    = GL_TEXTURE_BUFFER,
	Texture2DMultisample      = GL_TEXTURE_2D_MULTISAMPLE,
	Texture2DMultisampleArray = GL_TEXTURE_2D_MULTISAMPLE_ARRAY,
};

enum class BlendFunction : UInt
{
	One                   = GL_ONE,
	Zero                  = GL_ZERO,
	SrcColor              = GL_SRC_COLOR,
	DstColor              = GL_DST_COLOR,
	SrcAlpha              = GL_SRC_ALPHA,
	DstAlpha              = GL_DST_ALPHA,
	ConstantColor         = GL_CONSTANT_COLOR,
	ConstantAlpha         = GL_CONSTANT_ALPHA,
	SrcAlphaSaturate      = GL_SRC_ALPHA_SATURATE,
	OneMinusSrcColor      = GL_ONE_MINUS_SRC_COLOR,
	OneMinusDstColor      = GL_ONE_MINUS_DST_COLOR,
	OneMinusSrcAlpha      = GL_ONE_MINUS_SRC_ALPHA,
	OneMinusDstAlpha      = GL_ONE_MINUS_DST_ALPHA,
	OneMinusConstantColor = GL_ONE_MINUS_CONSTANT_COLOR,
	OneMinusConstantAlpha = GL_ONE_MINUS_CONSTANT_ALPHA,
};

enum class GetParameters : UInt
{
	ActiveTexture                        = GL_ACTIVE_TEXTURE,
	AliasedLineWidthRange                = GL_ALIASED_LINE_WIDTH_RANGE,
	ArrayBufferBinding                   = GL_ARRAY_BUFFER_BINDING,
	Blend                                = GL_BLEND,
	BlendColor                           = GL_BLEND_COLOR,
	BlendDestinationAlpha                = GL_BLEND_DST_ALPHA,
	BlendDestinationRGB                  = GL_BLEND_DST_RGB,
	BlendEquationRGB                     = GL_BLEND_EQUATION_RGB,
	BlendEquationAlpha                   = GL_BLEND_EQUATION_ALPHA,
	BlendSourceAlpha                     = GL_BLEND_SRC_ALPHA,
	BlendSourceRGB                       = GL_BLEND_SRC_RGB,
	ColorClearValue                      = GL_COLOR_CLEAR_VALUE,
	ColorLogicOp                         = GL_COLOR_LOGIC_OP,
	ColorWriteMask                       = GL_COLOR_WRITEMASK,
	CompressedTextureFormats             = GL_COMPRESSED_TEXTURE_FORMATS,
	MaxComputeShaderStorageBlocks        = GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS,
	MaxCombinedShaderStorageBlocks       = GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS,
	MaxComputeUniformBlocks              = GL_MAX_COMPUTE_UNIFORM_BLOCKS,
	MaxComputeTextureImageUnits          = GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS,
	MaxComputeUniformComponents          = GL_MAX_COMPUTE_UNIFORM_COMPONENTS,
	MaxComputeAtomicCounters             = GL_MAX_COMPUTE_ATOMIC_COUNTERS,
	MaxComputeAtomicCounterBuffers       = GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS,
	MaxCombinedComputeUniformComponents  = GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS,
	MaxComputeWorkGroupInvocations       = GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS,
	MaxComputeWorkGroupCount             = GL_MAX_COMPUTE_WORK_GROUP_COUNT,
	MaxComputeWorkGroupSize              = GL_MAX_COMPUTE_WORK_GROUP_SIZE,
	DispatchIndirectBufferBinding        = GL_DISPATCH_INDIRECT_BUFFER_BINDING,
	MaxDebugGroupStackDepth              = GL_MAX_DEBUG_GROUP_STACK_DEPTH,
	DebugGroupStackDepth                 = GL_DEBUG_GROUP_STACK_DEPTH,
	ContextFlags                         = GL_CONTEXT_FLAGS,
	CullFace                             = GL_CULL_FACE,
	CullFaceMode                         = GL_CULL_FACE_MODE,
	CurrentProgram                       = GL_CURRENT_PROGRAM,
	DepthClearValue                      = GL_DEPTH_CLEAR_VALUE,
	DepthFunc                            = GL_DEPTH_FUNC,
	DepthRange                           = GL_DEPTH_RANGE,
	DepthTest                            = GL_DEPTH_TEST,
	DepthWriteMask                       = GL_DEPTH_WRITEMASK,
	Dither                               = GL_DITHER,
	DoubleBuffer                         = GL_DOUBLEBUFFER,
	DrawBuffer                           = GL_DRAW_BUFFER,
	DrawBuffer0                          = GL_DRAW_BUFFER0,
	DrawBuffer1                          = GL_DRAW_BUFFER1,
	DrawBuffer2                          = GL_DRAW_BUFFER2,
	DrawBuffer3                          = GL_DRAW_BUFFER3,
	DrawBuffer4                          = GL_DRAW_BUFFER4,
	DrawBuffer5                          = GL_DRAW_BUFFER5,
	DrawBuffer6                          = GL_DRAW_BUFFER6,
	DrawBuffer7                          = GL_DRAW_BUFFER7,
	DrawBuffer8                          = GL_DRAW_BUFFER8,
	DrawBuffer9                          = GL_DRAW_BUFFER9,
	DrawBuffer10                         = GL_DRAW_BUFFER10,
	DrawBuffer11                         = GL_DRAW_BUFFER11,
	DrawBuffer12                         = GL_DRAW_BUFFER12,
	DrawBuffer13                         = GL_DRAW_BUFFER13,
	DrawBuffer14                         = GL_DRAW_BUFFER14,
	DrawBuffer15                         = GL_DRAW_BUFFER15,
	DrawFramebufferBinding               = GL_DRAW_FRAMEBUFFER_BINDING,
	ReadFramebufferBinding               = GL_READ_FRAMEBUFFER_BINDING,
	ElementArrayBufferBinding            = GL_ELEMENT_ARRAY_BUFFER_BINDING,
	FragmentShaderDerivativeHint         = GL_FRAGMENT_SHADER_DERIVATIVE_HINT,
	ImplementationColorReadFormat        = GL_IMPLEMENTATION_COLOR_READ_FORMAT,
	ImplementationColorReadType          = GL_IMPLEMENTATION_COLOR_READ_TYPE,
	LineSmooth                           = GL_LINE_SMOOTH,
	LineSmoothHint                       = GL_LINE_SMOOTH_HINT,
	LineWidth                            = GL_LINE_WIDTH,
	LayerProvokingVertex                 = GL_LAYER_PROVOKING_VERTEX,
	LogicOpMode                          = GL_LOGIC_OP_MODE,
	MajorVersion                         = GL_MAJOR_VERSION,
	Max3DTextureSize                     = GL_MAX_3D_TEXTURE_SIZE,
	MaxArrayTextureLayers                = GL_MAX_ARRAY_TEXTURE_LAYERS,
	MaxClipDistances                     = GL_MAX_CLIP_DISTANCES,
	MaxColorTextureSamples               = GL_MAX_COLOR_TEXTURE_SAMPLES,
	MaxCombinedAtomicCounters            = GL_MAX_COMBINED_ATOMIC_COUNTERS,
	MaxCombinedFragmentUniformComponents = GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS,
	MaxCombinedGeometryUniformComponents = GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS,
	MaxCombinedTextureImageUnits         = GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
	MaxCombinedUniformBlocks             = GL_MAX_COMBINED_UNIFORM_BLOCKS,
	MaxCombinedVertexUniformComponents   = GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS,
	MaxCubeMapTextureSize                = GL_MAX_CUBE_MAP_TEXTURE_SIZE,
	MaxDepthTextureSamples               = GL_MAX_DEPTH_TEXTURE_SAMPLES,
	MaxDrawBuffers                       = GL_MAX_DRAW_BUFFERS,
	MaxDualSourceDrawBuffers             = GL_MAX_DUAL_SOURCE_DRAW_BUFFERS,
	MaxElementsIndices                   = GL_MAX_ELEMENTS_INDICES,
	MaxElementsVertices                  = GL_MAX_ELEMENTS_VERTICES,
	MaxFragmentAtomicCounters            = GL_MAX_FRAGMENT_ATOMIC_COUNTERS,
	MaxFragmentShaderStorageBlocks       = GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS,
	MaxFragmentInputComponents           = GL_MAX_FRAGMENT_INPUT_COMPONENTS,
	MaxFragmentUniformComponents         = GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,
	MaxFragmentUniformVectors            = GL_MAX_FRAGMENT_UNIFORM_VECTORS,
	MaxFragmentUniformBlocks             = GL_MAX_FRAGMENT_UNIFORM_BLOCKS,
	MaxFramebufferWidth                  = GL_MAX_FRAMEBUFFER_WIDTH,
	MaxFramebufferHeight                 = GL_MAX_FRAMEBUFFER_HEIGHT,
	MaxFramebufferLayers                 = GL_MAX_FRAMEBUFFER_LAYERS,
	MaxFramebufferSamples                = GL_MAX_FRAMEBUFFER_SAMPLES,
	MaxGeometryAtomicCounters            = GL_MAX_GEOMETRY_ATOMIC_COUNTERS,
	MaxGeometryShaderStorageBlocks       = GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS,
	MaxGeometryInputComponents           = GL_MAX_GEOMETRY_INPUT_COMPONENTS,
	MaxGeometryOutputComponents          = GL_MAX_GEOMETRY_OUTPUT_COMPONENTS,
	MaxGeometryTextureImageUnits         = GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS,
	MaxGeometryUniformBlocks             = GL_MAX_GEOMETRY_UNIFORM_BLOCKS,
	MaxGeometryUniformComponents         = GL_MAX_GEOMETRY_UNIFORM_COMPONENTS,
	MaxIntegerSamples                    = GL_MAX_INTEGER_SAMPLES,
	MinMapBufferAlignment                = GL_MIN_MAP_BUFFER_ALIGNMENT,
	MaxLabelLength                       = GL_MAX_LABEL_LENGTH,
	MaxProgramTexelOffset                = GL_MAX_PROGRAM_TEXEL_OFFSET,
	MinProgramTexelOffset                = GL_MIN_PROGRAM_TEXEL_OFFSET,
	MaxRectangleTextureSize              = GL_MAX_RECTANGLE_TEXTURE_SIZE,
	MaxRenderBufferSize                  = GL_MAX_RENDERBUFFER_SIZE,
	MaxSampleMaskWords                   = GL_MAX_SAMPLE_MASK_WORDS,
	MaxServerWaitTimeout                 = GL_MAX_SERVER_WAIT_TIMEOUT,
	MaxShaderStorageBufferBindings       = GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS,
	MaxTessControlAtomicCounters         = GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS,
	MaxTessEvaluationAtomicCounters      = GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS,
	MaxTessControlShaderStorageBlocks    = GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS,
	MaxTessEvaluationShaderStorageBlocks = GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS,
	MaxTextureBufferSize                 = GL_MAX_TEXTURE_BUFFER_SIZE,
	MaxTextureImageUnits                 = GL_MAX_TEXTURE_IMAGE_UNITS,
	MaxTextureLODBias                    = GL_MAX_TEXTURE_LOD_BIAS,
	MaxTextureSize                       = GL_MAX_TEXTURE_SIZE,
	MaxUniformBufferBindings             = GL_MAX_UNIFORM_BUFFER_BINDINGS,
	MaxUniformBlockSize                  = GL_MAX_UNIFORM_BLOCK_SIZE,
	MaxUniformLocations                  = GL_MAX_UNIFORM_LOCATIONS,
	MaxVaryingComponents                 = GL_MAX_VARYING_COMPONENTS,
	MaxVaryingVectors                    = GL_MAX_VARYING_VECTORS,
	MaxVaryingFloats                     = GL_MAX_VARYING_FLOATS,
	MaxVertexAtomicCounters              = GL_MAX_VERTEX_ATOMIC_COUNTERS,
	MaxVertexAttributes                  = GL_MAX_VERTEX_ATTRIBS,
	MaxVertexShaderStorageBlocks         = GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS,
	MaxVertexTextureImageUnits           = GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,
	MaxVertexUniformComponents           = GL_MAX_VERTEX_UNIFORM_COMPONENTS,
	MaxVertexUniformVectors              = GL_MAX_VERTEX_UNIFORM_VECTORS,
	MaxVertexOutputComponents            = GL_MAX_VERTEX_OUTPUT_COMPONENTS,
	MaxVertexUniformBlocks               = GL_MAX_VERTEX_UNIFORM_BLOCKS,
	MaxViewportDims                      = GL_MAX_VIEWPORT_DIMS,
	MaxViewports                         = GL_MAX_VIEWPORTS,
	MinorVersion                         = GL_MINOR_VERSION,
	NumCompressedTextureFormats          = GL_NUM_COMPRESSED_TEXTURE_FORMATS,
	NumExtensions                        = GL_NUM_EXTENSIONS,
	NumProgramBinaryFormats              = GL_NUM_PROGRAM_BINARY_FORMATS,
	NumShaderBinaryFormats               = GL_NUM_SHADER_BINARY_FORMATS,
	PackAlignment                        = GL_PACK_ALIGNMENT,
	PackImageHeight                      = GL_PACK_IMAGE_HEIGHT,
	PackLSBFirst                         = GL_PACK_LSB_FIRST,
	PackRowLength                        = GL_PACK_ROW_LENGTH,
	PackSkipImages                       = GL_PACK_SKIP_IMAGES,
	PackSkipPixels                       = GL_PACK_SKIP_PIXELS,
	PackSkipRows                         = GL_PACK_SKIP_ROWS,
	PackSwapBytes                        = GL_PACK_SWAP_BYTES,
	PixelPackBufferBinding               = GL_PIXEL_PACK_BUFFER_BINDING,
	PixelUnpackBufferBinding             = GL_PIXEL_UNPACK_BUFFER_BINDING,
	PointFadeThresholdSize               = GL_POINT_FADE_THRESHOLD_SIZE,
	PrimitiveRestartIndex                = GL_PRIMITIVE_RESTART_INDEX,
	ProgramBinaryFormats                 = GL_PROGRAM_BINARY_FORMATS,
	ProgramPipelineBinding               = GL_PROGRAM_PIPELINE_BINDING,
	ProgramPointSize                     = GL_PROGRAM_POINT_SIZE,
	ProvokingVertex                      = GL_PROVOKING_VERTEX,
	PointSize                            = GL_POINT_SIZE,
	PointSizeGranularity                 = GL_POINT_SIZE_GRANULARITY,
	PointSizeRange                       = GL_POINT_SIZE_RANGE,
	PolygonOffsetFactor                  = GL_POLYGON_OFFSET_FACTOR,
	PolygonOffsetUnits                   = GL_POLYGON_OFFSET_UNITS,
	PolygonOffsetFill                    = GL_POLYGON_OFFSET_FILL,
	PolygonOffsetLine                    = GL_POLYGON_OFFSET_LINE,
	PolygonOffsetPoint                   = GL_POLYGON_OFFSET_POINT,
	PolygonSmooth                        = GL_POLYGON_SMOOTH,
	PolygonSmoothHint                    = GL_POLYGON_SMOOTH_HINT,
	ReadBuffer                           = GL_READ_BUFFER,
	RenderbufferBinding                  = GL_RENDERBUFFER_BINDING,
	SampleBuffers                        = GL_SAMPLE_BUFFERS,
	SampleCoverageValue                  = GL_SAMPLE_COVERAGE_VALUE,
	SampleCoverageInvert                 = GL_SAMPLE_COVERAGE_INVERT,
	SampleMaskValue                      = GL_SAMPLE_MASK_VALUE,
	SamplerBinding                       = GL_SAMPLER_BINDING,
	Samples                              = GL_SAMPLES,
	ScissorBox                           = GL_SCISSOR_BOX,
	ScissorTest                          = GL_SCISSOR_TEST,
	ShaderCompiler                       = GL_SHADER_COMPILER,
	ShaderStorageBufferBinding           = GL_SHADER_STORAGE_BUFFER_BINDING,
	ShaderStorageBufferOffsetAlignment   = GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT,
	ShaderStorageBufferStart             = GL_SHADER_STORAGE_BUFFER_START,
	ShaderStorageBufferSize              = GL_SHADER_STORAGE_BUFFER_SIZE,
	SmoothLineWidthRange                 = GL_SMOOTH_LINE_WIDTH_RANGE,
	SmoothLineWidthGranularity           = GL_SMOOTH_LINE_WIDTH_GRANULARITY,
	StencilBackFail                      = GL_STENCIL_BACK_FAIL,
	StencilBackFunc                      = GL_STENCIL_BACK_FUNC,
	StencilBackPassDepthFail             = GL_STENCIL_BACK_PASS_DEPTH_FAIL,
	StencilBackPassDepthPass             = GL_STENCIL_BACK_PASS_DEPTH_PASS,
	StencilBackRef                       = GL_STENCIL_BACK_REF,
	StencilBackValueMask                 = GL_STENCIL_BACK_VALUE_MASK,
	StencilBackWritemask                 = GL_STENCIL_BACK_WRITEMASK,
	StencilClearValue                    = GL_STENCIL_CLEAR_VALUE,
	StencilFail                          = GL_STENCIL_FAIL,
	StencilFunc                          = GL_STENCIL_FUNC,
	StencilPassDepthFail                 = GL_STENCIL_PASS_DEPTH_FAIL,
	StencilPassDepthPass                 = GL_STENCIL_PASS_DEPTH_PASS,
	StencilRef                           = GL_STENCIL_REF,
	StencilTest                          = GL_STENCIL_TEST,
	StencilValueMask                     = GL_STENCIL_VALUE_MASK,
	StencilWritemask                     = GL_STENCIL_WRITEMASK,
	Stereo                               = GL_STEREO,
	SubpixelBits                         = GL_SUBPIXEL_BITS,
	TextureBinding1D                     = GL_TEXTURE_BINDING_1D,
	TextureBinding1DArray                = GL_TEXTURE_BINDING_1D_ARRAY,
	TextureBinding2D                     = GL_TEXTURE_BINDING_2D,
	TextureBinding2DArray                = GL_TEXTURE_BINDING_2D_ARRAY,
	TextureBinding2DMultisample          = GL_TEXTURE_BINDING_2D_MULTISAMPLE,
	TextureBinding2DMultisampleArray     = GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY,
	TextureBinding3D                     = GL_TEXTURE_BINDING_3D,
	TextureBindingBuffer                 = GL_TEXTURE_BINDING_BUFFER,
	TextureBindingCubeMap                = GL_TEXTURE_BINDING_CUBE_MAP,
	TextureBindingRectangle              = GL_TEXTURE_BINDING_RECTANGLE,
	TextureCompressionHint               = GL_TEXTURE_COMPRESSION_HINT,
	TextureBufferOffsetAlignment         = GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT,
	Timestamp                            = GL_TIMESTAMP,
	TransformFeedbackBufferBinding       = GL_TRANSFORM_FEEDBACK_BUFFER_BINDING,
	TransformFeedbackBufferStart         = GL_TRANSFORM_FEEDBACK_BUFFER_START,
	TransformFeedbackBufferSize          = GL_TRANSFORM_FEEDBACK_BUFFER_SIZE,
	UniformBufferBinding                 = GL_UNIFORM_BUFFER_BINDING,
	UniformBufferOffsetAlignment         = GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT,
	UniformBufferSize                    = GL_UNIFORM_BUFFER_SIZE,
	UniformBufferStart                   = GL_UNIFORM_BUFFER_START,
	UnpackAlignment                      = GL_UNPACK_ALIGNMENT,
	UnpackImageHeight                    = GL_UNPACK_IMAGE_HEIGHT,
	UnpackLSBFirst                       = GL_UNPACK_LSB_FIRST,
	UnpackRowLength                      = GL_UNPACK_ROW_LENGTH,
	UnpackSkipImages                     = GL_UNPACK_SKIP_IMAGES,
	UnpackSkipPixels                     = GL_UNPACK_SKIP_PIXELS,
	UnpackSkipRows                       = GL_UNPACK_SKIP_ROWS,
	UnpackSwapBytes                      = GL_UNPACK_SWAP_BYTES,
	VertexArrayBinding                   = GL_VERTEX_ARRAY_BINDING,
	VertexBindingDivisor                 = GL_VERTEX_BINDING_DIVISOR,
	VertexBindingOffset                  = GL_VERTEX_BINDING_OFFSET,
	VertexBindingStride                  = GL_VERTEX_BINDING_STRIDE,
	VertexBindingBuffer                  = GL_VERTEX_BINDING_BUFFER,
	MaxVertexAttribRelativeOffset        = GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET,
	MaxVertexAttribBindings              = GL_MAX_VERTEX_ATTRIB_BINDINGS,
	Viewport                             = GL_VIEWPORT,
	ViewportBoundsRange                  = GL_VIEWPORT_BOUNDS_RANGE,
	ViewportIndexProvokingVertex         = GL_VIEWPORT_INDEX_PROVOKING_VERTEX,
	ViewportSubpixelBits                 = GL_VIEWPORT_SUBPIXEL_BITS,
	MaxElementIndex                      = GL_MAX_ELEMENT_INDEX
};

enum class ObjectLabelType : UInt
{
	Buffer            = GL_BUFFER,
	Shader            = GL_SHADER,
	Program           = GL_PROGRAM,
	VertexArray       = GL_VERTEX_ARRAY,
	Query             = GL_QUERY,
	Pipeline          = GL_PROGRAM_PIPELINE,
	TransformFeedback = GL_TRANSFORM_FEEDBACK,
	Sampler           = GL_SAMPLER,
	Texture           = GL_TEXTURE,
	RenderBuffer      = GL_RENDERBUFFER,
	Framebuffer       = GL_FRAMEBUFFER,
};

enum class FramebufferMode : UInt
{
	FrontLeft  = GL_FRONT_LEFT,
	FrontRight = GL_FRONT_RIGHT,
	BackLeft   = GL_BACK_LEFT,
	BackRight  = GL_BACK_RIGHT,
	Front      = GL_FRONT,
	Back       = GL_BACK,
	Left       = GL_LEFT,
	Right      = GL_RIGHT,
	Color0     = GL_COLOR_ATTACHMENT0,
	Color1     = GL_COLOR_ATTACHMENT1,
	Color2     = GL_COLOR_ATTACHMENT2,
	Color3     = GL_COLOR_ATTACHMENT3,
	Color4     = GL_COLOR_ATTACHMENT4,
	Color5     = GL_COLOR_ATTACHMENT5,
	Color6     = GL_COLOR_ATTACHMENT6,
	Color7     = GL_COLOR_ATTACHMENT7,
	Color8     = GL_COLOR_ATTACHMENT8,
	Color9     = GL_COLOR_ATTACHMENT9,
	Color10    = GL_COLOR_ATTACHMENT10,
	Color11    = GL_COLOR_ATTACHMENT11,
	Color12    = GL_COLOR_ATTACHMENT12,
	Color13    = GL_COLOR_ATTACHMENT13,
	Color14    = GL_COLOR_ATTACHMENT14,
	Color15    = GL_COLOR_ATTACHMENT15,
};

enum class ClearBufferType : UInt
{
	Color           = GL_COLOR,
	Depth           = GL_DEPTH,
	Stencil         = GL_STENCIL,
	DepthAndStencil = GL_DEPTH_STENCIL,
};

enum class ShaderParameters : UInt
{
	ShaderType         = GL_SHADER_TYPE,
	DeleteStatus       = GL_DELETE_STATUS,
	CompileStatus      = GL_COMPILE_STATUS,
	InfoLogLength      = GL_INFO_LOG_LENGTH,
	ShaderSourceLength = GL_SHADER_SOURCE_LENGTH,
};

enum class ReadPixelsFormat : UInt
{
	StencilIndex = GL_STENCIL_INDEX,
	Depth        = GL_DEPTH_COMPONENT,
	DepthStencil = GL_DEPTH_STENCIL,
	Red          = GL_RED,
	Green        = GL_GREEN,
	Blue         = GL_BLUE,
	RGB          = GL_RGB,
	BGR          = GL_BGR,
	RGBA         = GL_RGBA,
	BGRA         = GL_BGRA,
	RedInteger   = GL_RED_INTEGER,
	RGInteger    = GL_RG_INTEGER,
	RGBInteger   = GL_RGB_INTEGER,
	RGBAInteger  = GL_RGBA_INTEGER,
};

enum class EnableParameters : UInt
{
	AlphaText                  = GL_ALPHA_TEST,
	AutoNormal                 = GL_AUTO_NORMAL,
	Blend                      = GL_BLEND,
	ClipDistance0              = GL_CLIP_DISTANCE0,
	ClipDistance1              = GL_CLIP_DISTANCE1,
	ClipDistance2              = GL_CLIP_DISTANCE2,
	ClipDistance3              = GL_CLIP_DISTANCE3,
	ClipDistance4              = GL_CLIP_DISTANCE4,
	ClipDistance5              = GL_CLIP_DISTANCE5,
	ClipPlane0                 = GL_CLIP_PLANE0,
	ClipPlane1                 = GL_CLIP_PLANE1,
	ClipPlane2                 = GL_CLIP_PLANE2,
	ClipPlane3                 = GL_CLIP_PLANE3,
	ClipPlane4                 = GL_CLIP_PLANE4,
	ClipPlane5                 = GL_CLIP_PLANE5,
	ColorLogicOp               = GL_COLOR_LOGIC_OP,
	ColorMaterial              = GL_COLOR_MATERIAL,
	ColorSum                   = GL_COLOR_SUM,
	ColorTable                 = GL_COLOR_TABLE,
	Convolution1D              = GL_CONVOLUTION_1D,
	Convolution2D              = GL_CONVOLUTION_2D,
	CullFace                   = GL_CULL_FACE,
	DebugOutput                = GL_DEBUG_OUTPUT,
	DebugOutputSyncronous      = GL_DEBUG_OUTPUT_SYNCHRONOUS,
	DepthClamp                 = GL_DEPTH_CLAMP,
	DepthTest                  = GL_DEPTH_TEST,
	Dither                     = GL_DITHER,
	Fog                        = GL_FOG,
	FramebufferSRGB            = GL_FRAMEBUFFER_SRGB,
	Histogram                  = GL_HISTOGRAM,
	IndexLogicOp               = GL_INDEX_LOGIC_OP,
	Light0                     = GL_LIGHT0,
	Light1                     = GL_LIGHT1,
	Light2                     = GL_LIGHT2,
	Light3                     = GL_LIGHT3,
	Light4                     = GL_LIGHT4,
	Light5                     = GL_LIGHT5,
	Light6                     = GL_LIGHT6,
	Light7                     = GL_LIGHT7,
	Lighting                   = GL_LIGHTING,
	LineSmooth                 = GL_LINE_SMOOTH,
	LineStipple                = GL_LINE_STIPPLE,
	Color4                     = GL_MAP1_COLOR_4,
	Map1Index                  = GL_MAP1_INDEX,
	Map1Normal                 = GL_MAP1_NORMAL,
	Map1TextureCoord1          = GL_MAP1_TEXTURE_COORD_1,
	Map1TextureCoord2          = GL_MAP1_TEXTURE_COORD_2,
	Map1TextureCoord3          = GL_MAP1_TEXTURE_COORD_3,
	Map1TextureCoord4          = GL_MAP1_TEXTURE_COORD_4,
	Map1Vertex3                = GL_MAP1_VERTEX_3,
	Map1Vertex4                = GL_MAP1_VERTEX_4,
	Map2Color4                 = GL_MAP2_COLOR_4,
	Map2Index                  = GL_MAP2_INDEX,
	Map2Normal                 = GL_MAP2_NORMAL,
	Map2TextureCoord1          = GL_MAP2_TEXTURE_COORD_1,
	Map2TextureCoord2          = GL_MAP2_TEXTURE_COORD_2,
	Map2TextureCoord3          = GL_MAP2_TEXTURE_COORD_3,
	Map2TextureCoord4          = GL_MAP2_TEXTURE_COORD_4,
	Map2Vertex3                = GL_MAP2_VERTEX_3,
	Map2Vertex4                = GL_MAP2_VERTEX_4,
	MinMax                     = GL_MINMAX,
	Multisample                = GL_MULTISAMPLE,
	Normalize                  = GL_NORMALIZE,
	PointSmooth                = GL_POINT_SMOOTH,
	PointSprite                = GL_POINT_SPRITE,
	PolygonOffsetFill          = GL_POLYGON_OFFSET_FILL,
	PolygonOffsetLine          = GL_POLYGON_OFFSET_LINE,
	PolygonOffsetPoint         = GL_POLYGON_OFFSET_POINT,
	PolygonSmooth              = GL_POLYGON_SMOOTH,
	PolygonStipple             = GL_POLYGON_STIPPLE,
	PostColorMatrixColorTable  = GL_POST_COLOR_MATRIX_COLOR_TABLE,
	PostConvolutionColorTable  = GL_POST_CONVOLUTION_COLOR_TABLE,
	PrimitiveRestart           = GL_PRIMITIVE_RESTART,
	PrimitiveRestartFixedIndex = GL_PRIMITIVE_RESTART_FIXED_INDEX,
	ProgramPointSize           = GL_PROGRAM_POINT_SIZE,
	RasterizerDiscard          = GL_RASTERIZER_DISCARD,
	RescaleNormal              = GL_RESCALE_NORMAL,
	SampleAlphaToCoverage      = GL_SAMPLE_ALPHA_TO_COVERAGE,
	SampleAlphaToOne           = GL_SAMPLE_ALPHA_TO_ONE,
	SampleCoverage             = GL_SAMPLE_COVERAGE,
	SampleShading              = GL_SAMPLE_SHADING,
	SampleMask                 = GL_SAMPLE_MASK,
	Separable2D                = GL_SEPARABLE_2D,
	ScissorTest                = GL_SCISSOR_TEST,
	StencilTest                = GL_STENCIL_TEST,
	Texture1D                  = GL_TEXTURE_1D,
	Texture2D                  = GL_TEXTURE_2D,
	Texture3D                  = GL_TEXTURE_3D,
	TextureCubeMap             = GL_TEXTURE_CUBE_MAP,
	TextureCubeMapSeamless     = GL_TEXTURE_CUBE_MAP_SEAMLESS,
	TextureGenQ                = GL_TEXTURE_GEN_Q,
	TextureGenR                = GL_TEXTURE_GEN_R,
	TextureGenS                = GL_TEXTURE_GEN_S,
	TextureGenT                = GL_TEXTURE_GEN_T,
	VertexProgramPointSize     = GL_VERTEX_PROGRAM_POINT_SIZE,
	VertexProgramTwoSide       = GL_VERTEX_PROGRAM_TWO_SIDE
};

enum class WritePixelsFormat : UInt
{
	Red            = GL_RED,
	RG             = GL_RG,
	RGB            = GL_RGB,
	RGBA           = GL_RGBA,
	RedInteger     = GL_RED_INTEGER,
	RGInteger      = GL_RG_INTEGER,
	RGBInteger     = GL_RGB_INTEGER,
	RGBAInteger    = GL_RGBA_INTEGER,
	Depth          = GL_DEPTH_COMPONENT,
	DepthStencil   = GL_DEPTH_STENCIL,
	LuminanceAlpha = GL_LUMINANCE_ALPHA,
	Luminance      = GL_LUMINANCE,
	Alpha          = GL_ALPHA,
};

enum class FramebufferTarget : UInt
{
	// GL_FRAMEBUFFER is equivalent to GL_DRAW_FRAMEBUFFER
	Draw = GL_DRAW_FRAMEBUFFER,
	Read = GL_READ_FRAMEBUFFER
};

enum class FramebufferStatus : UInt
{
	Complete                    = GL_FRAMEBUFFER_COMPLETE,
	Undefined                   = GL_FRAMEBUFFER_UNDEFINED,
	IncompleteAttachment        = GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT,
	IncompleteMissingAttachment = GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT,
	IncompleteDrawBuffer        = GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER,
	IncompleteReadBuffer        = GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER,
	Unsupported                 = GL_FRAMEBUFFER_UNSUPPORTED,
	IncompleteMultisample       = GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE,
	IncompleteLayerTargets      = GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS,
};

enum class SizedInternalFormat : UInt
{
	// 2 [0 to 3] -> [0 to 1]
	RGBA2 = GL_RGBA2,
	// 4 [0 to 15] -> [0 to 1]
	RGB4  = GL_RGB4,
	RGBA4 = GL_RGBA4,
	// 5 [0 to 31] -> [0 to 1]
	RGB5 = GL_RGB5,
	// 8 [0 - 255] -> [0 to 1]
	R8    = GL_R8,
	RG8   = GL_RG8,
	RGB8  = GL_RGB8,
	RGBA8 = GL_RGBA8,
	// 8I [-127 to 128] -> [-127 to 128] 
	R8I    = GL_R8I,
	RG8I   = GL_RG8I,
	RGB8I  = GL_RGB8I,
	RGBA8I = GL_RGBA8I,
	// 8S [-127 to 128] -> [-1 to 1]
	R8S    = GL_R8_SNORM,
	RG8S   = GL_RG8_SNORM,
	RGB8S  = GL_RGB8_SNORM,
	RGBA8S = GL_RGBA8_SNORM,
	// 8UI [0 to 255] -> [0 to 255]
	R8UI    = GL_R8UI,
	RG8UI   = GL_RG8UI,
	RGB8UI  = GL_RGB8UI,
	RGBA8UI = GL_RGBA8UI,
	// 10 [0 to 1023] -> [0 to 1]
	RGB10 = GL_RGB10,
	// 12 [0 to 4095] -> [0 to 1]
	RGB12  = GL_RGB12,
	RGBA12 = GL_RGBA12,
	// 16 [0 to 65535] -> [0 to 1]
	R16    = GL_R16,
	RG16   = GL_RG16,
	RGB16  = GL_RGB16,
	RGBA16 = GL_RGBA16,
	// 16F 
	R16F    = GL_R16F,
	RG16F   = GL_RG16F,
	RGB16F  = GL_RGB16F,
	RGBA16F = GL_RGBA16F,
	// 16S [-32768 to 32767] -> [-1 to 1]
	R16S   = GL_R16_SNORM,
	RG16S  = GL_RG16_SNORM,
	RGB16S = GL_RGB16_SNORM,
	// 16I [-32768 to 32767] -> [-32768 to 32767]
	R16I    = GL_R16I,
	RG16I   = GL_RG16I,
	RGB16I  = GL_RGB16I,
	RGBA16I = GL_RGBA16I,
	// 16UI [0 to 65535] -> [0 to 65535]
	R16UI    = GL_R16UI,
	RG16UI   = GL_RG16UI,
	RGB16UI  = GL_RGB16UI,
	RGBA16UI = GL_RGBA16UI,
	// 32F
	R32F    = GL_R32F,
	RG32F   = GL_RG32F,
	RGB32F  = GL_RGB32F,
	RGBA32F = GL_RGBA32F,
	// 32I [-2147483648 to 2147483647] -> [-2147483648 to 2147483647]
	R32I    = GL_R32I,
	RG32I   = GL_RG32I,
	RGB32I  = GL_RGB32I,
	RGBA32I = GL_RGBA32I,
	// 32UI [0 to 4294967295] -> [0 to 4294967295]
	R32UI    = GL_R32UI,
	RG32UI   = GL_RG32UI,
	RGB32UI  = GL_RGB32UI,
	RGBA32UI = GL_RGBA32UI,
	// sRGB display
	SRGB8   = GL_SRGB8,
	SRGB8A8 = GL_SRGB8_ALPHA8,
	// other
	R3G3B2       = GL_R3_G3_B2,
	RGB5_A1      = GL_RGB5_A1,
	RGB10_A2     = GL_RGB10_A2,
	RGB10_A2UI   = GL_RGB10_A2UI,
	RGB9E5       = GL_RGB9_E5,
	R11FG11FB10F = GL_R11F_G11F_B10F,
	// depth formats
	Depth16  = GL_DEPTH_COMPONENT16,
	Depth24  = GL_DEPTH_COMPONENT24,
	Depth32  = GL_DEPTH_COMPONENT32,
	Depth32F = GL_DEPTH_COMPONENT32F,
	// stencil formats
	// compressed formats
};

enum class TextureParameterSet : UInt
{
	DepthStencilTextureMode = GL_DEPTH_STENCIL_TEXTURE_MODE,
	BaseLevel               = GL_TEXTURE_BASE_LEVEL,
	CompareFunction         = GL_TEXTURE_COMPARE_FUNC,
	CompareMode             = GL_TEXTURE_COMPARE_MODE,
	LODBias                 = GL_TEXTURE_LOD_BIAS,
	MinFilter               = GL_TEXTURE_MIN_FILTER,
	MagFilter               = GL_TEXTURE_MAG_FILTER,
	MinLOD                  = GL_TEXTURE_MIN_LOD,
	MaxLOD                  = GL_TEXTURE_MAX_LOD,
	MaxLevel                = GL_TEXTURE_MAX_LEVEL,
	SwizzleR                = GL_TEXTURE_SWIZZLE_R,
	SwizzleG                = GL_TEXTURE_SWIZZLE_G,
	SwizzleB                = GL_TEXTURE_SWIZZLE_B,
	SwizzleA                = GL_TEXTURE_SWIZZLE_A,
	WrapS                   = GL_TEXTURE_WRAP_S,
	WrapT                   = GL_TEXTURE_WRAP_T,
	WrapR                   = GL_TEXTURE_WRAP_R,
	BorderColor             = GL_TEXTURE_BORDER_COLOR,
	SwizzleRGBA             = GL_TEXTURE_SWIZZLE_RGBA,
};

enum class TextureParameterGet : UInt
{
	DepthStencilTextureMode      = GL_DEPTH_STENCIL_TEXTURE_MODE,
	ImageFormatCompatibilityType = GL_IMAGE_FORMAT_COMPATIBILITY_TYPE,
	BaseLevel                    = GL_TEXTURE_BASE_LEVEL,
	BorderColor                  = GL_TEXTURE_BORDER_COLOR,
	CompareMode                  = GL_TEXTURE_COMPARE_MODE,
	CompareFunction              = GL_TEXTURE_COMPARE_FUNC,
	ImmutableFormat              = GL_TEXTURE_IMMUTABLE_FORMAT,
	ImmutableLevels              = GL_TEXTURE_IMMUTABLE_LEVELS,
	LODBias                      = GL_TEXTURE_LOD_BIAS,
	MagFilter                    = GL_TEXTURE_MAG_FILTER,
	MaxLevel                     = GL_TEXTURE_MAX_LEVEL,
	MaxLOD                       = GL_TEXTURE_MAX_LOD,
	MinFilter                    = GL_TEXTURE_MIN_FILTER,
	MinLOD                       = GL_TEXTURE_MIN_LOD,
	SwizzleR                     = GL_TEXTURE_SWIZZLE_R,
	SwizzleG                     = GL_TEXTURE_SWIZZLE_G,
	SwizzleB                     = GL_TEXTURE_SWIZZLE_B,
	SwizzleA                     = GL_TEXTURE_SWIZZLE_A,
	SwizzleRGBA                  = GL_TEXTURE_SWIZZLE_RGBA,
	Target                       = GL_TEXTURE_TARGET,
	ViewMinLayer                 = GL_TEXTURE_VIEW_MIN_LAYER,
	ViewMinLevel                 = GL_TEXTURE_VIEW_MIN_LEVEL,
	ViewNumLayers                = GL_TEXTURE_VIEW_NUM_LAYERS,
	ViewNumLevels                = GL_TEXTURE_VIEW_NUM_LEVELS,
	WrapS                        = GL_TEXTURE_WRAP_S,
	WrapT                        = GL_TEXTURE_WRAP_T,
	WrapR                        = GL_TEXTURE_WRAP_R,
};

enum class BufferDataUsageFlags :UInt
{
	DynamicStorage      = GL_DYNAMIC_STORAGE_BIT,
	MapRead             = GL_MAP_READ_BIT,
	MapWrite            = GL_MAP_WRITE_BIT,
	MapPersistent       = GL_MAP_PERSISTENT_BIT,
	MapCoherent         = GL_MAP_COHERENT_BIT,
	ClientStorage       = GL_CLIENT_STORAGE_BIT,
	MapInvalidateRange  = GL_MAP_INVALIDATE_RANGE_BIT,
	MapInvalidateBuffer = GL_MAP_INVALIDATE_BUFFER_BIT,
};

enum class FramebufferAttachment : UInt
{
	Color0       = GL_COLOR_ATTACHMENT0,
	Color1       = GL_COLOR_ATTACHMENT1,
	Color2       = GL_COLOR_ATTACHMENT2,
	Color3       = GL_COLOR_ATTACHMENT3,
	Color4       = GL_COLOR_ATTACHMENT4,
	Color5       = GL_COLOR_ATTACHMENT5,
	Color6       = GL_COLOR_ATTACHMENT6,
	Color7       = GL_COLOR_ATTACHMENT7,
	Color8       = GL_COLOR_ATTACHMENT8,
	Color9       = GL_COLOR_ATTACHMENT9,
	Color10      = GL_COLOR_ATTACHMENT10,
	Color11      = GL_COLOR_ATTACHMENT11,
	Color12      = GL_COLOR_ATTACHMENT12,
	Color13      = GL_COLOR_ATTACHMENT13,
	Color14      = GL_COLOR_ATTACHMENT14,
	Color15      = GL_COLOR_ATTACHMENT15,
	Depth        = GL_DEPTH_ATTACHMENT,
	Stencil      = GL_STENCIL_ATTACHMENT,
	DepthStencil = GL_DEPTH_STENCIL_ATTACHMENT,
};

enum class ShaderProgramParameters : UInt
{
	DeleteStatus              = GL_DELETE_STATUS,
	LinkStatus                = GL_LINK_STATUS,
	Validate                  = GL_VALIDATE_STATUS,
	InfoLogLength             = GL_INFO_LOG_LENGTH,
	AttachedShaders           = GL_ATTACHED_SHADERS,
	ActiveAttributes          = GL_ACTIVE_ATTRIBUTES,
	ActiveAttributesMaxLength = GL_ACTIVE_ATTRIBUTE_MAX_LENGTH,
	ActiveUniforms            = GL_ACTIVE_UNIFORMS,
	ActiveUniformsMaxLength   = GL_ACTIVE_UNIFORM_MAX_LENGTH,
};

using Mask           = UInt;
using ShaderID       = UInt; // id of a shader object that need to be linked to a program
using BufferID       = UInt; // the id of a buffer object
using ProgramID      = UInt; // id of a shader program object
using TextureID      = UInt; // the id of a texture object
using TextureSlot    = UInt; // one of 16 texture slots used in shaders
using BindingPoint   = UInt; // a predefined slot for each target that data is bound to
using FramebufferID  = UInt; // the id of a framebuffer object
using VertexArrayID  = UInt; // the id of a vertex array object, it stores multiple pieces of mesh data using vertex buffers
using AttributeIndex = UInt; // represents the global id for mesh data or "attributes"
using VertexBufferID = UInt; // the id of a vertex buffer, it stores individual mesh data
using UniformBlockID = UInt; // id for a uniform block inside the shader code

inline void __stdcall debugOutputCallback(const UInt source, const UInt type, UInt id, const UInt severity, Int length, const Char* message, const void* userParam)
{
	if (severity != GL_DEBUG_SEVERITY_NOTIFICATION)
	{
		String outputMessage = "OpenGL Debug Output message : ";

		if (source == GL_DEBUG_SOURCE_API_ARB)
		{
			outputMessage = "Source : API; ";
		}
		else if (source == GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB)
		{
			outputMessage = "Source : WINDOW_SYSTEM; ";
		}
		else if (source == GL_DEBUG_SOURCE_SHADER_COMPILER_ARB)
		{
			outputMessage = "Source : SHADER_COMPILER; ";
		}
		else if (source == GL_DEBUG_SOURCE_THIRD_PARTY_ARB)
		{
			outputMessage = "Source : THIRD_PARTY; ";
		}
		else if (source == GL_DEBUG_SOURCE_APPLICATION_ARB)
		{
			outputMessage = "Source : APPLICATION; ";
		}
		else if (source == GL_DEBUG_SOURCE_OTHER_ARB) outputMessage = "Source : OTHER; ";

		if (type == GL_DEBUG_TYPE_ERROR_ARB)
		{
			outputMessage.append("Type : ERROR; ");
		}
		else if (type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB)
		{
			outputMessage.append("Type : DEPRECATED_BEHAVIOR; ");
		}
		else if (type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB)
		{
			outputMessage.append("Type : UNDEFINED_BEHAVIOR; ");
		}
		else if (type == GL_DEBUG_TYPE_PORTABILITY_ARB)
		{
			outputMessage.append("Type : PORTABILITY; ");
		}
		else if (type == GL_DEBUG_TYPE_PERFORMANCE_ARB)
		{
			outputMessage.append("Type : PERFORMANCE; ");
		}
		else if (type == GL_DEBUG_TYPE_OTHER_ARB) outputMessage.append("Type : OTHER; ");

		if (severity == GL_DEBUG_SEVERITY_HIGH_ARB)
		{
			outputMessage.append("Severity : HIGH; ");
		}
		else if (severity == GL_DEBUG_SEVERITY_MEDIUM_ARB)
		{
			outputMessage.append("Severity : MEDIUM; ");
		}
		else if (severity == GL_DEBUG_SEVERITY_LOW_ARB) outputMessage.append("Severity : LOW; ");

		outputMessage.append(message);

		if (severity == GL_DEBUG_SEVERITY_HIGH_ARB)
		{
			logError(outputMessage);
		}
		else if (severity == GL_DEBUG_SEVERITY_MEDIUM_ARB)
		{
			logWarning(outputMessage);
		}
		else
		{
			logDebug(outputMessage);
		}
	}
}

struct OpenGLStateCopy
{
	const Bool openglStateTracking  = false;
	const Bool errorCheckingEnabled = true;
	Bool       loggingEnabled       = false;
	#ifdef TRACE_OPENGL
	Logger log            = Logger("OpenGL");
	#endif
	// current state
	Bool  culling                = false;
	Bool  blending               = false;
	Bool  depthTest              = false;
	Bool  debugging              = false;
	Bool  scissorTest            = false;
	Int   currentDrawFramebuffer = -1;
	Int   currentReadFramebuffer = -1;
	Int   viewportX              = -1;
	Int   viewportY              = -1;
	Int   viewportW              = -1;
	Int   viewportH              = -1;
	Int   scissorX               = -1;
	Int   scissorY               = -1;
	Int   scissorW               = -1;
	Int   scissorH               = -1;
	Color clearColor             = Color(0, 0, 0, 0);
	// objects
	List<TextureID>     textures;
	List<VertexArrayID> vaos;

	// static state
	UInt maxVertexAttributes = 0;

	void print() const;
	~OpenGLStateCopy();
	void enableLogging();
	void disableLogging();
	void printOpenGLInfo() const;
	void addTexture(TextureID textureID);
	void write(const String& message) const;
	void removeTexture(TextureID textureID);
	void addVAO(VertexArrayID vertexArrayID);
	void removeVAO(VertexArrayID vertexArrayID);
};

extern OpenGLStateCopy openGlState;

namespace OpenGL
{
	void apiInit();
	void apiFinish();
	UInt apiGetError();
	void apiClearColor();
	void apiClearDepth();
	void apiSetCulling(Bool value);
	void apiSetBlending(Bool value);
	void apiSetDebugging(Bool value);
	void apiSetDepthTest(Bool value);
	void apiSetScissorTest(Bool value);
	void apiSetClearColor(Color color);
	void apiPolygonMode(UInt face, UInt mode);
	void apiEnable(EnableParameters parameter);
	void apiDisable(EnableParameters parameter);
	void apiScissor(Int x, Int y, Int w, Int h);
	Int  apiGetIntegerv(GetParameters parameter);
	void apiViewport(Int x, Int y, Int w, Int h);
	void apiClipControl(UInt origin, UInt depth);
	void apiBlendEquation(BlendEquation equation);
	void apiBindDrawFramebuffer(FramebufferID framebufferID);
	void apiBindReadFramebuffer(FramebufferID framebufferID);
	void apiObjectLabel(ObjectLabelType type, UInt objectID, const String& label);
	void apiBlendFunc(BlendFunction sourceFactor, BlendFunction destinationFactor);
}

namespace OpenGL::Textures
{
	[[nodiscard]] TextureID apiCreateTexture(TextureTarget target);

	void apiDeleteTexture(TextureID textureID);
	void apiGenerateTextureMipmap(TextureID textureID);
	void apiBindTextureUnit(TextureSlot textureUnitSlot, TextureID textureID);
	void apiTextureParameteri(TextureID textureId, TextureParameterSet parameter, Int value);
	void apiTextureParameterf(TextureID textureId, TextureParameterSet parameter, Float value);
	void apiTextureParameteriv(TextureID textureId, TextureParameterSet parameter, const Int* value);
	void apiTextureParameterIiv(TextureID textureId, TextureParameterSet parameter, const Int* value);
	void apiGetTextureParameteriv(TextureID textureID, TextureParameterGet parameterName, Int& output);
	void apiTextureParameterfv(TextureID textureId, TextureParameterSet parameter, const Float* value);
	void apiTextureParameterIuiv(TextureID textureId, TextureParameterSet parameter, const UInt* value);
	void apiGetTextureParameterIiv(TextureID textureID, TextureParameterGet parameterName, Int* output);
	void apiGetTextureParameterfv(TextureID textureID, TextureParameterGet parameterName, Float& output);
	void apiGetTextureParameterIuiv(TextureID textureID, TextureParameterGet parameterName, UInt* output);
	void apiClearTexImage(TextureID texture, Int mipmapLevel, WritePixelsFormat format, DataType type, const void* data);
	void apiTextureStorage2D(TextureID texture, Int mipmapLevel, SizedInternalFormat internalFormat, Int width, Int height);
	void apiTextureSubImage2D(TextureID texture, Int mipmapLevel, Int xOffset, Int yOffset, Int width, Int height, WritePixelsFormat format, DataType type, const void* pixels);
}

namespace OpenGL::Buffers
{
	[[nodiscard]] inline BufferID apiCreateBuffer();

	inline void apiDeleteBuffer(UInt buffer);
	inline void apiBindBuffer(BufferTarget target, BufferID bufferID);
	inline void apiBindBufferBase(BufferTarget target, BindingPoint bindingLocation, BufferID buffer);
	inline void apiNamedBufferData(BufferID bufferID, ULL byteCount, const void* data, BufferUsage usage);
}

namespace OpenGL::VertexArray
{
	[[nodiscard]] VertexArrayID apiCreateVertexArray();

	void apiBindVertexArray(VertexArrayID vertexArrayID);
	void apiDeleteVertexArray(const VertexArrayID& vertexArrayID);
	void apiDrawElements(PrimitiveMode mode, Int indexCount, const void* indices);
	void apiVertexArrayElementBuffer(VertexArrayID vertexArrayID, BufferID indexBufferID);
	void apiEnableVertexArrayAttrib(VertexArrayID vertexArrayID, AttributeIndex attributeID);
	void apiDisableVertexArrayAttrib(VertexArrayID vertexArrayID, AttributeIndex attributeID);
	void apiVertexArrayBindingDivisor(VertexArrayID vertexArrayID, Index layoutBinding, UInt divisor);
	void apiDrawElementsInstanced(PrimitiveMode mode, Int indexCount, const void* indices, Int primitiveCount);
	void apiVertexArrayAttribBinding(VertexArrayID vertexArrayID, AttributeIndex attributeID, Index layoutBinding);
	void apiVertexArrayVertexBuffer(VertexArrayID vertexArrayID, UInt layoutBinding, VertexBufferID buffer, LL offset, Int stride);
	void apiVertexArrayAttribFormat(VertexArrayID vertexArrayID, AttributeIndex attributeID, Int size, DataType type, Bool normalized, UInt relativeOffset);
}

namespace OpenGL::Framebuffer
{
	[[nodiscard]] FramebufferID apiCreateFramebuffer();
	Int                         attachmentToIndex(FramebufferAttachment attachment);
	FramebufferStatus           apiCheckNamedFramebufferStatus(FramebufferID framebufferID, FramebufferTarget target);

	void apiDeleteFramebuffer(FramebufferID framebufferID);
	void apiNamedFramebufferReadBuffer(FramebufferID framebufferID, FramebufferMode mode);
	void apiNamedFramebufferDrawBuffers(FramebufferID framebuffer, const Vector<UInt>& buffers);
	void apiReadPixels(Int x, Int y, Int width, Int height, ReadPixelsFormat format, DataType type, void* data);
	void apiNamedFramebufferTexture(FramebufferID framebufferID, FramebufferAttachment attachmentID, TextureID texture, Int mipmapLevel);
	void apiClearNamedFramebufferiv(FramebufferID framebufferID, ClearBufferType buffer, FramebufferAttachment attachmentSlot, const Int* value);
	void apiClearNamedFramebufferuiv(FramebufferID framebufferID, ClearBufferType buffer, FramebufferAttachment attachmentSlot, const UInt* value);
	void apiClearNamedFramebufferfv(FramebufferID framebufferID, ClearBufferType buffer, FramebufferAttachment attachmentSlot, const Float* value);
	void apiClearNamedFramebufferfi(FramebufferID framebufferID, ClearBufferType buffer, FramebufferAttachment attachmentSlot, Float depth, Int stencil);
}

namespace OpenGL::Shaders
{
	[[nodiscard]] ProgramID apiCreateProgram();
	[[nodiscard]] ShaderID  apiCreateShader(ShaderType type);
	[[nodiscard]] String    apiGetShaderInfoLog(ShaderID shaderId);
	[[nodiscard]] String    apiGetProgramInfoLog(ProgramID programId);
	[[nodiscard]] Int       apiGetShaderIntegerValue(ShaderID shaderId, ShaderParameters parameter);
	[[nodiscard]] Int       apiGetProgramIntegerValue(ProgramID programId, ShaderProgramParameters parameter);

	void apiUseProgram(ProgramID programId);
	void apiDeleteShader(ShaderID shaderId);
	void apiCompileShader(ShaderID shaderId);
	void apiLinkProgram(ProgramID programId);
	void apiDeleteProgram(ProgramID programId);
	void apiAttachShader(ProgramID programId, ShaderID shaderId);
	void apiDetachShader(ProgramID programId, ShaderID shaderId);
	void apiShaderSource(ShaderID shaderId, Int count, const Char* const* string, const Int* length);
	void apiUniformBlockBinding(ProgramID programId, UniformBlockID uniformBlockIndex, BindingPoint uniformBlockBinding);
}

struct OpenGLTexture2D
{
	[[nodiscard]] UInt getID() const;

	void destroy() const;
	void generateMipMap() const;
	void create(const String& name);
	void setBorderColor(Color color) const;
	void setWrapping(Wrapping wrapping) const;
	void useTextureInSlot(UInt textureUnitSlot) const;
	void emptyTextureFromSlot(UInt textureUnitSlot) const;
	void setFilters(Filter nearFilter, Filter farFilter) const;
	void setDataToDepth(Int width, Int height, const void* data) const;
	void setData(SizedInternalFormat internalFormat, Int width, Int height, WritePixelsFormat colorFormat, DataType dataType, const void* data) const;

private:
	UInt textureID = -1;
};

struct OpenGLBuffer
{
	void bind() const;
	void unbind() const;
	void destroy() const;
	void update(const Matrix& data) const;
	void update(const Vec2Vector& data) const;
	void update(const Vec3Vector& data) const;
	void update(const Vector<Int>& data) const;
	void update(const Vector<UInt>& data) const;
	void update(const Vector<Vec2>& data) const;
	void update(const Vector<Vec3>& data) const;
	void update(const Vector<Matrix>& data) const;
	void update(const GlobalUniformData& data) const;

	[[nodiscard]] BufferID getID() const;

	template <typename T>
	void update(T data) const;
	template <typename T>
	void create(BufferTarget targetValue, T data, BufferUsage usageValue, const String& name, BindingPoint bindingLocation);

private:
	BufferTarget   target         = BufferTarget::Array;
	BufferUsage    usage          = BufferUsage::StaticDraw;
	VertexBufferID bufferObjectID = -1;
};

struct OpenGLIndexBuffer : OpenGLBuffer
{
	void create(const Vector<UInt>& indices, BufferUsage usage, const String& name);
};

struct OpenGLVertexBuffer : OpenGLBuffer
{
	template <typename T>
	void create(const Vector<T>& data, BufferUsage usage, const String& name);
};

struct OpenGLUniformBuffer : OpenGLBuffer
{
	template <typename T>
	void create(T data, BindingPoint bindingLocation);
};

struct OpenGLVertexArrayObject
{
	void create();
	void bind() const;
	void unbind() const;
	void destroy() const;
	void unbindIndexBuffer() const;
	void bindIndexBuffer(BufferID indexBufferID) const;
	void unbindVertexBuffer(AttributeIndex attributeIndex) const;
	void render(PrimitiveMode primitiveMode, Int indexCount, const void* indicesOrOffset) const;
	void renderInstanced(PrimitiveMode primitiveMode, Int indexCount, const void* indicesOrOffset, Int instanceCount) const;
	void bindVertexBuffer(AttributeIndex attributeIndex, BufferID bufferID, LL offset, Int stride, UInt divisor, Int numComponents, DataType type, Bool normalized, UInt relativeOffset) const;

private:
	VertexArrayID vertexArrayID = -1;
};

struct OpenGLFramebuffer
{
	[[nodiscard]] UInt  getID() const;
	[[nodiscard]] Bool  isComplete() const;
	[[nodiscard]] Vec4  readPixelVec4(Int x, Int y, FramebufferMode mode) const;
	[[nodiscard]] IVec4 readPixelIVec4(Int x, Int y, FramebufferMode mode) const;

	void destroy() const;
	void bindDraw() const;
	void bindRead() const;
	void create(const String& name);
	void clearDepth(Float clearDepth) const;
	void drawBuffers(const Vector<UInt>& attachments) const;
	void clearColor(FramebufferAttachment attachmentSlot, Vec4 color) const;
	void clearColor(FramebufferAttachment attachmentSlot, IVec4 color) const;
	void attachTexture(FramebufferAttachment attachment, TextureID textureId) const;

private:
	UInt framebufferId = -1;
};

struct OpenGLShaderProgram
{
	void               destroy();
	void               use() const;
	[[nodiscard]] Bool create(const String& name, const String& vertexShaderSource, const String& fragmentShaderSource);

private:
	UInt programId = -1;
};

// TEMPLATES

template <typename T>
void OpenGLBuffer::update(T data) const
{
	logError("OpenGLBuffer update implementation missing!");
	//OpenGL::Buffers::apiNamedBufferData(bufferObjectID, sizeof(T), &data, usage);
}

template <typename T>
void OpenGLUniformBuffer::create(const T data, BindingPoint bindingLocation)
{
	OpenGLBuffer::create(BufferTarget::Uniform, data, BufferUsage::DynamicDraw, "Uniform Buffer", bindingLocation);
}

template <typename T>
void OpenGLVertexBuffer::create(const Vector<T>& data, const BufferUsage usage, const String& name)
{
	OpenGLBuffer::create(BufferTarget::Array, data, usage, name, 0);
}

template <typename T>
void OpenGLBuffer::create(const BufferTarget targetValue, const T data, const BufferUsage usageValue, const String& name, const BindingPoint bindingLocation)
{
	this->target   = targetValue;
	this->usage    = usageValue;
	bufferObjectID = OpenGL::Buffers::apiCreateBuffer();
	OpenGL::apiObjectLabel(ObjectLabelType::Buffer, bufferObjectID, name);
	if (target == BufferTarget::TransformFeedback || target == BufferTarget::Uniform || target == BufferTarget::ShaderStorage || target == BufferTarget::AtomicCounter) OpenGL::Buffers::apiBindBufferBase(target, bindingLocation, bufferObjectID);
	update(data);
}
