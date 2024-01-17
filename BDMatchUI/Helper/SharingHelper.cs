using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Documents;
using System;
using System.Collections.Generic;
using Windows.Foundation;

namespace BDMatchUI.Helper
{
    internal class SharingHelper
    {
        public SharingHelper() { }
        public volatile List<Tuple<Paragraph, LogLevel>> logs = new List<Tuple<Paragraph, LogLevel>>();
        public volatile SettingHelper settings = new SettingHelper();
        public NavigationView navi_view = null;
        public volatile Action load_log = null;
        public volatile DrawHelper tv_draw = null, bd_draw = null;

        public List<TypedEventHandler<FrameworkElement, object>> theme_change_action_list = new List<TypedEventHandler<FrameworkElement, object>>();
    }
}
