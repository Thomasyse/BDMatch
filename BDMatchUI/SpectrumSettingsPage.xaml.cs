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
        }

        SharingHelper sharing_helper = null;
        SettingHelper settings = null;
        TextHelper text_helper = null;
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            if (e.Parameter is SharingHelper && e.Parameter != null)
            {
                sharing_helper = e.Parameter as SharingHelper;
                settings = sharing_helper.settings;
                text_helper = sharing_helper.text_helper;
                DrawSpec.IsOn = Convert.ToBoolean(settings[SettingType.DrawSpec]);
            }
            base.OnNavigatedTo(e);
        }

        private void DrawSpec_Toggled(object sender, RoutedEventArgs e)
        {
            settings[SettingType.DrawSpec] = Convert.ToInt32(DrawSpec.IsOn);
        }
    }
}
