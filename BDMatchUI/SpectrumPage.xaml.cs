using BDMatchUI.Helper;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Media.Imaging;
using Microsoft.UI.Xaml.Navigation;
using System;
using Windows.Graphics.Capture;
using Windows.Security.Authentication.Identity.Core;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace BDMatchUI
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class SpectrumPage : Page
    {
        public SpectrumPage()
        {
            no_draw = true;
            this.InitializeComponent();
            load_control_text();
            reset_control();
            no_draw = false;
        }

        public const int JumpSel_Time = 0, JumpSel_Timeline = 1;
        private void load_control_text()
        {
            ChannelText.Text = AppResources.get_string("BDMatchUI/SpectrumPage/ChannelText/Text");
            ChannelSel.Items[0] = AppResources.get_string("BDMatchUI/SpectrumPage/Channel_Left");
            ChannelSel.Items[1] = AppResources.get_string("BDMatchUI/SpectrumPage/Channel_Right");
            JumpSelText.Text = AppResources.get_string("BDMatchUI/SpectrumPage/JumpSelText/Text");
            JumpSel.Items[JumpSel_Time] = AppResources.get_string("BDMatchUI/SpectrumPage/JumpSel_Time");
            JumpSel.Items[JumpSel_Timeline] = AppResources.get_string("BDMatchUI/SpectrumPage/JumpSel_Timeline");
            ScaleSliderText.Text = AppResources.get_string("BDMatchUI/SpectrumPage/ScaleSliderText/Text");

            current_language = new string(AppResources.current_language);
        }
        private void reset_control()
        {
            bool no_draw_store = no_draw;
            no_draw = true;
            ChannelSel.SelectedIndex = 0;
            JumpSel.SelectedIndex = JumpSel_Time;
            HourNumber.Value = 0;
            MinuNumber.Value = 0;
            SecNumber.Value = 0;
            CentiSecNumber.Value = 0;
            TimelineNumber.Value = 1;
            TimeSlider.Value = 0;
            ScaleSlider.Value = 1.0;
            no_draw = no_draw_store;
        }

        // Controls
        string current_language = null;
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            no_draw = true;
            if (current_language != AppResources.current_language)
            {
                reset_control();
                load_control_text();
            }
            base.OnNavigatedTo(e);
            if (e.Parameter is SharingHelper && e.Parameter != null)
            {
                SharingHelper sharing_helper = e.Parameter as SharingHelper;
                tv_draw = sharing_helper.tv_draw;
                bd_draw = sharing_helper.bd_draw;
                bool timeline_valid = (tv_draw != null && tv_draw.time_list != null) && (bd_draw != null && bd_draw.time_list != null);
                JumpSel.IsEnabled = timeline_valid;
                if (timeline_valid)
                {
                    TimelineNumber.Maximum = Math.Min(tv_draw.line_num, bd_draw.line_num);
                    TimeSlider.Maximum = Math.Max(tv_draw.centi_sec, bd_draw.centi_sec);
                }
                if (DrawHelper.new_draw)reset_control();
            }
            no_draw = false;
            if (DrawHelper.new_draw) draw_spec();
        }

        private void Time_ValueChanged(NumberBox sender, NumberBoxValueChangedEventArgs args)
        {
            if (!drawing && HourNumber.IsEnabled)
            {
                bool no_draw_store = no_draw;
                no_draw = true;
                TimeSpan time_span = new TimeSpan(0, (int)HourNumber.Value, (int)MinuNumber.Value, (int)SecNumber.Value, (int)CentiSecNumber.Value * 10);
                TimeSlider.Value = time_span.TotalMilliseconds / 10;
                no_draw = no_draw_store;
                draw_spec();
            }
        }

        private void ChannelSel_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (!drawing) draw_spec();
        }

        private void TimeSlider_ValueChanged(object sender, Microsoft.UI.Xaml.Controls.Primitives.RangeBaseValueChangedEventArgs e)
        {
            if (!drawing && TimeSlider.IsEnabled)
            {
                bool no_draw_store = no_draw;
                no_draw = true;
                TimeSpan time_span = TimeSpan.FromMilliseconds(TimeSlider.Value * 10);
                HourNumber.Value = time_span.Days * 24 + time_span.Hours;
                MinuNumber.Value = time_span.Minutes;
                SecNumber.Value = time_span.Seconds;
                CentiSecNumber.Value = time_span.Milliseconds / 10;
                no_draw = no_draw_store;
                draw_spec();
            }
        }

        private void ScaleSlider_ValueChanged(object sender, Microsoft.UI.Xaml.Controls.Primitives.RangeBaseValueChangedEventArgs e)
        {
            const double small_scale_step = 0.1, large_scale_step = 0.5;
            const double small_scale_step_end = 1.3, large_scale_step_start = 1.5;
            if (ScaleSlider.Value > small_scale_step_end)
            {
                ScaleSlider.SmallChange = large_scale_step;
                ScaleSlider.StepFrequency = large_scale_step;
                if (ScaleSlider.Value < (large_scale_step_start + (large_scale_step / 2)))
                {
                    if (ScaleSlider.Value > (small_scale_step_end + large_scale_step_start) / 2) ScaleSlider.Value = large_scale_step_start;
                    else ScaleSlider.Value = small_scale_step_end;
                    double intermediate_step = Math.Round(large_scale_step_start - small_scale_step_end, 1);
                    ScaleSlider.SmallChange = intermediate_step;
                    ScaleSlider.StepFrequency = intermediate_step;
                }
                else if (ScaleSlider.Value < (large_scale_step_start + large_scale_step)) ScaleSlider.Value = large_scale_step_start + large_scale_step;
            }
            else
            {
                ScaleSlider.SmallChange = small_scale_step;
                ScaleSlider.StepFrequency = small_scale_step;
            }
            if (!drawing) draw_spec();
        }

        private void SpectrumPage_SizeChanged(object sender, Microsoft.UI.Xaml.SizeChangedEventArgs e)
        {
            SpecGrid.Height = this.ActualHeight / 2;
            if (!drawing) draw_spec();
        }

        private void Timeline_ValueChanged(NumberBox sender, NumberBoxValueChangedEventArgs args)
        {
            if (!drawing && TimelineNumber.IsEnabled) draw_spec();
        }

        private void JumpSel_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var switch_time_panel = (bool enabled) =>
            {
                HourNumber.IsEnabled = enabled;
                MinuNumber.IsEnabled = enabled;
                SecNumber.IsEnabled = enabled;
                CentiSecNumber.IsEnabled = enabled;
                TimeSlider.IsEnabled = enabled;
            };
            switch(JumpSel.SelectedIndex)
            {
                case JumpSel_Time:
                    switch_time_panel(true);
                    TimelineNumber.IsEnabled = false;
                    break;
                case JumpSel_Timeline:
                    switch_time_panel(false);
                    TimelineNumber.IsEnabled = true;
                    break;
                default:
                    break;
            }
            if (!drawing && JumpSel.IsEnabled) draw_spec();
        }

        // Draw
        bool drawing = false;
        bool no_draw = false;

        DrawHelper tv_draw = null, bd_draw = null;
        private int draw_spec()
        {
            if (no_draw) return -1;
            drawing = true;

            WriteableBitmap spec_image = null;
            int re = DrawHelper.draw_spec_image(ref spec_image, tv_draw, bd_draw, ChannelSel.SelectedIndex, JumpSel.SelectedIndex,
                (UInt64)TimelineNumber.Value, ScaleSlider.Value, TimeSlider, TimeText);
            //SpecImage.Source = spec_image;
            ImageBrush brush = new ImageBrush();
            brush.ImageSource = spec_image;
            SpecGrid.Background = brush;

            DrawHelper.new_draw = false;
            drawing = false;
            return re;
        }

    }
}
