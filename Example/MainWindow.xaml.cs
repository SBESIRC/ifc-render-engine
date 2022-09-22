using System.Runtime.InteropServices;
using System.Windows.Forms.Integration;
using System.Text;
using System.Windows;
using OpenTK;
using OpenTK.WinForms;
using System;
using System.Windows.Forms;
using System.Collections.Generic;

namespace Example {
    /// <summary>
    ///     Interaction logic for MainWindow.xaml
    /// </summary>
    public sealed partial class MainWindow {
        public MainWindow() {
            InitializeComponent();
        }

        GLControl glControl = null;

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            //var childConrol = formHost.Child as GLControl;
            OpenFileDialog fileDialog = new OpenFileDialog();
            var filter = "ifc files | *.ifc;*.midfile;";
            fileDialog.Filter = filter;
            var dialogRst = fileDialog.ShowDialog();
            if (dialogRst != System.Windows.Forms.DialogResult.OK) return;
            var fileName = fileDialog.FileName;
            var glControl = formHost.Child as GLControl;
            Example.ExampleScene.Init(glControl.Handle, glControl.Width, glControl.Height, fileName);
            Example.ExampleScene.Render();
        }

        private void formHost_Initialized(object sender, EventArgs e)
        {
			var glCtrlSetting = new GLControlSettings();
			glCtrlSetting.Profile = OpenTK.Windowing.Common.ContextProfile.Core;
			glCtrlSetting.API = OpenTK.Windowing.Common.ContextAPI.OpenGL;
			glCtrlSetting.APIVersion = new Version(4, 6);

            glControl = new GLControl(glCtrlSetting);
            glControl.EnableNativeInput();
            (sender as WindowsFormsHost).Child = glControl;
            glControl.MakeCurrent();
        }
        const int WM_CLOSE = 0x0010;
        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            SendMessage(glControl.Handle, WM_CLOSE, IntPtr.Zero, IntPtr.Zero);
        }

        [DllImport("user32.dll", EntryPoint = "SendMessage", CharSet = CharSet.Unicode)]
        internal static extern int SendMessage(IntPtr hwnd,
            int msg,
            IntPtr wParam,
            IntPtr lParam);

        [DllImport("user32.dll", EntryPoint = "SendMessage", CharSet = CharSet.Unicode)]
        internal static extern int SendMessage(IntPtr hwnd,
            int msg,
            int wParam,
            [MarshalAs(UnmanagedType.LPWStr)] StringBuilder lParam);

        [DllImport("user32.dll", EntryPoint = "SendMessage", CharSet = CharSet.Unicode)]
        internal static extern IntPtr SendMessage(IntPtr hwnd,
            int msg,
            IntPtr wParam,
            string lParam);


        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            List<int> list = Example.ExampleScene.GetCurrentCompIDs();
            string ss = "";
            foreach(int id2 in list)
            {
                ss += id2.ToString() + ", ";
            }
            System.Windows.Forms.MessageBox.Show(ss);
        }

		private void Button_Click_2(object sender, RoutedEventArgs e)
		{
			Example.ExampleScene.SetSelectCompIDs("0", Convert.ToInt32(tbIdsToShow.Text));
		}

		private void Button_Click_onelooptime(object sender, RoutedEventArgs e)
        {
            int time = int.Parse(tbIdsToShow.Text);
            Example.ExampleScene.SetSleepTime(time);
        }

        private void Button_Click_multichose(object sender, RoutedEventArgs e)
		{
            Example.ExampleScene.SetSelectCompIDs("1", Convert.ToInt32(tbIdsToShow.Text));
        }

        private void Button_Click_SaveImage(object sender, RoutedEventArgs e)
        {
            SaveFileDialog save = new SaveFileDialog();
            save.Filter = "(*.png)|*.png";
            save.Title = "请选择保存位置和文件名";
            if (save.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                if (Example.ExampleScene.SaveImage(save.FileName))
                {
                    System.Windows.Forms.MessageBox.Show("保存图像成功");
                }
                else
                {
                    System.Windows.Forms.MessageBox.Show("保存图像失败");
                }
            }
        }

        private void Button_Click_Home(object sender, RoutedEventArgs e)
        {
            Example.ExampleScene.ifcre_home();
        }
    }
}
