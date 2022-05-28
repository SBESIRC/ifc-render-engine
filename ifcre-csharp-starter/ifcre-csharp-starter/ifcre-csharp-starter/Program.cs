using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

namespace ifcre_csharp_starter
{
    class Program
    {

        [DllImport("ifc-render-engine.dll", EntryPoint = "ifcre_set_config", ExactSpelling = false, CallingConvention = CallingConvention.Cdecl)]
        public static extern void ifcre_set_config(String key, String value);

        [DllImport("ifc-render-engine.dll", EntryPoint = "ifcre_init", ExactSpelling = false, CallingConvention = CallingConvention.Cdecl)]
        public static extern void ifcre_init();

        [DllImport("ifc-render-engine.dll", EntryPoint = "ifcre_run", ExactSpelling = false, CallingConvention = CallingConvention.Cdecl)]
        public static extern void ifcre_run();

        static void Main(string[] args)
        {
            const String model = "resources\\models\\ifc\\ff.ifc";
            ifcre_set_config("width", "1600");
            ifcre_set_config("height", "900");
            ifcre_set_config("model_type", "ifc");
            ifcre_set_config("use_transparency", "true");
            ifcre_set_config("file", args.Length == 0 ? model : args[0]);

            if (args.Length == 2 && args[1].Equals("-vk"))
            {
                ifcre_set_config("render_api", "vulkan");
                Console.WriteLine("rendering by vulkan");
            }
            else
            {
                ifcre_set_config("render_api", "opengl");
                Console.WriteLine("rendering by opengl");
            }

            ifcre_init();
            ifcre_run();
        }
    }
}
