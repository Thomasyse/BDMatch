#pragma once
#include "datastruct.h"
#include <vector>
#include <array>
#include <memory>
#include <regex>
#include <stop_token>
#include "language_pack.h"

namespace Match {

	using namespace DataStruct;

	enum class Sub_Type { ASS, SRT };

	constexpr size_t NB_PER_SUB_TASK_MIN = 50;
	constexpr size_t NB_PER_SUB_TASK_MAX = 100;
	constexpr size_t NB_PER_SE_SUB_TASK = 256;
	constexpr size_t TV_MAXIMUN_CNT = 10;
	constexpr size_t TV_MINIMUN_CNT = 14;
	constexpr int ALLOWED_OFFSET = 2;

	class Timeline
	{
	public:
		Timeline(const int64_t& start0, const int64_t& end0, const bool& iscom0,
			const std::string_view &head0, const std::string_view &text0);
		int64_t start() const;
		int64_t end() const;
		int64_t duration() const;
		bool iscom() const;
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

	struct Samp_Info {
		int32_t val, ch;
		int64_t time;
	};

	typedef std::array<int64_t, 2> Se_Re; // [0]: diff, [1]: time
	typedef std::array<int64_t, 3> Diffa_t; // No need to make atomic

	class BDSearch
	{
	public:
		BDSearch();
		int allocate(const int& num);
		int assign(const size_t& pos, const int64_t& time, const int64_t& delta);
		int64_t read(const size_t& pos) const;
		int64_t find(const int64_t& search_time, const int& retype) const;
		int sort();
		size_t size() const;
		int clear();
	private:
		std::vector<Se_Re> bd_items;
	};

	struct Debug_Info_Line {
		int64_t delta_1 = -1, min_diff = 0, diffa_0 = 0;
		double found_index = -5, task_index = -5;
	};

	struct Debug_Info {//debug info in matching
		double ave_index = 0.0, ave_task_index = 0.0, max_index = 0.0, diffa_consis = 0.0;
		int64_t max_delta = 0, max_line = 0, nb_line = 0;
		std::vector<Debug_Info_Line> lines;
	};

	class Match {
	public:
		Match(const Language_Pack& lang_pack0, std::stop_source& stop_src0);
		virtual ~Match();
		int load_settings(const int &min_cnfrm_num, const int &search_range, const int &sub_offset, 
			const int &max_length,
			const bool &fast_match, const bool &debug_mode0, const prog_func &prog_single0 = nullptr);
		int load_decode_info(Spec_Node** const& tv_fft_data0, Spec_Node** const& bd_fft_data0,
			const int& tv_ch0, const int& bd_ch0, const int64_t& tv_fft_samp_num0, const int64_t& bd_fft_samp_num0,
			const int64_t& tv_centi_sec0, const int64_t& bd_centi_sec0, const int& tv_samp_rate,
			const std::string_view& tv_file_name0, const std::string_view& bd_file_name0,
			const bool& bd_audio_only0);
		Match_Core_Return load_sub(const std::string_view &sub_path0); // load subtitle file
		Match_Core_Return match(); // match all sub lines
		Match_Core_Return output(const std::string_view &output_path); // write and check results at specific address
		Match_Core_Return output(); // write and check results at auto address
		int64_t get_nb_timeline() const; // return num of timeline
		int64_t get_timeline(const size_t& line, const Timeline_Time_Type& type);//return timeline info
		std::string_view get_feedback(); // return timeline info
	protected:
		Match_Core_Return match_batch_lines(const int64_t& start_line, const int64_t& end_line); // match a batch of sub lines
		std::pair<Match_Core_Return, size_t> open_sub(); // open sub file
		Match_Core_Return load_srt(); // load srt file
		Match_Core_Return load_ass(); // load ass file
		Match_Core_Return decode_sub(const size_t& sub_file_size, const std::regex& timeline_regex, const std::regex& time_regex,
			const std::regex* header_regex); // decode sub text to timelines
		int add_timeline(const int64_t& start, const int64_t& end, const bool& iscom,
			const std::string_view& header, const std::string_view& text); // add and check timeline
		std::string cs_to_time(const int &cs0);
		int time_to_cs(const std::string_view &time) const;
		void sub_prog_back();
		int get_node_sum(Spec_Node& node);
		virtual char cal_node_max_v(Spec_Node &node);
		virtual int cal_node_sum(Spec_Node& node);
		int cal_se_delta(const int64_t& bd_time_start, const size_t& se_start, const size_t& se_cnt, BDSearch& bd_se, const int32_t& tv_samp_avg, const std::array<Samp_Info, TV_MAXIMUN_CNT + TV_MINIMUN_CNT>& tv_samp_arr);
		int sync_match_res(Diffa_t& diffa, Se_Re& feedback, const int& min_cnfrm_num, const int64_t& bd_start, const int64_t& check_field, const int64_t& sum);
		virtual int cal_diff(const int64_t& tv_start, const size_t& se_start, const size_t& se_end, const int64_t& duration, const int& min_cnfrm_num,
			const int64_t& check_field, const BDSearch& bd_se, Diffa_t& diffa, Se_Re& re);
		Debug_Info deb_info; // debug info in matching
		prog_func prog_single = nullptr; // func_ptr for progress bar
		std::stop_source &stop_src; // multithreading cancel source
		const Language_Pack& lang_pack; // language pack
		// sub info and data
		Sub_Type sub_type = Sub_Type::ASS;
		std::string_view sub_path;
		std::string tv_sub_text;
		std::string head, content;
		std::vector<Timeline> timeline_list;
		int64_t nb_timeline = 0;
		// decode info
		Spec_Node** tv_fft_data = nullptr, ** bd_fft_data = nullptr;
		int tv_ch = 0, bd_ch = 0;
		int64_t tv_fft_samp_num = 0, bd_fft_samp_num = 0;
		int64_t tv_centi_sec = 0, bd_centi_sec = 0;
		std::string tv_file_name, bd_file_name;
		bool bd_audio_only = false;
		// settings of matching
		int serach_range = 10, min_cnfrm_num = 200, sub_offset = 0, max_length = 20;
		bool fast_match = false, debug_mode = false;
		std::vector<int64_t> tv_time, bd_time;
		int ch = 0; // num of audio channels
		int64_t interval = 1; // search interval
		int64_t	overlap_interval = 1; // interval to judge overlap
		int64_t search_range_fft = 10;
		int fft_size = 256; // size of single fft data
		size_t nb_sub_threads = 0, nb_per_sub_task = 0, nb_sub_tasks = 0; // parameters of multithreading
		size_t nb_batch_threads = 0; // parameters of multithreading
		std::atomic<int64_t> matched_line_cnt = 0;
		volatile double prog_val = 0.0;
		double t2f = 1.0; // Time to Frequency
		double f2t = 1.0; // Frequency to Time
		std::string feedback;
	};

	class Match_SSE : public Match {
	public:
		Match_SSE(const Language_Pack& lang_pack0, std::stop_source& stop_src0)
			:Match(lang_pack0, stop_src0) {}
		char cal_node_max_v(Spec_Node& node);
		int cal_node_sum(Spec_Node& node);
		int cal_diff(const int64_t& tv_start, const size_t& se_start, const size_t& se_end, const int64_t& duration, const int& min_cnfrm_num,
			const int64_t check_field, const BDSearch& bd_se, Diffa_t& diffa, Se_Re& re);
	};

	class Match_AVX2 : public Match {
	public:
		Match_AVX2(const Language_Pack& lang_pack0, std::stop_source& stop_src0)
			:Match(lang_pack0, stop_src0) {}
		char cal_node_max_v(Spec_Node& node);
		int cal_node_sum(Spec_Node& node);
		int cal_diff(const int64_t& tv_start, const size_t& se_start, const size_t& se_end, const int64_t& duration, const int& min_cnfrm_num,
			const int64_t& check_field, const BDSearch& bd_se, Diffa_t& diffa, Se_Re& re);
	};
}