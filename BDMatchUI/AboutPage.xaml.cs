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
        }

        private void AboutPage_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            AboutPanel.MaxWidth = this.ActualWidth;
            AboutHeaderPanel.MaxWidth = this.ActualWidth;
        }

        SharingHelper sharing_helper = null;
        TextHelper text_helper = null;
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            if (e.Parameter is SharingHelper && e.Parameter != null)
            {
                sharing_helper = e.Parameter as SharingHelper;
                text_helper = sharing_helper.text_helper;
            }
            base.OnNavigatedTo(e);
        }
    }
}
