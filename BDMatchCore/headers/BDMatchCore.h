#pragma once
#include <atomic>
#include "decoder.h"
#include "matching.h"
#include "language_pack.h"

typedef void(__stdcall* prog_func)(int, double);
typedef void(__stdcall* feedback_func)(const char*, const long long len);

class BDMatchCore {
public:
	BDMatchCore(std::atomic_flag* keep_processing0 = nullptr);
	~BDMatchCore();
	int clear_data();
	int load_interface(const prog_func& prog_back0 = nullptr, const feedback_func& feed_func0 = nullptr);
	int load_settings(const int& isa_mode0 = 0, const int& fft_num0 = 512, const int& min_db0 = -14,
		const bool& output_pcm0 = false, const bool& parallel_decode0 = false, const bool& vol_match0 = false,
		const int& min_check_num0 = 100, const int& find_field0 = 10, const int& ass_offset0 = 0, const int& max_length0 = 20,
		const bool& match_ass0 = true, const bool& fast_match0 = false, const bool& debug_mode0 = false);
	int decode(const char* tv_path, const char* bd_path);
	int match_1(const char* ass_path);
	int match_2(const char* output_path);
	int clear_match();
	size_t get_nb_timeline();
	int get_timeline(const int& index, const int& type);
	int get_decode_info(const Deocde_File& file, const Decode_Info& type);
	char** get_decode_spec(const Deocde_File& file);
private:
	language_pack lang_pack;//language pack
	int feedback_tv(const std::string& tv_path);
	int feedback_bd(const std::string& bd_path, const double& bd_pre_avg_vol);
	int feedback_time(const double& spend);
	int feedback_match();
	std::unique_ptr<Decode::Decode> tv_decode;
	std::unique_ptr<Decode::Decode> bd_decode;
	std::unique_ptr<Matching::Match> match;
	std::atomic_flag* const keep_processing;//multithreading cancel token
	prog_func prog_back = nullptr;
	feedback_func feed_func = nullptr;
	int isa_mode = 0;
	int fft_num = 512;
	int min_db = -14;
	bool output_pcm = false;
	bool vol_match = false;
	bool parallel_decode = false;
	int min_check_num = 100;
	int find_field = 10;
	int ass_offset = 0;
	int max_length = 20;
	bool match_ass = true;
	bool fast_match = false;
	bool debug_mode = false;
}; 