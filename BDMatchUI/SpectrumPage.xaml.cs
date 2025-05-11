using BDMatchUI.Helper;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Media.Imaging;
using Microsoft.UI.Xaml.Navigation;
using System;
using Windows.ApplicationModel.Contacts;

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
            this.InitializeComponent();
        }


        // Controls
        SharingHelper sharing_helper = null;
        TextHelper text_helper = null;
        Draw_Control draw_ctrl = null;
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            if (e.Parameter is SharingHelper && e.Parameter != null)
            {
                bool first_naviagted = (sharing_helper == null);
                sharing_helper = e.Parameter as SharingHelper;
                text_helper = sharing_helper.text_helper;
                draw_ctrl = sharing_helper.draw_ctrl;
                if (first_naviagted) draw_ctrl.draw_spec = setup_and_draw_spec;
            }
            if (draw_ctrl != null)
            {
                control_setup();
                if (DrawHelper.new_draw) draw_ctrl.reset_control();
            }
            base.OnNavigatedTo(e);
            if (DrawHelper.new_draw) draw_spec();
            if (draw_ctrl != null) draw_ctrl.add_no_draw_cnt();
        }

        private void control_setup()
        {
            if (sharing_helper == null) return;
            bool timeline_valid = (sharing_helper.tv_draw != null && sharing_helper.tv_draw.time_list != null) && (sharing_helper.bd_draw != null && sharing_helper.bd_draw.time_list != null);
            JumpSel.IsEnabled = timeline_valid;
            if (timeline_valid)
            {
                TimelineNumber.Maximum = Math.Min(sharing_helper.tv_draw.line_num, sharing_helper.bd_draw.line_num);
                TimeSlider.Maximum = Math.Max(sharing_helper.tv_draw.centi_sec, sharing_helper.bd_draw.centi_sec);
            }
        }

        private bool draw_ctrl_vld()
        {
            return draw_ctrl != null;
        }

        public void sub_no_draw_cnt()
        {
            draw_ctrl.no_draw_cnt--;
        }

        private void Time_ValueChanged(NumberBox sender, NumberBoxValueChangedEventArgs args)
        {
            if (draw_ctrl_vld())
            {
                if (HourNumber.IsEnabled)
                {
                    if (!draw_ctrl.no_ctrl_change)
                    {
                        bool no_ctrl_change_store = draw_ctrl.no_ctrl_change;
                        draw_ctrl.no_ctrl_change = true;
                        TimeSpan time_span = new TimeSpan(0, (int)HourNumber.Value, (int)MinuNumber.Value, (int)SecNumber.Value, (int)CentiSecNumber.Value * 10);
                        draw_ctrl.TimeSlider = time_span.TotalMilliseconds / 10;
                        draw_ctrl.no_ctrl_change = no_ctrl_change_store;
                        draw_spec();
                        sub_no_draw_cnt();
                    }
                }
                else sub_no_draw_cnt();
            }
        }

        private void ChannelSel_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (draw_ctrl_vld()) draw_spec();
        }

        private void TimeSlider_ValueChanged(object sender, Microsoft.UI.Xaml.Controls.Primitives.RangeBaseValueChangedEventArgs e)
        {
            if (draw_ctrl_vld())
            {
                if (TimeSlider.IsEnabled)
                {
                    if (!draw_ctrl.no_ctrl_change)
                    {
                        bool no_ctrl_change_store = draw_ctrl.no_ctrl_change;
                        draw_ctrl.no_ctrl_change = true;
                        draw_ctrl.add_no_draw_cnt();
                        TimeSpan time_span = TimeSpan.FromMilliseconds(TimeSlider.Value * 10);
                        draw_ctrl.HourNumber = time_span.Days * 24 + time_span.Hours;
                        draw_ctrl.MinuNumber = time_span.Minutes;
                        draw_ctrl.SecNumber = time_span.Seconds;
                        draw_ctrl.CentiSecNumber = time_span.Milliseconds / 10;
                        draw_ctrl.no_ctrl_change = no_ctrl_change_store;
                        draw_spec();
                        sub_no_draw_cnt();
                    }
                }
                else sub_no_draw_cnt();
            }
        }

        private void ScaleSlider_ValueChanged(object sender, Microsoft.UI.Xaml.Controls.Primitives.RangeBaseValueChangedEventArgs e)
        {
            const double small_scale_step = 0.1, large_scale_step = 0.5;
            const double small_scale_step_end = 1.3, large_scale_step_start = 1.5;
            if (draw_ctrl_vld())
            {
                if (ScaleSlider.Value > small_scale_step_end)
                {
                    ScaleSlider.SmallChange = large_scale_step;
                    ScaleSlider.StepFrequency = large_scale_step;
                    if (ScaleSlider.Value < (large_scale_step_start + (large_scale_step / 2)))
                    {
                        if (ScaleSlider.Value > (small_scale_step_end + large_scale_step_start) / 2) draw_ctrl.ScaleSlider = large_scale_step_start;
                        else draw_ctrl.ScaleSlider = small_scale_step_end;
                        double intermediate_step = Math.Round(large_scale_step_start - small_scale_step_end, 1);
                        ScaleSlider.SmallChange = intermediate_step;
                        ScaleSlider.StepFrequency = intermediate_step;
                        draw_ctrl.add_no_draw_cnt();
                    }
                    else if (ScaleSlider.Value < (large_scale_step_start + large_scale_step))
                    {
                        draw_ctrl.ScaleSlider = large_scale_step_start + large_scale_step;
                        draw_ctrl.add_no_draw_cnt();
                    }
                }
                else
                {
                    ScaleSlider.SmallChange = small_scale_step;
                    ScaleSlider.StepFrequency = small_scale_step;
                }
                draw_spec();
                sub_no_draw_cnt();
            }
        }

        private void SpectrumPage_SizeChanged(object sender, Microsoft.UI.Xaml.SizeChangedEventArgs e)
        {
            SpecGrid.Height = this.ActualHeight / 2;
            if (draw_ctrl_vld()) draw_spec();
        }

        private void Timeline_ValueChanged(NumberBox sender, NumberBoxValueChangedEventArgs args)
        {
            if (draw_ctrl_vld())
            {
                if (TimelineNumber.IsEnabled) draw_spec();
                sub_no_draw_cnt();
            }
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
                case Draw_Control.JumpSel_Time:
                    switch_time_panel(true);
                    TimelineNumber.IsEnabled = false;
                    break;
                case Draw_Control.JumpSel_Timeline:
                    switch_time_panel(false);
                    TimelineNumber.IsEnabled = true;
                    break;
                default:
                    break;
            }
            if (draw_ctrl_vld())
            {
                if (JumpSel.IsEnabled) draw_spec();
                else sub_no_draw_cnt();
            }
        }

        // Draw
        private int draw_spec()
        {
            if (draw_ctrl.no_draw_cnt > 0)
            {
                draw_ctrl.no_draw_cnt--;
                if (draw_ctrl.no_draw_cnt > 0) return -1;
            }
            if (sharing_helper == null || (sharing_helper.navi_view != null
                && sharing_helper.navi_view.SelectedItem != sharing_helper.navi_view.MenuItems[3])) return -1;
            draw_ctrl.drawing = true;

            WriteableBitmap spec_image = null;
            int re = DrawHelper.draw_spec_image(ref spec_image, sharing_helper.tv_draw, sharing_helper.bd_draw, draw_ctrl.ChannelSel, draw_ctrl.JumpSel,
                (UInt64)TimelineNumber.Value, ScaleSlider.Value, TimeSlider, TimeText);
            draw_ctrl.DrawBrush.ImageSource = spec_image;

            DrawHelper.new_draw = false;
            draw_ctrl.drawing = false;
            return re;
        }

        public void setup_and_draw_spec()
        {
            control_setup();
            draw_spec();
        }

    }

    public class Draw_Control : DependencyObject
    {

        public const int JumpSel_Time = 0, JumpSel_Timeline = 1;

        public ImageBrush DrawBrush
        {
            get { return (ImageBrush)GetValue(DrawBrushProperty); }
            set { SetValue(DrawBrushProperty, value); }
        }
        public static readonly DependencyProperty DrawBrushProperty =
            DependencyProperty.Register(nameof(DrawBrush), typeof(ImageBrush), typeof(Draw_Control), new PropertyMetadata(0));

        public int ChannelSel
        {
            get { add_no_draw_cnt(); return (int)GetValue(ChannelSelProperty); }
            set
            {
                if (value >= 0)
                {
                    SetValue(ChannelSelProperty, value);
                }
            }
        }
        public static readonly DependencyProperty ChannelSelProperty =
            DependencyProperty.Register(nameof(ChannelSel), typeof(int), typeof(Draw_Control), new PropertyMetadata(0));

        public int JumpSel
        {
            get { add_no_draw_cnt(); return (int)GetValue(JumpSelProperty); }
            set { if (value >= 0)
                {
                    SetValue(JumpSelProperty, value);
                }
            }
        }
        public static readonly DependencyProperty JumpSelProperty =
            DependencyProperty.Register(nameof(JumpSel), typeof(int), typeof(Draw_Control), new PropertyMetadata(0));

        public double HourNumber
        {
            get { add_no_draw_cnt(); return (double)GetValue(HourNumberProperty); }
            set { SetValue(HourNumberProperty, value); }
        }
        public static readonly DependencyProperty HourNumberProperty =
            DependencyProperty.Register(nameof(HourNumber), typeof(double), typeof(Draw_Control), new PropertyMetadata(0));

        public double MinuNumber
        {
            get { add_no_draw_cnt(); return (double)GetValue(MinuNumberProperty); }
            set { SetValue(MinuNumberProperty, value); }
        }
        public static readonly DependencyProperty MinuNumberProperty =
            DependencyProperty.Register(nameof(MinuNumber), typeof(double), typeof(Draw_Control), new PropertyMetadata(0));

        public double SecNumber
        {
            get { add_no_draw_cnt(); return (double)GetValue(SecNumberProperty); }
            set { SetValue(SecNumberProperty, value); }
        }
        public static readonly DependencyProperty SecNumberProperty =
            DependencyProperty.Register(nameof(SecNumber), typeof(double), typeof(Draw_Control), new PropertyMetadata(0));

        public double CentiSecNumber
        {
            get { add_no_draw_cnt(); return (double)GetValue(CentiSecNumberProperty); }
            set { SetValue(CentiSecNumberProperty, value); }
        }
        public static readonly DependencyProperty CentiSecNumberProperty =
            DependencyProperty.Register(nameof(CentiSecNumber), typeof(double), typeof(Draw_Control), new PropertyMetadata(0));

        public double TimelineNumber
        {
            get { add_no_draw_cnt(); return (double)GetValue(TimelineNumberProperty); }
            set { SetValue(TimelineNumberProperty, value); }
        }
        public static readonly DependencyProperty TimelineNumberProperty =
            DependencyProperty.Register(nameof(TimelineNumber), typeof(double), typeof(Draw_Control), new PropertyMetadata(0));

        public double TimeSlider
        {
            get { add_no_draw_cnt(); return (double)GetValue(TimeSliderProperty); }
            set { SetValue(TimeSliderProperty, value); }
        }
        public static readonly DependencyProperty TimeSliderProperty =
            DependencyProperty.Register(nameof(TimeSlider), typeof(double), typeof(Draw_Control), new PropertyMetadata(0));

        public double ScaleSlider
        {
            get { add_no_draw_cnt(); return (double)GetValue(ScaleSliderProperty); }
            set { SetValue(ScaleSliderProperty, value); }
        }
        public static readonly DependencyProperty ScaleSliderProperty =
            DependencyProperty.Register(nameof(ScaleSlider), typeof(double), typeof(Draw_Control), new PropertyMetadata(0));
        
        public volatile bool drawing = false;
        public volatile bool no_ctrl_change = false;
        public volatile Action draw_spec = null;
        public volatile int no_draw_cnt = 0;

        public void add_no_draw_cnt()
        {
            if (!drawing && !no_ctrl_change) 
                no_draw_cnt++;
        }

        public void reset_control()
        {
            add_no_draw_cnt();
            ChannelSel = 0;
            JumpSel = JumpSel_Time;
            HourNumber = 0;
            MinuNumber = 0;
            SecNumber = 0;
            CentiSecNumber = 0;
            TimelineNumber = 1;
            TimeSlider = 0;
            ScaleSlider = 1.0;
            no_draw_cnt = 0;
            WriteableBitmap null_image = null;
            DrawBrush.ImageSource = null_image;
        }

        public Draw_Control ()
        {
            DrawBrush = new ImageBrush();
            reset_control();
        }
    }
}
