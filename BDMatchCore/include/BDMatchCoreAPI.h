#pragma once

#define _EXPORT_

// No need for platform other than Windows
#include "headers/datastruct.h"
#ifdef BDMATCHCORE_EXPORTS
#define BDMatchCore_API  __declspec( dllexport )
#else
#define BDMatchCore_API  __declspec( dllimport )
#endif

typedef void(__stdcall* prog_func)(const Prog_Mode, const double);
typedef void(__stdcall* feedback_func)(const char*, const int64_t len);

namespace BDMatchCoreAPI {
	extern "C" {
		BDMatchCore_API int new_BDMatchCore();
		BDMatchCore_API int clear_data();
		BDMatchCore_API int set_language(const char* language);
		BDMatchCore_API int load_interface(const prog_func& prog_back = nullptr, const feedback_func& feed_func = nullptr);
		BDMatchCore_API int load_settings(const ISA_Mode& isa_mode = ISA_Mode::Auto, const int& fft_num = 512, const int& min_db = -14,
			const bool& output_pcm = false, const bool& parallel_decode = false, const bool& vol_match = false,
			const int& min_cnfrm_num = 100, const int& search_range = 10, const int& sub_offset = 0, const int& max_length = 20,
			const bool& match_ass = true, const bool& fast_match = false, const bool& debug_mode = false);
		BDMatchCore_API Match_Core_Return decode(const char* tv_path, const char* bd_path); // UTF-8 required
		BDMatchCore_API Match_Core_Return match_1(const char* sub_path, const char* encoded_tv_path = nullptr, const char* encoded_bd_path = nullptr);
		BDMatchCore_API Match_Core_Return match_2(const char* output_path);
		BDMatchCore_API int clear_match();
		BDMatchCore_API size_t get_nb_timeline();
		BDMatchCore_API int64_t get_timeline(const size_t& index, const Match::Timeline_Time_Type& type);
		BDMatchCore_API int64_t get_decode_info(const Decode::Decode_File& file, const Decode::Decode_Info& type);
		BDMatchCore_API char** get_decode_spec(const Decode::Decode_File& file);
		BDMatchCore_API ISA_Mode search_ISA_mode();
		BDMatchCore_API const char* get_CPU_brand();
		BDMatchCore_API int start_process();
		BDMatchCore_API int stop_process();
		int cal_ISA_mode();
	}
}