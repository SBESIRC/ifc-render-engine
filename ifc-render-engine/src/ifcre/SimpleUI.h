#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

struct SimpleUI {
	SimpleUI(){}

	SimpleUI(GLFWwindow* window, const char* version) {
		SetUpDearImGuiContext(window, version);
		inited = true;
	}
	~SimpleUI() {
		CleanUp();
	}
	void SetUpDearImGuiContext(GLFWwindow* window, const char* version) {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();	(void)io;

		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init(version);
	}
	bool show_demo_window = true;
	bool show_another_window = false;
	bool inited = false;
	ImVec4 clear_color = ImVec4(.45f, .55f, .6f, 1.f);

	void updateFrame(float& f, ImVec4& my_color) {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (show_demo_window) {
			ImGui::ShowDemoWindow(&show_demo_window);
		}

		{
			static int counter = 0;
			ImGui::Begin("Hello world!");
			ImGui::Text("This is some useful text");
			ImGui::Checkbox("Demo window", &show_demo_window);
			ImGui::Checkbox("Another window", &show_another_window);

			ImGui::SliderFloat("float", &f, .0f, 1.f);
			ImGui::ColorEdit3("clear color", (float*)&my_color); // Edit 3 floats representing a color
			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);
			//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		if (show_another_window)
		{
			ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				show_another_window = false;
			ImGui::End();
		}
	}
	void render() {
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
	void CleanUp() {
		// Cleanup
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
};
