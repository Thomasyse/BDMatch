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
    public sealed partial class MatchSettingsPage : Page
    {
        public MatchSettingsPage()
        {
            this.InitializeComponent();
            load_control_text();
        }

        private void load_control_text()
        {
            MatchSettingsText.Text = AppResources.get_string("BDMatchUI/MainWindow/Navi/Match_Settings/Content");
            MatchEnText.Text = AppResources.get_string("BDMatchUI/MatchSettingsPage/MatchEnText/Text");
            MatchEn.OffContent = AppResources.get_string("BDMatchUI/Common/Text_Off");
            MatchEn.OnContent = AppResources.get_string("BDMatchUI/Common/Text_On");
            MatchEnElbr.Text = AppResources.get_string("BDMatchUI/MatchSettingsPage/MatchEnElbr/Text");
            SearchRangeText.Text = AppResources.get_string("BDMatchUI/MatchSettingsPage/SearchRangeText/Text");
            SecText.Text = AppResources.get_string("BDMatchUI/Common/Text_Sec");
            SearchRangeElbr.Text = AppResources.get_string("BDMatchUI/MatchSettingsPage/SearchRangeElbr/Text");
            MaxLengthText.Text = AppResources.get_string("BDMatchUI/MatchSettingsPage/MaxLengthText/Text");
            SecText2.Text = AppResources.get_string("BDMatchUI/Common/Text_Sec");
            MaxLengthElbr.Text = AppResources.get_string("BDMatchUI/MatchSettingsPage/MaxLengthElbr/Text");
            MinCnfrmNumText.Text = AppResources.get_string("BDMatchUI/MatchSettingsPage/MinCnfrmNumText/Text");
            MinCnfrmNumElbr.Text = AppResources.get_string("BDMatchUI/MatchSettingsPage/MinCnfrmNumElbr/Text");
            SubOffsetText.Text = AppResources.get_string("BDMatchUI/Common/Text_SubTimeDelay");
            CentiSecText.Text = AppResources.get_string("BDMatchUI/Common/Text_CentiSec");
            SubOffsetElbr.Text = AppResources.get_string("BDMatchUI/MatchSettingsPage/SubOffsetElbr/Text");
            FastMatchText.Text = AppResources.get_string("BDMatchUI/MatchSettingsPage/FastMatchText/Text");
            FastMatch.OffContent = AppResources.get_string("BDMatchUI/Common/Text_Off");
            FastMatch.OnContent = AppResources.get_string("BDMatchUI/Common/Text_On");
            FastMatchElbr.Text = AppResources.get_string("BDMatchUI/MatchSettingsPage/FastMatchElbr/Text");

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
                MatchEn.IsOn = Convert.ToBoolean(settings[SettingType.MatchAss]);
                FastMatch.IsOn = Convert.ToBoolean(settings[SettingType.FastMatch]);
                SearchRange.Value = settings[SettingType.SearchRange];
                MaxLength.Value = settings[SettingType.MaxLength];
                MinCnfrmNum.Value = settings[SettingType.MinCnfrmNum];
                SubOffset.Value = settings[SettingType.SubOffset];
            }
            if (current_language != AppResources.current_language)load_control_text();
            base.OnNavigatedTo(e);
        }

        private void MatchEn_Toggled(object sender, RoutedEventArgs e)
        {
            settings[SettingType.MatchAss] = Convert.ToInt32(MatchEn.IsOn);
        }

        private void SearchRange_ValueChanged(NumberBox sender, NumberBoxValueChangedEventArgs args)
        {
            settings[SettingType.SearchRange] = Convert.ToInt32(SearchRange.Value);
        }

        private void MaxLength_ValueChanged(NumberBox sender, NumberBoxValueChangedEventArgs args)
        {
            settings[SettingType.MaxLength] = Convert.ToInt32(MaxLength.Value);
        }

        private void MinCnfrmNum_ValueChanged(NumberBox sender, NumberBoxValueChangedEventArgs args)
        {
            settings[SettingType.MinCnfrmNum] = Convert.ToInt32(MinCnfrmNum.Value);
        }

        private void SubOffset_ValueChanged(NumberBox sender, NumberBoxValueChangedEventArgs args)
        {
            settings[SettingType.SubOffset] = Convert.ToInt32(SubOffset.Value);
        }

        private void FastMatch_Toggled(object sender, RoutedEventArgs e)
        {
            settings[SettingType.FastMatch] = Convert.ToInt32(FastMatch.IsOn);
        }

    }
}