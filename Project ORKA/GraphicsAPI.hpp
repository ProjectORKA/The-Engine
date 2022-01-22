#pragma once

#include "Basics.hpp"
#include "Debug.hpp"

//opengl/glew
#define GLAPI
#define GLEW_STATIC
#include "GL/glew.h"

enum class ShaderType {
	vertex = GL_VERTEX_SHADER,
	compute = GL_COMPUTE_SHADER,
	geometry = GL_GEOMETRY_SHADER,
	fragment = GL_FRAGMENT_SHADER,
	tessellationControl = GL_TESS_CONTROL_SHADER,
	tessellationEvaluation = GL_TESS_EVALUATION_SHADER
};
enum class MeshDrawMode {
	staticMode = GL_STATIC_DRAW,
	dynamicMode = GL_DYNAMIC_DRAW,
	streamMode = GL_STREAM_DRAW,
};
enum class PrimitiveMode
{
	Lines = GL_LINES,
	Points = GL_POINTS,
	Triangles = GL_TRIANGLES,
	LineStrip = GL_LINE_STRIP,
	TriangleStrip = GL_TRIANGLE_STRIP
};

using Enum = UInt;
using SizeI = Int;
using SizeIPtr = __int64;
void apiClearColor();
void apiClearDepth();
Bool apiGetCullFace();
void apiEnable(Enum cap);
void apiDisable(Enum cap);
void apiCullFace(Enum mode);
void apiDepthFunc(Enum func);
void apiDrawBuffer(Enum mode);
void apiGenVertexArray(UInt& id);
void apiDeleteBuffer(UInt buffer);
void apiGenBuffer(UInt& bufferID);
void apiSetClearColor(Color color);
void apiGenerateMipmap(Enum target);
void apiLinkProgram(UInt programID);
void apiDeleteShader(UInt shaderID);
void apiActiveTexture(Enum texture);
void apiUseProgram(UInt programID);
void apiCompileShader(Int shaderID);
void apiGenTexture(UInt& textureID);
void apiDeleteTexture(UInt textureID);
void apiBindVertexArray(UInt arrayID);
void apiDeleteProgram(UInt programID);
void apiDeleteVertexArray(UInt arrayID);
void apiPolygonMode(Enum face, Enum mode);
void apiUniform1i(Int location, Int value);
void apiBindFramebuffer(UInt framebufferID);
void apiGenFramebuffer(UInt& framebufferID);
void apiEnableVertexAttribArray(UInt index);
void apiDisableVertexAttribArray(UInt index);
void apiClipControl(Enum origin, Enum depth);
void apiBlendFunc(Enum sfactor, Enum dfactor);
void apiDeleteFramebuffer(UInt framebufferID);
void apiBindBuffer(Enum target, UInt bufferID);
void apiBindDrawFramebuffer(UInt framebufferID);
void apiBindReadFramebuffer(UInt framebufferID);
void apiScissor(Int x, Int y, SizeI w, SizeI h);
void apiBindTexture(Enum target, UInt textureID);
void apiViewport(Int x, Int y, SizeI w, SizeI h);
void apiAttachShader(UInt programID, UInt shaderID);
void apiDetachShader(UInt programID, UInt shaderID);
void apiNamedFramebufferDrawBuffer(UInt framebufferID);
void apiTexParameteri(Enum target, Enum name, Int param);
void apiTexParameterf(Enum target, Enum name, Float param);
void apiBlendEquationSeparate(Enum modeRGB, Enum modeAlpha);
void apiBindBufferBase(Enum target, UInt index, UInt buffer);
void apiTexParameterfv(Enum target, Enum name, const Float* param);
void apiBufferData(Enum target, SizeIPtr size, void* data, Enum usage);
void apiBlitFramebuffer(Int width, Int height, UInt mask, Enum filter);
void apiDebugMessageCallback(GLDEBUGPROC callback, const void* userParam);
void apiDrawElements(Enum mode, SizeI count, Enum type, const void* indices);
void apiUniformBlockBinding(UInt programID, UInt uniformBlockIndex, UInt value);
void apiShaderSource(UInt shaderID, SizeI count, const Char* const* string, const Int* length);
void apiBlendFuncSeparate(Enum sfactorRGB, Enum dfactorRGB, Enum sfactorAlpha, Enum dfactorAlpha);
void apiDrawElementsInstanced(Enum mode, SizeI size, Enum type, void* data, SizeI primitiveCount);
void apiFramebufferTexture2D(Enum target, Enum attachment, Enum textarget, UInt texture, Int level);
void apiVertexAttribPointer(UInt index, Int size, Enum type, Bool normalized, SizeI stride, const void* pointer);
void __stdcall DebugOutputCallback(Enum source, Enum type, UInt id, Enum severity, SizeI length, const Char* message, const void* userParam);
void apiTexImage2D(Enum target, Int level, Int internalFormat, SizeI width, UInt height, Int border, Enum format, Enum type, const void* pixels);

Int apiGetError();
Int apiCreateProgram();
Int apiCreateShader(Enum type);
Int apiCheckFramebufferStatus(Enum target);
Int apiGetShaderiv(UInt shaderID, Enum valueType);
Int apiGetUniformLocation(UInt programID, const Char* name);
Int apiGetProgramIntegerValue(UInt programID, Enum valueType);
Int apiGetUniformBlockIndex(UInt programID, const Char* name);


String apiGetShaderInfoLog(UInt shaderID, SizeI infoLogLength);
String apiGetProgramInfoLog(UInt programID, SizeI infoLogLength);