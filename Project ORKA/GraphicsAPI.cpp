#include "GraphicsAPI.hpp"
#include "Debug.hpp"

OpenGlStateCopy openGlState;

void OpenGlStateCopy::print() const
{
	logDebug(String("Culling: ").append(toString(culling)));
	logDebug(String("Blending: ").append(toString(blending)));
	logDebug(String("Depth Test: ").append(toString(depthTest)));
	logDebug(String("Background Color: ").append(toString(clearColor)));
	logDebug(String("Current Draw: ").append(toString(currentDrawFramebuffer)));
	logDebug(String("Current Read: ").append(toString(currentReadFramebuffer)));
	logDebug(String("Current Viewport: ").append(toString(viewportX)).append(" ").append(toString(viewportY)).append(" ").append(toString(viewportW)).append(" ").append(toString(viewportH)));
	logDebug(String("Scissor Test: ").append(toString(scissorTest)));
	logDebug(String("Current Scissor: ").append(toString(scissorX)).append(" ").append(toString(scissorY)).append(" ").append(toString(scissorW)).append(" ").append(toString(scissorH)));
}

void apiClearColor()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void apiClearDepth()
{
	glClear(GL_DEPTH_BUFFER_BIT);
}

void apiEnableBlend()
{
	glEnable(GL_BLEND);
}

void apiGenVertexArray(UInt& id)
{
	glGenVertexArrays(1, &id);
}

void apiCullFace(const UInt mode)
{
	glCullFace(mode);
}

void apiGenBuffer(UInt& bufferId)
{
	glGenBuffers(1, &bufferId);
}

void apiDepthFunc(const UInt func)
{
	if(! openGlState.depthTest) logError("Depth is not being tested!");
	glDepthFunc(func);
}

void apiDrawBuffer(const UInt mode)
{
	glDrawBuffer(mode);
}

void apiGenTexture(UInt& textureId)
{
	glGenTextures(1, &textureId);
}

void apiSetCulling(const Bool value)
{
	if(value && ! openGlState.culling) glEnable(GL_CULL_FACE);
	if(! value && openGlState.culling) glDisable(GL_CULL_FACE);
	openGlState.culling = value;
}

void apiSetBlending(const Bool value)
{
	if(value && ! openGlState.blending) glEnable(GL_BLEND);
	if(! value && openGlState.blending) glDisable(GL_BLEND);
	openGlState.blending = value;
}

void apiSetDebugging(const Bool value)
{
	if(value)
	{
		glDebugMessageCallback(debugOutputCallback, nullptr);
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	}
	else
	{
		glDebugMessageCallback(nullptr, nullptr);
		glDisable(GL_DEBUG_OUTPUT);
		glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	}
	openGlState.debugging = value;
}

void apiSetDepthTest(const Bool value)
{
	if(value && ! openGlState.depthTest) glEnable(GL_DEPTH_TEST);
	if(! value && openGlState.depthTest) glDisable(GL_DEPTH_TEST);
	openGlState.depthTest = value;
}

void apiDeleteBuffer(const UInt buffer)
{
	glDeleteBuffers(1, &buffer);
}

void apiSetScissorTest(const Bool value)
{
	if(value && ! openGlState.scissorTest) glEnable(GL_SCISSOR_TEST);
	if(! value && openGlState.scissorTest) glDisable(GL_SCISSOR_TEST);
	openGlState.scissorTest = value;
}

void apiSetClearColor(const Color color)
{
	if(openGlState.clearColor != color) glClearColor(color.r, color.g, color.b, color.a);
}

void apiUseProgram(const UInt programId)
{
	glUseProgram(programId);
}

void apiGenerateMipmap(const UInt target)
{
	glGenerateMipmap(target);
}

void apiLinkProgram(const UInt programId)
{
	glLinkProgram(programId);
}

void apiDeleteShader(const UInt shaderId)
{
	glDeleteShader(shaderId);
}

void apiActiveTexture(const UInt texture)
{
	glActiveTexture(texture);
}

void apiCompileShader(const Int shaderId)
{
	glCompileShader(shaderId);
}

void apiBindFramebuffer(const Index index)
{
	glBindFramebuffer(GL_FRAMEBUFFER, index);
}

void apiDeleteTexture(const UInt textureId)
{
	glDeleteTextures(1, &textureId);
}

void apiBindVertexArray(const UInt arrayId)
{
	glBindVertexArray(arrayId);
}

void apiDeleteProgram(const UInt programId)
{
	glDeleteProgram(programId);
}

void apiGenFramebuffer(UInt& framebufferId)
{
	glCreateFramebuffers(1, &framebufferId);
}

void apiDeleteVertexArray(const UInt arrayId)
{
	glDeleteVertexArrays(1, &arrayId);
}

void apiBindDrawFramebuffer(UInt framebufferId)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebufferId);
#ifdef OPENGLSTATE
	openglState.currentDrawFramebuffer = framebufferId;
#endif // OPENGLSTATE
}

void apiBindReadFramebuffer(UInt framebufferId)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, framebufferId);
#ifdef OPENGLSTATE
	openglState.currentReadFramebuffer = framebufferId;
#endif // OPENGLSTATE
}

void apiDeleteFramebuffer(const UInt framebufferId)
{
	glDeleteFramebuffers(1, &framebufferId); //does not work. ask Nvidia
}

void apiBlendEquation(const BlendEquation equation)
{
	glBlendEquation(static_cast<UInt>(equation));
}

void apiEnableVertexAttributeArray(const UInt index)
{
	glEnableVertexAttribArray(index);
}

void apiPolygonMode(const UInt face, const UInt mode)
{
	glPolygonMode(face, mode);
}

void apiDisableVertexAttributeArray(const UInt index)
{
	glDisableVertexAttribArray(index);
}

void apiUniformInt1(const Int location, const Int value)
{
	glUniform1i(location, value);
}

void apiClipControl(const UInt origin, const UInt depth)
{
	glClipControl(origin, depth);
}

void apiDrawBuffers(const Vector<UInt>& data) {
	glDrawBuffers(toIntSafe(data.size()), data.data());
}

void apiBindBuffer(const UInt target, const UInt bufferId)
{
	glBindBuffer(target, bufferId);
}

void apiBindTexture(const UInt target, const UInt textureId)
{
	glBindTexture(target, textureId);
}

void apiNamedFramebufferDrawBuffer(const UInt framebufferId)
{
	glNamedFramebufferDrawBuffer(framebufferId, GL_COLOR_ATTACHMENT0);
}

void apiAttachShader(const UInt programId, const UInt shaderId)
{
	glAttachShader(programId, shaderId);
}

void apiDetachShader(const UInt programId, const UInt shaderId)
{
	glDetachShader(programId, shaderId);
}

void apiScissor(const Int x, const Int y, const Int w, const Int h)
{
	if((openGlState.scissorX != x) || (openGlState.scissorY != y) || (openGlState.scissorW != w) || (openGlState.scissorH != h))
	{
		glScissor(x, y, w, h);
		openGlState.scissorX = x;
		openGlState.scissorY = y;
		openGlState.scissorW = w;
		openGlState.scissorH = h;
	}
}

void apiViewport(const Int x, const Int y, const Int w, const Int h)
{
	if((openGlState.viewportX != x) || (openGlState.viewportY != y) || (openGlState.viewportW != w) || (openGlState.viewportH != h))
	{
		glViewport(x, y, w, h);
		openGlState.viewportX = x;
		openGlState.viewportY = y;
		openGlState.viewportW = w;
		openGlState.viewportH = h;
	}
}

void apiVertexAttributeDivisor(const UInt index, const UInt divisor)
{
	glVertexAttribDivisor(index, divisor);
}

void apiBlendEquationSeparate(const UInt modeRgb, const UInt modeAlpha)
{
	glBlendEquationSeparate(modeRgb, modeAlpha);
}

void apiTexParameterInt(const UInt target, const UInt name, const Int param)
{
	glTexParameteri(target, name, param);
}

void apiBlitFramebuffer(const Int width, const Int height, const UInt filter)
{
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, filter);
}

void apiBindBufferBase(const UInt target, const UInt index, const UInt buffer)
{
	glBindBufferBase(target, index, buffer);
}

void apiTexParameterFloat(const UInt target, const UInt name, const Float param)
{
	glTexParameterf(target, name, param);
}

void apiDrawElements(const PrimitiveMode mode, const Int count, const void* indices)
{
	glDrawElements(static_cast<UInt>(mode), count, GL_UNSIGNED_INT, indices);
}

void apiTexParameterFloatValue(const UInt target, const UInt name, const Float* param)
{
	glTexParameterfv(target, name, param);
}

void apiObjectLabel(ObjectLabelType framebuffer, const Index index, const String& label)
{
	glObjectLabel(static_cast<UInt>(framebuffer), index, static_cast<Int>(label.size()), label.c_str());
}

void apiBlendFunc(const BlendFunction sourceFactor, const BlendFunction destinationFactor)
{
	glBlendFunc(static_cast<UInt>(sourceFactor), static_cast<UInt>(destinationFactor));
}

void apiBlitFramebuffer(const Int width, const Int height, const UInt mask, const UInt filter)
{
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, mask, filter);
}

void apiBufferData(const UInt target, const LL size, const void* data, const MeshDrawMode usage)
{
	glBufferData(target, size, data, static_cast<UInt>(usage));
}

void apiUniformBlockBinding(const UInt programId, const UInt uniformBlockIndex, const UInt value)
{
	glUniformBlockBinding(programId, uniformBlockIndex, value);
}

void apiShaderSource(const UInt shaderId, const Int count, const Char* const* string, const Int* length)
{
	glShaderSource(shaderId, count, string, length);
}

void apiBlitFramebuffer(const Int srcX, const Int srcY, const Int dstX, const Int dstY, const UInt filter)
{
	glBlitFramebuffer(0, 0, srcX, srcY, 0, 0, dstX, dstY, GL_COLOR_BUFFER_BIT, filter);
}

void apiDrawElementsInstanced(const PrimitiveMode mode, const Int size, const void* data, const Int primitiveCount)
{
	glDrawElementsInstanced(static_cast<UInt>(mode), size, GL_UNSIGNED_INT, data, primitiveCount);
}

void apiBlitFramebuffer(const Int srcX, const Int srcY, const Int dstX, const Int dstY, const UInt mask, const UInt filter)
{
	glBlitFramebuffer(0, 0, srcX, srcY, 0, 0, dstX, dstY, mask, filter);
}

void apiFramebufferTexture2D(const UInt target, const UInt attachment, const UInt texTarget, const UInt texture, const Int level)
{
	glFramebufferTexture2D(target, attachment, texTarget, texture, level);
}

void apiVertexAttributePointer(const UInt index, const Int size, const UInt type, const Bool normalized, const Int stride, const void* pointer)
{
	glVertexAttribPointer(index, size, type, normalized, stride, pointer);
}

void apiBlendFuncSeparate(const UInt sourceFactorRgb, const UInt destinationFactorRgb, const UInt sourceFactorAlpha, const UInt destinationFactorAlpha)
{
	glBlendFuncSeparate(sourceFactorRgb, destinationFactorRgb, sourceFactorAlpha, destinationFactorAlpha);
}

void __stdcall debugOutputCallback(const UInt source, const UInt type, UInt id, const UInt severity, Int length, const Char* message, const void* userParam)
{
	if(severity != GL_DEBUG_SEVERITY_NOTIFICATION)
	{
		String outputMessage = "OpenGL Debug Output message : ";

		if(source == GL_DEBUG_SOURCE_API_ARB) outputMessage = "Source : API; ";
		else if(source == GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB) outputMessage = "Source : WINDOW_SYSTEM; ";
		else if(source == GL_DEBUG_SOURCE_SHADER_COMPILER_ARB) outputMessage = "Source : SHADER_COMPILER; ";
		else if(source == GL_DEBUG_SOURCE_THIRD_PARTY_ARB) outputMessage = "Source : THIRD_PARTY; ";
		else if(source == GL_DEBUG_SOURCE_APPLICATION_ARB) outputMessage = "Source : APPLICATION; ";
		else if(source == GL_DEBUG_SOURCE_OTHER_ARB) outputMessage = "Source : OTHER; ";

		if(type == GL_DEBUG_TYPE_ERROR_ARB) outputMessage.append("Type : ERROR; ");
		else if(type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB) outputMessage.append("Type : DEPRECATED_BEHAVIOR; ");
		else if(type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB) outputMessage.append("Type : UNDEFINED_BEHAVIOR; ");
		else if(type == GL_DEBUG_TYPE_PORTABILITY_ARB) outputMessage.append("Type : PORTABILITY; ");
		else if(type == GL_DEBUG_TYPE_PERFORMANCE_ARB) outputMessage.append("Type : PERFORMANCE; ");
		else if(type == GL_DEBUG_TYPE_OTHER_ARB) outputMessage.append("Type : OTHER; ");

		if(severity == GL_DEBUG_SEVERITY_HIGH_ARB) outputMessage.append("Severity : HIGH; ");
		else if(severity == GL_DEBUG_SEVERITY_MEDIUM_ARB) outputMessage.append("Severity : MEDIUM; ");
		else if(severity == GL_DEBUG_SEVERITY_LOW_ARB) outputMessage.append("Severity : LOW; ");

		outputMessage.append(message);

		if(severity == GL_DEBUG_SEVERITY_HIGH_ARB) logError(outputMessage);
		else if(severity == GL_DEBUG_SEVERITY_MEDIUM_ARB) logWarning(outputMessage);
		else logDebug(outputMessage);
	}
}

void apiTexImage2D(const UInt target, const Int level, const Int internalFormat, const Int width, const Int height, const Int border, const UInt format, const UInt type, const void* pixels)
{
	glTexImage2D(target, level, internalFormat, width, height, border, format, type, pixels);
}

UInt apiGetError()
{
	return glGetError();
}

Int apiCreateProgram()
{
	return glCreateProgram();
}

Int apiCreateShader(const ShaderType type)
{
	return glCreateShader(static_cast<UInt>(type));
}

Int apiCheckFramebufferStatus(const UInt target)
{
	return glCheckFramebufferStatus(target);
}

Int apiGetUniformLocation(const UInt programId, const Char* name)
{
	return glGetUniformLocation(programId, name);
}

Int apiGetUniformBlockIndex(const UInt programId, const Char* name)
{
	return glGetUniformBlockIndex(programId, name);
}

Int apiGetShaderIntegerValue(const UInt shaderId, const UInt valueType)
{
	Int result;
	glGetShaderiv(shaderId, valueType, &result);
	return result;
}

Int apiGetProgramIntegerValue(const UInt programId, const UInt valueType)
{
	Int value;
	glGetProgramiv(programId, valueType, &value);
	return value;
}

String apiGetShaderInfoLog(const UInt shaderId, const Int infoLogLength)
{
	Vector<Char> log(1024);
	glGetShaderInfoLog(shaderId, infoLogLength, nullptr, log.data());
	return { log.begin(), log.end() };
}

String apiGetProgramInfoLog(const UInt programId, const Int infoLogLength)
{
	Vector<Char> log(1024);
	glGetProgramInfoLog(programId, infoLogLength, nullptr, log.data());
	return { log.begin(), log.end() };
}