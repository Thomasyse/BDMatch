using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using System;
using BDMatchUI.Helper;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace BDMatchUI
{
    using Windows.Graphics;
    /// <summary>
    /// An empty window that can be used on its own or navigated to within a Frame.
    /// </summary>
    using WinRT; // required to support Window.As<ICompositionSupportsSystemBackdrop>()

    public sealed partial class MainWindow : Window
    {

        WindowsSystemDispatcherQueueHelper m_wsdqHelper; // See separate sample below for implementation
        BackdropType m_currentBackdrop;
        Microsoft.UI.Composition.SystemBackdrops.MicaController m_micaController;
        Microsoft.UI.Composition.SystemBackdrops.DesktopAcrylicController m_acrylicController;
        Microsoft.UI.Composition.SystemBackdrops.SystemBackdropConfiguration m_configurationSource;

        private bool TrySetMicaBackdrop(BackdropType type)
        {
            if (Microsoft.UI.Composition.SystemBackdrops.MicaController.IsSupported())
            {
                m_wsdqHelper = new WindowsSystemDispatcherQueueHelper();
                m_wsdqHelper.EnsureWindowsSystemDispatcherQueueController();

                // Hooking up the policy object
                m_configurationSource = new Microsoft.UI.Composition.SystemBackdrops.SystemBackdropConfiguration();
                this.Activated += Window_Activated;
                this.Closed += Window_Closed;
                ((FrameworkElement)this.Content).ActualThemeChanged += Window_ThemeChanged;

                // Initial configuration state.
                m_configurationSource.IsInputActive = true;
                SetConfigurationSourceTheme();

                m_micaController = new Microsoft.UI.Composition.SystemBackdrops.MicaController();
                m_micaController.Kind = (type == BackdropType.MicaAlt) ? Microsoft.UI.Composition.SystemBackdrops.MicaKind.BaseAlt : Microsoft.UI.Composition.SystemBackdrops.MicaKind.Base;

                // Enable the system backdrop.
                // Note: Be sure to have "using WinRT;" to support the Window.As<...>() call.
                m_micaController.AddSystemBackdropTarget(this.As<Microsoft.UI.Composition.ICompositionSupportsSystemBackdrop>());
                m_micaController.SetSystemBackdropConfiguration(m_configurationSource);
                return true; // succeeded
            }

            return false; // Mica is not supported on this system
        }

        private bool TrySetAcrylicBackdrop(BackdropType type)
        {
            if (Microsoft.UI.Composition.SystemBackdrops.DesktopAcrylicController.IsSupported())
            {
                m_wsdqHelper = new WindowsSystemDispatcherQueueHelper();
                m_wsdqHelper.EnsureWindowsSystemDispatcherQueueController();

                // Hooking up the policy object
                m_configurationSource = new Microsoft.UI.Composition.SystemBackdrops.SystemBackdropConfiguration();
                this.Activated += Window_Activated;
                this.Closed += Window_Closed;
                ((FrameworkElement)this.Content).ActualThemeChanged += Window_ThemeChanged;

                // Initial configuration state.
                m_configurationSource.IsInputActive = true;
                SetConfigurationSourceTheme();

                m_acrylicController = new Microsoft.UI.Composition.SystemBackdrops.DesktopAcrylicController();
                m_acrylicController.Kind = (type == BackdropType.AcrylicThin) ? Microsoft.UI.Composition.SystemBackdrops.DesktopAcrylicKind.Thin : Microsoft.UI.Composition.SystemBackdrops.DesktopAcrylicKind.Base;

                // Enable the system backdrop.
                // Note: Be sure to have "using WinRT;" to support the Window.As<...>() call.
                m_acrylicController.AddSystemBackdropTarget(this.As<Microsoft.UI.Composition.ICompositionSupportsSystemBackdrop>());
                m_acrylicController.SetSystemBackdropConfiguration(m_configurationSource);
                return true; // succeeded
            }

            return false; // Mica is not supported on this system
        }

        public void SetBackdrop(BackdropType type)
        {
            // Reset to default color. If the requested type is supported, we'll update to that.
            // Note: This sample completely removes any previous controller to reset to the default
            //       state. This is done so this sample can show what is expected to be the most
            //       common pattern of an app simply choosing one controller type which it sets at
            //       startup. If an app wants to toggle between Mica and Acrylic it could simply
            //       call RemoveSystemBackdropTarget() on the old controller and then setup the new
            //       controller, reusing any existing m_configurationSource and Activated/Closed
            //       event handlers.
            m_currentBackdrop = BackdropType.DefaultColor;
            if (m_micaController != null)
            {
                m_micaController.Dispose();
                m_micaController = null;
            }
            if (m_acrylicController != null)
            {
                m_acrylicController.Dispose();
                m_acrylicController = null;
            }
            this.Activated -= Window_Activated;
            this.Closed -= Window_Closed;
            ((FrameworkElement)this.Content).ActualThemeChanged -= Window_ThemeChanged;
            m_configurationSource = null;

            if (type == BackdropType.Mica || type == BackdropType.MicaAlt)
            {
                if (TrySetMicaBackdrop(type))m_currentBackdrop = type;
                else type = BackdropType.AcrylicBase; // Mica isn't supported. Try Acrylic.
            }
            if (type == BackdropType.AcrylicBase || type == BackdropType.AcrylicThin)
            {
                if (TrySetAcrylicBackdrop(type)) m_currentBackdrop = type;
                else { } // Acrylic isn't supported, so take the next option, which is DefaultColor, which is already set.
            }
            sharing_helper.settings.backdrop = m_currentBackdrop;

            // add_theme_change_actions();
        }

        private void add_theme_change_actions()
        {
            foreach (var action in sharing_helper.theme_change_action_list)
            {
                ((FrameworkElement)this.Content).ActualThemeChanged += action;
            }
        }

        private void Window_Activated(object sender, WindowActivatedEventArgs args)
        {
            m_configurationSource.IsInputActive = args.WindowActivationState != WindowActivationState.Deactivated;
        }

        private void Window_Closed(object sender, WindowEventArgs args)
        {
            // Make sure any Mica/Acrylic controller is disposed so it doesn't try to
            // use this closed window.
            if (m_micaController != null)
            {
                m_micaController.Dispose();
                m_micaController = null;
            }
            if (m_acrylicController != null)
            {
                m_acrylicController.Dispose();
                m_acrylicController = null;
            }
            this.Activated -= Window_Activated;
            m_configurationSource = null;
        }

        private void Window_ThemeChanged(FrameworkElement sender, object args)
        {
            if (m_configurationSource != null)
            {
                SetConfigurationSourceTheme();
            }
        }

        private void SetConfigurationSourceTheme()
        {
            switch (((FrameworkElement)this.Content).ActualTheme)
            {
                case ElementTheme.Dark: m_configurationSource.Theme = Microsoft.UI.Composition.SystemBackdrops.SystemBackdropTheme.Dark; break;
                case ElementTheme.Light: m_configurationSource.Theme = Microsoft.UI.Composition.SystemBackdrops.SystemBackdropTheme.Light; break;
                case ElementTheme.Default: m_configurationSource.Theme = Microsoft.UI.Composition.SystemBackdrops.SystemBackdropTheme.Default; break;
            }
        }

        SharingHelper sharing_helper = new SharingHelper();
        public MainWindow()
        {
            // Settings
            sharing_helper.settings.load_settings();
            int win_width = sharing_helper.settings[SettingType.WindowWidth] <= 0 ? 800 : sharing_helper.settings[SettingType.WindowWidth];
            int win_height = sharing_helper.settings[SettingType.WindowHeight] <= 0 ? 800 : sharing_helper.settings[SettingType.WindowHeight];
            SizeInt32 win_size = new SizeInt32(win_width, win_height);
            AppWindow.Resize(win_size);

            // Controls
            InitializeComponent();
            Title = "BDMatch";
            ExtendsContentIntoTitleBar = true;
            SetTitleBar(this.AppTitleBar);
            SetBackdrop(sharing_helper.settings.backdrop);

            sharing_helper.navi_view = this.NavigationViewControl;
            load_control_text();
            NavigationViewControl.SelectedItem = LogsPage_Item;
            NavigationViewControl.SelectedItem = MatchPage_Item;
            add_theme_change_actions();

            // Match Core
            CoreHelper.new_BDMatchCore();
            CoreHelper.set_language(AppResources.current_language);
        }

        private void NavigationView_SelectionChanged(NavigationView sender, NavigationViewSelectionChangedEventArgs args)
        {
            var selectedItem = (Microsoft.UI.Xaml.Controls.NavigationViewItem)args.SelectedItem;
            string pageName = "BDMatchUI." + ((string)selectedItem.Tag);
            Type pageType = Type.GetType(pageName);

            if (pageType != null) NavigationFrame.Navigate(pageType, sharing_helper, args.RecommendedNavigationTransitionInfo);
        }

        private void MainWindow_Closed(object sender, WindowEventArgs args)
        {
            sharing_helper.settings.save_settings();
        }

        public void load_control_text()
        {
            Match_and_Result_Header.Content = AppResources.get_string("BDMatchUI/MainWindow/Navi/Match_and_Result/Content");
            MatchPage_Item.Content = AppResources.get_string("BDMatchUI/MainWindow/Navi/Match/Content");
            LogsPage_Item.Content = AppResources.get_string("BDMatchUI/MainWindow/Navi/Logs/Content");
            SpectrumPage_Item.Content = AppResources.get_string("BDMatchUI/MainWindow/Navi/Spectrum/Content");
            Settings_Header.Content = AppResources.get_string("BDMatchUI/MainWindow/Navi/Settings/Content");
            MatchSettingsPage_Item.Content = AppResources.get_string("BDMatchUI/MainWindow/Navi/Match_Settings/Content");
            DecodeSettingsPage_Item.Content = AppResources.get_string("BDMatchUI/MainWindow/Navi/Decode_Settings/Content");
            SpectrumSettingsPage_Item.Content = AppResources.get_string("BDMatchUI/MainWindow/Navi/Spectrum_Settings/Content");
            OtherSettingsPage_Item.Content = AppResources.get_string("BDMatchUI/MainWindow/Navi/Other_Settings/Content");
            Others_Header.Content = AppResources.get_string("BDMatchUI/MainWindow/Navi/Others/Content");
            HelpPage_Item.Content = AppResources.get_string("BDMatchUI/MainWindow/Navi/Help/Content");
            AboutPage_Item.Content = AppResources.get_string("BDMatchUI/MainWindow/Navi/About/Content");
        }

        private void MainWindow_SizeChanged(object sender, WindowSizeChangedEventArgs args)
        {
            sharing_helper.settings[SettingType.WindowWidth] = AppWindow.Size.Width;
            sharing_helper.settings[SettingType.WindowHeight] = AppWindow.Size.Height;
        }
    }
}
