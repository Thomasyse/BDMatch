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
    }
}
