using System;
using System.Diagnostics;
using System.Runtime.InteropServices;
using OpenTK.Graphics.OpenGL;
//using OpenTK.Mathematics;
//using OpenTK.Windowing.GraphicsLibraryFramework;
namespace Example {
    /// Example class handling the rendering for OpenGL.
    public static class ExampleScene {

        public struct Window
        {
        }

        private static readonly Stopwatch _stopwatch = Stopwatch.StartNew();

        [DllImport("ifc-render-engine.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.None, ExactSpelling = false)]
        public static extern void ifcre_clear_model_data();
        [DllImport("ifc-render-engine.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.None, ExactSpelling = false)]
        public static extern void ifcre_set_g_indices(int val);
        [DllImport("ifc-render-engine.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.None, ExactSpelling = false)]
        public static extern void ifcre_set_g_vertices(float val);
        [DllImport("ifc-render-engine.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.None, ExactSpelling = false)]
        public static extern void ifcre_set_g_normals(float val);
        [DllImport("ifc-render-engine.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.None, ExactSpelling = false)]
        public static extern void ifcre_set_c_indices(int val);
        [DllImport("ifc-render-engine.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.None, ExactSpelling = false)]
        public static extern void ifcre_set_face_mat(float val);

        [DllImport("ifc-render-engine.dll")]//, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.None, ExactSpelling = false)]
#pragma warning disable CS3001 // 参数类型不符合 CLS
		public static unsafe extern void ifcre_init(Window* wndPtr);
#pragma warning restore CS3001 // 参数类型不符合 CLS

		[DllImport("ifc-render-engine.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.None, ExactSpelling = false)]
        public static extern void ifcre_run();

        [DllImport("ifc-render-engine.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.None, ExactSpelling = false)]
        public static extern void ifcre_set_config(string key, string value);

        [DllImport("ifc-render-engine.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.None, ExactSpelling = false)]
        public static extern int ifcre_get_comp_id();

        [DllImport("ifc-render-engine.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.None, ExactSpelling = false)]
        public static extern void ifcre_set_comp_ids();

        [DllImport("ifc-render-engine.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.None, ExactSpelling = false)]
        public static extern void ifcre_set_sleep_time(int val);


        public static unsafe void Init(IntPtr wndPtr, int width, int height, string fileName)
        {
            ifcre_set_config("width", width.ToString());
            ifcre_set_config("height", height.ToString());
            ifcre_set_config("model_type", "ifc");
            ifcre_set_config("use_transparency", "true");
            if(fileName == null)
			{
                fileName = ".\\ff.ifc";
#if DEBUG
                fileName = ".\\temp3.midfile";
#endif
            }
            ifcre_set_config("file", fileName);
            ifcre_set_config("render_api", "opengl");
            //ifcre_set_config("render_api", "vulkan");
            ifcre_set_config("reset_view_pos", ""); // 设置为空则不改变视口，不为空则改变当前视口
            Window* ptrToWnd = (Window*)wndPtr.ToPointer();

			ifcre_init(ptrToWnd);
        }
        public static void Ready() {
            Console.WriteLine("GlWpfControl is now ready");
        }

        public static void Render()
        {
            //render by c++ code
            ifcre_run();
        }

        public static int GetCurrentCompID()
        {
            int i = ifcre_get_comp_id();
            return i;
        }

        public static void SetSelectCompIDs(string selected_comp_ids)
        {
			ifcre_set_config("selectIds", selected_comp_ids);
			ifcre_set_comp_ids();
		}

        public static void SetSleepTime(int val)
		{
            ifcre_set_sleep_time(val);
        }
    }
}
