#pragma once

#include "targetver.h"

namespace DataStruct {

	class node
	{
	public:
		node();
		int init_data(const int &num, char*& data_ptr);
		char read0(const int &pos);
		char *getdata();
		int sum();
		char maxv();
		int size();
	private:
		char *data = nullptr;
		int count = 0;
		int sumval = -2147483647;
	};

}

enum class Decode_File { TV_Decode, BD_Decode };
enum class Decode_Info { Channels, FFT_Samp_Num, Milisec, Samp_Rate, FFT_Num };
enum class Sub_Type { ASS, SRT };

namespace Matching {
	constexpr int Timeline_Start_Time = 0;
	constexpr int Timeline_End_Time = 1;
}

