#pragma once
// No need for platform other than Windows
#include <atomic>
#include "headers/datastruct.h"
#ifdef BDMATCHCORE_EXPORTS
#define BDMatchCore_API  __declspec( dllexport )
#else
#define BDMatchCore_API  __declspec( dllimport )
#endif

typedef void(__stdcall* prog_func)(int, double);
typedef void(__stdcall* feedback_func)(const char*, const long long len);

namespace BDMatchCoreAPI {
	BDMatchCore_API int new_BDMatchCore();
	BDMatchCore_API int clear_data();
	BDMatchCore_API int load_interface(const prog_func& prog_back = nullptr, const feedback_func& feed_func = nullptr);
	BDMatchCore_API int load_settings(const int& isa_mode = 0, const int& fft_num = 512, const int& min_db = -14,
		const bool& output_pcm = false, const bool& parallel_decode = false, const bool& vol_match = false,
		const int& min_check_num = 100, const int& find_field = 10, const int& sub_offset = 0, const int& max_length = 20,
		const bool& match_ass = true, const bool& fast_match = false, const bool& debug_mode = false);
	BDMatchCore_API int decode(const char* tv_path, const char* bd_path);
	BDMatchCore_API int match_1(const char* sub_path);
	BDMatchCore_API int match_2(const char* output_path);
	BDMatchCore_API int clear_match();
	BDMatchCore_API size_t get_nb_timeline();
	BDMatchCore_API int get_timeline(const int& index, const Match::Timeline_Time_Type& type);
	BDMatchCore_API int get_decode_info(const Decode::Decode_File& file, const Decode::Decode_Info& type);
	BDMatchCore_API char** get_decode_spec(const Decode::Decode_File& file);
	BDMatchCore_API int search_ISA_mode();
	BDMatchCore_API const char* get_CPU_brand();
	BDMatchCore_API int start_process();
	BDMatchCore_API int stop_process();
	int cal_ISA_mode();
}