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
            load_control_text();
        }

        private void load_control_text()
        {
            DecodeSettingsText.Text = AppResources.get_string("BDMatchUI/MainWindow/Navi/Decode_Settings/Content");
            FFTNumText.Text = AppResources.get_string("BDMatchUI/DeccodeSettingsPage/FFTNumText/Text");
            FFTNumElbr.Text = AppResources.get_string("BDMatchUI/DeccodeSettingsPage/FFTNumElbr/Text");
            MinFindDBText.Text = AppResources.get_string("BDMatchUI/DeccodeSettingsPage/MinFindDBText/Text");
            DecibelText.Text = AppResources.get_string("BDMatchUI/Common/Text_Decibel");
            MinFindDBElbr.Text = AppResources.get_string("BDMatchUI/DeccodeSettingsPage/MinFindDBElbr/Text");
            OutputPCMText.Text = AppResources.get_string("BDMatchUI/DeccodeSettingsPage/OutputPCMText/Text");
            OutputPCM.OffContent = AppResources.get_string("BDMatchUI/Common/Text_Off");
            OutputPCM.OnContent = AppResources.get_string("BDMatchUI/Common/Text_On");
            OutputPCMElbr.Text = AppResources.get_string("BDMatchUI/DeccodeSettingsPage/OutputPCMElbr/Text");
            ParallelDecodeText.Text = AppResources.get_string("BDMatchUI/DeccodeSettingsPage/ParallelDecodeText/Text");
            ParallelDecode.OffContent = AppResources.get_string("BDMatchUI/Common/Text_Off");
            ParallelDecode.OnContent = AppResources.get_string("BDMatchUI/Common/Text_On");
            ParallelDecodeElbr.Text = AppResources.get_string("BDMatchUI/DeccodeSettingsPage/ParallelDecodeElbr/Text");
            VolMatchText.Text = AppResources.get_string("BDMatchUI/DeccodeSettingsPage/VolMatchText/Text");
            VolMatch.OffContent = AppResources.get_string("BDMatchUI/Common/Text_Off");
            VolMatch.OnContent = AppResources.get_string("BDMatchUI/Common/Text_On");
            VolMatchElbr.Text = AppResources.get_string("BDMatchUI/DeccodeSettingsPage/VolMatchElbr/Text");

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
            if (current_language != AppResources.current_language) load_control_text();
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
