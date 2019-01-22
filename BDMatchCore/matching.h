#pragma once
#include "datastruct.h"
#include <atomic>
#include <array>
#include <vector>

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
		Match(std::atomic_flag *keep_processing0 = nullptr);
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
		prog_func prog_single;//func_ptr for progress bar
		std::atomic_flag * const keep_processing;//multithreading cancel token
		long startclock = 0;//timing
		//ass info and data
		std::string ass_path;
		std::string tv_ass_text;
		std::string head, content;
		std::vector<timeline> timeline_list;
		int nb_timeline;
		//decode info
		node ** tv_fft_data, ** bd_fft_data;
		int tv_ch = 0, bd_ch = 0;
		int tv_fft_samp_num, bd_fft_samp_num;
		int tv_milisec, bd_milisec;
		std::string tv_file_name, bd_file_name;
		bool bd_audio_only;
		//settings of matching
		int find_field, min_check_num, ass_offset, max_length;
		bool fast_match, debug_mode;
		//vars for searching
		bdsearch bd_se;
		std::vector<int> tv_time, bd_time;
		int ch;
		int interval; //search interval
		int	overlap_interval; //interval to judge overlap
		int find_range;
		int fft_size;//size of single fft data
		int nb_threads, nb_per_task, nb_tasks;//parameters of multithreading
		int rightshift;
		int duration;
		double t2f;//Time to Frequency
		double f2t;//Frequency to Time
		std::atomic<long long*> diffa = new long long[3];//multithreading flags
		se_re *search_result = nullptr;
		std::string feedback;
	};

	class Match_SSE : public Match {
	public:
		Match_SSE(std::atomic_flag *keep_processing0 = nullptr)
			:Match(keep_processing0) {}
		int caldiff(const int tv_start, const int se_start, const int se_end, const int min_check_num,
			const int check_field, se_re *re);
	};

	class Match_AVX2 : public Match {
	public:
		Match_AVX2(std::atomic_flag *keep_processing0 = nullptr)
			:Match(keep_processing0) {}
		int caldiff(const int tv_start, const int se_start, const int se_end, const int min_check_num,
			const int check_field, se_re *re);
	};
}