#pragma once

#include "Basics.hpp"
#include "GL/glew.h"

//#define OPENGLSTATE

enum class BlendEquation : UInt
{
	Add = GL_FUNC_ADD,
	Subtract = GL_FUNC_SUBTRACT,
	ReverseSubtract = GL_FUNC_REVERSE_SUBTRACT,
	Minimum = GL_MIN,
	Maximum = GL_MAX,
};

enum class BlendFunction : UInt
{
	One = GL_ONE,
	Zero = GL_ZERO,
	SrcColor = GL_SRC_COLOR,
	DstColor = GL_DST_COLOR,
	SrcAlpha = GL_SRC_ALPHA,
	DstAlpha = GL_DST_ALPHA,
	ConstantColor = GL_CONSTANT_COLOR,
	ConstantAlpha = GL_CONSTANT_ALPHA,
	SrcAlphaSaturate = GL_SRC_ALPHA_SATURATE,

	OneMinusSrcColor = GL_ONE_MINUS_SRC_COLOR,
	OneMinusDstColor = GL_ONE_MINUS_DST_COLOR,
	OneMinusSrcAlpha = GL_ONE_MINUS_SRC_ALPHA,
	OneMinusDstAlpha = GL_ONE_MINUS_DST_ALPHA,
	OneMinusConstantColor = GL_ONE_MINUS_CONSTANT_COLOR,
	OneMinusConstantAlpha = GL_ONE_MINUS_CONSTANT_ALPHA,
};

enum class ShaderType : UInt
{
	Vertex = GL_VERTEX_SHADER,
	Compute = GL_COMPUTE_SHADER,
	Geometry = GL_GEOMETRY_SHADER,
	Fragment = GL_FRAGMENT_SHADER,
	TessellationControl = GL_TESS_CONTROL_SHADER,
	TessellationEvaluation = GL_TESS_EVALUATION_SHADER
};

enum class MeshDrawMode : UInt
{
	StaticMode = GL_STATIC_DRAW,
	DynamicMode = GL_DYNAMIC_DRAW,
	StreamMode = GL_STREAM_DRAW
};

enum class PrimitiveMode : UInt
{
	Lines = GL_LINES,
	Points = GL_POINTS,
	Triangles = GL_TRIANGLES,
	LineStrip = GL_LINE_STRIP,
	TriangleStrip = GL_TRIANGLE_STRIP
};

enum class ObjectLabelType : UInt
{
	Buffer = GL_BUFFER,
	Shader = GL_SHADER,
	Program = GL_PROGRAM,
	VertexArray = GL_VERTEX_ARRAY,
	Query = GL_QUERY,
	Pipeline = GL_PROGRAM_PIPELINE,
	TransformFeedback = GL_TRANSFORM_FEEDBACK,
	Sampler = GL_SAMPLER,
	Texture = GL_TEXTURE,
	RenderBuffer = GL_RENDERBUFFER,
	Framebuffer = GL_FRAMEBUFFER,
};

struct OpenGlStateCopy
{
	Bool culling = false;
	Bool blending = false;
	Bool depthTest = false;
	Bool debugging = false;
	Bool scissorTest = false;
	Int currentDrawFramebuffer = -1;
	Int currentReadFramebuffer = -1;

	Int viewportX = -1;
	Int viewportY = -1;
	Int viewportW = -1;
	Int viewportH = -1;

	Int scissorX = -1;
	Int scissorY = -1;
	Int scissorW = -1;
	Int scissorH = -1;

	Color clearColor = Color(0, 0, 0, 0);

	void print() const;
};

extern OpenGlStateCopy openGlState;

void apiClearColor();
void apiClearDepth();
void apiEnableBlend();
void apiCullFace(UInt mode);
void apiDepthFunc(UInt func);
void apiDrawBuffer(UInt mode);
void apiSetCulling(Bool value);
void apiSetBlending(Bool value);
void apiSetDebugging(Bool value);
void apiSetDepthTest(Bool value);
void apiGenVertexArray(UInt& id);
void apiDeleteBuffer(UInt buffer);
void apiGenBuffer(UInt& bufferId);
void apiEnable(UInt cap) = delete;
void apiDisable(UInt cap) = delete;
void apiSetClearColor(Color color);
void apiUseProgram(UInt programId);
void apiSetScissorTest(Bool value);
void apiGenerateMipmap(UInt target);
void apiLinkProgram(UInt programId);
void apiDeleteShader(UInt shaderId);
void apiActiveTexture(UInt texture);
void apiCompileShader(Int shaderId);
void apiGenTexture(UInt& textureId);
void apiDeleteTexture(UInt textureId);
void apiBindVertexArray(UInt arrayId);
void apiDeleteProgram(UInt programId);
void apiDeleteVertexArray(UInt arrayId);
void apiPolygonMode(UInt face, UInt mode);
void apiBindFramebuffer(const Index index);
void apiGenFramebuffer(UInt& framebufferId);
void apiScissor(Int x, Int y, Int w, Int h);
void apiViewport(Int x, Int y, Int w, Int h);
void apiUniformInt1(Int location, Int value);
void apiClipControl(UInt origin, UInt depth);
void apiDrawBuffers(const Vector<UInt>& data);
void apiDeleteFramebuffer(UInt framebufferId);
void apiBlendEquation(BlendEquation equation);
void apiEnableVertexAttributeArray(UInt index);
void apiBindBuffer(UInt target, UInt bufferId);
void apiDisableVertexAttributeArray(UInt index);
void apiBindDrawFramebuffer(UInt framebufferId);
void apiBindReadFramebuffer(UInt framebufferId);
void apiBindTexture(UInt target, UInt textureId);
void apiAttachShader(UInt programId, UInt shaderId);
void apiDetachShader(UInt programId, UInt shaderId);
void apiNamedFramebufferDrawBuffer(UInt framebufferId);
void apiVertexAttributeDivisor(UInt index, UInt divisor);
void apiTexParameterInt(UInt target, UInt name, Int param);
void apiBlendEquationSeparate(UInt modeRgb, UInt modeAlpha);
void apiBlitFramebuffer(Int width, Int height, UInt filter);
void apiBindBufferBase(UInt target, UInt index, UInt buffer);
void apiTexParameterFloat(UInt target, UInt name, Float param);
void apiDrawElements(PrimitiveMode mode, Int count, const void* indices);
void apiBlitFramebuffer(Int width, Int height, UInt mask, UInt filter);
void apiTexParameterFloatValue(UInt target, UInt name, const Float* param);
void apiBlendFunc(BlendFunction sourceFactor, BlendFunction destinationFactor);
void apiBufferData(UInt target, LL size, const void* data, MeshDrawMode usage);
void apiUniformBlockBinding(UInt programId, UInt uniformBlockIndex, UInt value);
void apiBlitFramebuffer(Int srcX, Int srcY, Int dstX, Int dstY, UInt filter);
void apiObjectLabel(ObjectLabelType framebuffer, const Index index, const String& label);
void apiBlitFramebuffer(Int srcX, Int srcY, Int dstX, Int dstY, UInt mask, UInt filter);
void apiShaderSource(UInt shaderId, Int count, const Char* const* string, const Int* length);
void apiDrawElementsInstanced(PrimitiveMode mode, Int size, const void* data, Int primitiveCount);
void apiFramebufferTexture2D(UInt target, UInt attachment, UInt texTarget, UInt texture, Int level);
void apiVertexAttributePointer(UInt index, Int size, UInt type, Bool normalized, Int stride, const void* pointer);
void apiBlendFuncSeparate(UInt sourceFactorRgb, UInt destinationFactorRgb, UInt sourceFactorAlpha, UInt destinationFactorAlpha);
void __stdcall debugOutputCallback(UInt source, UInt type, UInt id, UInt severity, Int length, const Char* message, const void* userParam);
void apiTexImage2D(UInt target, Int level, Int internalFormat, Int width, Int height, Int border, UInt format, UInt type, const void* pixels);

UInt apiGetError();

Int apiCreateProgram();
Int apiCreateShader(ShaderType type);
Int apiCheckFramebufferStatus(UInt target);
Int apiGetShaderIntegerValue(UInt shaderId, UInt valueType);
Int apiGetUniformLocation(UInt programId, const Char* name);
Int apiGetProgramIntegerValue(UInt programId, UInt valueType);
Int apiGetUniformBlockIndex(UInt programId, const Char* name);

String apiGetShaderInfoLog(UInt shaderId, Int infoLogLength);
String apiGetProgramInfoLog(UInt programId, Int infoLogLength);