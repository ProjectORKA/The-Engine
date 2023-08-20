#include "ImguiExample.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

void ImguiExample::run()
{
	resourceManager.create();
	ui.create();
	ui.window("Imgui Wrapper", Area(1920, 1080), true, true, WindowState::Windowed, renderer, resourceManager);
	ui.run();
}

void ImguiWrapperRenderer::update(Window& window) {}

void ImguiWrapperRenderer::destroy(Window& window)
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void ImguiWrapperRenderer::connect(GameSimulation& simulation) {}

void ImguiWrapperRenderer::inputEvent(Window& window, InputEvent input) {}

void ImguiWrapperRenderer::create(ResourceManager& resourceManager, Window& window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window.apiWindow, true);
	ImGui_ImplOpenGL3_Init("#version 450");
}

void ImguiWrapperRenderer::render(ResourceManager& resourceManager, Window& window, TiledRectangle area)
{
	window.renderer.clearBackground(Color(0, 0, 0, 1));

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::Begin("Hello I am ImGui");
	ImGui::Text("Hello there!");
	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImguiWrapperRenderer::renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) {}
