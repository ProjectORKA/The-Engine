
#include "GraphicsAPI.hpp"

Int viewportX = -1;
Int viewportY = -1;
Int viewportW = -1;
Int viewportH = -1;

Int scissorX = -1;
Int scissorY = -1;
Int scissorW = -1;
Int scissorH = -1;

Color clearColor = Color(0,0,0,0);

void apiClearColor() {
	glClear(GL_COLOR_BUFFER_BIT);
}
void apiClearDepth() {
	glClear(GL_DEPTH_BUFFER_BIT);
}
void apiEnable(Enum cap) {
	glEnable(cap);
}
void apiDisable(Enum cap) {
	glDisable(cap);
}
void apiCullFace(Enum mode) {
	glCullFace(mode);
}
void apiDepthFunc(Enum func) {
	glDepthFunc(func);
}
void apiDrawBuffer(Enum mode) {
	glDrawBuffer(mode);
}
void apiDeleteBuffer(UInt buffer) {
	glDeleteBuffers(1, &buffer);
}
void apiGenVertexArray(UInt & id) {
	glGenVertexArrays(1, &id);
}
void apiSetClearColor(Color color) {
	if (clearColor != color) glClearColor(color.r, color.g, color.b, color.a);
}
void apiUseProgram(UInt programID) {
	glUseProgram(programID);
}
void apiGenBuffer(UInt & bufferID) {
	glGenBuffers(1, &bufferID);
}
void apiGenTexture(UInt& textureID) {
	glGenTextures(1, &textureID);
}
void apiGenerateMipmap(Enum target) {
	glGenerateMipmap(target);
}
void apiLinkProgram(UInt programID) {
	glLinkProgram(programID);
}
void apiDeleteShader(UInt shaderID) {
	glDeleteShader(shaderID);
}
void apiActiveTexture(Enum texture) {
	glActiveTexture(texture);
}
void apiCompileShader(Int shaderID) {
	glCompileShader(shaderID);
}
void apiDeleteTexture(UInt textureID) {
	glDeleteTextures(1, &textureID);
}
void apiBindVertexArray(UInt arrayID) {

	glBindVertexArray(arrayID);
}
void apiDeleteProgram(UInt programID) {
	glDeleteProgram(programID);
}
void apiDeleteVertexArray(UInt arrayID) {
	glDeleteVertexArrays(1, &arrayID);
}
void apiDeleteNamedStringARB(String name) {
	glDeleteNamedStringARB(name.size(), name.c_str());
}
void apiPolygonMode(Enum face, Enum mode) {
	glPolygonMode(face, mode);
}
void apiUniform1i(Int location, Int value) {
	glUniform1i(location, value);
}
void apiEnableVertexAttribArray(UInt index) {
	glEnableVertexAttribArray(index);
}
void apiClipControl(Enum origin, Enum depth) {
	glClipControl(origin, depth);
}
void apiGenFramebuffer(UInt & framebufferID) {
	glGenFramebuffers(1, &framebufferID);
}
void apiDeleteFramebuffer(UInt framebufferID) {
	glDeleteFramebuffers(1, &framebufferID); //doesent work. ask Nvidia
}
void apiBlendFunc(Enum sfactor, Enum dfactor) {
	glBlendFunc(sfactor, dfactor);
}
void apiBindBuffer(Enum target, UInt bufferID) {
	glBindBuffer(target, bufferID);
}
void apiScissor(Int x, Int y, SizeI w, SizeI h) {
	if ((scissorX != x) |
		(scissorY != y) |
		(scissorW != w) |
		(scissorH != h))glScissor(x, y, w, h);
	scissorX = x;
	scissorY = y;
	scissorW = w;
	scissorH = h;
}
void apiNamedStringARB(String name, String code) {
	glNamedStringARB(GL_SHADER_INCLUDE_ARB, name.size(), name.c_str(), code.size(), code.c_str());
}
void apiViewport(Int x, Int y, SizeI w, SizeI h) {
	if(	(viewportX != x)|
		(viewportY != y)|
		(viewportW != w)|
		(viewportH != h))glViewport(x, y, w, h);
	viewportX = x;
	viewportY = y;
	viewportW = w;
	viewportH = h;
	//logDebug(String("New x: ").append(std::to_string(viewportX)));
	//logDebug(String("New y:").append(std::to_string(viewportY)));
	//logDebug(String("New width:").append(std::to_string(viewportW)));
	//logDebug(String("New height:").append(std::to_string(viewportH)));
}
void apiBindTexture(Enum target, UInt textureID) {
	glBindTexture(target, textureID);
}
void apiAttachShader(UInt programID, UInt shaderID) {
	glAttachShader(programID, shaderID);
}
void apiDetachShader(UInt programID, UInt shaderID) {
	glDetachShader(programID, shaderID);
}
void apiNamedFramebufferDrawBuffer(UInt framebufferID) {

	glNamedFramebufferDrawBuffer(framebufferID, GL_COLOR_ATTACHMENT0);
}
void apiBindFramebuffer(Enum target, UInt framebufferID) {
	glBindFramebuffer(target, framebufferID);
}
void apiTexParameteri(Enum target, Enum name, Int param) {
	glTexParameteri(target, name, param);
}
void apiTexParameterf(Enum target, Enum name, Float param) {
	glTexParameterf(target, name, param);
}
void apiBlendEquationSeparate(Enum modeRGB, Enum modeAlpha) {
	glBlendEquationSeparate(modeRGB, modeAlpha);
}
void apiBindBufferBase(Enum target, UInt index, UInt buffer) {
	glBindBufferBase(target, index, buffer);
}
void apiTexParameterfv(Enum target, Enum name, const Float* param) {
	glTexParameterfv(target, name, param);
}
void apiBlitFramebuffer(Int width, Int height, UInt mask, Enum filter) {
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, mask, filter);
}
void apiBufferData(Enum target, SizeIPtr size, void* data, Enum usage) {
	glBufferData(target, size, data, usage);
}
void apiDrawElements(Enum mode, SizeI count, Enum type, const void* indices) {
	glDrawElements(mode, count, type, indices);
}
void apiDebugMessageCallback(GLDEBUGPROC callback, const void* userParam) {
	glDebugMessageCallback(callback, userParam);
}
void apiUniformBlockBinding(UInt programID, UInt uniformBlockIndex, UInt value) {
	glUniformBlockBinding(programID, uniformBlockIndex, value);
}
void apiShaderSource(UInt shaderID, SizeI count, const Char* const*string, const Int* length) {
	glShaderSource(shaderID, count, string, length);
}
void apiBlendFuncSeparate(Enum sfactorRGB, Enum dfactorRGB, Enum sfactorAlpha, Enum dfactorAlpha) {
	glBlendFuncSeparate(sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
}
void apiFramebufferTexture2D(Enum target, Enum attachment, Enum textarget, UInt texture, Int level) {
	glFramebufferTexture2D(target, attachment, textarget, texture, level);
}
void apiVertexAttribPointer(UInt index, Int size, Enum type, Bool normalized, SizeI stride, const void* pointer) {
	glVertexAttribPointer(index, size, type, normalized, stride, pointer);
}
void __stdcall DebugOutputCallback(Enum source, Enum type, UInt id, Enum severity, SizeI length, const Char* message, const void* userParam) {
	if (severity != GL_DEBUG_SEVERITY_NOTIFICATION) {
		
		String outputMessage;

		printf("OpenGL Debug Output message : ");

		if (source == GL_DEBUG_SOURCE_API_ARB)					outputMessage = "Source : API; ";
		else if (source == GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB)	outputMessage = "Source : WINDOW_SYSTEM; ";
		else if (source == GL_DEBUG_SOURCE_SHADER_COMPILER_ARB)	outputMessage = "Source : SHADER_COMPILER; ";
		else if (source == GL_DEBUG_SOURCE_THIRD_PARTY_ARB)		outputMessage = "Source : THIRD_PARTY; ";
		else if (source == GL_DEBUG_SOURCE_APPLICATION_ARB)		outputMessage = "Source : APPLICATION; ";
		else if (source == GL_DEBUG_SOURCE_OTHER_ARB)			outputMessage = "Source : OTHER; ";

		if (type == GL_DEBUG_TYPE_ERROR_ARB)					outputMessage.append("Type : ERROR; ");
		else if (type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB)	outputMessage.append("Type : DEPRECATED_BEHAVIOR; ");
		else if (type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB)	outputMessage.append("Type : UNDEFINED_BEHAVIOR; ");
		else if (type == GL_DEBUG_TYPE_PORTABILITY_ARB)			outputMessage.append("Type : PORTABILITY; ");
		else if (type == GL_DEBUG_TYPE_PERFORMANCE_ARB)			outputMessage.append("Type : PERFORMANCE; ");
		else if (type == GL_DEBUG_TYPE_OTHER_ARB)				outputMessage.append("Type : OTHER; ");

		if (severity == GL_DEBUG_SEVERITY_HIGH_ARB)				outputMessage.append("Severity : HIGH; ");
		else if (severity == GL_DEBUG_SEVERITY_MEDIUM_ARB)		outputMessage.append("Severity : MEDIUM; ");
		else if (severity == GL_DEBUG_SEVERITY_LOW_ARB)			outputMessage.append("Severity : LOW; ");
																
																outputMessage.append(message);

		if (severity == GL_DEBUG_SEVERITY_HIGH_ARB)				logError(outputMessage);
		else if (severity == GL_DEBUG_SEVERITY_MEDIUM_ARB)		logWarning(outputMessage);
		else if (severity == GL_DEBUG_SEVERITY_LOW_ARB)			logDebug(outputMessage);
	}
}
void apiTexImage2D(Enum target, Int level, Int internalFormat, SizeI width, UInt height, Int border, Enum format, Enum type, const void* pixels) {
	glTexImage2D(target, level, internalFormat, width, height, border, format, type, pixels);
}

Int apiGetError() {
	return glGetError();
}
Int apiCreateProgram() {
	return glCreateProgram();
}
Int apiCreateShader(Enum type) {
	return glCreateShader(type);
}
Int apiCheckFramebufferStatus(Enum target) {
	return glCheckFramebufferStatus(target);
}
Int apiGetShaderiv(UInt shaderID, Enum valueType) {
	Int result;
	glGetShaderiv(shaderID, valueType, &result);
	return result;
}
Int apiGetProgramIntegerValue(UInt programID, Enum valueType) {
	Int value;
	glGetProgramiv(programID, valueType, &value);
	return value;
}
Int apiGetUniformLocation(UInt programID, const Char* name) {
	return glGetUniformLocation(programID, name);
}
Int apiGetUniformBlockIndex(UInt programID, const Char* name) {
	return glGetUniformBlockIndex(programID, name);
}

String apiGetShaderInfoLog(UInt shaderID, SizeI infoLogLength) {
	Vector<Char> log(infoLogLength++);
	glGetShaderInfoLog(shaderID, infoLogLength, 0, &log[0]);
	return String(log.begin(), log.end());
}
String apiGetProgramInfoLog(UInt programID, SizeI infoLogLength) {
	Vector<Char> log(infoLogLength++);
	glGetProgramInfoLog(programID, infoLogLength, 0, &log[0]);
	return String(log.begin(), log.end());
}