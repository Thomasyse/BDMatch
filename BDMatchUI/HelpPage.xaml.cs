using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Controls.Primitives;
using Microsoft.UI.Xaml.Data;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Navigation;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection.Emit;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace BDMatchUI
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class HelpPage : Page
    {
        public HelpPage()
        {
            this.InitializeComponent();
            load_control_text();
            HelpScrollBar_Load_Labels();
        }

        private void load_control_text()
        {
            HelpText.Text = AppResources.get_string("BDMatchUI/MainWindow/Navi/Help/Content");
            BatchProcessingHeader.Text = AppResources.get_string("BDMatchUI/HelpPage/BatchProcessingHeader/Text");
            BatchProcessingText.Text = AppResources.get_string("BDMatchUI/HelpPage/BatchProcessingText/Text");
            OneToMultiHeader.Text = AppResources.get_string("BDMatchUI/HelpPage/OneToMultiHeader/Text");
            OneToMultiText.Text = AppResources.get_string("BDMatchUI/HelpPage/OneToMultiText/Text");
            UsingRegexHeader.Text = AppResources.get_string("BDMatchUI/HelpPage/UsingRegexHeader/Text");
            UsingRegexText.Text = AppResources.get_string("BDMatchUI/HelpPage/UsingRegexText/Text");
            ExcludeSubExprHeader.Text = AppResources.get_string("BDMatchUI/HelpPage/ExcludeSubExprHeader/Text");
            ExcludeSubExprText.Text = AppResources.get_string("BDMatchUI/HelpPage/ExcludeSubExprText/Text");
            FastRegexHeader.Text = AppResources.get_string("BDMatchUI/HelpPage/FastRegexHeader/Text");
            FastRegexText.Text = AppResources.get_string("BDMatchUI/HelpPage/FastRegexText/Text");

            current_language = new string(AppResources.current_language);
        }

        string current_language = null;
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            if (current_language != AppResources.current_language) load_control_text();
            base.OnNavigatedTo(e);
        }

        private void HelpScrollBar_DetailLabelRequested(AnnotatedScrollBar sender, AnnotatedScrollBarDetailLabelRequestedEventArgs args)
        {
            AnnotatedScrollBarLabel anchor_label = null;
            foreach (var label in HelpScrollBar.Labels)
            {
                if (label.ScrollOffset <= args.ScrollOffset) anchor_label = label;
            }
            if (anchor_label != null) args.Content = anchor_label.Content;
        }

        private void HelpScrollBar_Load_Labels()
        {
            if (HelpScrollBar != null)
            {
                HelpScrollBar.Labels.Clear();

                double offset = HelpPanel.ActualOffset.Y;
                var cal_scroll_offset = (double y) =>
                {
                    return (y + offset);
                };
                HelpScrollBar.Labels.Add(new AnnotatedScrollBarLabel(BatchProcessingHeader.Text, cal_scroll_offset(BatchProcessingHeader.ActualOffset.Y)));
                HelpScrollBar.Labels.Add(new AnnotatedScrollBarLabel(OneToMultiHeader.Text, cal_scroll_offset(OneToMultiHeader.ActualOffset.Y)));
                HelpScrollBar.Labels.Add(new AnnotatedScrollBarLabel(UsingRegexHeader.Text, cal_scroll_offset(UsingRegexHeader.ActualOffset.Y)));
                HelpScrollBar.Labels.Add(new AnnotatedScrollBarLabel(ExcludeSubExprHeader.Text, cal_scroll_offset(ExcludeSubExprHeader.ActualOffset.Y)));
                HelpScrollBar.Labels.Add(new AnnotatedScrollBarLabel(FastRegexHeader.Text, cal_scroll_offset(FastRegexHeader.ActualOffset.Y)));
            }
        }

        private void HelpPage_Loaded(object sender, RoutedEventArgs e)
        {
            HelpScrollView.ScrollPresenter.VerticalScrollController = HelpScrollBar.ScrollController;
        }

        private void HelpPanel_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            HelpScrollBar_Load_Labels();
        }
    }
}
