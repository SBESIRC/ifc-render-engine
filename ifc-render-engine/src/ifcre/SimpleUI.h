#ifndef SIMPLEUI_H
#define SIMPLEUI_H 
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

#include "Clips.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
inline ImVec4 vec32floatp(glm::vec3 vec) {
	return { vec.x,vec.y,vec.z,1. };
}
inline glm::vec3 floatp2vec3(ImVec4 p) {
	return glm::vec3(p.x, p.y, p.z);
}
inline float imvec2length(const ImVec2& a) {
	return a.x * a.x + a.y * a.y;
}
inline float imvec2glmvec2(const ImVec2& a, const glm::vec2 b) {
	return a.x * b.x + a.y * b.y;
}
struct SimpleUI {
	SimpleUI() {}

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
	bool show_demo_window = false;
	bool show_another_window = false;
	bool inited = false;
	ImVec4 clear_color = ImVec4(.45f, .55f, .6f, 1.f);

	void updateFrame(glm::vec4& my_color, glm::vec3& base_pos) {
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

			//ImGui::SliderFloat("float", &f, .0f, 1.f);
			ImGui::ColorEdit3("clear color", (float*)&my_color); // Edit 3 floats representing a color
			ImGui::DragFloat3("basepos", (float*)&base_pos, .33f);
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
namespace ifcre {
	struct ClipBoxUI : public SimpleUI {
		SharedPtr<ClipBox> clipbox;
		ImU32 line_color = ImColor::HSV(.1f, 1.f, 1.f, 1.f);
		ImVec2 last_drag_delta = ImVec2(0.f, 0.f);
		ClipBoxUI() :SimpleUI() {}
		ClipBoxUI(GLFWwindow* window, const char* version) :SimpleUI(window, version) {}
		void bind_clip_box(SharedPtr<ClipBox> _clipbox) {
			clipbox = _clipbox;
		}
		void updateFrame(SharedPtr<bool>& mousemove, bool ishidden, int update_face_key, glm::vec2 this_normal, glm::vec4& my_color, glm::vec3& base_pos, glm::vec3& drawingplane_pos
			, float& global_alpha, float& trans_alpha) {
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			/*if (show_demo_window) {
				ImGui::ShowDemoWindow(&show_demo_window);
			}*/

			{
				//static int counter = 0;
				ImGui::Begin("Clipping information");
				ImGui::Text("Use for clipping box & drawing plane.");
				//ImGui::Checkbox("Demo window", &show_demo_window);
				//ImGui::Checkbox("Another window", &show_another_window);

				ImGuiIO& io = ImGui::GetIO();
				if (io.WantCaptureMouse) {
					*mousemove = false;
					//ImGui::SliderFloat("float", &f, .0f, 1.f);
					ImGui::ColorEdit3("clear color", (float*)&my_color); // Edit 3 floats representing a color
					ImGui::DragFloat3("basepos", (float*)&base_pos, ishidden ? 0.f : .33f);
					ImGui::DragFloat3("drawingplane_basepos", (float*)&drawingplane_pos, ishidden ? 0.f : .33f);
					ImGui::DragFloat("width", (float*)&clipbox->width, ishidden ? 0.f : .33f, .01f, FLT_MAX);
					ImGui::DragFloat("height", (float*)&clipbox->height, ishidden ? 0.f : .33f, .01f, FLT_MAX);
					ImGui::DragFloat("length", (float*)&clipbox->length, ishidden ? 0.f : .33f, .01f, FLT_MAX);
					if (update_face_key >= 0) {

						ImGui::Button("forward");
						if (ImGui::IsItemActive()) {
							clipbox->updateBox(update_face_key * 2);

						}
						ImGui::Button("Drag me!");
						if (ImGui::IsItemActive()) {
							ImGui::GetForegroundDrawList()->AddLine(io.MouseClickedPos[0], io.MousePos, line_color);
							ImVec2 mouse_drag_delta = ImGui::GetMouseDragDelta(0);//ImGui::GetMouseDragDelta(0, 0.0f);//io.MouseDelta;
							/*float mddl = imvec2length(mouse_drag_delta);
							float lddl = imvec2length(last_drag_delta);*/
							ImVec2 div_of_this = ImVec2(mouse_drag_delta.x - last_drag_delta.x, last_drag_delta.y - mouse_drag_delta.y);

							float times = imvec2glmvec2(div_of_this, this_normal);
							if (/*mddl > lddl*/times > 0)
								clipbox->updateBox(update_face_key * 2);
							else if (/*mddl < lddl*/times < 0)
								clipbox->updateBox(update_face_key * 2 + 1);

							last_drag_delta = mouse_drag_delta;
						}
						ImGui::Button("backward");
						if (ImGui::IsItemActive()) {
							clipbox->updateBox(update_face_key * 2 + 1);
						}
					}
				}
				else {
					*mousemove = true;
				}

				//if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				//	counter++;
				/*ImGui::SameLine();
				ImGui::Text("counter = %d", counter);*/
				//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
				ImGui::End();
			}

			{
				ImGui::Begin("Alpha test");
				ImGui::Text("Control the alpha of each component.");
				ImGui::SliderFloat("global_alpha", &global_alpha, .0f, 1.f);
				ImGui::SliderFloat("trans_alpha", &trans_alpha, .0f, 1.f);
				ImGui::End();
			}

			//if (show_another_window)
			//{
			//	ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			//	ImGui::Text("Hello from another window!");
			//	if (ImGui::Button("Close Me"))
			//		show_another_window = false;
			//	ImGui::End();
			//}
		}
	};
}
#endif
