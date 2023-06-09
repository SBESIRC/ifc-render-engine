#include "ifcre/ifc_render_engine.h"

using namespace ifcre;

SharedPtr<RenderEngine> render_engine;

//const char bunny_model[] = "resources\\models\\bunny\\bunny.obj";
//const char bunny_model[] = "resources\\models\\ifc_midfile\\rectangle.midfile";
const char bunny_model[] = "resources\\models\\ifc_midfile\\temp.midfile";
int main(void) {
	render_engine = IFCRenderEngine::getSingleton();

	auto& re = *render_engine;
	std::map<String, String> configs;
	//configs.insert(std::make_pair("width", "1600"));
	//configs.insert(std::make_pair("height", "900"));
	//configs.insert(std::make_pair("model_type", "ifc"));
	//configs.insert(std::make_pair("use_transparency", "true"));
	//configs.insert(std::make_pair("file", bunny_model));
	//re.initialize(configs);
	re.setConfig("width", "1600");
	re.setConfig("height", "900");
	re.setConfig("model_type", "ifc");
	re.setConfig("use_transparency", "true");
	re.setConfig("file", bunny_model);
	re.init();
	re.run();

	return 0;
}