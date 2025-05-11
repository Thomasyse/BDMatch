using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;

namespace BDMatchUI.Helper
{
    public class SettingHelper
    {
        public SettingHelper()
        {
            this[SettingType.FFTNum] = 512;
            this[SettingType.MinCnfrmNum] = 100;
            this[SettingType.SearchRange] = 10;
            this[SettingType.MinFinddB] = -14;
            this[SettingType.MaxLength] = 20;
            this[SettingType.SubOffset] = 0;
            this[SettingType.OutputPCM] = 0;
            this[SettingType.DrawSpec] = 0;
            this[SettingType.MatchAss] = 1;
            this[SettingType.ParallelDecode] = 0;
        }
        public void save_settings(string path = "settings.ini")
        {
            try
            {
                StringBuilder out_builder = new StringBuilder();
                if (!string.IsNullOrEmpty(language))
                {
                    out_builder.Append("Language=");
                    out_builder.Append(language);
                    out_builder.Append("\r\n");
                }

                out_builder.Append("Theme=");
                out_builder.Append(backdrop.ToString());
                out_builder.Append("\r\n");

                foreach (SettingType type in Enum.GetValues(typeof(SettingType)))
                {
                    out_builder.Append(Enum.GetName(type));
                    out_builder.Append('=');
                    out_builder.Append(this[type].ToString());
                    out_builder.Append("\r\n");
                }
                FileStream fs = File.OpenWrite(path);
                byte[] info = (new UTF8Encoding(true)).GetBytes(out_builder.ToString());
                fs.Seek(0, SeekOrigin.Begin);
                fs.Write(info, 0, info.Length);
                fs.Close();
            }
            finally
            {
            }
        }
        public void load_settings(string path = "settings.ini")
        {
            if (File.Exists(path))
            {
                string set_text = "";
                try
                {
                    set_text = File.ReadAllText(path);
                }
                finally
                {
                    Regex lang_key = new Regex("Language=(.+)\\r\\n");
                    string lang_str = lang_key.Match(set_text).Groups[1].Value;
                    if (!string.IsNullOrEmpty(lang_str) && Windows.Globalization.ApplicationLanguages.Languages.Contains(lang_str))
                    {
                        language = lang_str;
                        AppResources.set_language(language);
                    }

                    Regex theme_key = new Regex("Theme=(.+)\\r\\n");
                    string theme_str = theme_key.Match(set_text).Groups[1].Value;
                    if (!string.IsNullOrEmpty(theme_str) && Enum.GetNames(typeof(BackdropType)).Contains(theme_str))
                    {
                        backdrop = (BackdropType)Enum.Parse(typeof(BackdropType), theme_str);
                    }

                    Regex num_key = new Regex("[\\+-]?[0-9]+");
                    foreach (SettingType type in Enum.GetValues(typeof(SettingType)))
                    {
                        Regex set_key = new Regex(Enum.GetName(type) + "=[\\+-]?[0-9]+\\r\\n");
                        string setting_str = set_key.Match(set_text).Value;
                        if (!string.IsNullOrEmpty(setting_str))
                        {
                            int val = int.Parse(num_key.Match(setting_str).Value);
                            this[type] = val;
                        }
                    }
                }
            }
        }

        private int[] setting_arr = new int[Enum.GetValues(typeof(SettingType)).Length];

        public int this[SettingType type]
        {
            get { return setting_arr[(UInt32)type]; }
            set {
                switch (type)
                {
                    case SettingType.FFTNum:
                        value = Math.Max(value, 64);
                        value = Math.Min(value, 16384);
                        break;
                    case SettingType.MinFinddB:
                        value = Math.Max(value, -80);
                        value = Math.Min(value, 5);
                        break;
                    case SettingType.SearchRange:
                        value = Math.Max(value, 1);
                        break;
                    case SettingType.MaxLength:
                        value = Math.Max(value, 20);
                        break;
                    case SettingType.MinCnfrmNum:
                        value = Math.Max(value, 10);
                        break;
                    case SettingType.SubOffset:
                        break;
                    case SettingType.WindowWidth:
                        break;
                    case SettingType.WindowHeight:
                        break;
                    default:
                        value = Convert.ToInt32(Convert.ToBoolean(value));
                        break;
                }
                setting_arr[(UInt32)type] = value;
            }
        }

        public bool debug_mode { get; set; } = false;
        public string language { get; set; } = null;
        public BackdropType backdrop = BackdropType.Mica;
    }
}
