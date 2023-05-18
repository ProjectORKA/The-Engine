#pragma once

//#include "imgui/imgui.h"
//#include "imgui/imgui_impl_glfw.h"
//#include "imgui/imgui_impl_opengl3.h"
#include "WindowAPI.hpp"

struct ImguiWrapper {
	Mutex mutex;

	void create(APIWindow window) {
		//mutex.lock();

		//IMGUI_CHECKVERSION();
		//ImGui::CreateContext();
		//ImGuiIO& io = ImGui::GetIO();
		//ImGui::StyleColorsDark();
		//ImGui_ImplGlfw_InitForOpenGL(window, true);
		//ImGui_ImplOpenGL3_Init("#version 450");
		//
		//mutex.unlock();
	}

	void render() {
		//mutex.lock();

		//ImGui_ImplOpenGL3_NewFrame();
		//ImGui_ImplGlfw_NewFrame();
		//ImGui::NewFrame();
		//ImGui::Begin("Hello I am ImGui");
		//ImGui::Text("Hello there!");
		//ImGui::End();
		//ImGui::Render();
		//ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		//mutex.unlock();
	}

	void destroy() {
		//mutex.lock();

		//ImGui_ImplOpenGL3_Shutdown();
		//ImGui_ImplGlfw_Shutdown();
		//ImGui::DestroyContext();

		//mutex.unlock();
	}
};
