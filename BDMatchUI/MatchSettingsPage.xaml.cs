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
                MatchEn.IsOn = Convert.ToBoolean(settings[SettingType.MatchAss]);
                FastMatch.IsOn = Convert.ToBoolean(settings[SettingType.FastMatch]);
                SearchRange.Value = settings[SettingType.SearchRange];
                MaxLength.Value = settings[SettingType.MaxLength];
                MinCnfrmNum.Value = settings[SettingType.MinCnfrmNum];
                SubOffset.Value = settings[SettingType.SubOffset];
            }
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