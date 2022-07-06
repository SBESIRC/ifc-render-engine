#include <iostream>
#include <Windows.h>
#include <string.h>
//const char bunny_model[] = "resources\\models\\bunny\\bunny.obj";
//const char bunny_model[] = "resources\\models\\ifc_midfile\\rectangle.midfile";
#ifdef _DEBUG
	const char model[] = "resources\\models\\ifc_midfile\\temp3.midfile";
#else
	//const char model[] = "resources\\models\\ifc\\ff.ifc";
	const char model[] = "resources\\models\\ifc\\0407.ifc";
#endif // _DEBUG

typedef void (*ifcre_set_config)(const char* key, const char* value);
typedef void (*ifcre_init)(void *);
typedef void (*ifcre_run)();
typedef int (*ifcre_get_comp_id)();
typedef void (*ifcre_set_comp_ids)();

struct ifcre {
	ifcre_set_config set_config;
	ifcre_init init;
	ifcre_run run;
	ifcre_get_comp_id get_comp_id;
	ifcre_set_comp_ids set_comp_ids;
	bool loaded = false;
};

void create_ifcre(ifcre* re) {
	HMODULE module = LoadLibrary(L"ifc-render-engine.dll");
	if (module == NULL)
	{
		std::cout << GetLastError() << "\n";
		printf("load ifc-render-engine failed.\n");
		return;
	}
	re->set_config = (ifcre_set_config)GetProcAddress(module, "ifcre_set_config");
	re->init = (ifcre_init)GetProcAddress(module, "ifcre_init");
	re->run = (ifcre_run)GetProcAddress(module, "ifcre_run");
	re->get_comp_id = (ifcre_get_comp_id)GetProcAddress(module, "ifcre_get_comp_id");
	re->set_comp_ids = (ifcre_set_comp_ids)GetProcAddress(module, "ifcre_set_comp_ids");
	if (re->set_config != nullptr
		&& re->init != nullptr
		&& re->run != nullptr
		&& re->get_comp_id != nullptr) {
		re->loaded = true;
	}
	return;
}
int main(int argc, char**argv) {
	ifcre re;
	create_ifcre(&re);
	if (!re.loaded) {
		return -1;
	}

	re.set_config("width", "1600");
	re.set_config("height", "900");
	re.set_config("model_type", "ifc");
	re.set_config("use_transparency", "true");
	re.set_config("file", argc == 1? model : argv[1]);

	//if (argc == 3 && strcmp(argv[2], "-vk") == 0) {
	//	re.set_config("render_api", "vulkan");
	//	printf("rendering by vulkan\n");
	//}
	//else {
		re.set_config("render_api", "opengl");
		printf("rendering by opengl\n");
	//}

//#ifdef _DEBUG
//	re.set_config("render_api", "vulkan");
//#endif

	re.init(NULL);
	re.run();
	printf("%d\n", re.get_comp_id());
	
	return 0;
}