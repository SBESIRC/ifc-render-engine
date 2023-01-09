using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Windows.Documents;
using OpenTK.Graphics.OpenGL;
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
        [DllImport("ifc-render-engine.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.None, ExactSpelling = false)]
        public static extern void ifcre_set_edge_indices(int val);

        [DllImport("ifc-render-engine.dll")]//, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.None, ExactSpelling = false)]
#pragma warning disable CS3001 // 参数类型不符合 CLS
		public static unsafe extern void ifcre_init(Window* wndPtr);

#pragma warning restore CS3001 // 参数类型不符合 CLS

        [DllImport("ifc-render-engine.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.None, ExactSpelling = false)]
        public static extern int ifcre_get_comp_ids_size();

        [DllImport("ifc-render-engine.dll", EntryPoint = "ifcre_get_comp_ids")]
        internal extern unsafe static void ifcre_get_comp_ids(int* arr);

        [DllImport("ifc-render-engine.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.None, ExactSpelling = false)]
        public static extern void ifcre_run();

        [DllImport("ifc-render-engine.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.None, ExactSpelling = false)]
        public static extern void ifcre_set_config(string key, string value);

        [DllImport("ifc-render-engine.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.None, ExactSpelling = false)]
        public static extern int ifcre_get_comp_id();

        [DllImport("ifc-render-engine.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.None, ExactSpelling = false)]
        public static extern void ifcre_set_comp_ids(int val);

        [DllImport("ifc-render-engine.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.None, ExactSpelling = false)]
        public static extern void ifcre_set_sleep_time(int val);

        [DllImport("ifc-render-engine.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.None, ExactSpelling = false)]
        public static extern void ifcre_set_data_ready_status(bool val);

        [DllImport("ifc-render-engine.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.None, ExactSpelling = false)]
        public static extern bool ifcre_save_image(string value);

        [DllImport("ifc-render-engine.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.None, ExactSpelling = false)]
        public static extern void ifcre_home();

        //[DllImport("ifc-render-engine.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.None, ExactSpelling = false)]
        //public static extern void ifcre_set_collide_command(int val);

        //[DllImport("ifc-render-engine.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.None, ExactSpelling = false)]
        //public static extern void ifcre_set_collide_idsA(int val);

        //[DllImport("ifc-render-engine.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.None, ExactSpelling = false)]
        //public static extern void ifcre_set_collide_idsB(int val);

        //[DllImport("ifc-render-engine.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.None, ExactSpelling = false)]
        //public static extern int ifcre_get_collide_ids_size();

        //[DllImport("ifc-render-engine.dll", EntryPoint = "ifcre_set_collide_ids")]
        //internal extern unsafe static void ifcre_get_collide_ids(int* val);

        public static unsafe void Init(IntPtr wndPtr, int width, int height, string fileName)
        {
            ifcre_set_config("width", width.ToString());
            ifcre_set_config("height", height.ToString());
            ifcre_set_config("model_type", "ifc");
            ifcre_set_config("use_transparency", "true");

            ifcre_set_config("file", fileName);
            ifcre_set_config("render_api", "opengl");
            //ifcre_set_config("render_api", "vulkan");
            ifcre_set_config("reset_view_pos", ""); // 设置为空则不改变视口，不为空则改变当前视口
            Window* ptrToWnd = (Window*)wndPtr.ToPointer();

            ifcre_set_data_ready_status(false);
            ifcre_init(ptrToWnd);
            ifcre_set_data_ready_status(true);
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

        public static List<int> GetCurrentCompIDs()
        {
            List<int> list = new List<int>();
            unsafe
            {
                int size = ifcre_get_comp_ids_size();

                var arr = Marshal.AllocHGlobal(size * sizeof(int));
                var p = (int*)arr.ToPointer();
                ifcre_get_comp_ids(p);

                for (int j = 0; j < size; j++)
                {
                    list.Add(p[j]);
                }
                Marshal.FreeHGlobal(arr);
            }
            return list;
        }

        public static void SetSelectCompIDs(string to_show_states, int val)
        {
            // to_show_states = "0": 设置显示一些物件；= "1": 高亮选中一些物件
            ifcre_set_config("to_show_states", to_show_states);

            if (val == -2) // send -2 to show all components
            {
                ifcre_set_comp_ids(val);
            }
            else if (val == -1) // send -1 to hide all components
            {
                ifcre_set_comp_ids(val);
            }
            else
            {
                ifcre_set_comp_ids(-1);
                for (int i = 0; i < val; ++i)
                {
                    ifcre_set_comp_ids(i);
                }
            }
		}

        public static void SetSleepTime(int val)
		{
            if(val > 1000)
            {
                val = 1000;
            }
            ifcre_set_sleep_time(val);
        }

        public static bool SaveImage(string filePath)
        {
            return ifcre_save_image(filePath);
        }
    }
}
