
#pragma once
#include <iostream>

namespace External
{
	#include "GL/glew.h"
	#include "GLFW/glfw3.h"
}

const char *vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec2 position;
    void main() {
        gl_Position = vec4(position, 0.0, 1.0);
    }
)";

const char *fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;

    uniform vec2 iResolution;
    uniform float iTime;
    uniform vec2 iMouse;

    vec3 hsv2rgb(in vec3 c) {
        vec3 rgb = clamp( abs(mod(c.x*6.0+vec3(0.0,4.0,2.0),6.0)-3.0)-1.0, 0.0, 1.0 );
        rgb = rgb*rgb*(3.0-2.0*rgb);
    	return c.z * mix( vec3(1.0), rgb, c.y);
    }

    vec3 getColor(in vec2 fragCoord) {
        vec2 resolution = iResolution.xy;
        float time = iTime;

        float radius = min(resolution.x, resolution.y);

        vec2 uv = fragCoord.xy / resolution.xy;
        uv -= 0.5;
        uv.x *= resolution.x / resolution.y;

        float centerX = resolution.x / 2.0;
        float centerY = resolution.y / 2.0;

        float angle = atan(uv.y, uv.x);
        angle = degrees(angle) + (time * 100.0);
        if (angle < 0.0) {
            angle += 360.0;
        }
        float hue = angle / 360.0;
        vec3 color = hsv2rgb(vec3(hue, 2.0 - length(uv) * 3.0, length(uv) * 3.0));
        color *= vec3(1.0, 0.7, 0.8);
        return color;
    }

    void main() {
        vec2 fragCoord = gl_FragCoord.xy;
        FragColor = vec4(mix(getColor(iMouse.xy), getColor(fragCoord), 0.2), 1.0);
    }
)";

inline void errorCallback(int error, const char *description) {
    std::cerr << "Error: " << description << std::endl;
}

int musicVisualizer() {
    if (!External::glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

	External::glfwSetErrorCallback(errorCallback);

	External::glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	External::glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	External::glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	External::GLFWwindow *window = External::glfwCreateWindow(2560, 1440, "ShaderToy Shader", External::glfwGetPrimaryMonitor(), nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
		External::glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (External::glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
		External::glfwTerminate();
        return -1;
    }

	External::GLuint vertexShader = External::__glewCreateShader(GL_VERTEX_SHADER);
    External::glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    External::glCompileShader(vertexShader);

	External::GLuint fragmentShader = External::glCreateShader(GL_FRAGMENT_SHADER);
    External::glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    External::glCompileShader(fragmentShader);

	External::GLuint shaderProgram = External::glCreateProgram();
    External::glAttachShader(shaderProgram, vertexShader);
    External::glAttachShader(shaderProgram, fragmentShader);
    External::glLinkProgram(shaderProgram);
    External::glUseProgram(shaderProgram);

	External::GLuint vao, vbo;
    External::glGenVertexArrays(1, &vao);
    External::glGenBuffers(1, &vbo);

    External::glBindVertexArray(vao);
    External::glBindBuffer(GL_ARRAY_BUFFER, vbo);

    float vertices[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f,
    };
    External::glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    External::glEnableVertexAttribArray(0);
    External::glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), static_cast<void*>(0));

    External::glBindBuffer(GL_ARRAY_BUFFER, 0);
    External::glBindVertexArray(0);

	External::GLint resolutionLoc = External::glGetUniformLocation(shaderProgram, "iResolution");
    External::glUniform2f(resolutionLoc, 2560.0f, 1440.0f);

	External::GLint           timeLoc  = External::glGetUniformLocation(shaderProgram, "iTime");
	External::GLint mouseLoc = External::glGetUniformLocation(shaderProgram, "iMouse");

    while (!glfwWindowShouldClose(window)) {
        float time = External::glfwGetTime();
        External::glUniform1f(timeLoc, time);

        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        External::glUniform2f(mouseLoc, static_cast<float>(mouseX), static_cast<float>(mouseY));

		External::glClear(GL_COLOR_BUFFER_BIT);
        External::glBindVertexArray(vao);
		External::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        External::glBindVertexArray(0);

		glfwSwapBuffers(window);
		External::glfwPollEvents();
    }

    External::glDeleteVertexArrays(1, &vao);
    External::glDeleteBuffers(1, &vbo);
    External::glDeleteShader(vertexShader);
    External::glDeleteShader(fragmentShader);
    External::glDeleteProgram(shaderProgram);

	External::glfwTerminate();
    return 0;
}