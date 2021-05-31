#pragma once
#include "datastruct.h"
#include <array>
#include <vector>
#include <memory>
#include <stop_token>
#include "language_pack.h"

namespace Match {

	using namespace DataStruct;

	typedef void (*prog_func)(int, double);

	enum class Sub_Type { ASS, SRT };

	class Timeline
	{
	public:
		Timeline(const int64_t& start0, const int64_t& end0, const bool& iscom0,
			const std::string_view &head0, const std::string_view &text0);
		int64_t start();
		int64_t end();
		int64_t duration();
		bool iscom();
		std::string_view head();
		std::string_view former_text();
		int start(const int64_t& start0);
		int end(const int64_t& end0);
		int iscom(const bool& iscom0);
		int head(std::string_view & head0);
	private:
		int64_t start_;
		int64_t end_;
		bool iscom_;
		std::string head_;
		std::string former_text_;
	};

	class BDSearch
	{
	public:
		BDSearch();
		int reserve(const int& num);
		int push(const int64_t& time, const int64_t& diff);
		int64_t read(const size_t& pos);
		int64_t find(const int& searchnum, const int& retype);
		int sort();
		size_t size();
		int clear();
	private:
		std::vector<std::array<int64_t, 2>>bditem;
	};

	class Se_Re
	{
	public:
		Se_Re();
		int64_t& operator[](const int &index);
		Se_Re(Se_Re &in);
		int init();
	private:
		int64_t data[2] = { std::numeric_limits<int64_t>::max(),0 };
	};

	struct Debug_Info {//debug info in matching
		double ave_index = 0.0, max_index = 0.0, diffa_consis = 0.0;
		int64_t max_delta = 0, max_line = 0, nb_line = 0;
	};

	class Match {
	public:
		Match(Language_Pack& lang_pack0, std::stop_source& stop_src0);
		virtual ~Match();
		int load_settings(const int &min_check_num0, const int &find_field0, const int &sub_offset0, 
			const int &max_length0,
			const bool &fast_match0, const bool &debug_mode0, const prog_func &prog_single0 = nullptr);
		int load_decode_info(Spec_Node** const& tv_fft_data0, Spec_Node** const& bd_fft_data0,
			const int& tv_ch0, const int& bd_ch0, const int64_t& tv_fft_samp_num0, const int64_t& bd_fft_samp_num0,
			const int64_t& tv_milisec0, const int64_t& bd_milisec0, const int& tv_samp_rate,
			const std::string_view& tv_file_name0, const std::string_view& bd_file_name0,
			const bool& bd_audio_only0);
		int load_sub(const std::string_view &sub_path0);//load subtitle file
		int match();//match ass lines
		int output(const std::string_view &output_path);//write and check results at specific address
		int output();//write and check results at auto address
		int64_t get_nb_timeline();//return num of timeline
		int64_t get_timeline(const int& line, const Timeline_Time_Type& type);//return timeline info
		std::string_view get_feedback();//return timeline info
	protected:
		int load_ass();//load ass file
		int load_srt();//load srt file
		int add_timeline(const int64_t& start, const int64_t& end, const bool& iscom,
			const std::string_view& header, const std::string_view& text);//add and check timeline
		std::string cs2time(const int &cs0);
		int time2cs(const std::string_view &time);
		virtual int caldiff(const int64_t tv_start, const size_t se_start, const size_t se_end, const int min_check_num,
			const int64_t check_field, Se_Re* re);
		Debug_Info deb_info;//debug info in matching
		prog_func prog_single = nullptr;//func_ptr for progress bar
		std::stop_source &stop_src;//multithreading cancel source
		Language_Pack& lang_pack;//language pack
		//sub info and data
		Sub_Type sub_type = Sub_Type::ASS;
		std::string_view sub_path;
		std::string tv_sub_text;
		std::string head, content;
		std::vector<Timeline> timeline_list;
		int64_t nb_timeline = 0;
		//decode info
		Spec_Node** tv_fft_data = nullptr, ** bd_fft_data = nullptr;
		int tv_ch = 0, bd_ch = 0;
		int64_t tv_fft_samp_num = 0, bd_fft_samp_num = 0;
		int64_t tv_milisec = 0, bd_milisec = 0;
		std::string tv_file_name, bd_file_name;
		bool bd_audio_only = false;
		//settings of matching
		int find_field = 10, min_check_num = 200, sub_offset = 0, max_length = 20;
		bool fast_match = false, debug_mode = false;
		//vars for searching
		BDSearch bd_se;
		std::vector<int64_t> tv_time, bd_time;
		int ch = 0; // num of audio channels
		int64_t interval = 1; //search interval
		int64_t	overlap_interval = 1; //interval to judge overlap
		int64_t find_range = 10;
		int fft_size = 256;//size of single fft data
		size_t nb_threads = 0, nb_per_task = 0, nb_tasks = 0;//parameters of multithreading
		int rightshift = 0;
		int64_t duration = 0;
		double t2f = 1.0;//Time to Frequency
		double f2t = 1.0;//Frequency to Time
		int64_t diffa[3] = { 0 };//No need to make atomic
		Se_Re *search_result = nullptr;
		std::string feedback;
	};

	class Match_SSE : public Match {
	public:
		Match_SSE(Language_Pack& lang_pack0, std::stop_source& stop_src0)
			:Match(lang_pack0, stop_src0) {}
		int caldiff(const int64_t tv_start, const size_t se_start, const size_t se_end, const int min_check_num,
			const int64_t check_field, Se_Re* re);
	};

	class Match_AVX2 : public Match {
	public:
		Match_AVX2(Language_Pack& lang_pack0, std::stop_source& stop_src0)
			:Match(lang_pack0, stop_src0) {}
		int caldiff(const int64_t tv_start, const size_t se_start, const size_t se_end, const int min_check_num,
			const int64_t check_field, Se_Re* re);
	};
}