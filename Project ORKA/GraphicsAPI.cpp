
#include "GraphicsAPI.hpp"

void __stdcall DebugOutputCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
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