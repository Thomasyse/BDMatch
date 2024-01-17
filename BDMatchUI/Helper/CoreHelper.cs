using System;
using System.Runtime.InteropServices;

namespace BDMatchUI.Helper
{
    public delegate void ProgressCallback(Prog_Mode type, double val);
    public delegate void FeedbackCallback(IntPtr input, Int64 len);

    public static class CoreHelper
    {
        public static IntPtr match_core = 0;

        [DllImport(@"BDMatchCore.dll", EntryPoint = "new_BDMatchCore", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern Int32 new_BDMatchCore();

        [DllImport(@"BDMatchCore.dll", EntryPoint = "get_CPU_brand", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern IntPtr get_CPU_brand();

        [DllImport(@"BDMatchCore.dll", EntryPoint = "search_ISA_mode", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern UInt32 search_ISA_mode();

        [DllImport(@"BDMatchCore.dll", EntryPoint = "start_process", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern Int32 start_process();

        [DllImport(@"BDMatchCore.dll", EntryPoint = "stop_process", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern Int32 stop_process();

        [DllImport(@"BDMatchCore.dll", EntryPoint = "set_language", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern Int32 set_language([MarshalAs(UnmanagedType.LPStr)] string language);

        [DllImport(@"BDMatchCore.dll", EntryPoint = "load_interface", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern Int32 load_interface([MarshalAs(UnmanagedType.FunctionPtr)]in ProgressCallback prog_ptr, [MarshalAs(UnmanagedType.FunctionPtr)]in FeedbackCallback feedback_ptr);

        [DllImport(@"BDMatchCore.dll", EntryPoint = "load_settings", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern Int32 load_settings(in ISA_Mode isa_mode, in Int32 fft_num, in Int32 min_db,
			in bool output_pcm, in bool parallel_decode, in bool vol_match,
            in Int32 min_cnfrm_num, in Int32 search_range, in Int32 sub_offset, in Int32 max_length,
            in bool match_ass, in bool fast_match, in bool debug_mode);

        [DllImport(@"BDMatchCore.dll", EntryPoint = "decode", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern MatchReturn decode([MarshalAs(UnmanagedType.LPStr)] string tv_path, [MarshalAs(UnmanagedType.LPStr)] string bd_path);

        [DllImport(@"BDMatchCore.dll", EntryPoint = "match_1", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern MatchReturn match_1([MarshalAs(UnmanagedType.LPStr)] string sub_path, [MarshalAs(UnmanagedType.LPArray)] byte[] tv_path, [MarshalAs(UnmanagedType.LPArray)] byte[] bd_path);

        [DllImport(@"BDMatchCore.dll", EntryPoint = "match_2", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern MatchReturn match_2([MarshalAs(UnmanagedType.LPStr)] string output_path);

        [DllImport(@"BDMatchCore.dll", EntryPoint = "clear_data", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern Int32 clear_data();

        [DllImport(@"BDMatchCore.dll", EntryPoint = "clear_match", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern Int32 clear_match();

        [DllImport(@"BDMatchCore.dll", EntryPoint = "get_decode_spec", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern IntPtr get_decode_spec(in Decode_File file);

        [DllImport(@"BDMatchCore.dll", EntryPoint = "get_decode_info", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern Int64 get_decode_info(in Decode_File file, in Decode_Info type);

        [DllImport(@"BDMatchCore.dll", EntryPoint = "get_nb_timeline", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern UInt64 get_nb_timeline();

        [DllImport(@"BDMatchCore.dll", EntryPoint = "get_timeline", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern Int64 get_timeline(in UInt64 index, in Timeline_Time_Type type);
    }
}
