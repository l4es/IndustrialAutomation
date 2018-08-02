// ================================================================================
// PopupKeyboard.xaml.cs
//
// This file contains the implementation of a numeric on-screen keyboard using
// attached behaviors in WPF
// Weidong Shen - Dec. 2008
// ================================================================================
using System;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Data;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.Security;
using System.Security.Permissions;
using ScadaShell.Interfaces;

namespace ScadaShell.Infrastructure
{
    /// <summary>
    /// Interaction logic for PopupKeyboard.xaml
    /// </summary>

    public partial class VirtualKeyboard : UserControl
    {
        #region Private data

        private Popup _parentPopup;
        private Storyboard storyboard;
        private const double AnimationDelay = 150;
        #endregion Private data


        #region Constructor

        /// <summary>
        /// Constructor
        /// </summary>
        public VirtualKeyboard()
        {
            InitializeComponent();

        }

        #endregion Constructor

        #region Properties

        /// <summary>
        /// Placement
        /// </summary>
        public static readonly DependencyProperty PlacementProperty =
            Popup.PlacementProperty.AddOwner(typeof(VirtualKeyboard));

        public PlacementMode Placement
        {
            get { return (PlacementMode)GetValue(PlacementProperty); }
            set { SetValue(PlacementProperty, value); }
        }

        /// <summary>
        /// PlacementTarget
        /// </summary>
        public static readonly DependencyProperty PlacementTargetProperty =
           Popup.PlacementTargetProperty.AddOwner(typeof(VirtualKeyboard));

        public UIElement PlacementTarget
        {
            get { return (UIElement)GetValue(PlacementTargetProperty); }
            set { SetValue(PlacementTargetProperty, value); }
        }

        /// <summary>
        /// PlacementRectangle
        /// </summary>
        /// 
        public static readonly DependencyProperty PlacementRectangleProperty =
            Popup.PlacementRectangleProperty.AddOwner(typeof(VirtualKeyboard));

        public Rect PlacementRectangle
        {
            get { return (Rect)GetValue(PlacementRectangleProperty); }
            set { SetValue(PlacementRectangleProperty, value); }
        }

        /// <summary>
        /// HorizontalOffset
        /// </summary>
        public static readonly DependencyProperty HorizontalOffsetProperty =
            Popup.HorizontalOffsetProperty.AddOwner(typeof(VirtualKeyboard));

        public double HorizontalOffset
        {
            get { return (double)GetValue(HorizontalOffsetProperty); }
            set { SetValue(HorizontalOffsetProperty, value); }
        }

        /// <summary>
        /// VerticalOffset
        /// </summary>
        public static readonly DependencyProperty VerticalOffsetProperty =
            Popup.VerticalOffsetProperty.AddOwner(typeof(VirtualKeyboard));

        public double VerticalOffset
        {
            get { return (double)GetValue(VerticalOffsetProperty); }
            set { SetValue(VerticalOffsetProperty, value); }
        }

        /// <summary>
        /// StaysOpen
        /// </summary>
        public static readonly DependencyProperty StaysOpenProperty =
            Popup.StaysOpenProperty.AddOwner(typeof(VirtualKeyboard));

        public bool StaysOpen
        {
            get { return (bool)GetValue(StaysOpenProperty); }
            set { SetValue(StaysOpenProperty, value); }
        }

        /// <summary>
        /// CustomPopupPlacementCallback
        /// </summary>
        public static readonly DependencyProperty CustomPopupPlacementCallbackProperty =
            Popup.CustomPopupPlacementCallbackProperty.AddOwner(typeof(VirtualKeyboard));

        public CustomPopupPlacementCallback CustomPopupPlacementCallback
        {
            get { return (CustomPopupPlacementCallback)GetValue(CustomPopupPlacementCallbackProperty); }
            set { SetValue(CustomPopupPlacementCallbackProperty, value); }
        }

        /// <summary>
        /// IsOpen
        /// </summary>
        public static readonly DependencyProperty IsOpenProperty =
            Popup.IsOpenProperty.AddOwner(
            typeof(VirtualKeyboard),
            new FrameworkPropertyMetadata(
                false,
                FrameworkPropertyMetadataOptions.BindsTwoWayByDefault,
                new PropertyChangedCallback(IsOpenChanged)));

        public bool IsOpen
        {
            get { return (bool)GetValue(IsOpenProperty); }
            set { SetValue(IsOpenProperty, value); }
        }



        #endregion Properties

        #region Private Methods

        /// <summary>
        /// PropertyChangedCallback method for IsOpen Property
        /// </summary>
        /// <param name="element"></param>
        /// <param name="e"></param>
        private static void IsOpenChanged(DependencyObject element, DependencyPropertyChangedEventArgs e)
        {
            VirtualKeyboard ctrl = (VirtualKeyboard)element;

            if ((bool)e.NewValue)
            {
                if (ctrl._parentPopup == null)
                {
                    ctrl.HookupParentPopup();
                }
            }
        }

        /// <summary>
        /// Create the Popup and attach the CustomControl to it.
        /// </summary>

        private void HookupParentPopup()
        {
            _parentPopup = new Popup();

            _parentPopup.AllowsTransparency = true;
            _parentPopup.PopupAnimation = PopupAnimation.Scroll;

            // Set Height and Width
            _parentPopup.Height = this.Height;
            _parentPopup.Width = this.Width;

            Popup.CreateRootPopup(_parentPopup, this);
            _parentPopup.StaysOpen = true;

        }
        /// <summary>
        /// Animation to hide keyboard
        /// </summary>
        private void HideKeyboard()
        {
            // Animation to hide the keyboard
            this.RegisterName("HidePopupKeyboard", this);

            storyboard = new Storyboard();
            storyboard.Completed += new EventHandler(storyboard_Completed);

            DoubleAnimation widthAnimation = new DoubleAnimation();
            widthAnimation.From = this.Width;
            widthAnimation.To = 0.0;
            widthAnimation.Duration = TimeSpan.FromMilliseconds(AnimationDelay);
            widthAnimation.FillBehavior = FillBehavior.Stop;

            DoubleAnimation heightAnimation = new DoubleAnimation();
            heightAnimation.From = this.Height;
            heightAnimation.To = 0.0;
            heightAnimation.Duration = TimeSpan.FromMilliseconds(AnimationDelay);
            heightAnimation.FillBehavior = FillBehavior.Stop;

            Storyboard.SetTargetName(widthAnimation, "HidePopupKeyboard");
            Storyboard.SetTargetProperty(widthAnimation, new PropertyPath(VirtualKeyboard.WidthProperty));
            Storyboard.SetTargetName(heightAnimation, "HidePopupKeyboard");
            Storyboard.SetTargetProperty(heightAnimation, new PropertyPath(VirtualKeyboard.HeightProperty));
            storyboard.Children.Add(widthAnimation);
            storyboard.Children.Add(heightAnimation);

            storyboard.Begin(this);
        }

        /// <summary>
        /// Event handler for storyboard Completed
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void storyboard_Completed(object sender, EventArgs e)
        {
            this.IsOpen = false;
        }

        #endregion Private Methods

        #region Keyboard Constants

        private const uint KEYEVENTF_KEYUP = 0x2;  // Release key
        private const byte VK_BACK = 0x8;          // back space
        private const byte VK_LEFT = 0x25;
        private const byte VK_RIGHT = 0x27;
        private const byte VK_0 = 0x30;
        private const byte VK_1 = 0x31;
        private const byte VK_2 = 0x32;
        private const byte VK_3 = 0x33;
        private const byte VK_4 = 0x34;
        private const byte VK_5 = 0x35;
        private const byte VK_6 = 0x36;
        private const byte VK_7 = 0x37;
        private const byte VK_8 = 0x38;
        private const byte VK_9 = 0x39;

        #endregion Keyboard Constants

        #region Keyboard Private Methods

        /// <summary>
        /// Event handler for all numeric keyboard events
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void cmdNumericButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                Keyboard.Focus(this.InputBox);
                Button key = (Button)sender;

                switch (key.Name)
                {
                    // Number 1
                    case "btn010300":
                        keybd_event(VK_1, 0, 0, (UIntPtr)0);
                        keybd_event(VK_1, 0, KEYEVENTF_KEYUP, (UIntPtr)0);
                        // event already handle
                        e.Handled = true;
                        break;
                    // Number 2
                    case "btn010301":
                        keybd_event(VK_2, 0, 0, (UIntPtr)0);
                        keybd_event(VK_2, 0, KEYEVENTF_KEYUP, (UIntPtr)0);
                        // event already handle
                        e.Handled = true;
                        break;
                    // Number 3
                    case "btn010302":
                        keybd_event(VK_3, 0, 0, (UIntPtr)0);
                        keybd_event(VK_3, 0, KEYEVENTF_KEYUP, (UIntPtr)0);
                        // event already handle
                        e.Handled = true;
                        break;
                    // Number 4
                    case "btn010200":
                        keybd_event(VK_4, 0, 0, (UIntPtr)0);
                        keybd_event(VK_4, 0, KEYEVENTF_KEYUP, (UIntPtr)0);
                        // event already handle
                        e.Handled = true;
                        break;
                    // Number 5
                    case "btn010201":
                        keybd_event(VK_5, 0, 0, (UIntPtr)0);
                        keybd_event(VK_5, 0, KEYEVENTF_KEYUP, (UIntPtr)0);
                        // event already handle
                        e.Handled = true;
                        break;
                    // Number 6
                    case "btn010202":
                        keybd_event(VK_6, 0, 0, (UIntPtr)0);
                        keybd_event(VK_6, 0, KEYEVENTF_KEYUP, (UIntPtr)0);
                        // event already handle
                        e.Handled = true;
                        break;
                    // Number 7
                    case "btn010100":
                        keybd_event(VK_7, 0, 0, (UIntPtr)0);
                        keybd_event(VK_7, 0, KEYEVENTF_KEYUP, (UIntPtr)0);
                        // event already handle
                        e.Handled = true;
                        break;
                    // Number 8
                    case "btn010101":
                        keybd_event(VK_8, 0, 0, (UIntPtr)0);
                        keybd_event(VK_8, 0, KEYEVENTF_KEYUP, (UIntPtr)0);
                        // event already handle
                        e.Handled = true;
                        break;
                    // Number 9
                    case "btn010102":
                        keybd_event(VK_9, 0, 0, (UIntPtr)0);
                        keybd_event(VK_9, 0, KEYEVENTF_KEYUP, (UIntPtr)0);
                        // event already handle
                        e.Handled = true;
                        break;
                    // Number 0
                    case "btn010400":
                        keybd_event(VK_0, 0, 0, (UIntPtr)0);
                        keybd_event(VK_0, 0, KEYEVENTF_KEYUP, (UIntPtr)0);
                        // event already handle
                        e.Handled = true;
                        break;
                    // Symbol minus sign
                    case "btn010103":
                        keybd_event(0xbd, 0, 0, (UIntPtr)0);
                        keybd_event(0xbd, 0, KEYEVENTF_KEYUP, (UIntPtr)0);
                        // event already handle
                        e.Handled = true;
                        break;
                    // Back Space
                    case "btn010402":
                        keybd_event(VK_BACK, 0, 0, (UIntPtr)0);
                        keybd_event(VK_BACK, 0, KEYEVENTF_KEYUP, (UIntPtr)0);
                        // event already handle
                        e.Handled = true;
                        break;
                    // Left arrow Key
                    case "btn010203":
                        keybd_event(VK_LEFT, 0, 0, (UIntPtr)0);
                        keybd_event(VK_LEFT, 0, KEYEVENTF_KEYUP, (UIntPtr)0);
                        // event already handle
                        e.Handled = true;
                        break;
                    // Right arrow Key
                    case "btn010303":
                        keybd_event(VK_RIGHT, 0, 0, (UIntPtr)0);
                        keybd_event(VK_RIGHT, 0, KEYEVENTF_KEYUP, (UIntPtr)0);
                        // event already handle
                        e.Handled = true;
                        break;
                    // Symbol full stop
                    case "btn010401":
                        keybd_event(0xbe, 0, 0, (UIntPtr)0);
                        keybd_event(0xbe, 0, KEYEVENTF_KEYUP, (UIntPtr)0);
                        // event already handle
                        e.Handled = true;
                        break;
                }
            }
            catch
            {
                // Any exception handling here.  Otherwise, swallow the exception.
            }
        }

        private void enter_Click(object sender, RoutedEventArgs e)
        {
            this.IsOpen = false;
            GetCommand(PlacementTarget).Execute(InputBox.Text);
        }
        /// <summary>
        /// Event handler to close the UserControl when close button is clicked
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void CloseButton_Click(object sender, RoutedEventArgs e)
        {
            this.IsOpen = false;
        }

        #endregion Keyboard Private Methods

        #region Windows API Functions

        [DllImport("user32.dll", CharSet = CharSet.Auto)]
        private static extern void keybd_event(byte bVk, byte bScan, uint dwFlags, UIntPtr dwExtraInfo);

        #endregion Windows API Functions

        #region AttachedProperties

        public static readonly DependencyProperty DescriptionProperty = DependencyProperty.RegisterAttached(
            "Description", typeof(String), typeof(VirtualKeyboard), new PropertyMetadata(null, null));

        public static String GetDescription(DependencyObject dp)
        {
            return (String)dp.GetValue(DescriptionProperty);
        }
        public static void SetDescription(DependencyObject dp, String value)
        {
            dp.SetValue(DescriptionProperty, value);
        }

     

        public static readonly DependencyProperty EnabledProperty = DependencyProperty.RegisterAttached(
            "Enabled", typeof(Boolean), typeof(VirtualKeyboard), new PropertyMetadata(false, OnEnabledPropertyChanged));
        public static Boolean GetEnabled(DependencyObject dp)
        {
            return (Boolean)dp.GetValue(EnabledProperty);
        }
        public static void SetEnabled(DependencyObject dp, Boolean value)
        {
            dp.SetValue(EnabledProperty, value);
        }

        public static readonly DependencyProperty CommandProperty =
            DependencyProperty.RegisterAttached("Command",
            typeof(ICommand),
            typeof(VirtualKeyboard),
            new FrameworkPropertyMetadata(null, null));


        public static ICommand GetCommand(DependencyObject dp)
        {

            return (ICommand)dp.GetValue(CommandProperty);
        }

        public static void SetCommand(DependencyObject dp, ICommand value)
        {
               dp.SetValue(CommandProperty, value);
        }

        
        
        private static void OnEnabledPropertyChanged(DependencyObject dp, DependencyPropertyChangedEventArgs e)
        {
            UIElement element = dp as UIElement;

            if (element == null)
            {
                return;
            }

            
            if ((bool)e.NewValue == false)
            {
                element.MouseLeftButtonUp -= onMouseLeftButtonUp;
            }

            if ((bool)e.NewValue == true)
            {
                //element.GotKeyboardFocus += element_GotKeyboardFocus;
                element.MouseLeftButtonUp += onMouseLeftButtonUp;
            }
        }
        
        static void onMouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            // button released
            
            FrameworkElement frameworkElement = sender as FrameworkElement;
            
            if (frameworkElement == null)
                throw new ArgumentNullException("frameworkElement");

            if (GetCommand(frameworkElement).CanExecute(null))
            {

                VirtualKeyboard _popupKeyboardUserControl = new VirtualKeyboard();
                // Set all the necessary properties
                _popupKeyboardUserControl.Placement = PlacementMode.Center;
                _popupKeyboardUserControl.PlacementTarget = frameworkElement;
                _popupKeyboardUserControl.StaysOpen = false;
                _popupKeyboardUserControl.description.Text = GetDescription(frameworkElement);
                _popupKeyboardUserControl.IsOpen = true;
                e.Handled = true;
            }
        }
        
        #endregion
    }



    public enum KeyboardState
    {
        Normal,
        Hidden
    }
}
