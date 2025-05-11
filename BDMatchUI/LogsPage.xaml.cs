using BDMatchUI.Helper;
using Microsoft.UI.Dispatching;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Documents;
using Microsoft.UI.Xaml.Navigation;
using System.Numerics;
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
        }

        private void LogsPage_ActualThemeChanged(FrameworkElement sender, object args)
        {
            load_log();
        }

        SharingHelper sharing_helper = null;
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            bool first_navigated = (sharing_helper == null);
            if (e.Parameter is SharingHelper && e.Parameter != null) 
                sharing_helper = e.Parameter as SharingHelper;
            if (sharing_helper != null)
            {
                sharing_helper.log.load_log = load_log;
                if (first_navigated) sharing_helper.theme_change_action_list.Add(LogsPage_ActualThemeChanged);
                load_log();
            }
            base.OnNavigatedTo(e);
        }

        int anchor_update_req_cnt = 0;
        private void load_log()
        {
            if (sharing_helper != null)
            {
                LogBlock.Blocks.Clear();
                if (sharing_helper.log.logs.Count > 500) sharing_helper.log.logs.RemoveRange(0, sharing_helper.log.logs.Count - 500);
                foreach (var log_item in sharing_helper.log.logs)
                {
                    Paragraph paragraph = new();
                    Run run = new()
                    {
                        Text = log_item.Item1,
                        FontSize = 20,
                        Foreground = StyleHelper.color_brush(log_item.Item2)
                    };
                    paragraph.Inlines.Add(run);
                    LogBlock.Blocks.Add(paragraph);
                }
                anchor_update_req_cnt = 2;
            }
        }

        private void LogsPage_SizeChanged(object sender, SizeChangedEventArgs e) => LogPanel.MaxWidth = ActualWidth;

        private void LogScroll_ViewChanged(object sender, ScrollViewerViewChangedEventArgs e)
        {
            if (anchor_update_req_cnt == 0)
            {
                sharing_helper.log.ScrollVerticalOffset = LogScroll.VerticalOffset;
                if (LogScroll.VerticalOffset == LogScroll.ScrollableHeight)
                {
                    sharing_helper.log.scrollAncherType = Scroll_Anchor_Type.End;
                    sharing_helper.log.badge.Value = 0;
                }
                else sharing_helper.log.scrollAncherType = Scroll_Anchor_Type.Other;
            }
        }

        private void LogScroll_AnchorRequested(ScrollViewer sender, AnchorRequestedEventArgs args)
        {
            bool scroll_to_end = sharing_helper.log.scrollAncherType == Scroll_Anchor_Type.End;
            double scroll_offset = scroll_to_end ? LogScroll.ScrollableHeight : sharing_helper.log.ScrollVerticalOffset;
            LogScroll.ChangeView(null, scroll_offset, null);
            if (scroll_offset == 0 && LogScroll.ScrollableHeight == 0) sharing_helper.log.badge.Value = 0;
            if (anchor_update_req_cnt > 0)
            {
                anchor_update_req_cnt--;
                if (scroll_to_end)sharing_helper.log.badge.Value = 0;
            }
        }
    }
}
