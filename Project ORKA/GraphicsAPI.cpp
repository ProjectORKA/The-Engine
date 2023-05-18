#include "GraphicsAPI.hpp"

OpenGLStateCopy openglState;

Int viewportX = -1;
Int viewportY = -1;
Int viewportW = -1;
Int viewportH = -1;

Int scissorX = -1;
Int scissorY = -1;
Int scissorW = -1;
Int scissorH = -1;

Color clearColor = Color(0, 0, 0, 0);

void apiClearColor() { glClear(GL_COLOR_BUFFER_BIT); }
void apiClearDepth() { glClear(GL_DEPTH_BUFFER_BIT); }

void apiEnableBlend() { glEnable(GL_BLEND); }
void apiCullFace(const Enum mode) { glCullFace(mode); }

void apiDepthFunc(const Enum func) {
	if (!openglState.depthTest)logError("Depth is not being tested!");
	glDepthFunc(func);
}

void apiDrawBuffer(const Enum mode) { glDrawBuffer(mode); }

void apiSetDebugging(const Bool value) {
	if (value) {
		glDebugMessageCallback(DebugOutputCallback, nullptr);
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	}
	else {
		glDebugMessageCallback(nullptr, nullptr);
		glDisable(GL_DEBUG_OUTPUT);
		glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	}
	openglState.debugging = value;
}

void apiSetCulling(const Bool value) {
	if (value && !openglState.culling) glEnable(GL_CULL_FACE);
	if (!value && openglState.culling) glDisable(GL_CULL_FACE);
	openglState.culling = value;
}

void apiSetBlending(const Bool value) {
	if (value && !openglState.blending) glEnable(GL_BLEND);
	if (!value && openglState.blending) glDisable(GL_BLEND);
	openglState.blending = value;
}

void apiSetDepthTest(const Bool value) {
	if (value && !openglState.depthTest) glEnable(GL_DEPTH_TEST);
	if (!value && openglState.depthTest) glDisable(GL_DEPTH_TEST);
	openglState.depthTest = value;
}

void apiGenVertexArray(UInt& id) {
	glGenVertexArrays(1, &id);
}

void apiDeleteBuffer(const UInt buffer) {
	glDeleteBuffers(1, &buffer);
}

void apiGenBuffer(UInt& bufferID) {
	glGenBuffers(1, &bufferID);
}

void apiSetScissorTest(const Bool value) {
	if (value && !openglState.scissorTest) glEnable(GL_SCISSOR_TEST);
	if (!value && openglState.scissorTest) glDisable(GL_SCISSOR_TEST);
	openglState.scissorTest = value;
}

void apiSetClearColor(const Color color) { if (clearColor != color) glClearColor(color.r, color.g, color.b, color.a); }

void apiUseProgram(const UInt programID) {
	glUseProgram(programID);
}

void apiGenTexture(UInt& textureID) { glGenTextures(1, &textureID); }

void apiGenerateMipmap(const Enum target) {
	glGenerateMipmap(target);
}

void apiLinkProgram(const UInt programID) {
	glLinkProgram(programID);
}

void apiDeleteShader(const UInt shaderID) {
	glDeleteShader(shaderID);
}

void apiActiveTexture(const Enum texture) {
	glActiveTexture(texture);
}

void apiCompileShader(const Int shaderID) {
	glCompileShader(shaderID);
}

void apiDeleteTexture(const UInt textureID) { glDeleteTextures(1, &textureID); }

void apiBindVertexArray(const UInt arrayID) {
	glBindVertexArray(arrayID);
}

void apiDeleteProgram(const UInt programID) {
	glDeleteProgram(programID);
}

void apiDeleteVertexArray(const UInt arrayID) {
	glDeleteVertexArrays(1, &arrayID);
}

void apiPolygonMode(const Enum face, const Enum mode) { glPolygonMode(face, mode); }

void apiUniform1i(const Int location, const Int value) {
	glUniform1i(location, value);
}

void apiEnableVertexAttribArray(const UInt index) {
	glEnableVertexAttribArray(index);
}

void apiGenFramebuffer(UInt& framebufferID) {
	glCreateFramebuffers(1, &framebufferID);
}

void apiDisableVertexAttribArray(const UInt index) {
	glDisableVertexAttribArray(index);
}

void apiClipControl(const Enum origin, const Enum depth) {
	glClipControl(origin, depth);
}

void apiDeleteFramebuffer(const UInt framebufferID) {
	glDeleteFramebuffers(1, &framebufferID); //doesent work. ask Nvidia
}

void apiBlendFunc(const Enum sfactor, const Enum dfactor) { glBlendFunc(sfactor, dfactor); }

void apiBlendEquation(const BlendEquation equation) {
	glBlendEquation(equation);
}

void apiBindBuffer(const Enum target, const UInt bufferID) {
	glBindBuffer(target, bufferID);
}

void apiBindDrawFramebuffer(UInt framebufferID) {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebufferID);
#ifdef OPENGLSTATE
	openglState.currentDrawFramebuffer = framebufferID;
#endif // OPENGLSTATE
}

void apiBindReadFramebuffer(UInt framebufferID) {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, framebufferID);
#ifdef OPENGLSTATE
	openglState.currentReadFramebuffer = framebufferID;
#endif // OPENGLSTATE
}

void apiScissor(const Int x, const Int y, const SizeI w, const SizeI h) {
	if ((scissorX != x) ||
		(scissorY != y) ||
		(scissorW != w) ||
		(scissorH != h)) {
		glScissor(x, y, w, h);
		scissorX = x;
		scissorY = y;
		scissorW = w;
		scissorH = h;
	}
}

void apiViewport(const Int x, const Int y, const SizeI w, const SizeI h) {
	if ((viewportX != x) ||
		(viewportY != y) ||
		(viewportW != w) ||
		(viewportH != h)) {
		glViewport(x, y, w, h);
		viewportX = x;
		viewportY = y;
		viewportW = w;
		viewportH = h;
	}
}

void apiBindTexture(const Enum target, const UInt textureID) { glBindTexture(target, textureID); }

void apiAttachShader(const UInt programID, const UInt shaderID) {
	glAttachShader(programID, shaderID);
}

void apiDetachShader(const UInt programID, const UInt shaderID) {
	glDetachShader(programID, shaderID);
}

void apiVertexAttribDivisor(const UInt index, const UInt divisor) {
	glVertexAttribDivisor(index, divisor);
}

void apiNamedFramebufferDrawBuffer(const UInt framebufferID) {
	glNamedFramebufferDrawBuffer(framebufferID, GL_COLOR_ATTACHMENT0);
}

void apiTexParameteri(const Enum target, const Enum name, const Int param) { glTexParameteri(target, name, param); }
void apiTexParameterf(const Enum target, const Enum name, const Float param) { glTexParameterf(target, name, param); }

void apiBlendEquationSeparate(const Enum modeRGB, const Enum modeAlpha) {
	glBlendEquationSeparate(modeRGB, modeAlpha);
}

void apiBlitFramebuffer(const Int width, const Int height, const Enum filter) {
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, filter);
}

void apiBindBufferBase(const Enum target, const UInt index, const UInt buffer) {
	glBindBufferBase(target, index, buffer);
}

void apiTexParameterfv(const Enum target, const Enum name, const Float* param) {
	glTexParameterfv(target, name, param);
}

void apiBufferData(const Enum target, const SizeIPtr size, const void* data, const Enum usage) {
	glBufferData(target, size, data, usage);
}

void apiBlitFramebuffer(const UInt width, const UInt height, const UInt mask, const UInt filter) {
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, mask, filter);
}

void apiDrawElements(const Enum mode, const SizeI count, const Enum type, const void* indices) {
	glDrawElements(mode, count, type, indices);
}

void apiUniformBlockBinding(const UInt programID, const UInt uniformBlockIndex, const UInt value) {
	glUniformBlockBinding(programID, uniformBlockIndex, value);
}

void apiBlitFramebuffer(const UInt srcX, const UInt srcY, const UInt dstX, const UInt dstY, const UInt filter) {
	glBlitFramebuffer(0, 0, srcX, srcY, 0, 0, dstX, dstY, GL_COLOR_BUFFER_BIT, filter);
}

void apiBlitFramebuffer(const UInt srcX, const UInt srcY, const UInt dstX, const UInt dstY, const UInt mask,
                        const UInt filter) {
	glBlitFramebuffer(0, 0, srcX, srcY, 0, 0, dstX, dstY, mask, filter);
}

void apiShaderSource(const UInt shaderID, const SizeI count, const Char* const* string, const Int* length) {
	glShaderSource(shaderID, count, string, length);
}

void apiBlendFuncSeparate(const Enum sfactorRGB, const Enum dfactorRGB, const Enum sfactorAlpha,
                          const Enum dfactorAlpha) {
	glBlendFuncSeparate(sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
}

void apiDrawElementsInstanced(const Enum mode, const SizeI size, const Enum type, const void* data,
                              const SizeI primitiveCount) {
	glDrawElementsInstanced(mode, size, type, data, primitiveCount);
}

void apiFramebufferTexture2D(const Enum target, const Enum attachment, const Enum textarget, const UInt texture,
                             const Int level) {
	glFramebufferTexture2D(target, attachment, textarget, texture, level);
}

void apiVertexAttribPointer(const UInt index, const Int size, const Enum type, const Bool normalized,
                            const SizeI stride, const void* pointer) {
	glVertexAttribPointer(index, size, type, normalized, stride, pointer);
}

void __stdcall DebugOutputCallback(const Enum source, const Enum type, UInt id, const Enum severity, SizeI length,
                                   const Char* message, const void* userParam) {
	if (severity != GL_DEBUG_SEVERITY_NOTIFICATION) {
		String outputMessage = "OpenGL Debug Output message : ";

		if (source == GL_DEBUG_SOURCE_API_ARB) outputMessage = "Source : API; ";
		else if (source == GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB) outputMessage = "Source : WINDOW_SYSTEM; ";
		else if (source == GL_DEBUG_SOURCE_SHADER_COMPILER_ARB) outputMessage = "Source : SHADER_COMPILER; ";
		else if (source == GL_DEBUG_SOURCE_THIRD_PARTY_ARB) outputMessage = "Source : THIRD_PARTY; ";
		else if (source == GL_DEBUG_SOURCE_APPLICATION_ARB) outputMessage = "Source : APPLICATION; ";
		else if (source == GL_DEBUG_SOURCE_OTHER_ARB) outputMessage = "Source : OTHER; ";

		if (type == GL_DEBUG_TYPE_ERROR_ARB) outputMessage.append("Type : ERROR; ");
		else if (type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB) outputMessage.append("Type : DEPRECATED_BEHAVIOR; ");
		else if (type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB) outputMessage.append("Type : UNDEFINED_BEHAVIOR; ");
		else if (type == GL_DEBUG_TYPE_PORTABILITY_ARB) outputMessage.append("Type : PORTABILITY; ");
		else if (type == GL_DEBUG_TYPE_PERFORMANCE_ARB) outputMessage.append("Type : PERFORMANCE; ");
		else if (type == GL_DEBUG_TYPE_OTHER_ARB) outputMessage.append("Type : OTHER; ");

		if (severity == GL_DEBUG_SEVERITY_HIGH_ARB) outputMessage.append("Severity : HIGH; ");
		else if (severity == GL_DEBUG_SEVERITY_MEDIUM_ARB) outputMessage.append("Severity : MEDIUM; ");
		else if (severity == GL_DEBUG_SEVERITY_LOW_ARB) outputMessage.append("Severity : LOW; ");

		outputMessage.append(message);

		if (severity == GL_DEBUG_SEVERITY_HIGH_ARB) logError(outputMessage);
		else if (severity == GL_DEBUG_SEVERITY_MEDIUM_ARB) logWarning(outputMessage);
		else logDebug(outputMessage);
	}
}

void apiTexImage2D(const Enum target, const Int level, const Int internalFormat, const SizeI width, const UInt height,
                   const Int border, const Enum format, const Enum type, const void* pixels) {
	glTexImage2D(target, level, internalFormat, width, height, border, format, type, pixels);
}

Int apiGetError() { return glGetError(); }
Int apiCreateProgram() { return glCreateProgram(); }
Int apiCreateShader(const Enum type) { return glCreateShader(type); }
Int apiCheckFramebufferStatus(const Enum target) { return glCheckFramebufferStatus(target); }

Int apiGetShaderiv(const UInt shaderID, const Enum valueType) {
	Int result;
	glGetShaderiv(shaderID, valueType, &result);
	return result;
}

Int apiGetUniformLocation(const UInt programID, const Char* name) { return glGetUniformLocation(programID, name); }

Int apiGetProgramIntegerValue(const UInt programID, const Enum valueType) {
	Int value;
	glGetProgramiv(programID, valueType, &value);
	return value;
}

Int apiGetUniformBlockIndex(const UInt programID, const Char* name) { return glGetUniformBlockIndex(programID, name); }

String apiGetShaderInfoLog(const UInt shaderID, const SizeI infoLogLength) {
	Vector<Char> log(1024);
	glGetShaderInfoLog(shaderID, infoLogLength, nullptr, &log[0]);
	return String(log.begin(), log.end());
}

String apiGetProgramInfoLog(const UInt programID, const SizeI infoLogLength) {
	Vector<Char> log(1024);
	glGetProgramInfoLog(programID, infoLogLength, nullptr, &log[0]);
	return String(log.begin(), log.end());
}

void OpenGLStateCopy::print() const {
	logDebug(String("Culling: ").append(toString(culling)));
	logDebug(String("Blending: ").append(toString(blending)));
	logDebug(String("Depth Test: ").append(toString(depthTest)));
	logDebug(String("Current Draw: ").append(toString(currentDrawFramebuffer)));
	logDebug(String("Current Read: ").append(toString(currentReadFramebuffer)));
}
