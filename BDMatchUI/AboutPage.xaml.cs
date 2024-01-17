using BDMatchUI.Helper;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Navigation;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace BDMatchUI
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class AboutPage : Page
    {
        public AboutPage()
        {
            this.InitializeComponent();
            load_control_text();
        }

        private void load_control_text()
        {
            AboutHeader.Text = AppResources.get_string("BDMatchUI/MainWindow/Navi/About/Content");
            var about_format = AppResources.get_string("BDMatchUI/AboutPage/Text_About");
            AboutText.Text = string.Format(about_format, Constants.App_Version, Constants.FFmpeg_Version, Constants.FFTW_Version, Constants.CopyRight_Year);

            current_language = new string(AppResources.current_language);
        }

        private void AboutPage_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            AboutPanel.MaxWidth = this.ActualWidth;
            AboutHeaderPanel.MaxWidth = this.ActualWidth;
        }

        string current_language = null;
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            if (current_language != AppResources.current_language) load_control_text();
            base.OnNavigatedTo(e);
        }
    }
}
