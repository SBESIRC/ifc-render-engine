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

        [DllImport("ifc-render-engine.dll")]//, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.None, ExactSpelling = false)]
#pragma warning disable CS3001 // 参数类型不符合 CLS
		public static unsafe extern void ifcre_init(Window* wndPtr);
#pragma warning restore CS3001 // 参数类型不符合 CLS

		[DllImport("ifc-render-engine.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.None, ExactSpelling = false)]
        public static extern void ifcre_run();

        [DllImport("ifc-render-engine.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.None, ExactSpelling = false)]
        public static extern void ifcre_set_config(string key, string value);

        public static unsafe void Init(IntPtr wndPtr, int width, int height)
        {
            ifcre_set_config("width", width.ToString());
            ifcre_set_config("height", height.ToString());
            ifcre_set_config("model_type", "ifc");
            ifcre_set_config("use_transparency", "true");
            var filePath = ".\\ff.ifc";
#if DEBUG
            filePath = ".\\temp3.midfile";
#endif
            ifcre_set_config("file", filePath);
            ifcre_set_config("render_api", "opengl");
            //ifcre_set_config("render_api", "vulkan");
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
    }
}
