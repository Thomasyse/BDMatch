using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Documents;
using Microsoft.UI.Xaml.Media;
using System;
using System.Collections.Generic;
using System.Reflection.Emit;
using Windows.Foundation;

namespace BDMatchUI.Helper
{
    public class SharingHelper
    {
        public volatile SettingHelper settings = new();

        public volatile TextHelper text_helper = new();

        public NavigationView navi_view = null;

        public LogSource log = new();

        public volatile DrawHelper tv_draw = null, bd_draw = null;
        public Draw_Control draw_ctrl = new();

        public List<TypedEventHandler<FrameworkElement, object>> theme_change_action_list = [];
        
        public SharingHelper() { }
    }

    public class BadgeSource : DependencyObject
    {
        // Dependency Properties for binding.
        public int Value
        {
            get { return (int)GetValue(ValueProperty); }
            set
            {
                Visibility = value > 0;
                SetValue(ValueProperty, value);
            }
        }
        public static readonly DependencyProperty ValueProperty =
            DependencyProperty.Register(nameof(Value), typeof(int), typeof(BadgeSource), new PropertyMetadata(0));

        public bool Visibility
        {
            get { return (bool)GetValue(VisibilityProperty); }
            set { SetValue(VisibilityProperty, value); }
        }
        public static readonly DependencyProperty VisibilityProperty =
            DependencyProperty.Register("Visibility", typeof(bool), typeof(BadgeSource), new PropertyMetadata(false));

        public Style BStyle
        {
            get { return (Style)GetValue(BStyleProperty); }
            set { SetValue(BStyleProperty, value); }
        }
        public static readonly DependencyProperty BStyleProperty =
            DependencyProperty.Register("BStyle", typeof(Style), typeof(BadgeSource), new PropertyMetadata(false));

        public LogLevel log_level = LogLevel.Info;

        public BadgeSource()
        {
            Value = 0;
            Visibility = false;
            BStyle = StyleHelper.badge_style(LogLevel.Info);
        }
    }


    public class LogSource
    {
        public double ScrollVerticalOffset { get; set; }

        public Scroll_Anchor_Type scrollAncherType { get; set; }

        public volatile List<Tuple<string, LogLevel>> logs = [];
        public volatile Action load_log = null;
        public BadgeSource badge = new();

        public LogSource()
        {
            ScrollVerticalOffset = 0.0;
            scrollAncherType = Scroll_Anchor_Type.End;
        }
    }
}
