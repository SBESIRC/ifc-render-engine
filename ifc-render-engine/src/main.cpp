#include "ifcre/ifc_render_engine.h"

using namespace ifcre;

SharedPtr<RenderEngine> render_engine;

const char spot_model[] = "resources\\models\\spot\\spot_triangulated_good.obj";

int main(void) {
	render_engine = IFCRenderEngine::getSingleton();

	auto& re = *render_engine;
	std::map<String, String> configs;
	configs.insert(std::make_pair("width", "1600"));
	configs.insert(std::make_pair("height", "900"));
	configs.insert(std::make_pair("file", spot_model));
	re.initialize(configs);
	re.run();

	return 0;
}