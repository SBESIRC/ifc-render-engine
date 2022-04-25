#include "../ifc-render-engine/src/ifcre/ifcre_export.h"

//const char bunny_model[] = "resources\\models\\bunny\\bunny.obj";
//const char bunny_model[] = "resources\\models\\ifc_midfile\\rectangle.midfile";
const char model[] = "resources\\models\\ifc_midfile\\temp.midfile";
int main(void) {
	ifcre_set_config("width", "1600");
	ifcre_set_config("height", "900");
	ifcre_set_config("model_type", "ifc");
	ifcre_set_config("use_transparency", "true");
	ifcre_set_config("file", model);

	ifcre_init();
	ifcre_run();

	return 0;
}