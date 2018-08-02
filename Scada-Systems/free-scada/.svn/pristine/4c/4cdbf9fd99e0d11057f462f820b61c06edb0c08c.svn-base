using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls.Primitives;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Runtime.InteropServices;
using System.Windows.Interop;

namespace ScadaShell.View
{
    /// <summary>
    /// Interaction logic for PopupView.xaml
    /// </summary>
    public partial class PopupView : Popup
    {
        public ViewModel.ViewModelBase _viewModel
        {
            get;
            protected set;
        }

        public PopupView(double w, double h, ViewModel.ViewModelBase content)
        {
            InitializeComponent();
            _viewModel = content;
            Width = w;
            Height = h;
            ResourceDictionary rd = new ResourceDictionary();
            //rd.Source = new Uri("pack://application:,,,/View/SchemaResources.xaml");
            //Resources = rd;
            _content.Content = content;
            Placement = PlacementMode.Center;
            PlacementTarget = (FrameworkElement)Application.Current.MainWindow.Content;
            EventHandler handler = null;
            DataContext = content;
            handler = delegate
            {
                content.RequestClose -= handler;
                IsOpen = false;
            };
            content.RequestClose += handler;
            IsOpen = true;
        }

        protected override void OnOpened(EventArgs e)
        {
            var hwnd = ((HwndSource)PresentationSource.FromVisual(this.Child)).Handle;
            RECT rect;

            if (GetWindowRect(hwnd, out rect))
            {
                SetWindowPos(hwnd, -2, rect.Left, rect.Top, (int)this.Width, (int)this.Height, 0);
            }
        }

        #region P/Invoke imports & definitions

        [StructLayout(LayoutKind.Sequential)]
        public struct RECT
        {
            public int Left;
            public int Top;
            public int Right;
            public int Bottom;
        }

        [DllImport("user32.dll")]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool GetWindowRect(IntPtr hWnd, out RECT lpRect);

        [DllImport("user32", EntryPoint = "SetWindowPos")]
        private static extern int SetWindowPos(IntPtr hWnd, int hwndInsertAfter, int x, int y, int cx, int cy, int wFlags);

        #endregion

    }
}
