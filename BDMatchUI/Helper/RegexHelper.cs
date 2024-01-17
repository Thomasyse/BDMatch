using System;
using System.Collections.Generic;
using System.IO;
using System.Text.RegularExpressions;


namespace BDMatchUI.Helper
{
    struct PathSection
    {
        public string section { get; set; }
        public bool regex { get; set; }

        public PathSection(string section, bool regex) { 
            this.section = section;
            this.regex = regex;
        }
    }

    public struct PathMatch
    {
        public string path { get; set; }
        public List<string> pattern { get; set; }

        public PathMatch(string path, List<string> pattern)
        {
            this.path = path;
            this.pattern = pattern;
        }
    }

    public static class RegexHelper
    {
        public static bool verif_regex_path(string path)
        {
            int quote_regex_cnt = Regex.Matches(path, "\".+?\"").Count;
            var quote_matches = Regex.Matches(path, "(\"\\\\\")|((\\\\|^)\")|(\"(\\\\|$))");
            bool begin_quote = true;
            int begin_quote_cnt = 0, end_quote_cnt = 0;
            foreach (Match match in quote_matches)
            {
                if (match.Value == "\"")
                {
                    if (begin_quote) begin_quote_cnt++;
                    else end_quote_cnt++;
                }
                else if (match.Value == "\\\"")
                {
                    if (begin_quote) begin_quote_cnt++;
                    else return false;
                }
                else if (match.Value == "\"\\")
                {
                    if (begin_quote) return false;
                    else end_quote_cnt++;
                }
                else if (match.Value == "\"\\\"")
                {
                    if (begin_quote) return false;
                    else
                    {
                        end_quote_cnt++;
                        begin_quote = !begin_quote;
                        begin_quote_cnt++;
                    }
                }
                begin_quote = !begin_quote;
            }
            if (quote_regex_cnt > 0 && quote_regex_cnt == begin_quote_cnt && quote_regex_cnt == end_quote_cnt) return true;
            else return false;
        }

        private static void search_path(List<PathSection> path_sec_list, int depth, string cur_path, List<string> cur_patterns, List<PathMatch> matches)
        {
            var cur_sec = path_sec_list[depth];
            bool deepest = (depth + 1 == path_sec_list.Count);
            if (cur_sec.regex)
            {
                string[] file_path_list;
                try
                {
                    if (deepest)
                        file_path_list = Directory.GetFiles(cur_path, "*", SearchOption.TopDirectoryOnly);
                    else
                        file_path_list = Directory.GetDirectories(cur_path, "*", SearchOption.TopDirectoryOnly);
                } 
                catch {
                    return;
                }
                foreach (string file_path in file_path_list)
                {
                    string file_name = Path.GetFileName(file_path);
                    var regex_match = Regex.Match(file_name, "^" + cur_sec.section + "$");
                    if (regex_match.Success)
                    {
                        int sec_pattern_cnt = regex_match.Groups.Count - 1;
                        cur_path += file_name;
                        for (int i = 1; i < regex_match.Groups.Count; i++)cur_patterns.Add(regex_match.Groups[i].Value);
                        if (deepest) matches.Add(new PathMatch(cur_path, new List<string>(cur_patterns)));
                        else
                        {
                            search_path(path_sec_list, depth + 1, cur_path, cur_patterns, matches);
                        }
                        cur_path = cur_path.Remove(cur_path.Length - file_name.Length);
                        cur_patterns.RemoveRange(cur_patterns.Count - sec_pattern_cnt, sec_pattern_cnt);
                    }
                }
            }
            else
            {
                cur_path += cur_sec.section;
                if (deepest)
                {
                    if (Path.Exists(cur_path)) matches.Add(new PathMatch(cur_path, new List<string>(cur_patterns)));
                }
                else search_path(path_sec_list, depth + 1, cur_path, cur_patterns, matches);
                cur_path = cur_path.Remove(cur_path.Length - cur_sec.section.Length);
            }
        }

        public static List<PathMatch> match_regex_path(string path, Action<string> log = null)
        {
            var path_sub_list = path.Split('\"');
            bool is_regex = path.StartsWith('\"');
            List<PathSection> path_sec_list = new List<PathSection>();
            foreach (string sub_path in path_sub_list)
            {
                if (!string.IsNullOrWhiteSpace(sub_path))
                {
                    if (is_regex)
                    {
                        try
                        {
                            Regex.Match("", sub_path);
                        }
                        catch (Exception e)
                        {
                            if (log != null) log(e.Message);
                            return null;
                        }
                    }
                    path_sec_list.Add(new PathSection(sub_path, is_regex));
                    is_regex = !is_regex;
                }
            }
            List<PathMatch> matches = new List<PathMatch>();
            search_path(path_sec_list, 0, "", new List<string>(), matches);
            return matches;
        }
    }
}
