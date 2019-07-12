#pragma once
#include "datastruct.h"
#include <atomic>
#include <array>
#include <vector>
#include <string> 
#include "language_pack.h"

namespace Matching {

	constexpr int Timeline_Start_Time = 0;
	constexpr int Timeline_End_Time = 1;

	using namespace DataStruct;

	typedef void (__stdcall *prog_func)(int, double);

	class timeline
	{
	public:
		timeline(int start0, int end0, bool iscom0, 
			const std::string &head0, const std::string &text0);
		int start();
		int end();
		int duration();
		bool iscom();
		std::string head();
		std::string former_text();
		int start(const int &start0);
		int end(const int &end0);
		int iscom(const bool&iscom0);
		int head(std::string &head0);
	private:
		int start_;
		int end_;
		bool iscom_;
		std::string head_;
		std::string former_text_;
	};

	class bdsearch
	{
	public:
		bdsearch();
		int reserve(const int &num);
		int push(const int &time, const int &diff);
		int read(const int &pos);
		int find(const int &searchnum, const int &retype);
		int sort();
		int size();
		int clear();
	private:
		std::vector<std::array<int, 2>>bditem;
	};

	class se_re
	{
	public:
		se_re();
		long long& operator[](const int &index);
		se_re(se_re &in);
		int init();
	private:
		long long data[2] = { 922372036854775808,0 };
	};

	struct debug_info {//debug info in matching
		double aveindex = 0, maxindex = 0;
		int maxdelta = 0, maxline = 0;
	};

	class Match {
	public:
		Match(language_pack& lang_pack0, std::atomic_flag* keep_processing0 = nullptr);
		~Match();
		int load_settings(const int &min_check_num0, const int &find_field0, const int &ass_offset0, 
			const int &max_length0,
			const bool &fast_match0, const bool &debug_mode0, const prog_func &prog_single0 = nullptr);
		int load_decode_info(node ** const &tv_fft_data0, node ** const &bd_fft_data0,
			const int &tv_ch0, const int &bd_ch0, const int &tv_fft_samp_num0, const int &bd_fft_samp_num0,
			const int &tv_milisec0, const int &bd_milisec0, const int &tv_samp_rate,
			const std::string &tv_file_name0, const std::string &bd_file_name0, 
			const bool &bd_audio_only0);
		int load_ass(const std::string &ass_path0);//load ass file
		int match();//match ass lines
		int output(const std::string &output_path);//write and check results at specific address
		int output();//write and check results at auto address
		int get_nb_timeline();//return num of timeline
		int get_timeline(const int &line, const int &type);//return timeline info
		std::string get_feedback();//return timeline info
	protected:
		std::string cs2time(const int &cs0);
		int time2cs(const std::string &time);
		virtual int caldiff(const int tv_start, const int se_start, const int se_end, const int min_check_num,
			const int check_field, se_re *re);
		debug_info deb_info;//debug info in matching
		prog_func prog_single = nullptr;//func_ptr for progress bar
		std::atomic_flag * const keep_processing;//multithreading cancel token
		language_pack& lang_pack;//language pack
		long startclock = 0;//timing
		//ass info and data
		std::string ass_path;
		std::string tv_ass_text;
		std::string head, content;
		std::vector<timeline> timeline_list;
		long long nb_timeline = 0;
		//decode info
		node** tv_fft_data = nullptr, ** bd_fft_data = nullptr;
		int tv_ch = 0, bd_ch = 0;
		int tv_fft_samp_num = 0, bd_fft_samp_num = 0;
		int tv_milisec = 0, bd_milisec = 0;
		std::string tv_file_name, bd_file_name;
		bool bd_audio_only = false;
		//settings of matching
		int find_field = 10, min_check_num = 200, ass_offset = 0, max_length = 20;
		bool fast_match = false, debug_mode = false;
		//vars for searching
		bdsearch bd_se;
		std::vector<int> tv_time, bd_time;
		int ch = 2;
		int interval = 1; //search interval
		int	overlap_interval = 1; //interval to judge overlap
		int find_range = 10;
		int fft_size = 256;//size of single fft data
		int nb_threads = 0, nb_per_task = 0, nb_tasks = 0;//parameters of multithreading
		int rightshift = 0;
		int duration = 0;
		double t2f = 1.0;//Time to Frequency
		double f2t = 1.0;//Frequency to Time
		std::atomic<long long*> diffa = new long long[3];//multithreading flags
		se_re *search_result = nullptr;
		std::string feedback;
	};

	class Match_SSE : public Match {
	public:
		Match_SSE(language_pack& lang_pack0, std::atomic_flag* keep_processing0 = nullptr)
			:Match(lang_pack0, keep_processing0) {}
		int caldiff(const int tv_start, const int se_start, const int se_end, const int min_check_num,
			const int check_field, se_re *re);
	};

	class Match_AVX2 : public Match {
	public:
		Match_AVX2(language_pack& lang_pack0, std::atomic_flag* keep_processing0 = nullptr)
			:Match(lang_pack0, keep_processing0) {}
		int caldiff(const int tv_start, const int se_start, const int se_end, const int min_check_num,
			const int check_field, se_re *re);
	};
}