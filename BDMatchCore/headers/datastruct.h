#pragma once

#include "targetver.h"
#include <limits>
#include <cstdint>
#include <atomic>

enum class ISA_Mode : int32_t { Auto = -1, No_SIMD = 0, SSE = 1, AVX = 2, AVX2_FMA = 3 };

enum class Prog_Mode : int32_t { Setting = 0, TV = 1, BD = 2, Sub = 3 };

enum class Match_Core_Return : int32_t { 
	Success = 0,
	Invalid = -100, User_Stop = -99,
	File_Open_Err, File_Stream_Err, No_Audio_Err, No_Codec_Err, Codec_Ctx_Err, Codec_Copy_Para_Err, Codec_Open_Err, Channel_Limit_Err, 
	PCM_Open_Err, 
	Resample_Ctx_Alloc_Err, Resample_Ctx_Init_Err, Resample_Dst_Alloc_Err, Convert_Err, 
	Frame_Alloc_Err, Submit_Packet_Err, Decode_Err, Data_Size_Err, 
	Sub_Open_Err, Sub_Illegal_Err, Sub_Out_Open_Err, Sub_Write_Err
};

#ifndef _EXPORT_

typedef void (*prog_func)(const Prog_Mode, const double);
typedef void(*feedback_func)(const char*, const int64_t len);

namespace DataStruct {
	class Spec_Node
	{
	public:
		Spec_Node();
		int init_data(const int &num, char* data_ptr);
		char read0(const int &pos) const;
		char *getdata() const;
		int size() const;
		int get_sum() const;
		int set_sum(const int& sum);
	private:
		char *data = nullptr;
		int count = 0;
		std::atomic<int> sum = std::numeric_limits<int>::min();
	};
}

constexpr double MIN_AWARE_PROG_VAL = 0.04;
#endif

namespace Decode {
	enum class Decode_File : uint32_t { TV_Decode, BD_Decode };
	enum class Decode_Info : uint32_t { Channels, FFT_Samp_Num, Centi_Sec, Samp_Rate, FFT_Num };
	constexpr size_t MAX_CHANNEL_CNT = 8;
}

namespace Match {
	enum class Timeline_Time_Type : uint32_t { Start_Time, End_Time };
}
