#pragma once
#include <atomic>
#include <chrono>
#include "decoder.h"
#include "match.h"
#include "language_pack.h"

typedef void(*prog_func)(int, double);
typedef void(*feedback_func)(const char*, const long long len);

enum class Procedure { Decode = 4, Match = 6 };

class BDMatchCore {
public:
	BDMatchCore();
	~BDMatchCore();
	int clear_data();
	int load_interface(const prog_func& prog_back0 = nullptr, const feedback_func& feed_func0 = nullptr);
	int load_settings(const int& isa_mode0 = 0, const int& fft_num0 = 512, const int& min_db0 = -14,
		const bool& output_pcm0 = false, const bool& parallel_decode0 = false, const bool& vol_match0 = false,
		const int& min_check_num0 = 100, const int& find_field0 = 10, const int& sub_offset0 = 0, const int& max_length0 = 20,
		const bool& match_ass0 = true, const bool& fast_match0 = false, const bool& debug_mode0 = false);
	int decode(const char* tv_path0, const char* bd_path0);
	int match_1(const char* sub_path0);
	int match_2(const char* output_path0);
	int clear_match();
	size_t get_nb_timeline();
	int get_timeline(const int& index, const Match::Timeline_Time_Type& type);
	int get_decode_info(const Decode::Decode_File& file, const Decode::Decode_Info& type);
	char** get_decode_spec(const Decode::Decode_File& file);
	int start_process();
	int stop_process();
private:
	language_pack lang_pack;//language pack
	int feedback_tv(const std::string& tv_path);
	int feedback_bd(const std::string& bd_path, const double& bd_pre_avg_vol);
	int feedback_time(const Procedure& proc);
	int feedback_match();
	std::unique_ptr<Decode::Decode> tv_decode;
	std::unique_ptr<Decode::Decode> bd_decode;
	std::unique_ptr<Match::Match> match;
	std::shared_ptr<std::atomic_flag> keep_processing = nullptr;//multithreading cancel token
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
	int sub_offset = 0;
	int max_length = 20;
	bool match_ass = true;
	bool fast_match = false;
	bool debug_mode = false;
	std::chrono::time_point<std::chrono::high_resolution_clock> clock_start;
}; 