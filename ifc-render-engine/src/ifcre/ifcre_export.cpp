#include "ifc_render_engine.h"
#include "ifcre_export.h"

using namespace ifcre;
void ifcre_set_config(const char* key, const char* value)
{
	SharedPtr<RenderEngine> render_engine = IFCRenderEngine::getSingleton();
	auto& re = *render_engine;
	re.setConfig(key, value);
}

void ifcre_init()
{
	SharedPtr<RenderEngine> render_engine = IFCRenderEngine::getSingleton();
	auto& re = *render_engine;
	re.init();
}

void ifcre_run()
{
	SharedPtr<RenderEngine> render_engine = IFCRenderEngine::getSingleton();
	auto& re = *render_engine;
	re.run();
}
