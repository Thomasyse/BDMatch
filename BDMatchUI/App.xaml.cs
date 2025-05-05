using Microsoft.UI.Xaml;
using System;
using Microsoft.Windows.ApplicationModel.Resources;
using BDMatchUI.Helper;
using System.Collections.Generic;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace BDMatchUI
{
    static class Constants
    {
        public const string App_Version = "2.0.6";
        public const string FFmpeg_Version = "7.1";
        public const string FFTW_Version = "3.3.10";
        public const string CopyRight_Year = "2025";
    }
    static class AppResources
    {
        public static readonly List<string> supported_languages = new List<string>(["zh-Hans-CN", "en-US"]);
        // public static ResourceLoader m_resourceLoader;
        public static ResourceManager m_resourceManager;
        public static ResourceContext m_resourceContext;
        public static ResourceMap m_resourceMap;
        private static string current_language_ = null;
        public static string current_language { get => current_language_; }

        public static string get_string(string key) { return m_resourceMap.GetValue(key, m_resourceContext).ValueAsString; }
        public static string get_err_str(MatchReturn key) { return m_resourceMap.GetValue("BDMatchUI/Error/" + Enum.GetName(key), m_resourceContext).ValueAsString; }
        public static void set_language(string language)
        {
            current_language_ = language;
            m_resourceContext.QualifierValues["Language"] = current_language;
            m_resourceMap = m_resourceManager.MainResourceMap.GetSubtree("Resources");
        }
    }
    /// <summary>
    /// Provides application-specific behavior to supplement the default Application class.
    /// </summary>
    public partial class App : Application
    {
        /// <summary>
        /// Initializes the singleton application object.  This is the first line of authored code
        /// executed, and as such is the logical equivalent of main() or WinMain().
        /// </summary>
        public App()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Invoked when the application is launched.
        /// </summary>
        /// <param name="args">Details about the launch request and process.</param>
        protected override void OnLaunched(Microsoft.UI.Xaml.LaunchActivatedEventArgs args)
        {
            ///AppResources.m_resourceLoader = new ResourceLoader();
            AppResources.m_resourceManager = new ResourceManager();
            AppResources.m_resourceContext = AppResources.m_resourceManager.CreateResourceContext();
            AppResources.set_language(Windows.Globalization.ApplicationLanguages.Languages[0]);
            m_window = new MainWindow();
            m_window.Activate();
            WindowHelper.TrackWindow(m_window);
            WindowHelper.RestoreWindowSize(WinRT.Interop.WindowNative.GetWindowHandle(m_window));
        }

        private Window m_window;
    }
}
