using BDMatchUI.Helper;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Navigation;
using System;
using System.Text.RegularExpressions;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.
namespace BDMatchUI
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class SpectrumSettingsPage : Page
    {
        public SpectrumSettingsPage()
        {
            this.InitializeComponent();
        }

        private void load_control_text()
        {
            SpectrumSettingsText.Text = AppResources.get_string("BDMatchUI/MainWindow/Navi/Spectrum_Settings/Content");
            DrawSpecText.Text = AppResources.get_string("BDMatchUI/SpectrumSettingsPage/DrawSpecText/Text");
            DrawSpec.OffContent = AppResources.get_string("BDMatchUI/Common/Text_Off");
            DrawSpec.OnContent = AppResources.get_string("BDMatchUI/Common/Text_On");
            DrawSpecElbr.Text = AppResources.get_string("BDMatchUI/SpectrumSettingsPage/DrawSpecElbr/Text");

            current_language = new string(AppResources.current_language);
        }

        SharingHelper sharing_helper;
        SettingHelper settings;
        string current_language = null;
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            if (e.Parameter is SharingHelper && e.Parameter != null)
            {
                sharing_helper = e.Parameter as SharingHelper;
                settings = sharing_helper.settings;
                DrawSpec.IsOn = Convert.ToBoolean(settings[SettingType.DrawSpec]);
            }
            if (current_language != AppResources.current_language) load_control_text();
            base.OnNavigatedTo(e);
        }

        private void DrawSpec_Toggled(object sender, RoutedEventArgs e)
        {
            settings[SettingType.DrawSpec] = Convert.ToInt32(DrawSpec.IsOn);
        }
    }
}
