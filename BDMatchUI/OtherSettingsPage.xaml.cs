using BDMatchUI.Helper;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Controls.Primitives;
using Microsoft.UI.Xaml.Data;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Navigation;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace BDMatchUI
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class OtherSettingsPage : Page
    {
        public OtherSettingsPage()
        {
            this.InitializeComponent();
            LanguageCombo.ItemsSource = AppResources.supported_languages;
            ThemeCombo.ItemsSource = Enum.GetNames(typeof(BackdropType));
        }

        private void load_control_text()
        {
            OtherSettingsText.Text = AppResources.get_string("BDMatchUI/MainWindow/Navi/Other_Settings/Content");
            LanguageText.Text = AppResources.get_string("BDMatchUI/OtherSettingsPage/LanguageText/Text");
            ThemeText.Text = AppResources.get_string("BDMatchUI/OtherSettingsPage/ThemeText/Text");

            current_language = new string(AppResources.current_language);
        }

        SharingHelper sharing_helper;
        SettingHelper settings;
        string current_language = null;
        bool page_loading = false;
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            page_loading = true;
            if (e.Parameter is SharingHelper && e.Parameter != null)
            {
                sharing_helper = e.Parameter as SharingHelper;
                settings = sharing_helper.settings;
                foreach (var item in LanguageCombo.Items)
                {
                    if (item.ToString() == AppResources.current_language)
                    {
                        LanguageCombo.SelectedItem = item;
                        break;
                    }
                }
                load_ThemeCombo_selection();
            }
            if (current_language != AppResources.current_language) load_control_text();
            base.OnNavigatedTo(e);
            page_loading = false;
        }

        private void LanguageCombo_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (!page_loading)
            {
                settings.language = LanguageCombo.SelectedItem.ToString();
                AppResources.set_language(settings.language);
                var main_window = WindowHelper.GetWindowForElement(this) as MainWindow;
                main_window.load_control_text();
                load_control_text();
                CoreHelper.set_language(AppResources.current_language);
            }
        }

        private void load_ThemeCombo_selection()
        {
            foreach (var item in ThemeCombo.Items)
            {
                if (item.ToString() == settings.backdrop.ToString())
                {
                    ThemeCombo.SelectedItem = item;
                    break;
                }
            }
        }

        private void ThemeCombo_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (!page_loading)
            {
                page_loading = true;
                BackdropType new_backdrop = (BackdropType)Enum.Parse(typeof(BackdropType), ThemeCombo.SelectedItem.ToString());
                var main_window = WindowHelper.GetWindowForElement(this) as MainWindow;
                main_window.SetBackdrop(new_backdrop);
                if (settings.backdrop != new_backdrop) {
                    load_ThemeCombo_selection();
                }
                page_loading= false;
            }
        }
    }
}
