using System;

namespace BDMatchUI.Helper
{
    enum SettingType : UInt32 { FFTNum, MinCnfrmNum, SearchRange, MinFinddB, MaxLength, SubOffset, OutputPCM, DrawSpec, MatchAss, ParallelDecode, FastMatch, VolMatch, WindowWidth, WindowHeight };

    public enum LogLevel : UInt32 { Info, Warning, Error };
 
    public enum ISA_Mode : Int32 { Auto = -1, No_SIMD = 0, SSE = 1, AVX = 2, AVX2_FMA = 3 };

    public enum Prog_Mode : Int32 { Setting = 0, TV = 1, BD = 2, Sub = 3 };
    
    public enum MatchReturn : Int32
    {
        Success = 0, 
        EmptyPath = -1, PathNotExist = -2, InvalidRegex = -3, NotRegexPath = -4,

        Invalid = -100, User_Stop = -99,
        File_Open_Err, File_Stream_Err, No_Audio_Err, No_Codec_Err, Codec_Ctx_Err, Codec_Copy_Para_Err, Codec_Open_Err,
        PCM_Open_Err,
        Resample_Ctx_Alloc_Err, Resample_Ctx_Init_Err, Resample_Dst_Alloc_Err, Convert_Err,
        Frame_Alloc_Err, Submit_Packet_Err, Decode_Err, Data_Size_Err,
        Sub_Open_Err, Sub_Illegal_Err, Sub_Out_Open_Err, Sub_Write_Err
    };

    public enum Decode_File : UInt32 { TV_Decode, BD_Decode };
    public enum Decode_Info : UInt32 { Channels, FFT_Samp_Num, Centi_Sec, Samp_Rate, FFT_Num };

    public enum Timeline_Time_Type : UInt32 { Start_Time, End_Time };
}
