#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

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

void errorCallback(int error, const char *description) {
    std::cerr << "Error: " << description << std::endl;
}

int musicVisualizer() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwSetErrorCallback(errorCallback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(2560, 1440, "ShaderToy Shader", glfwGetPrimaryMonitor(), nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    float vertices[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f,
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    GLint resolutionLoc = glGetUniformLocation(shaderProgram, "iResolution");
    glUniform2f(resolutionLoc, 2560.0f, 1440.0f);

    GLint timeLoc = glGetUniformLocation(shaderProgram, "iTime");
    GLint mouseLoc = glGetUniformLocation(shaderProgram, "iMouse");

    while (!glfwWindowShouldClose(window)) {
        float time = glfwGetTime();
        glUniform1f(timeLoc, time);

        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        glUniform2f(mouseLoc, (float)mouseX, (float)mouseY);

        glClear(GL_COLOR_BUFFER_BIT);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}