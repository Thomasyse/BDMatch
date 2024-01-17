using BDMatchUI.Helper;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Documents;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Navigation;
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text.RegularExpressions;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using Windows.Storage.Pickers;
using Windows.Storage;
using Microsoft.UI.Dispatching;
using Windows.ApplicationModel.DataTransfer;
using System.Diagnostics;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace BDMatchUI
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    /// 

    public sealed partial class MatchPage : Page
    {

        public MatchPage()
        {
            this.InitializeComponent();
            load_control_text();
            prog_cb = new ProgressCallback(prog_single);
            feedback_cb = new FeedbackCallback(feedback);
        }

        private void load_control_text()
        {
            SubPath.PlaceholderText = AppResources.get_string("BDMatchUI/MatchPage/SubPath/PlaceholderText");
            TVPath.PlaceholderText = AppResources.get_string("BDMatchUI/MatchPage/TVPath/PlaceholderText");
            BDPath.PlaceholderText = AppResources.get_string("BDMatchUI/MatchPage/BDPath/PlaceholderText");
            SubText.Text = AppResources.get_string("BDMatchUI/Common/Text_Sub");
            TVText.Text = AppResources.get_string("BDMatchUI/Common/Text_TV");
            BDText.Text = AppResources.get_string("BDMatchUI/Common/Text_BD");
            MatchButton.Content = AppResources.get_string("BDMatchUI/Common/Text_Match");
            MatchProgressText.Text = AppResources.get_string("BDMatchUI/Common/Text_Ready");
            LogButton.Content = AppResources.get_string("BDMatchUI/MatchPage/LogButton/Content");

            current_language = new string(AppResources.current_language);
        }
        private void load_control_theme()
        {
            MatchProgressText.Foreground = StyleHelper.color_brush(last_prog_level);
        }

        private nint get_hwnd()
        {
            // Retrieve the window handle (HWND) of the current WinUI 3 window.
            var window = WindowHelper.GetWindowForElement(this);
            var hWnd = WinRT.Interop.WindowNative.GetWindowHandle(window);
            return hWnd;
        }

        SharingHelper sharing_helper = null;
        SettingHelper settings = null;
        string current_language = null;
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            if (e.Parameter is SharingHelper && e.Parameter != null)
            {
                bool first_navigated = (sharing_helper == null);
                sharing_helper = e.Parameter as SharingHelper;
                settings = sharing_helper.settings;
                if (first_navigated) sharing_helper.theme_change_action_list.Add(MatchPage_ActualThemeChanged);
            }
            if (current_language != AppResources.current_language) load_control_text();
            load_control_theme();
            base.OnNavigatedTo(e);
        }

        private void MatchPage_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            PathGrid.MaxHeight = Math.Max(this.ActualHeight - MatchPanel.ActualHeight - StatusPanel.ActualHeight - 80, (SubPath.FontSize + 20) * 3);
            PathGrid.MaxWidth = this.ActualWidth;
            StatusPanel.MaxWidth = this.ActualWidth;
            Style flyout_style = new Style { TargetType = typeof(FlyoutPresenter) };
            flyout_style.Setters.Add(new Setter(WidthProperty, SubPath.ActualWidth.ToString()));
            flyout_style.Setters.Add(new Setter(MaxWidthProperty, "1000000000000000"));
            flyout_style.Setters.Add(new Setter(ScrollViewer.HorizontalScrollModeProperty, "Auto"));
            flyout_style.Setters.Add(new Setter(ScrollViewer.HorizontalScrollBarVisibilityProperty, "Auto"));
            SubPathFlyout.FlyoutPresenterStyle = flyout_style;
            TVPathFlyout.FlyoutPresenterStyle = flyout_style;
            BDPathFlyout.FlyoutPresenterStyle = flyout_style;
        }

        private void MatchPage_ActualThemeChanged(FrameworkElement sender, object args)
        {
            load_control_theme();
        }

        FileOpenPicker openPicker;
        private async void pick_files(string[] file_filters, TextBox text_box)
        {
            if (openPicker == null)
            {
                // Create a file picker
                openPicker = new Windows.Storage.Pickers.FileOpenPicker();
                // Set options for your file picker
                openPicker.ViewMode = PickerViewMode.Thumbnail;
                openPicker.SuggestedStartLocation = PickerLocationId.VideosLibrary;
            }

            // Initialize the file picker with the window handle (HWND).
            WinRT.Interop.InitializeWithWindow.Initialize(openPicker, get_hwnd());

            foreach (string filter in file_filters)
            {
                openPicker.FileTypeFilter.Add(filter);
            }

            // Open the picker for the user to pick a file
            IReadOnlyList<StorageFile> files = await openPicker.PickMultipleFilesAsync();
            if (files.Count > 0)
            {
                StringBuilder output = new StringBuilder("");
                foreach (StorageFile file in files)
                {
                    output.Append(file.Path);
                    output.Append('\n');
                }
                output.Remove(output.Length - 1, 1);
                text_box.Text = output.ToString();
            }
        }

        // Path Controls
        private void Regexer_Click(object sender, RoutedEventArgs e)
        {
            Button regexer = sender as Button;
            TextBox path_box = regexer.Name == "TVRegexer" ? TVPath : (regexer.Name == "BDRegexer" ? BDPath : SubPath);
            var path_section_list = path_box.Text.Split('\"');
            bool is_regex = path_box.Text.StartsWith('\"');
            StringBuilder regex_sb = new StringBuilder();
            foreach (var section in path_section_list)
            {
                if (is_regex)
                {
                    regex_sb.Append('\"');
                    foreach (char c in section)
                    {
                        switch (c)
                        {
                            case '[':
                                regex_sb.Append("\\[");
                                break;
                            case ']':
                                regex_sb.Append("\\]");
                                break;
                            case '(':
                                regex_sb.Append("\\(");
                                break;
                            case ')':
                                regex_sb.Append("\\)");
                                break;
                            case '{':
                                regex_sb.Append("\\{");
                                break;
                            case '}':
                                regex_sb.Append("\\}");
                                break;
                            case '+':
                                regex_sb.Append("\\+");
                                break;
                            case '.':
                                regex_sb.Append("\\.");
                                break;
                            case '^':
                                regex_sb.Append("\\^");
                                break;
                            case '$':
                                regex_sb.Append("\\$");
                                break;
                            default:
                                regex_sb.Append(c);
                                break;
                        }
                    }
                    regex_sb.Append('\"');
                }
                else regex_sb.Append(section);
                is_regex = !is_regex;
            }
            path_box.Text = regex_sb.ToString();
        }

        private void SubFileFind_Click(object sender, RoutedEventArgs e)
        {
            string[] sub_file_filters = [".ass", ".srt"];
            pick_files(sub_file_filters, SubPath);
        }
        private void TVBDFileFind_Click(object sender, RoutedEventArgs e)
        {
            string[] tv_bd_file_filters = [".mp4", ".mov", ".mkv", ".avi", ".wmv", ".mpeg", ".mpg", ".qt", ".rmvb", ".flv", "*"];
            pick_files(tv_bd_file_filters, (sender as Button).Name == "TVFind" ? TVPath : BDPath);
        }

        private void Path_DragOver(object sender, DragEventArgs e)
        {
            e.AcceptedOperation = DataPackageOperation.Copy;
        }
        private async void Path_Drop(object sender, DragEventArgs e)
        {
            if (e.DataView.Contains(StandardDataFormats.StorageItems))
            {
                var items = await e.DataView.GetStorageItemsAsync();
                StringBuilder item_strs = new StringBuilder();
                foreach (var item in items)
                {
                    item_strs.Append(item.Path);
                    item_strs.Append('\n');
                }
                if (item_strs.Length > 0)item_strs.Remove(item_strs.Length - 1, 1);
                if (sender is TextBox) (sender as TextBox).Text = item_strs.ToString();
            }
        }

        private void PathHistory_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            ListBox PathHistory = sender as ListBox;
            if (PathHistory.SelectedIndex != -1)
            {
                var path_select = (TextBox path, ListBox his_list, Button his_button) =>
                {
                    path.Text = his_list.Items[his_list.SelectedIndex].ToString();
                    his_button.Flyout.Hide();
                };
                if (PathHistory.Name == "SubPathHistory") path_select(SubPath, PathHistory, SubHisButton);
                if (PathHistory.Name == "TVPathHistory") path_select(TVPath, PathHistory, TVHisButton);
                if (PathHistory.Name == "BDPathHistory") path_select(BDPath, PathHistory, BDHisButton);
                if (PathHistory.Items.Count > 10) PathHistory.Items.RemoveAt(0);
                PathHistory.SelectedIndex = -1;
            }
        }
        private void add_path_to_history(string sub_path_all, string tv_path_all, string bd_path_all)
        {
            var add_path_his = (ListBox his_list, string path) =>
            {
                if (his_list.Items.Count == 0 || path != his_list.Items[^1].ToString()) his_list.Items.Add(path);
            };
            add_path_his(SubPathHistory, sub_path_all);
            add_path_his(TVPathHistory, tv_path_all);
            add_path_his(BDPathHistory, bd_path_all);
        }

        // Progress Bar/Ring
        double[] prog_val = [ 0, 0, 0 ];
        double find_field_ratio = 1.0;
        private void prog_single(Prog_Mode mode, double val)
        {
            switch (mode)
            {
                case Prog_Mode.TV:
                    prog_val[0] = val;
                    break;
                case Prog_Mode.BD:
                    prog_val[1] = val;
                    break;
                case Prog_Mode.Sub:
                    if (val > 0.0 && val < 1.0 && val < prog_val[2] + 0.01) return;
                    prog_val[2] = val;
                    prog_val[0] = 1;
                    prog_val[1] = 1;
                    break;
                default:
                    prog_val[0] = val;
                    prog_val[1] = val;
                    prog_val[2] = val;
                    find_field_ratio = Convert.ToBoolean(settings[SettingType.MatchAss]) ? settings[SettingType.SearchRange] * 0.06
                        * Math.Log10(Math.Min(settings[SettingType.SearchRange] * 100, settings[SettingType.MinCnfrmNum])) * 2.0 : 0.0;
                    return;
            }
            double line_ratio = CoreHelper.get_nb_timeline() / 500.0;
            if (line_ratio == 0.0) line_ratio = 1.0;
            double single_prog_val = ((prog_val[0] + prog_val[1]) * 20 + prog_val[2] * find_field_ratio * line_ratio)
                / (40 + find_field_ratio * line_ratio);
            double total_prog_val = (fin_match_task_cnt + single_prog_val) / total_match_task_cnt;

            DispatcherQueue.TryEnqueue(DispatcherQueuePriority.Low, () =>
            {
                MatchProgressRing.Value = single_prog_val;
                MatchProgressBar.Value = total_prog_val;
                TaskBarHelper.set_value(get_hwnd(), (UInt64)(total_prog_val * 200), 200);
            });
            return;
        }

        // Feedback
        UInt64 warning_cnt = 0, err_cnt = 0;
        StringBuilder fb_builder = new StringBuilder();
        private void log_fb_builder_info()
        {
            if (fb_builder.Length > 0)
            {
                log(fb_builder.ToString(), false, LogLevel.Info);
                fb_builder.Clear();
            }
        }
        private void feedback(IntPtr input, Int64 len)
        {
            string feedback_str = "";
            if (len > 0) feedback_str = Marshal.PtrToStringUTF8(input);
            else if (len == -1) feedback_str = Marshal.PtrToStringAnsi(input);

            if (feedback_str.StartsWith('\n')) log_fb_builder_info();
            feedback_str = feedback_str.TrimStart('\n');
            var log_list = feedback_str.Split('\n');
            var fb_builder_trim_end = () =>
            {
                if (fb_builder.Length > 0 && fb_builder[^1] == '\n') fb_builder.Remove(fb_builder.Length - 1, 1);
            };
            foreach (var line in log_list)
            {
                bool err_line = line.Trim().StartsWith(AppResources.get_string("BDMatchUI/Common/Text_Error"));
                bool warning_line = line.Trim().StartsWith(AppResources.get_string("BDMatchUI/Common/Text_Warning"));
                if (err_line || warning_line)
                {
                    fb_builder_trim_end();
                    log_fb_builder_info();
                    LogLevel log_level = err_line ? LogLevel.Error : LogLevel.Warning;
                    log(line, false, log_level);
                    string message = string.Format(AppResources.get_string("BDMatchUI/Common/Text_ProgStat"), fin_match_task_cnt + 1, total_match_task_cnt, line);
                    prog_log(message, log_level);
                    if (warning_line)warning_cnt++;
                    if (err_line)
                    {
                        err_cnt++;
                        while (!DispatcherQueue.TryEnqueue(DispatcherQueuePriority.High, () =>
                        {
                            print_info(AppResources.get_string("BDMatchUI/Common/Text_Error"), message, err_line ? InfoBarSeverity.Error : InfoBarSeverity.Warning);
                            TaskBarHelper.flash_window(get_hwnd());
                        })) ;
                    }
                }
                else
                {
                    fb_builder.Append(line);
                    fb_builder.Append('\n');
                }
            }
            fb_builder_trim_end();
        }

        // Other Controls
        private void LogButton_Click(object sender, RoutedEventArgs e)
        {
            if (sharing_helper.navi_view != null) sharing_helper.navi_view.SelectedItem = sharing_helper.navi_view.MenuItems[2];
        }

        // InfoBar
        private void print_info(string title, string message, InfoBarSeverity severity = InfoBarSeverity.Informational)
        {
            MatchInfoBar.Title = title;
            MatchInfoBar.Message = message;
            MatchInfoBar.IsOpen = true;
            MatchInfoBar.Severity = severity;
        }

        // Log
        LogLevel last_prog_level = LogLevel.Info;
        private void prog_log(string message, LogLevel level = LogLevel.Info)
        {
            while (!DispatcherQueue.TryEnqueue(DispatcherQueuePriority.High, () => {
                MatchProgressText.Text = message;
                MatchProgressText.Foreground = StyleHelper.color_brush(level);
                last_prog_level = level;
            })) ;
        }

        private void log(string message, bool print_to_prog_text = true, LogLevel level = LogLevel.Info)
        {
            if (print_to_prog_text) prog_log(message, level);
            if (sharing_helper != null)
            {
                while (!DispatcherQueue.TryEnqueue(DispatcherQueuePriority.Normal, () => {
                    Paragraph paragraph = new Paragraph();
                    Run run = new Run();
                    run.Text = message;
                    run.FontSize = 20;
                    run.Foreground = StyleHelper.color_brush(level);
                    paragraph.Inlines.Add(run);
                    sharing_helper.logs.Add(Tuple.Create(paragraph, level));
                    if (sharing_helper.navi_view != null && sharing_helper.navi_view.SelectedItem == sharing_helper.navi_view.MenuItems[2])
                    {
                        if (sharing_helper.load_log != null) sharing_helper.load_log();
                    }
                })) ;
            }
        }

        // ISA
        private void search_ISA()
        {
            string cpu_isa_str = Marshal.PtrToStringAnsi(CoreHelper.get_CPU_brand());
            ISA_Mode isa_mode = (ISA_Mode)CoreHelper.search_ISA_mode();
            switch (isa_mode)
            {
                case ISA_Mode.AVX2_FMA:
                    cpu_isa_str += AppResources.get_string("BDMatchUI/Common/Text_ISA_AVX2");
                    break;
                case ISA_Mode.AVX:
                    cpu_isa_str += AppResources.get_string("BDMatchUI/Common/Text_ISA_AVX");
                    break;
                case ISA_Mode.SSE:
                    cpu_isa_str += AppResources.get_string("BDMatchUI/Common/Text_ISA_SSE");
                    break;
                case ISA_Mode.No_SIMD:
                default:
                    cpu_isa_str += AppResources.get_string("BDMatchUI/Common/Text_ISA_NO_SIMD");
                    break;
            }
            log(cpu_isa_str);
        }

        // Match
        CancellationTokenSource cancel_source;
        bool processing = false;

        private void MatchButton_Click(object sender, RoutedEventArgs e)
        {
            if (!processing)
            {
                if (SubPath.Text == "debug mode")
                {
                    settings.debug_mode = !settings.debug_mode;
                    print_info(AppResources.get_string("BDMatchUI/Common/Text_Info"),
                        AppResources.get_string("BDMatchUI/Common/Text_DebugMode") +
                        AppResources.get_string(settings.debug_mode ? "BDMatchUI/Common/Text_On" : "BDMatchUI/Common/Text_Off"));
                    return;
                }
                match_control(false);
                cancel_source = new CancellationTokenSource();
                CoreHelper.start_process();
                string sub_path_text = SubPath.Text, tv_path_text = TVPath.Text, bd_path_text = BDPath.Text;
                Task<MatchReturn> match_task = new Task<MatchReturn>(() => match_complex(sub_path_text, tv_path_text, bd_path_text), TaskCreationOptions.LongRunning);
                MatchProgressBar.ShowError = false;
                MatchProgressBar.ShowPaused = false;
                MatchProgressRing.IsIndeterminate = true;
                TaskBarHelper.set_state(get_hwnd(), TBPFLAG.TBPF_NORMAL);
                match_task.Start();
            }
            else
            {
                cancel_source.Cancel();
                CoreHelper.stop_process();
                MatchProgressBar.ShowError = true;
                TaskBarHelper.set_state(get_hwnd(), TBPFLAG.TBPF_ERROR);
                match_control(true);
            }
        }

        private void match_control(bool val)
        {
            SubPath.IsEnabled = val; TVPath.IsEnabled = val; BDPath.IsEnabled = val;
            SubHisButton.IsEnabled = val; TVHisButton.IsEnabled = val; BDHisButton.IsEnabled = val;
            SubRegexer.IsEnabled = val; TVRegexer.IsEnabled = val; BDRegexer.IsEnabled = val;
            SubFind.IsEnabled = val; TVFind.IsEnabled = val; BDFind.IsEnabled = val;
            if (val) MatchButton.Content = AppResources.get_string("BDMatchUI/Common/Text_Match");
            else MatchButton.Content = AppResources.get_string("BDMatchUI/Common/Text_Stop");
            processing = !val;
            MatchProgressRing.IsActive = processing;
        }

        private MatchReturn match_error(MatchReturn match_return, bool log_error = true, params string[] strings)
        {
            while (!DispatcherQueue.TryEnqueue(DispatcherQueuePriority.High, () => {
                if (log_error)
                {
                    string err_message;
                    err_message = string.Format(AppResources.get_err_str(match_return), strings);
                    if (strings.Length > 0) log(AppResources.get_string("BDMatchUI/Error/Prefix") + err_message, true, LogLevel.Error);
                    print_info(AppResources.get_string("BDMatchUI/Common/Text_Error"), err_message, InfoBarSeverity.Error);
                }
                match_control(true);
                TaskBarHelper.set_state(get_hwnd(), TBPFLAG.TBPF_ERROR);
            })) ;
            return match_return;
        }

        int total_match_task_cnt = 0, fin_match_task_cnt = 0;
        Stopwatch stopwatch = new Stopwatch();
        private MatchReturn match_complex(string sub_path_all, string tv_path_all, string bd_path_all)
        {
            var replace_ret_syb = (ref string str) => { str = Regex.Replace(str.Trim(), "[\r\n]+", "\n", RegexOptions.Multiline); };
            replace_ret_syb(ref sub_path_all);
            replace_ret_syb(ref tv_path_all);
            replace_ret_syb(ref bd_path_all);
            if (sub_path_all == "" || tv_path_all == "" || bd_path_all == "") return match_error(MatchReturn.EmptyPath);
            bool sub_path_all_any_quote = sub_path_all.Contains('\"'), tv_path_all_any_quote = tv_path_all.Contains('\"');
            string[] sub_path_list = sub_path_all.Split('\n');
            string[] tv_path_list = tv_path_all.Split('\n');
            string[] bd_path_list = bd_path_all.Split('\n');
            while (!DispatcherQueue.TryEnqueue(DispatcherQueuePriority.Normal, () => {
                MatchProgressRing.IsIndeterminate = false;
            })) ;

            MatchReturn re = MatchReturn.Success;
            stopwatch.Restart();//开始计时
            if (sub_path_all_any_quote || tv_path_all_any_quote || bd_path_all.Contains('\"')) // 正则查找模式
            {
                re = search_regex_path(sub_path_list, tv_path_list, bd_path_list, sub_path_all_any_quote, tv_path_all_any_quote);
            }
            else // 匹配模式
            {
                search_ISA();
                var batch_no_draw = () =>
                {
                    if (total_match_task_cnt > 1 && Convert.ToBoolean(settings[SettingType.DrawSpec]))
                    {
                        while (!DispatcherQueue.TryEnqueue(DispatcherQueuePriority.High, () =>
                        {
                            print_info(AppResources.get_string("BDMatchUI/Common/Text_Info"), AppResources.get_string("BDMatchUI/Info/Batch_No_Draw"));
                        }));
                    }
                };
                if (settings[SettingType.SubOffset] != 0)
                {
                    StringBuilder offset_str = new StringBuilder();
                    offset_str.Append(AppResources.get_string("BDMatchUI/Common/Text_SubTimeDelay"));
                    offset_str.Append(" ");
                    offset_str.Append(settings[SettingType.SubOffset].ToString());
                    offset_str.Append(" ");
                    offset_str.Append(AppResources.get_string("BDMatchUI/Common/Text_CentiSec"));
                    log(offset_str.ToString());
                }
                var get_last_dot_index = (string str) => {
                    return str.LastIndexOf('.') >= 0 ? str.LastIndexOf('.') : str.Length;
                };
                fin_match_task_cnt = 0;
                warning_cnt = 0; err_cnt = 0;
                if (sub_path_list.Length == 1 && tv_path_list.Length == 1 && bd_path_list.Length > 1) // 1对多模式
                {
                    total_match_task_cnt = bd_path_list.Length;
                    batch_no_draw();
                    foreach (string bd_path_untrim in bd_path_list)
                    {
                        log(AppResources.get_string("BDMatchUI/Common/Text_Dash_Line"), false);
                        string bd_path = bd_path_untrim.Trim();
                        string output_path = bd_path.Substring(0, get_last_dot_index(bd_path)) + ".matched" + sub_path_all.Substring(get_last_dot_index(sub_path_all));
                        MatchReturn single_re = match(sub_path_all, tv_path_all, bd_path, output_path, false);
                        if (single_re != MatchReturn.Success) re = single_re;
                        else fin_match_task_cnt++;
                    }
                }
                else // 1对1模式
                {
                    if (sub_path_list.Length != tv_path_list.Length || sub_path_list.Length != bd_path_list.Length)
                    {
                        warning_cnt++;
                        log(AppResources.get_string("BDMatchUI/Warning/Diff_Batch_Cnt"), true, LogLevel.Warning);
                        while (!DispatcherQueue.TryEnqueue(DispatcherQueuePriority.High, () =>
                        {
                            print_info(AppResources.get_string("BDMatchUI/Common/Text_Warning"), AppResources.get_string("BDMatchUI/Warning/Diff_Batch_Cnt"), InfoBarSeverity.Warning);
                        }));
                    }
                    total_match_task_cnt = Math.Min( sub_path_list.Length, Math.Min(tv_path_list.Length, bd_path_list.Length));
                    batch_no_draw();
                    bool draw_apply = (total_match_task_cnt == 1 && Convert.ToBoolean(settings[SettingType.DrawSpec]));
                    for (int path_idx = 0; path_idx < total_match_task_cnt; path_idx++)
                    {
                        log(AppResources.get_string("BDMatchUI/Common/Text_Dash_Line"), false);
                        string sub_path = sub_path_list[path_idx].Trim(), tv_path = tv_path_list[path_idx].Trim(), bd_path = bd_path_list[path_idx].Trim();
                        string output_path = sub_path.Substring(0, get_last_dot_index(sub_path)) + ".matched" + sub_path.Substring(get_last_dot_index(sub_path));
                        MatchReturn single_re = match(sub_path, tv_path, bd_path, output_path, draw_apply);
                        if (single_re != MatchReturn.Success) re = single_re;
                        else fin_match_task_cnt++;
                    }
                }
                log(AppResources.get_string("BDMatchUI/Common/Text_Dash_Line"), false);
                log(string.Format(AppResources.get_string("BDMatchUI/Common/Text_Finish"), fin_match_task_cnt, warning_cnt, err_cnt,
                    stopwatch.ElapsedTicks / (double)Stopwatch.Frequency, AppResources.get_string("BDMatchUI/Common/Text_Sec")), true,
                    err_cnt > 0 ? LogLevel.Error : (warning_cnt > 0 ? LogLevel.Warning : LogLevel.Info));
            }
            stopwatch.Stop(); //结束计时

            if (re == MatchReturn.User_Stop)
            {
                log(AppResources.get_string("BDMatchUI/Error/User_Stop"), true, LogLevel.Error);
            }
            log(string.Format("\n{0}\n{0}\n", AppResources.get_string("BDMatchUI/Common/Text_Dash_Line")), false);
            while (!DispatcherQueue.TryEnqueue(DispatcherQueuePriority.High, () => { 
                match_control(true);
                if (re == MatchReturn.Success)
                {
                    TaskBarHelper.set_state(get_hwnd(), TBPFLAG.TBPF_NOPROGRESS);
                    add_path_to_history(sub_path_all, tv_path_all, bd_path_all);
                }
                else TaskBarHelper.set_state(get_hwnd(), TBPFLAG.TBPF_ERROR);
                TaskBarHelper.flash_window(get_hwnd());
            })) ;
            return re;
        }

        private MatchReturn search_regex_path(string [] sub_path_list, string [] tv_path_list, string [] bd_path_list, bool sub_path_all_any_quote, bool tv_path_all_any_quote)
        {
            prog_log(AppResources.get_string("BDMatchUI/Regex/Searching"));
            var match_regex_path = (string path) =>
            {
                if (RegexHelper.verif_regex_path(path))
                {
                    string err_message = "";
                    List<PathMatch> matches = RegexHelper.match_regex_path(path, (string log) => { err_message = log; });
                    if (matches == null) return Tuple.Create<MatchReturn, List<PathMatch>>(match_error(MatchReturn.InvalidRegex, true, err_message), null);
                    return Tuple.Create(MatchReturn.Success, matches);
                }
                else return Tuple.Create<MatchReturn, List<PathMatch>>(match_error(MatchReturn.InvalidRegex, true, path), null);
            };
            if (!sub_path_all_any_quote && sub_path_list.Length == 1 && !tv_path_all_any_quote && tv_path_list.Length == 1) // 1对多模式
            {
                StringBuilder new_bd_path_all = new StringBuilder();
                int all_match_cnt = 0;
                foreach (string bd_path_untrim in bd_path_list)
                {
                    int match_cnt = 0;
                    string bd_path = bd_path_untrim.Trim();
                    if (bd_path.Contains("\""))
                    {
                        (MatchReturn regex_match_re, var matches) = match_regex_path(bd_path);
                        if (regex_match_re != MatchReturn.Success) return regex_match_re;
                        match_cnt = matches.Count;
                        foreach (PathMatch match in matches)
                        {
                            new_bd_path_all.Append(match.path);
                            new_bd_path_all.Append('\n');
                        }
                    }
                    if (match_cnt == 0)
                    {
                        new_bd_path_all.Append(bd_path);
                        new_bd_path_all.Append('\n');
                    }
                    all_match_cnt += match_cnt;
                    log(string.Format(AppResources.get_string("BDMatchUI/Regex/Pattern_FoundNMatch"), bd_path, match_cnt.ToString()), false);
                }
                if (new_bd_path_all.Length > 0) new_bd_path_all.Remove(new_bd_path_all.Length - 1, 1);

                while (!DispatcherQueue.TryEnqueue(DispatcherQueuePriority.High, () => { BDPath.Text = new_bd_path_all.ToString(); })) ;
                log(string.Format(AppResources.get_string("BDMatchUI/Regex/FoundNMatch"), all_match_cnt.ToString()));
            }
            else // 1对1模式
            {
                int path_list_min_length = Math.Min(Math.Min(sub_path_list.Length, tv_path_list.Length), bd_path_list.Length);
                StringBuilder new_sub_path_all = new StringBuilder(), new_tv_path_all = new StringBuilder(), new_bd_path_all = new StringBuilder();
                int all_pair_match_cnt = 0;
                for (int path_idx = 0; path_idx < path_list_min_length; path_idx++)
                {
                    string sub_path = sub_path_list[path_idx].Trim(), tv_path = tv_path_list[path_idx].Trim(), bd_path = bd_path_list[path_idx].Trim();
                    bool sub_path_any_quote = sub_path.Contains('\"'), tv_path_any_quote = tv_path.Contains('\"'), bd_path_any_quote = bd_path.Contains('\"');
                    if (sub_path_all_any_quote != tv_path_all_any_quote || sub_path_any_quote != bd_path_any_quote)
                        return match_error(MatchReturn.NotRegexPath, true, sub_path_all_any_quote ? (tv_path_all_any_quote ? bd_path : tv_path) : sub_path);
                    if (sub_path_any_quote && tv_path_any_quote && bd_path_any_quote)
                    {
                        (MatchReturn sub_match_re, var sub_matches) = match_regex_path(sub_path);
                        if (sub_match_re != MatchReturn.Success) return sub_match_re;
                        (MatchReturn tv_match_re, var tv_matches) = match_regex_path(tv_path);
                        if (tv_match_re != MatchReturn.Success) return tv_match_re;
                        (MatchReturn bd_match_re, var bd_matches) = match_regex_path(bd_path);
                        if (bd_match_re != MatchReturn.Success) return bd_match_re;
                        log(string.Format(AppResources.get_string("BDMatchUI/Regex/Pattern_FoundNMatch"), sub_path, sub_matches.Count.ToString()), false);
                        log(string.Format(AppResources.get_string("BDMatchUI/Regex/Pattern_FoundNMatch"), tv_path, tv_matches.Count.ToString()), false);
                        log(string.Format(AppResources.get_string("BDMatchUI/Regex/Pattern_FoundNMatch"), bd_path, bd_matches.Count.ToString()), false);

                        bool[] sub_matched = new bool[sub_matches.Count];
                        bool[] tv_matched = new bool[tv_matches.Count];
                        bool[] bd_matched = new bool[bd_matches.Count];
                        var cmp_match_pattern = (List<string> left, List<string> right) =>
                        {
                            if (left.Count != right.Count) return false;
                            bool[] pattern_matched = new bool[left.Count];
                            int matched_pattern_cnt = 0;
                            foreach (string left_pattern in left)
                            {
                                for (int i = 0; i < right.Count; i++)
                                {
                                    if (!pattern_matched[i] && right[i] == left_pattern)
                                    {
                                        pattern_matched[i] = true;
                                        matched_pattern_cnt++;
                                    }
                                }
                            }
                            return matched_pattern_cnt == left.Count;
                        };
                        int pair_match_cnt = 0;
                        foreach (var sub_match in sub_matches)
                        {
                            int tv_match_idx = -1, bd_match_idx = -1;
                            var enumerate_matches = (List<PathMatch> matches, bool[] matched, ref int match_idx) => {
                                for (int i = 0; i < matches.Count; i++)
                                {
                                    if (!matched[i] && cmp_match_pattern(sub_match.pattern, matches[i].pattern))
                                    {
                                        match_idx = i;
                                        break;
                                    }
                                }
                            };
                            enumerate_matches(tv_matches, tv_matched, ref tv_match_idx);
                            enumerate_matches(bd_matches, bd_matched, ref bd_match_idx);
                            if (tv_match_idx >= 0 && bd_match_idx >= 0)
                            {
                                tv_matched[tv_match_idx] = true;
                                bd_matched[bd_match_idx] = true;
                                new_sub_path_all.Append(sub_match.path);
                                new_sub_path_all.Append('\n');
                                new_tv_path_all.Append(tv_matches[tv_match_idx].path);
                                new_tv_path_all.Append('\n');
                                new_bd_path_all.Append(bd_matches[bd_match_idx].path);
                                new_bd_path_all.Append('\n');
                                pair_match_cnt++;
                            }
                        }
                        log(string.Format(AppResources.get_string("BDMatchUI/Regex/Pair_FoundNMatch"), pair_match_cnt.ToString()));
                        all_pair_match_cnt += pair_match_cnt;
                    }
                    else
                    {
                        new_sub_path_all.Append(sub_path);
                        new_sub_path_all.Append('\n');
                        new_tv_path_all.Append(tv_path);
                        new_tv_path_all.Append('\n');
                        new_bd_path_all.Append(bd_path);
                        new_bd_path_all.Append('\n');
                    }
                }
                if (new_sub_path_all.Length > 0) new_sub_path_all.Remove(new_sub_path_all.Length - 1, 1);
                if (new_tv_path_all.Length > 0) new_tv_path_all.Remove(new_tv_path_all.Length - 1, 1);
                if (new_bd_path_all.Length > 0) new_bd_path_all.Remove(new_bd_path_all.Length - 1, 1);
                while (!DispatcherQueue.TryEnqueue(DispatcherQueuePriority.High, () => {
                    SubPath.Text = new_sub_path_all.ToString();
                    TVPath.Text = new_tv_path_all.ToString();
                    BDPath.Text = new_bd_path_all.ToString();
                })) ;
                log(string.Format(AppResources.get_string("BDMatchUI/Regex/FoundNMatch"), all_pair_match_cnt.ToString()));
            }
            return MatchReturn.Success;
        }

        ProgressCallback prog_cb;
        FeedbackCallback feedback_cb;
        private MatchReturn match(string sub_path, string tv_path, string bd_path, string output_path, bool draw)
        {
            draw_pre();
            prog_single(Prog_Mode.Setting, 0);

            CoreHelper.load_interface(prog_cb, feedback_cb);
            
            CoreHelper.load_settings(ISA_Mode.Auto, settings[SettingType.FFTNum], settings[SettingType.MinFinddB], 
                Convert.ToBoolean(settings[SettingType.OutputPCM]), Convert.ToBoolean(settings[SettingType.ParallelDecode]), Convert.ToBoolean(settings[SettingType.VolMatch]),
                settings[SettingType.MinCnfrmNum], settings[SettingType.SearchRange], settings[SettingType.SubOffset], settings[SettingType.MaxLength],
                Convert.ToBoolean(settings[SettingType.MatchAss]), Convert.ToBoolean(settings[SettingType.FastMatch]), settings.debug_mode);
            
            prog_log(string.Format(AppResources.get_string("BDMatchUI/Common/Text_ProgStat"), fin_match_task_cnt + 1, total_match_task_cnt, AppResources.get_string("BDMatchUI/Common/Text_Decoding")));
            MatchReturn re = CoreHelper.decode(tv_path, bd_path);
            if (re != MatchReturn.Success)
            {
                if (re != MatchReturn.User_Stop) log_fb_builder_info();
                else fb_builder.Clear();
                return re;
            }

            if (Convert.ToBoolean(settings[SettingType.MatchAss]))
            {
                prog_log(string.Format(AppResources.get_string("BDMatchUI/Common/Text_ProgStat"), fin_match_task_cnt + 1, total_match_task_cnt, AppResources.get_string("BDMatchUI/Common/Text_Matching")));
                var return_match_ass_err = (MatchReturn match_ass_re) =>
                {
                    if (re != MatchReturn.User_Stop) log_fb_builder_info();
                    else fb_builder.Clear();
                    CoreHelper.clear_match();
                    return match_ass_re;
                };
                Encoding encoder = Encoding.UTF8;
                byte[] tv_path_encoded = new byte[encoder.GetByteCount(tv_path)];
                encoder.GetBytes(tv_path, tv_path_encoded);
                tv_path_encoded[^1] = 0;
                byte[] bd_path_encoded = new byte[encoder.GetByteCount(bd_path)];
                encoder.GetBytes(bd_path, bd_path_encoded);
                bd_path_encoded[^1] = 0;
                re = CoreHelper.match_1(sub_path, tv_path_encoded, bd_path_encoded);
                if (re != MatchReturn.Success) return return_match_ass_err(re);

                //绘图相关
                UInt64 nb_timeline = CoreHelper.get_nb_timeline();
                if (draw)
                {
                    tv_draw.time_list = new Int64[nb_timeline, 2];
                    bd_draw.time_list = new Int64[nb_timeline, 2];
                    tv_draw.line_num = nb_timeline;
                    bd_draw.line_num = nb_timeline;
                    // LineSel->Maximum = nb_timeline;
                    for (UInt64 i = 0; i < nb_timeline; i++)
                    {
                        tv_draw.time_list[i, 0] = CoreHelper.get_timeline(i, Timeline_Time_Type.Start_Time);
                        tv_draw.time_list[i, 1] = CoreHelper.get_timeline(i, Timeline_Time_Type.End_Time);
                    }
                }

                re = CoreHelper.match_2(output_path);
                if (re != MatchReturn.Success) return return_match_ass_err(re);

                //绘图相关
                if (draw)
                {
                    for (UInt64 i = 0; i < nb_timeline; i++)
                    {
                        bd_draw.time_list[i, 0] = CoreHelper.get_timeline(i, Timeline_Time_Type.Start_Time);
                        bd_draw.time_list[i, 1] = CoreHelper.get_timeline(i, Timeline_Time_Type.End_Time);
                    }
                }

                CoreHelper.clear_match();
                prog_single(Prog_Mode.Sub, 1);
            }
            else prog_single(Prog_Mode.Sub, 0);

            draw_pre(re, draw);

            prog_single(0, 0);
            log_fb_builder_info();
            return re;
        }

        // Draw
        DrawHelper tv_draw = null, bd_draw = null;

        private void draw_pre()
        {
            sharing_helper.tv_draw = new DrawHelper();
            sharing_helper.bd_draw = new DrawHelper();
            tv_draw = sharing_helper.tv_draw;
            bd_draw = sharing_helper.bd_draw;
            DrawHelper.new_draw = true;
        }
        private void draw_pre(MatchReturn re, bool draw)
        {
            if (draw && re == MatchReturn.Success)
            {
                tv_draw.fft_cnt = CoreHelper.get_decode_info(Decode_File.TV_Decode, Decode_Info.FFT_Samp_Num);
                bd_draw.fft_cnt = CoreHelper.get_decode_info(Decode_File.BD_Decode, Decode_Info.FFT_Samp_Num);
                tv_draw.ch_cnt = (int)(CoreHelper.get_decode_info(Decode_File.TV_Decode, Decode_Info.Channels));
                bd_draw.ch_cnt = (int)(CoreHelper.get_decode_info(Decode_File.BD_Decode, Decode_Info.Channels));
                tv_draw.centi_sec = CoreHelper.get_decode_info(Decode_File.TV_Decode, Decode_Info.Centi_Sec);
                bd_draw.centi_sec = CoreHelper.get_decode_info(Decode_File.BD_Decode, Decode_Info.Centi_Sec);
                tv_draw.fft_num = (int)(CoreHelper.get_decode_info(Decode_File.TV_Decode, Decode_Info.FFT_Num));
                bd_draw.fft_num = (int)(CoreHelper.get_decode_info(Decode_File.BD_Decode, Decode_Info.FFT_Num));
                tv_draw.cs_to_fft = CoreHelper.get_decode_info(Decode_File.TV_Decode, Decode_Info.Samp_Rate) / (tv_draw.fft_num * 100.0);
                bd_draw.cs_to_fft = CoreHelper.get_decode_info(Decode_File.TV_Decode, Decode_Info.Samp_Rate) / (bd_draw.fft_num * 100.0);
                tv_draw.spec = CoreHelper.get_decode_spec(Decode_File.TV_Decode);
                bd_draw.spec = CoreHelper.get_decode_spec(Decode_File.BD_Decode);
            }
            else
            {
                sharing_helper.tv_draw = null;
                sharing_helper.bd_draw = null;
                if (re == MatchReturn.Success)
                    CoreHelper.clear_data();
            }
        }
    }
}