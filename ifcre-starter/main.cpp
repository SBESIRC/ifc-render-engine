#include <iostream>
#include <Windows.h>
//const char bunny_model[] = "resources\\models\\bunny\\bunny.obj";
//const char bunny_model[] = "resources\\models\\ifc_midfile\\rectangle.midfile";
#ifdef _DEBUG
	const char model[] = "resources\\models\\ifc_midfile\\rectangle.midfile";
#else
	const char model[] = "resources\\models\\ifc\\ff.ifc";
#endif // _DEBUG

typedef void (*ifcre_set_config)(const char* key, const char* value);
typedef void (*ifcre_init)();
typedef void (*ifcre_run)();

struct ifcre {
	ifcre_set_config set_config;
	ifcre_init init;
	ifcre_run run;
	bool loaded = false;
};

void create_ifcre(ifcre* re) {
	HMODULE module = LoadLibrary(L"ifc-render-engine.dll");
	if (module == NULL)
	{
		printf("load ifc-render-engine failed.\n");
		return;
	}
	re->set_config = (ifcre_set_config)GetProcAddress(module, "ifcre_set_config");
	re->init = (ifcre_init)GetProcAddress(module, "ifcre_init");
	re->run = (ifcre_run)GetProcAddress(module, "ifcre_run");
	if (re->set_config != nullptr
		&& re->init != nullptr
		&& re->run != nullptr) {
		re->loaded = true;
	}
	return;
}
int main(void) {
	ifcre re;
	create_ifcre(&re);
	if (!re.loaded) {
		return -1;
	}

	re.set_config("width", "1600");
	re.set_config("height", "900");
	re.set_config("model_type", "ifc");
	re.set_config("use_transparency", "true");
	re.set_config("file", model);

	re.init();
	re.run();

	return 0;
}