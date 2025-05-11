using Microsoft.UI;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Media;
using Windows.UI;

namespace BDMatchUI.Helper
{
    public static class StyleHelper
    {
        public static SolidColorBrush color_brush(LogLevel level)
        {
            switch (level)
            {
                case LogLevel.Info: return (SolidColorBrush)Application.Current.Resources["InfoBrush"];
                case LogLevel.Warning: return (SolidColorBrush)Application.Current.Resources["WarningBrush"];
                case LogLevel.Error: return (SolidColorBrush)Application.Current.Resources["ErrorBrush"];
            }
            return (SolidColorBrush)Application.Current.Resources["InfoBrush"];
        }

        public static Style badge_style(LogLevel level)
        {
            switch (level)
            {
                case LogLevel.Info: return (Style)Application.Current.Resources["AttentionValueInfoBadgeStyle"];
                case LogLevel.Warning: return (Style)Application.Current.Resources["CautionValueInfoBadgeStyle"];
                case LogLevel.Error: return (Style)Application.Current.Resources["CriticalValueInfoBadgeStyle"];
            }
            return (Style)Application.Current.Resources["AttentionValueInfoBadgeStyle"];
        }
    }
}
