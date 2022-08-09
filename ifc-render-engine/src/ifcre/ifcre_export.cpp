#include "ifc_render_engine.h"
#include "ifcre_export.h"

using namespace ifcre;

//union MtlMetaData {
//	Real f;
//	int i;
//};


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

void ifcre_clear_model_data() {
	SharedPtr<RenderEngine> render_engine = IFCRenderEngine::getSingleton();
	auto& re = *render_engine;
	re.clear_model_data();
}

void ifcre_set_g_indices(const int val) {
	SharedPtr<RenderEngine> render_engine = IFCRenderEngine::getSingleton();
	auto& re = *render_engine;
	re.set_g_indices(val);
}

void ifcre_set_g_vertices(const float val) {
	SharedPtr<RenderEngine> render_engine = IFCRenderEngine::getSingleton();
	auto& re = *render_engine;
	re.set_g_vertices(val);
}

void ifcre_set_g_normals(const float val) {
	SharedPtr<RenderEngine> render_engine = IFCRenderEngine::getSingleton();
	auto& re = *render_engine;
	re.set_g_normals(val);
}

void ifcre_set_c_indices(const int val) {
	SharedPtr<RenderEngine> render_engine = IFCRenderEngine::getSingleton();
	auto& re = *render_engine;
	re.set_c_indices(val);
}

//template <typename T>
void ifcre_set_face_mat(const float a) {
	SharedPtr<RenderEngine> render_engine = IFCRenderEngine::getSingleton();
	auto& re = *render_engine;
	re.set_face_mat(a);
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

void ifcre_set_comp_ids(const int val)
{
	SharedPtr<RenderEngine> render_engine = IFCRenderEngine::getSingleton();
	auto& re = *render_engine;
	re.setSelectCompIds(val);
}

void ifcre_set_sleep_time(int val) {
	SharedPtr<RenderEngine> render_engine = IFCRenderEngine::getSingleton();
	auto& re = *render_engine;
	re.SetSleepTime(val);
}
