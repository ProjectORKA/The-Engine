#include "ImguiExample.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

void ImguiWrapperRenderer::update(Window& window) {}

void ImguiWrapperRenderer::create(Window& window)
{
	glfwMakeContextCurrent(window.apiWindow);

	// Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();

    // Initialize ImGui for GLFW and OpenGL
    ImGui_ImplGlfw_InitForOpenGL(window.apiWindow, true);
    ImGui_ImplOpenGL3_Init("#version 450");
}

void ImguiWrapperRenderer::destroy(Window& window)
{
    // Make the context current before shutting down ImGui
    glfwMakeContextCurrent(window.apiWindow);

    // Shutdown ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImguiWrapperRenderer::connect(GameSimulation& simulation) {}

void ImguiWrapperRenderer::inputEvent(Window& window, InputEvent input) {}

void ImguiWrapperRenderer::render(Window& window, TiledRectangle area)
{
	window.renderer.clearBackground(Color(0, 0, 0, 1));

    // Start the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Place your ImGui code here
    ImGui::Begin("Hello, world!");
    ImGui::Text("This is some useful text.");
    ImGui::End();

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window.apiWindow, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImguiWrapperRenderer::renderInteractive(Window& window, TiledRectangle area) {}
