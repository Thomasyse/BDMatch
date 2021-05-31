#pragma once

#include "targetver.h"
#include <limits>
#include <cstdint>

#ifndef _EXPORT_
namespace DataStruct {
	class Spec_Node
	{
	public:
		Spec_Node();
		int init_data(const int &num, char* data_ptr);
		char read0(const int &pos);
		char *getdata();
		int sum();
		char maxv();
		int size();
	private:
		char *data = nullptr;
		int count = 0;
		int sumval = std::numeric_limits<int>::min();
	};
}
#endif

namespace Decode {
	enum class Decode_File { TV_Decode, BD_Decode };
	enum class Decode_Info { Channels, FFT_Samp_Num, Milisec, Samp_Rate, FFT_Num };
}

namespace Match {
	enum class Timeline_Time_Type { Start_Time, End_Time };
}

enum class ISA_Mode { No_SIMD = 0, SSE = 1, AVX = 2, AVX2_FMA = 3 };
