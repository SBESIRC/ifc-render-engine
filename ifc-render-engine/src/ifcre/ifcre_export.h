#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>


#ifdef __cplusplus
extern "C" {
#endif
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

	__declspec(dllexport) void ifcre_run();

	__declspec(dllexport) int ifcre_get_comp_id();

	__declspec(dllexport) void ifcre_set_comp_ids();

#ifdef __cplusplus
}
#endif