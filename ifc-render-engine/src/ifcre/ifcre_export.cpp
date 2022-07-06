#include "ifc_render_engine.h"
#include "ifcre_export.h"

using namespace ifcre;
void ifcre_set_config(const char* key, const char* value)
{
	SharedPtr<RenderEngine> render_engine = IFCRenderEngine::getSingleton();
	auto& re = *render_engine;
	re.setConfig(key, value);
}

void ifcre_init(GLFWwindow * wndPtr)
{
	SharedPtr<RenderEngine> render_engine = IFCRenderEngine::getSingleton();
	auto& re = *render_engine;
	re.init(wndPtr);
}

void ifcre_run()
{
	SharedPtr<RenderEngine> render_engine = IFCRenderEngine::getSingleton();
	auto& re = *render_engine;
	re.run();
}

int ifcre_get_comp_id()
{
	SharedPtr<RenderEngine> render_engine = IFCRenderEngine::getSingleton();
	auto& re = *render_engine;
	return re.getSelectedCompId();
}

void ifcre_set_comp_ids()
{
	SharedPtr<RenderEngine> render_engine = IFCRenderEngine::getSingleton();
	auto& re = *render_engine;
	re.setSelectCompIds();
}
