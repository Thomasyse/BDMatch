using BDMatchUI.Helper;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Documents;
using Microsoft.UI.Xaml.Navigation;
using Windows.UI.Core;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace BDMatchUI
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class LogsPage : Page
    {
        public LogsPage()
        {
            this.InitializeComponent();
            load_control_text();
        }

        private void load_control_text()
        {
            LogsText.Text = AppResources.get_string("BDMatchUI/MainWindow/Navi/Logs/Content");

            current_language = new string(AppResources.current_language);
        }

        private void LogsPage_ActualThemeChanged(FrameworkElement sender, object args)
        {
            load_log();
        }

        SharingHelper sharing_helper = null;
        string current_language = null;
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            if (e.Parameter is SharingHelper && e.Parameter != null)
            {
                bool first_navigated = (sharing_helper == null);
                sharing_helper = e.Parameter as SharingHelper;
                sharing_helper.load_log = load_log;
                if (first_navigated) sharing_helper.theme_change_action_list.Add(LogsPage_ActualThemeChanged);
                load_log();
            }
            if (current_language != AppResources.current_language) load_control_text();
            base.OnNavigatedTo(e);
        }

        private void load_log()
        {
            if (sharing_helper != null)
            {
                LogBlock.Blocks.Clear();
                if (sharing_helper.logs.Count > 500) sharing_helper.logs.RemoveRange(0, sharing_helper.logs.Count - 100);
                foreach (var log_item in sharing_helper.logs)
                {
                    Paragraph paragraph = log_item.Item1;
                    LogLevel level = log_item.Item2;
                    foreach (var item in  paragraph.Inlines)
                    {
                        item.Foreground = StyleHelper.color_brush(level);
                    }
                    paragraph.Foreground = StyleHelper.color_brush(level);
                    LogBlock.Blocks.Add(paragraph);
                }
                LogScroll.VerticalAnchorRatio = 1;
            }
        }

        private void LogsPage_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            LogPanel.MaxWidth = this.ActualWidth;
        }
    }
}
