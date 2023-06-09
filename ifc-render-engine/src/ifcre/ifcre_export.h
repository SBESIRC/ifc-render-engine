﻿#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>


#ifdef __cplusplus
extern "C" {
#endif
#include <wchar.h>
	/// <summary>
	/// First of using the render, one should set some configuration, such as 'filename', 'width'
	/// , 'height' and so on.
	/// Then one can call 'init()' and 'run()' function.
	/// Not thread safely
	/// </summary>
	/// <param name="key">configuration key string, not null</param>
	/// <param name="value">configuration value string, not null</param>
	/// 
	__declspec(dllexport) void ifcre_set_config(const char *key, const char *value);

	__declspec(dllexport) void ifcre_init(GLFWwindow * wndPtr);

	__declspec(dllexport) void ifcre_clear_model_data();
	__declspec(dllexport) void ifcre_set_g_indices(int val);
	__declspec(dllexport) void ifcre_set_g_vertices(float val);
	__declspec(dllexport) void ifcre_set_g_normals(float val);
	__declspec(dllexport) void ifcre_set_c_indices(int val);
	__declspec(dllexport) void ifcre_set_face_mat(float val);
	__declspec(dllexport) void ifcre_set_edge_indices(int val);
	//__declspec(dllexport) void ifcre_set_comp_types(int val);

	__declspec(dllexport) void ifcre_run();

	__declspec(dllexport) int ifcre_get_comp_id();
	__declspec(dllexport) int ifcre_get_comp_ids_size();
	__declspec(dllexport) void ifcre_get_comp_ids(int* arr);

	__declspec(dllexport) void ifcre_set_comp_ids(int val);
	__declspec(dllexport) void ifcre_set_sleep_time(int val);
	__declspec(dllexport) void ifcre_set_data_ready_status(bool val);
	__declspec(dllexport) bool ifcre_save_image(const char* filePath);
	__declspec(dllexport) void ifcre_home(const bool val);

	__declspec(dllexport) void ifcre_set_grid_data(int val);
	__declspec(dllexport) void ifcre_set_grid_lines(float val);
	__declspec(dllexport) void ifcre_set_grid_circles(float val);
	__declspec(dllexport) void ifcre_set_grid_text(const char* val);
	__declspec(dllexport) void ifcre_set_grid_text_data(float val);

	//__declspec(dllexport) void ifcre_set_collide_command(int val);
	//__declspec(dllexport) void ifcre_set_collide_idsA(int val);
	//__declspec(dllexport) void ifcre_set_collide_idsB(int val);
	//__declspec(dllexport) int ifcre_get_collide_ids_size();
	//__declspec(dllexport) void ifcre_get_collide_ids(int* arr);
#ifdef __cplusplus
}
#endif
