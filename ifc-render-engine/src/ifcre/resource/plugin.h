#pragma once
#ifndef IFC_PLUGIN_H_
#define IFC_PLUGIN_H_

#include <Windows.h>

typedef void (*fpn_call_hello)();
typedef int (*fpn_call_add)(int a, int b);

namespace ifcre {
	class Plugin {
	public:
	    Plugin(HINSTANCE dll) : hDLL(dll) {}
	    fpn_call_hello call_hello;
	    fpn_call_add call_add;
	private:
	    HINSTANCE hDLL;

	};
};

#endif