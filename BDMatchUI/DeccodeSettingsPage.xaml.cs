using BDMatchUI.Helper;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Navigation;
using System;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace BDMatchUI
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class DeccodeSettingsPage : Page
    {
        public DeccodeSettingsPage()
        {
            this.InitializeComponent();
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
                foreach (var fft_num in FFTNum.Items)
                {
                    if (Convert.ToInt32(fft_num) == settings[SettingType.FFTNum])
                    {
                        FFTNum.SelectedItem = fft_num;
                        break;
                    }
                }
                MinFindDB.Value = settings[SettingType.MinFinddB];
                OutputPCM.IsOn = Convert.ToBoolean(settings[SettingType.OutputPCM]);
                ParallelDecode.IsOn = Convert.ToBoolean(settings[SettingType.ParallelDecode]);
                VolMatch.IsOn = Convert.ToBoolean(settings[SettingType.VolMatch]);
            }
            base.OnNavigatedTo(e);
        }

        private void FFTNum_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            settings[SettingType.FFTNum] = Convert.ToInt32(FFTNum.SelectedValue);
        }

        private void MinFindDB_ValueChanged(NumberBox sender, NumberBoxValueChangedEventArgs args)
        {
            settings[SettingType.MinFinddB] = Convert.ToInt32(MinFindDB.Value);
        }

        private void OutputPCM_Toggled(object sender, RoutedEventArgs e)
        {
            settings[SettingType.OutputPCM] = Convert.ToInt32(OutputPCM.IsOn);
        }

        private void ParallelDecode_Toggled(object sender, RoutedEventArgs e)
        {
            settings[SettingType.ParallelDecode] = Convert.ToInt32(ParallelDecode.IsOn);
        }

        private void VolMatch_Toggled(object sender, RoutedEventArgs e)
        {
            settings[SettingType.VolMatch] = Convert.ToInt32(VolMatch.IsOn);
        }
    }

}
