
#include <iostream>
#include "Debug.hpp"

bool showEvents = true;
bool showDebug = true;
bool showError = true;

void logEvent(String message)
{
	if(showEvents) std::cout <<	"Event: " << message << "\n";
}
void logDebug(String message)
{
	if(showDebug) std::cout <<	"Debug: " << message << "\n";
}
void logError(String message)
{
	if(showError) std::cout <<	"Error: " << message << "\n";
}

void beep() {
	std::cout << '\a';
}
void pause() {
#ifdef _WIN32
	system("pause");
#else
	std::cout << "Thread was paused..." << "\n";
	char a;
	std::cin >> a;
#endif
}
void debugPrint(Vec2& t)
{
#ifdef DEBUG
	std::cout << "(" << t.x << "|" << t.y << ")" << "\n";
#endif // DEBUG
};

void debugPrint(Vec3 & t) {
#ifdef DEBUG
	std::cout << "(" << t.x << "|" << t.y << "|" << t.z << ")" << "\n";
#endif // DEBUG
}

void pollGraphicsAPIError() {
#ifdef GRAPHICS_API_OPENGL
	GLenum error = glGetError();
	if (error) {
		std::cout << "OpenGl Error: " << error << "\n";
	}
#endif
}

void debugPrint(Matrix& matrix) {
#ifdef DEBUG
	std::cout << "[ " << matrix[0][0] << "  " << matrix[0][1] << "  " << matrix[0][2] << "  " << matrix[0][3] << " ]" << "\n";
	std::cout << "[ " << matrix[1][0] << "  " << matrix[1][1] << "  " << matrix[1][2] << "  " << matrix[1][3] << " ]" << "\n";
	std::cout << "[ " << matrix[2][0] << "  " << matrix[2][1] << "  " << matrix[2][2] << "  " << matrix[2][3] << " ]" << "\n";
	std::cout << "[ " << matrix[3][0] << "  " << matrix[3][1] << "  " << matrix[3][2] << "  " << matrix[3][3] << " ]" << "\n";
#endif // DEBUG
}

void whenWindowAPIThrowsError(Int error, const char * description)
{
	std::cout << "Error: " << description << "\n";
	std::getchar();
	exit(EXIT_FAILURE);
}

#ifdef GRAPHICS_API_OPENGL
void __stdcall DebugOutputCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	if (severity != GL_DEBUG_SEVERITY_NOTIFICATION) {
		printf("OpenGL Debug Output message : ");

		if (source == GL_DEBUG_SOURCE_API_ARB)					printf("Source : API; ");
		else if (source == GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB)	printf("Source : WINDOW_SYSTEM; ");
		else if (source == GL_DEBUG_SOURCE_SHADER_COMPILER_ARB)	printf("Source : SHADER_COMPILER; ");
		else if (source == GL_DEBUG_SOURCE_THIRD_PARTY_ARB)		printf("Source : THIRD_PARTY; ");
		else if (source == GL_DEBUG_SOURCE_APPLICATION_ARB)		printf("Source : APPLICATION; ");
		else if (source == GL_DEBUG_SOURCE_OTHER_ARB)			printf("Source : OTHER; ");

		if (type == GL_DEBUG_TYPE_ERROR_ARB)					printf("Type : ERROR; ");
		else if (type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB)	printf("Type : DEPRECATED_BEHAVIOR; ");
		else if (type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB)	printf("Type : UNDEFINED_BEHAVIOR; ");
		else if (type == GL_DEBUG_TYPE_PORTABILITY_ARB)			printf("Type : PORTABILITY; ");
		else if (type == GL_DEBUG_TYPE_PERFORMANCE_ARB)			printf("Type : PERFORMANCE; ");
		else if (type == GL_DEBUG_TYPE_OTHER_ARB)				printf("Type : OTHER; ");

		if (severity == GL_DEBUG_SEVERITY_HIGH_ARB)				printf("Severity : HIGH; ");
		else if (severity == GL_DEBUG_SEVERITY_MEDIUM_ARB)		printf("Severity : MEDIUM; ");
		else if (severity == GL_DEBUG_SEVERITY_LOW_ARB)			printf("Severity : LOW; ");
		printf("Message : %s\n", message);
	}
}
#endif // GRAPHICS_API_OPENGL