using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Media.Imaging;
using System;
using System.Drawing;
using System.IO;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Text;

namespace BDMatchUI.Helper
{
    public class DrawHelper
    {
        public Int64 fft_cnt { get; set; } = 0;
        public int ch_cnt { get; set; } = 0;
        public Int64 centi_sec { get; set; } = 0;
        public UInt64 line_num { get; set; } = 0;
        public int fft_num { get; set; } = 0;
        public double cs_to_fft { get; set; } = 1.0; // Time to FFT Index
        public IntPtr spec { get; set; } = 0;
        public Int64[,] time_list { get; set; } = null;

        public volatile static bool new_draw = true;

        const int secpurple = 45;

        enum InLine { None, This, Other };
        enum Edge { None, Start, End, Other };

        public static int draw_spec_image(ref WriteableBitmap bitmap, DrawHelper tv_draw, DrawHelper bd_draw, int channel, int jump_sel, UInt64 timeline, double scale, Slider TimeSlider, TextBlock TimeText)
        {
            if (tv_draw == null || bd_draw == null || tv_draw.spec == 0 || bd_draw.spec == 0)
            {
                TimeText.Text = AppResources.get_string("BDMatchUI/SpectrumPage/No_Spectrum");
                return -1;
            }
            Int64 centi_sec = Math.Max(tv_draw.centi_sec, bd_draw.centi_sec);
            int half_range = (int)Math.Min(centi_sec / 2, TimeSlider.ActualWidth / 4 / scale);
            Int64 tv_start = 0, tv_end = 0, bd_start = 0;
            if (jump_sel == SpectrumPage.JumpSel_Time)
            {
                tv_start = (Int64)(Math.Round((TimeSlider.Value - half_range) * tv_draw.cs_to_fft));
                tv_end = (Int64)(Math.Round((TimeSlider.Value + half_range) * tv_draw.cs_to_fft));
                bd_start = (Int64)(Math.Round((TimeSlider.Value - half_range) * bd_draw.cs_to_fft));
                TimeText.Text = "";
            }
            else
            {
                Int64 tv_timeline_duration = tv_draw.time_list[timeline - 1, 1] - tv_draw.time_list[timeline - 1, 0];
                if (tv_timeline_duration > TimeSlider.ActualWidth / 2) half_range = (int)Math.Min(centi_sec / 2, tv_timeline_duration / (1.03 * scale));
                tv_start = (tv_draw.time_list[timeline - 1, 0] + tv_draw.time_list[timeline - 1, 1]) / 2 - half_range;
                tv_end = tv_start + 2 * half_range;
                bd_start = (bd_draw.time_list[timeline - 1, 0] + bd_draw.time_list[timeline - 1, 1]) / 2 - half_range;
                if (bd_draw.time_list[timeline - 1, 0] == -1) TimeText.Text = AppResources.get_string("BDMatchUI/SpectrumPage/Not_Matched");
                else
                {
                    var time_to_str = static (TimeSpan time_span) =>
                    {
                        StringBuilder time_builder = new StringBuilder();
                        time_builder.Append((time_span.Days * 24 + time_span.Hours).ToString());
                        time_builder.Append(':');
                        time_builder.Append(time_span.ToString(@"mm\:ss\.ff"));
                        return time_builder.ToString();
                    };
                    TimeSpan tv_time = TimeSpan.FromMilliseconds((int)(Math.Round(tv_draw.time_list[timeline - 1, 0] / tv_draw.cs_to_fft)) * 10);
                    TimeSpan bd_time = TimeSpan.FromMilliseconds((int)(Math.Round(bd_draw.time_list[timeline - 1, 0] / bd_draw.cs_to_fft)) * 10);
                    StringBuilder time_builder = new StringBuilder();
                    time_builder.Append(time_to_str(tv_time));
                    time_builder.Append("  =>  ");
                    time_builder.Append(time_to_str(bd_time));
                    TimeText.Text = time_builder.ToString();
                }
            }
            Int64 duration = (Int64)(tv_end - tv_start + 1);

            if (channel >= Math.Min(tv_draw.ch_cnt, bd_draw.ch_cnt))
            {
                TimeText.Text = AppResources.get_string("BDMatchUI/SpectrumPage/Channel_Invalid");
                return -2;
            }
            IntPtr tv_spec_ch = Marshal.ReadIntPtr(tv_draw.spec + channel * IntPtr.Size);
            IntPtr bd_spec_ch = Marshal.ReadIntPtr(bd_draw.spec + channel * IntPtr.Size);
            bitmap = new WriteableBitmap((int)duration, tv_draw.fft_num);
            Int64 bitmap_line_stide = duration * 4;
            byte[] buffer = new byte[bitmap_line_stide * (Int64)(tv_draw.fft_num)];
            Int64 buf_pos = 0;

            Int64 x, y;
            // Loop through the images pixels to reset color.
            for (x = 0; x < duration; x++)
            {
                InLine tv_in_line = InLine.None, bd_in_line = InLine.None;
                Edge tv_edge = Edge.None, bd_edge = Edge.None;
                for (UInt64 line_idx = 0; line_idx < tv_draw.line_num; line_idx++)
                {
                    if (tv_draw.time_list[line_idx, 0] >= 0)
                    {
                        var cal_line_property = (Int64 line_start, Int64 line_end, Int64 sec_start, ref InLine in_line, ref Edge edge) =>
                        {
                            if (line_start <= x + sec_start && line_end >= x + sec_start)
                            {
                                if (line_idx == timeline - 1 || jump_sel == SpectrumPage.JumpSel_Time) in_line = InLine.This;
                                else if (in_line == InLine.None) in_line = InLine.Other;
                                if (line_start == x + sec_start || line_end == x + sec_start)
                                {
                                    if (line_idx == timeline - 1 || jump_sel == SpectrumPage.JumpSel_Time)
                                    {
                                        if (line_start == x + sec_start) edge = Edge.Start;
                                        else if (edge != Edge.Start) edge = Edge.End;
                                    }
                                    else if (edge != Edge.Start && edge != Edge.End) edge = Edge.Other;
                                }
                            }
                        };
                        cal_line_property(tv_draw.time_list[line_idx, 0], tv_draw.time_list[line_idx, 1], tv_start, ref tv_in_line, ref tv_edge);
                        cal_line_property(bd_draw.time_list[line_idx, 0], bd_draw.time_list[line_idx, 1], bd_start, ref bd_in_line, ref bd_edge);
                    }
                }
                Int64 bd_spec_offset = (x + bd_start) * bd_draw.fft_num / 2 + bd_draw.fft_num - 1;
                Int64 tv_spec_offset = (x + tv_start) * tv_draw.fft_num / 2 + tv_draw.fft_num / 2 - 1;
                Int64 pos_offset = 0;
                for (y = 0; y < tv_draw.fft_num; y++)
                {
                    int color = -128;
                    if (y >= bd_draw.fft_num / 2)
                    {
                        if (0 <= x + bd_start && x + bd_start < bd_draw.fft_cnt)
                            color = (sbyte)Marshal.ReadByte(bd_spec_ch + (int)bd_spec_offset - (int)y);
                    }
                    else if (0 <= x + tv_start && x + tv_start < tv_draw.fft_cnt)
                        color = (sbyte)Marshal.ReadByte(tv_spec_ch + (int)tv_spec_offset - (int)y);
                    color += 128;
                    Color new_color = Color.FromArgb(color / 4, color, color);
                    var get_new_color = static (int color, Edge edge, InLine in_line) =>
                    {
                        switch (edge)
                        {
                            case Edge.Start:
                                return Color.FromArgb(0, 255, 0);
                            case Edge.End:
                                return Color.FromArgb(255, 255, 0);
                            case Edge.Other:
                                return Color.FromArgb(180, 180, 180);
                            default:
                                switch (in_line)
                                {
                                    case InLine.This:
                                        return Color.FromArgb(color / 4 + 70, color, Math.Max(color, 70));
                                    case InLine.Other:
                                        return Color.FromArgb(color / 4 + secpurple, color, Math.Max(color, secpurple));
                                    default:
                                        return Color.FromArgb(color / 4 + secpurple, color, Math.Max(color, secpurple));
                                }
                        }
                    };
                    if (y >= bd_draw.fft_num / 2)
                    {
                        if (bd_in_line != InLine.None) new_color = get_new_color(color, bd_edge, bd_in_line);                        
                    }
                    else
                    {
                        if (tv_in_line != InLine.None) new_color = get_new_color(color, tv_edge, tv_in_line);
                    }
                    buffer[buf_pos + pos_offset] = new_color.B;
                    buffer[buf_pos + pos_offset + 1] = new_color.G;
                    buffer[buf_pos + pos_offset + 2] = new_color.R;
                    buffer[buf_pos + pos_offset + 3] = new_color.A;
                    pos_offset += bitmap_line_stide;
                }
                buf_pos += 4;
            }
            using (Stream stream = bitmap.PixelBuffer.AsStream())
            {
                stream.Write(buffer, 0, buffer.Length);
            }
            return 0;
        }
    }
}
