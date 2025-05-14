#include "headers/match.h"
#include "headers/multithreading.h"
#include <fstream>
#include <immintrin.h>
#include <cmath>

using namespace DataStruct;

namespace Match {
	inline static char m128i_i8(__m128i& vec, const int& index)
	{
#ifndef _CLI_ // Windows
		return vec.m128i_i8[index];
#else // Other platforms
		return reinterpret_cast<const char*>(&vec)[index];
#endif 
	}

	inline static int32_t m128i_i32(__m128i& vec, const int& index)
	{
#ifndef _CLI_ // Windows
		return vec.m128i_i32[index];
#else // Other platforms
		return reinterpret_cast<const int32_t*>(&vec)[index];
#endif 
	}

	inline static char m256i_i8(__m256i& vec, const int& index)
	{
#ifndef _CLI_ // Windows
		return vec.m256i_i8[index];
#else // Other platforms
		return reinterpret_cast<const char*>(&vec)[index];
#endif 
	}

	inline static int32_t m256i_i32(__m256i& vec, const int& index)
	{
#ifndef _CLI_ // Windows
		return vec.m256i_i32[index];
#else // Other platforms
		return reinterpret_cast<const int32_t*>(&vec)[index];
#endif 
	}
}

Match::Timeline::Timeline(const int64_t& start0, const int64_t& end0, const bool& iscom0,
	const std::string_view &head0, const std::string_view &text0)
{
	start_ = start0;
	end_ = end0;
	iscom_ = iscom0;
	head_ = head0;
	former_text_ = text0;
}
int64_t Match::Timeline::start() const
{
	return start_;
}
int64_t Match::Timeline::end() const
{
	return end_;
}
int64_t Match::Timeline::duration() const
{
	return end_ - start_;
}
bool Match::Timeline::iscom() const
{
	return iscom_;
}
std::string_view Match::Timeline::head()
{
	return head_;
}
std::string_view Match::Timeline::former_text()
{
	return former_text_;
}
int Match::Timeline::start(const int64_t&start0)
{
	start_ = start0;
	return 0;
}
int Match::Timeline::end(const int64_t&end0)
{
	end_ = end0;
	return 0;
}
int Match::Timeline::iscom(const bool &iscom0)
{
	iscom_ = iscom0;
	return 0;
}
int Match::Timeline::head(std::string_view &head0)
{
	head_ = head0;
	return 0;
}

Match::BDSearch::BDSearch()
{
}
int Match::BDSearch::allocate(const int& num)
{
	bd_items.assign(num, {});
	return 0;
}
int Match::BDSearch::assign(const size_t& pos, const int64_t& time, const int64_t& delta)
{
	bd_items[pos] = { delta, time };
	return 0;
}
int64_t Match::BDSearch::read(const size_t &pos) const
{
	return bd_items[pos][1];
}
int64_t Match::BDSearch::find(const int64_t &search_time, const int &retype) const
{
	int index = 0;
	for (auto &bd_item : bd_items) {
		if (bd_item[1] == search_time) {
			if (!retype)return index;
			else return bd_item[0];
		};
		index++;
	}
	if (index >= bd_items.size())index = -1;
	return index;
}
int Match::BDSearch::sort()
{
	std::sort(bd_items.begin(), bd_items.end(), [](const Se_Re &a, const Se_Re &b) {
		return a[0] < b[0];
	});
	return 0;
}
size_t Match::BDSearch::size() const
{
	return bd_items.size();
}
int Match::BDSearch::clear()
{
	bd_items.clear();
	return 0;
}


Match::Match::Match(const Language_Pack& lang_pack0, std::stop_source& stop_src0)
	:stop_src(stop_src0), lang_pack(lang_pack0) {
}

Match::Match::~Match()
{
}

int Match::Match::load_settings(const int & min_cnfrm_num0, const int & search_range0, const int &sub_offset0,
	const int &max_length0,
	const bool & fast_match0, const bool & debug_mode0, const prog_func &prog_single0)
{
	min_cnfrm_num = min_cnfrm_num0;
	serach_range = search_range0;
	sub_offset = sub_offset0;
	max_length = max_length0;
	fast_match = fast_match0;
	debug_mode = debug_mode0;
	prog_single = prog_single0;
	return 0;
}

int Match::Match::load_decode_info(Spec_Node ** const & tv_fft_data0, Spec_Node ** const & bd_fft_data0,
	const int & tv_ch0, const int & bd_ch0, const int64_t& tv_fft_samp_num0, const int64_t& bd_fft_samp_num0,
	const int64_t& tv_centi_sec0, const int64_t& bd_centi_sec0, const int & tv_samp_rate,
	const std::string_view & tv_file_name0, const std::string_view & bd_file_name0, 
	const bool & bd_audio_only0)
{
	tv_fft_data = tv_fft_data0;
	bd_fft_data = bd_fft_data0;
	fft_size = tv_fft_data[0][0].size();
	tv_fft_samp_num = tv_fft_samp_num0;
	bd_fft_samp_num = bd_fft_samp_num0;
	//conversion paras between time and fft data
	t2f = static_cast<double>(tv_samp_rate) / (static_cast<double>(fft_size) * 2.0 * 100.0);
	f2t = static_cast<double>(fft_size) * 2.0 * 100.0 / static_cast<double>(tv_samp_rate);
	//intervals
	interval = static_cast<int>(t2f);
	if (interval < 1)interval = 1;
	overlap_interval = static_cast<int>(ceil(t2f));
	//channels
	tv_ch = tv_ch0;
	bd_ch = bd_ch0;
	ch = std::min(tv_ch, bd_ch);
	//time of audio
	tv_centi_sec = tv_centi_sec0;
	bd_centi_sec = bd_centi_sec0;
	//file names
	tv_file_name = tv_file_name0;
	bd_file_name = bd_file_name0;
	//search parameters
	search_range_fft = static_cast<int64_t>(round(static_cast<double>(serach_range) * 100.0 * t2f));
	//multithreading parameters
	nb_sub_threads = std::thread::hardware_concurrency();
	nb_per_sub_task = std::max(NB_PER_SUB_TASK_MIN, std::min(NB_PER_SUB_TASK_MAX, search_range_fft / nb_sub_threads / interval));
	size_t search_cnt = (2LL * search_range_fft / interval) + (((2LL * search_range_fft) % interval) ? 1 : 0);
	nb_sub_tasks = search_cnt / nb_per_sub_task + ((search_cnt % nb_per_sub_task) ? 1 : 0);
	nb_batch_threads = std::thread::hardware_concurrency();
	//other info
	bd_audio_only = bd_audio_only0;
	return 0;
}

Match_Core_Return Match::Match::load_sub(const std::string_view &sub_path0)
{
	sub_path = sub_path0;
	std::string ext_name = sub_path.find_last_of('.') != std::string::npos ? 
		sub_path.substr(sub_path.find_last_of('.')).data() : "";
	std::transform(ext_name.begin(), ext_name.end(), ext_name.begin(),
		[](unsigned char c) { return std::tolower(c); });
	if (ext_name == ".srt") {
		sub_type = Sub_Type::SRT;
		return load_srt();
	}
	else {
		sub_type = Sub_Type::ASS;
		return load_ass();
	}
	return Match_Core_Return::Success;
}
std::pair<Match_Core_Return, size_t> Match::Match::open_sub()
{
	using std::ios, std::ifstream;
	feedback.clear();
	ifstream tv_sub_file(sub_path.data(), ios::binary | ios::ate);
	if (!tv_sub_file.is_open()) {
		feedback += str_vfmt(lang_pack.get_text(Lang_Type::Notif, 2), lang_pack.get_text(Lang_Type::Match_Sub_Error, 0));//"\n错误：读取字幕文件失败!"
		return { Match_Core_Return::Sub_Open_Err, 0 };
	}
	auto sub_file_size = tv_sub_file.tellg();
	tv_sub_text.resize(sub_file_size, '\0');
	tv_sub_file.seekg(0);
	tv_sub_file.read(tv_sub_text.data(), sub_file_size);
	tv_sub_file.close();
	size_t rn_pos = tv_sub_text.find("\r\n");
	while (rn_pos != std::string::npos) {
		if (stop_src.stop_requested())return { Match_Core_Return::User_Stop, 0 };
		tv_sub_text = tv_sub_text.replace(rn_pos, 2, "\n");
		rn_pos = tv_sub_text.find("\r\n", rn_pos);
	}
	return { Match_Core_Return::Success, sub_file_size };
}
Match_Core_Return Match::Match::load_srt()
{
	using std::regex;
	auto [open_sub_re, srt_file_size] = open_sub();
	if (open_sub_re != Match_Core_Return::Success)return open_sub_re;
	if (tv_sub_text.find(" --> ") == std::string::npos) {
		tv_sub_text.clear();
		feedback += str_vfmt(lang_pack.get_text(Lang_Type::Notif, 2), lang_pack.get_text(Lang_Type::Match_Sub_Error, 1));//"\n错误：输入字幕文件无效！"
		return Match_Core_Return::Sub_Illegal_Err;
	}
	head.clear();
	content = tv_sub_text;
	tv_sub_text.clear();
	// 00:00:39,770 --> 00:00:41,880
	regex timeline_regex(
		"\\n[0-9]{2}:[0-9]{2}:[0-9]{2},[0-9]{3} --> [0-9]{2}:[0-9]{2}:[0-9]{2},[0-9]{3}\\n");
	regex time_regex("[0-9]{2}:[0-9]{2}:[0-9]{2},[0-9]{3}");
	return decode_sub(srt_file_size, timeline_regex, time_regex, nullptr);
}
Match_Core_Return Match::Match::load_ass()
{
	using std::regex;
	auto [open_sub_re, ass_file_size] = open_sub();
	if (open_sub_re != Match_Core_Return::Success)return open_sub_re;
	size_t event_pos = tv_sub_text.find("\n[Events]\n");
	if (event_pos == std::string::npos) {
		tv_sub_text.clear();
		feedback += str_vfmt(lang_pack.get_text(Lang_Type::Notif, 2), lang_pack.get_text(Lang_Type::Match_Sub_Error, 1));//"\n错误：输入字幕文件无效！"
		return Match_Core_Return::Sub_Illegal_Err;
	}
	event_pos += 2;
	head = tv_sub_text.substr(0, event_pos);
	content = tv_sub_text.substr(event_pos);
	tv_sub_text.clear();
	regex regex_audio_file("Audio ((File)|(URI)): .*?\\n");
	regex regex_video_file("Video File: .*?\\n");
	head = regex_replace(head, regex_audio_file, "Audio File: " + bd_file_name + "\n");
	if (bd_audio_only)head = regex_replace(head, regex_video_file, "");
	else head = regex_replace(head, regex_video_file, "Video File: " + bd_file_name + "\n");
	//: 0,0:22:38.77,0:22:43.35
	regex timeline_regex(
		"\\n[a-zA-Z]+: [0-9],[0-9]:[0-9]{2}:[0-9]{2}\\.[0-9]{2},[0-9]:[0-9]{2}:[0-9]{2}\\.[0-9]{2},");
	regex header_regex("\\n[a-zA-Z]+: [0-9],");
	regex time_regex("[0-9]:[0-9]{2}:[0-9]{2}\\.[0-9]{2}");
	return decode_sub(ass_file_size, timeline_regex, time_regex, &header_regex);
}
Match_Core_Return Match::Match::decode_sub(const size_t& sub_file_size, const std::regex& timeline_regex, const std::regex& time_regex, const std::regex* header_regex_ptr)
{
	using std::regex, std::smatch, std::string;
	size_t file_size_coef = (sub_type == Sub_Type::ASS) ? 60 : 40;
	timeline_list.reserve(sub_file_size / file_size_coef);
	tv_time.reserve(sub_file_size / file_size_coef);
	bd_time.reserve(sub_file_size / file_size_coef);
	nb_timeline = 0;
	smatch timeline_match;
	string temp = content;
	string header, match, text;
	smatch header_match, time_match;
	while (regex_search(temp, timeline_match, timeline_regex)) {
		if (stop_src.stop_requested())return Match_Core_Return::User_Stop;
		match = timeline_match.str();
		text = match;
		temp = timeline_match.suffix();
		nb_timeline++;
		//header
		header = "";
		if (header_regex_ptr) {
			regex_search(match, header_match, *header_regex_ptr);
			header = header_match.str();
		}
		bool iscom = (sub_type == Sub_Type::ASS) ? (match.find("Comment") == string::npos ? false : true) : false;
		//time
		int time[2] = { 0,0 };
		for (int i = 0; i < 2; i++) {
			regex_search(match, time_match, time_regex);
			time[i] = time_to_cs(time_match.str());
			match = time_match.suffix();
			//cs to fft
			time[i] = static_cast<int>(round(static_cast<double>(time[i]) * t2f));
		}
		int start = time[0];
		int end = time[1];
		add_timeline(start, end, iscom, header, text);
	}
	return Match_Core_Return::Success;
}
int Match::Match::add_timeline(const int64_t& start, const int64_t& end, const bool& iscom, 
	const std::string_view& header, const std::string_view& text)
{
	timeline_list.emplace_back(start, end, iscom, header, text);
	auto emplace_ignored_line = [&tv_time = tv_time, &bd_time = bd_time, &timeline_list = timeline_list]() {
		tv_time.emplace_back(-1);
		bd_time.emplace_back(-1);
		timeline_list.back().start(-1);
		timeline_list.back().end(-1);
	};
	if (iscom) {
		emplace_ignored_line();
		feedback += str_vfmt(lang_pack.get_text(Lang_Type::Notif, 0), 
			str_vfmt(lang_pack.get_text(Lang_Type::Match_Sub_Info, 0), nb_timeline));//"\n信息：第***行为注释，将不作处理。"
		return -1;
	}
	if (end <= start) {
		emplace_ignored_line();
		feedback += str_vfmt(lang_pack.get_text(Lang_Type::Notif, 0), 
			str_vfmt(lang_pack.get_text(Lang_Type::Match_Sub_Info, 1), nb_timeline));//"\n信息：第***行时长为零，将不作处理。"
		return -1;
	}
	if (double(end) - double(start) > max_length * 100.0 * t2f) {
		emplace_ignored_line();
		feedback += str_vfmt(lang_pack.get_text(Lang_Type::Notif, 1), 
			str_vfmt(lang_pack.get_text(Lang_Type::Match_Sub_Warning, 0), nb_timeline));//"\n警告：第***行时长过长，将不作处理。"
		return -1;
	}
	if (end >= tv_fft_samp_num || (end - search_range_fft) > bd_fft_samp_num) {
		emplace_ignored_line();
		feedback += str_vfmt(lang_pack.get_text(Lang_Type::Notif, 1), 
			str_vfmt(lang_pack.get_text(Lang_Type::Match_Sub_Warning, 1), nb_timeline));//"\n警告：第***行超过音频长度，将不作处理。"
		return -1;
	}
	int max_db = -128;
	for (int64_t j = start; j <= end; j++) {
		int frame_max_db = cal_node_max_v(tv_fft_data[0][j]);
		if (frame_max_db > max_db)max_db = frame_max_db;
	}
	if (max_db <= -128) {
		emplace_ignored_line();
		feedback += str_vfmt(lang_pack.get_text(Lang_Type::Notif, 1), 
			str_vfmt(lang_pack.get_text(Lang_Type::Match_Sub_Warning, 2), nb_timeline));//"\n警告：第***行声音过小，将不作处理。"
		return -1;
	}
	bool existed = false;
	int64_t same_line;
	for (same_line = 0; same_line < nb_timeline - 1; same_line++) {
		if (start == tv_time[same_line] && end == timeline_list[same_line].end()) {
			existed = true;
			break;
		}
	}
	if (existed) {
		tv_time.emplace_back(-same_line - 2);
		bd_time.emplace_back(-same_line - 2);
		return -2;
	}
	tv_time.emplace_back(start);
	bd_time.emplace_back(0);
	return 0;
}

Match_Core_Return Match::Match::match()
{
	matched_line_cnt = 0;
	feedback.clear();
	if (fast_match) {
		feedback += str_vfmt(lang_pack.get_text(Lang_Type::Notif, 0), 
			lang_pack.get_text(Lang_Type::Match_Sub_Info, 2));//"\n信息：使用快速匹配。"
	}
	if (debug_mode) {
		deb_info = {};
		deb_info.lines.assign(nb_timeline, {});
	}
	// multithreading
	int64_t nb_per_batch_task = std::max(nb_timeline / nb_batch_threads, 1ULL);
	std::vector<std::function<void()>> batch_tasks;
	batch_tasks.reserve((nb_timeline / nb_per_batch_task + 1));
	fixed_thread_pool batch_pool(nb_batch_threads, stop_src);

	int64_t batch_start_line = 0;
	while (batch_start_line < nb_timeline) {
		batch_tasks.emplace_back([=]() { match_batch_lines(batch_start_line, std::min((batch_start_line + nb_per_batch_task), nb_timeline)); });
		batch_start_line += nb_per_batch_task;
	}
	batch_pool.execute_batch(batch_tasks);
	batch_pool.wait();
	batch_tasks.clear();

	if (stop_src.stop_requested()) {
		deb_info.lines.clear();
		return Match_Core_Return::User_Stop;
	}

	// output debug info
	if (debug_mode) {
		feedback += "\nDebug Info:\n     Line | MM Delta | Found Index | Task Index | Min Diff | Diffa[0]";
		for (int64_t i = 0; i < nb_timeline; i++) {
			const auto &deb_line = deb_info.lines[i];
			if (deb_line.found_index > -2) {
				deb_info.nb_line++;
				if (deb_line.delta_1 > deb_info.max_delta)deb_info.max_delta = deb_line.delta_1;
				feedback += std::format("\n    {:^6} | {:^12} | {:^10f}%  | {:^8f}%", i + 1, deb_line.delta_1, deb_line.found_index * 100.0, deb_line.task_index * 100.0);//"\n    i | minmax delta | found_index"
				if (deb_line.min_diff == deb_line.diffa_0)deb_info.diffa_consis++;
				else feedback += std::format(" | {:^10} | {:^9}", deb_line.min_diff, deb_line.diffa_0);//" | min_diff | diffa[0]"
				deb_info.ave_index += deb_line.found_index;
				deb_info.ave_task_index += deb_line.task_index;
				if (deb_line.found_index > deb_info.max_index && timeline_list[i].duration() > 75 * interval) {
					deb_info.max_index = deb_line.found_index;
					deb_info.max_line = i + 1;
				}
			}
		}
		
		deb_info.ave_index = deb_info.ave_index * 100.0 / static_cast<double>(deb_info.nb_line);
		deb_info.ave_task_index = deb_info.ave_task_index * 100.0 / static_cast<double>(deb_info.nb_line);
		deb_info.max_index = deb_info.max_index * 100.0;
		deb_info.diffa_consis = deb_info.diffa_consis * 100.0 / static_cast<double>(deb_info.nb_line);
		feedback += std::format("\n{1}{0}{2}\n{3}{0}{4}\n{5}{0}{6}", lang_pack.get_text(Lang_Type::General, 5),
			str_vfmt(lang_pack.get_text(Lang_Type::Match_Sub_Debug, 0), deb_info.ave_index),      // "\nAverage Found Index = ***%    "
			str_vfmt(lang_pack.get_text(Lang_Type::Match_Sub_Debug, 1), deb_info.ave_task_index), // "Average Task Index = ***%\n"
			str_vfmt(lang_pack.get_text(Lang_Type::Match_Sub_Debug, 2), deb_info.max_index),      // "Max Found Index = ***%    "
			str_vfmt(lang_pack.get_text(Lang_Type::Match_Sub_Debug, 3), deb_info.max_line),       // "Max Found Line = ***    "
			str_vfmt(lang_pack.get_text(Lang_Type::Match_Sub_Debug, 4), deb_info.max_delta),      // "Max MM Delta= ***\n"
			str_vfmt(lang_pack.get_text(Lang_Type::Match_Sub_Debug, 5), deb_info.diffa_consis)    // "Diffa Consistency = ***%"
		);
	}
	return Match_Core_Return::Success;
}

Match_Core_Return Match::Match::match_batch_lines(const int64_t& start_line, const int64_t& end_line)
{
	// Multithreading
	std::vector<std::function<void()>> tasks;
	tasks.reserve(nb_sub_tasks);
	fixed_thread_pool sub_task_pool(nb_sub_threads, stop_src);
	// search vars
	BDSearch bd_se;
	std::vector<Se_Re> search_result(nb_sub_tasks, { std::numeric_limits<int64_t>::max() , 0 });
	// Fast matching parameters
	int64_t offset = 0; int64_t five_sec = 0; int64_t last_line_time = 0;
	if (fast_match)five_sec = static_cast<int64_t>(500 * t2f);
	Diffa_t diffa;
	for (int64_t line_idx = start_line; line_idx < end_line; line_idx++) {
		if (tv_time[line_idx] >= 0) {
			if (fast_match && offset && last_line_time > five_sec && tv_time[line_idx - 1] > 0 && llabs(tv_time[line_idx] - last_line_time) < five_sec) {
				bd_time[line_idx] = tv_time[line_idx] + offset;
				last_line_time = tv_time[line_idx];
				sub_prog_back();
				continue;
			}
			int64_t find_start = static_cast<int64_t>(tv_time[line_idx] - search_range_fft);
			int64_t find_end = static_cast<int64_t>(tv_time[line_idx] + search_range_fft);
			int64_t duration = timeline_list[line_idx].duration();
			find_start = std::max(static_cast<int64_t>(0), find_start);
			find_end = static_cast<int64_t>(std::min(bd_fft_samp_num - duration - 1, find_end));
			int find_num = static_cast<int>((find_end - find_start) / interval);
			//初筛
			std::array<Samp_Info, TV_MAXIMUN_CNT + TV_MINIMUN_CNT> tv_samp_arr;
			const int64_t uniform_samp_interval = duration / (TV_MINIMUN_CNT - 1);
			int64_t tv_samp_sum = 0;
			for (int64_t j = 0, uniform_j = 0; j <= duration; j++) {
				int tv_line_sum_ch0 = get_node_sum(tv_fft_data[0][j + tv_time[line_idx]]);
				int tv_line_sum_max = tv_line_sum_ch0, tv_line_sum_max_ch = 0;
				int tv_line_sum_min = tv_line_sum_ch0, tv_line_sum_min_ch = 0;
				if (ch > 1) {
					int tv_line_sum_ch1 = get_node_sum(tv_fft_data[1][j + tv_time[line_idx]]);
					tv_line_sum_max = std::max(tv_line_sum_ch0, tv_line_sum_ch1);
					tv_line_sum_max_ch = tv_line_sum_ch0 < tv_line_sum_ch1 ? 1 : 0;
					tv_line_sum_min = std::min(tv_line_sum_ch0, tv_line_sum_ch1);
					tv_line_sum_min_ch = tv_line_sum_ch0 > tv_line_sum_ch1 ? 1 : 0;
				}
				if (j == 0)tv_samp_arr.fill({ tv_line_sum_min, tv_line_sum_min_ch, 0 });
				for (size_t k = 0; k < TV_MAXIMUN_CNT; k++) {
					if (tv_line_sum_max > tv_samp_arr[k].val || (j == 0 && k == 0)) {
						for (size_t m = TV_MAXIMUN_CNT - 1; m > k; m--)tv_samp_arr[m] = tv_samp_arr[m - 1];
						tv_samp_sum += tv_line_sum_max;
						tv_samp_arr[k] = { tv_line_sum_max, tv_line_sum_max_ch, j };
						break;
					}
				}
				for (size_t k = 0; k < TV_MINIMUN_CNT; k++) {
					if (tv_line_sum_min < tv_samp_arr[k + TV_MAXIMUN_CNT].val || (j == 0 && k == 0)) {
						for (size_t m = TV_MINIMUN_CNT + TV_MAXIMUN_CNT - 1; m > k + TV_MAXIMUN_CNT; m--)tv_samp_arr[m] = tv_samp_arr[m - 1];
						tv_samp_arr[k + TV_MAXIMUN_CNT] = { tv_line_sum_min, tv_line_sum_min_ch, j };
						break;
					}
				}
			}
			int32_t tv_samp_avg = static_cast<int32_t>(tv_samp_sum / tv_samp_arr.size());
			auto tv_samp_time_cmp = [](const Samp_Info& a, const Samp_Info& b) { return a.ch <= b.ch && a.time < b.time; };
			std::sort(tv_samp_arr.begin(), tv_samp_arr.end(), tv_samp_time_cmp);
			size_t nb_se_sub_tasks = find_num / NB_PER_SE_SUB_TASK;
			size_t nb_per_se_sub_task = find_num / nb_se_sub_tasks + ((find_num % nb_se_sub_tasks) ? 1 : 0);
			bd_se.allocate(find_num);
			for (size_t se_start = 0, bd_time_start = find_start; se_start < find_num; se_start += nb_per_se_sub_task, bd_time_start += interval * nb_per_se_sub_task) {
				tasks.emplace_back([=, &bd_se, &tv_samp_arr]() { cal_se_delta(bd_time_start, se_start, std::min(se_start + nb_per_se_sub_task, bd_se.size()), bd_se, tv_samp_avg, tv_samp_arr); });
			}
			sub_task_pool.execute_batch(tasks);
			sub_task_pool.wait();
			tasks.clear();
			if (stop_src.stop_requested())
				return Match_Core_Return::User_Stop;
			bd_se.sort();
			// Accurate match
			int min_cnfrm_num_aply = min_cnfrm_num;
			if (duration <= 75 * overlap_interval)min_cnfrm_num_aply = find_num;
			else if (fast_match)min_cnfrm_num_aply = min_cnfrm_num / 2 * 3;
			int64_t check_field = min_cnfrm_num_aply * interval;
			diffa = { std::numeric_limits<int64_t>::max(), 0, min_cnfrm_num_aply };
			for (size_t j = 0, se_start = 0; j < nb_sub_tasks; j++, se_start += nb_per_sub_task) {
				search_result[j] = { std::numeric_limits<int64_t>::max(), 0 };
				if (se_start < bd_se.size()) {
					tasks.emplace_back([=, &bd_se, &diffa, &search_result]() { cal_diff(tv_time[line_idx], se_start, std::min(se_start + nb_per_sub_task, bd_se.size()), duration,
						min_cnfrm_num_aply, check_field, bd_se, diffa, search_result[j]); });
				}
			}
			sub_task_pool.execute_batch(tasks);
			sub_task_pool.wait();
			tasks.clear();
			const auto& [min_diff, best_time] = *std::min_element(search_result.begin(), search_result.end(), [](const Se_Re& left, const Se_Re& right) { return left[0] < right[0]; });
			bd_time[line_idx] = best_time;

			// For debug->
			/*
			volatile std::string besttimestr = cs_to_time(best_time * f2t);
			volatile int bestfind = bd_se.find(best_time, 0);
			std::string time = "0:03:09.35";
			int fftindex = static_cast<int>(time_to_cs(time) * t2f);
			fftindex += 1;
			volatile int pos1 = bd_se.find(fftindex, 0);
			volatile int64_t lsfeedback = -1;
			volatile int task_id = static_cast<int>(ceil(pos1 / double(nb_per_sub_task)) - 1.0);
			if (pos1 > 0)lsfeedback = search_result[task_id][0];
			volatile std::string besttimestr2 = cs_to_time(search_result[task_id][1] * f2t);
			volatile int pos2 = bd_se.find(search_result[task_id][1], 0);
			*/
			// Cal debug info
			if (debug_mode) {
				auto& deb_line = deb_info.lines[line_idx];
				deb_line.delta_1 = bd_se.find(best_time, 1);
				deb_line.min_diff = min_diff;
				deb_line.diffa_0 = diffa[0];
				int64_t se_idx = bd_se.find(best_time, 0);
				deb_line.found_index = bd_se.find(best_time, 0) / static_cast<double>(find_num);
				deb_line.task_index = (se_idx % nb_per_sub_task) / static_cast<double>(nb_per_sub_task);
			}
			//
			if (fast_match) {
				offset = bd_time[line_idx] - tv_time[line_idx];
				last_line_time = tv_time[line_idx];
			}
		}
		if (stop_src.stop_requested())
			return Match_Core_Return::User_Stop;
		sub_prog_back();
	}
	search_result.clear();
	return Match_Core_Return::Success;
}

Match_Core_Return Match::Match::output(const std::string_view &output_path)
{
	using std::vector, std::string, std::fstream, std::ios;
	feedback.clear();
	//check feedbacks
	vector<int64_t>time_diff(nb_timeline);
	vector<int>check_feedbacks(nb_timeline);
	for (int64_t i = 0; i < nb_timeline; i++) {
		if (tv_time[i] >= 0)time_diff.emplace_back(bd_time[i] - tv_time[i]);
		else if (tv_time[i] == -1)time_diff[i] = 0;
		else time_diff[i] = time_diff[-static_cast<int64_t>(tv_time[i]) - 2];
	}
	for (int64_t i = 0; i < nb_timeline; i++) {
		if (tv_time[i] == -1) {
			check_feedbacks[i] = -1;
			continue;
		};
		if (tv_time[i] < 0) {
			int64_t line_num = -tv_time[i] - 2;
			int check_feedback = check_feedbacks[line_num];
			check_feedbacks[i] = check_feedback;
			if (check_feedback > 0)
				feedback += str_vfmt(lang_pack.get_text(Lang_Type::Notif, 1), 
					str_vfmt(lang_pack.get_text(Lang_Type::Match_Sub_Warning, 3), i + 1, line_num + 1));//"\n警告：第***行（与第***行时间相同）可能存在匹配错误!";
			continue;
		}
		int temp = 0;
		int check = 0;
		bool check2 = true;
		if (i > 0 && tv_time[i - 1] != -1) {
			temp = tv_time[i] - tv_time[i - 1] >= 0 ? 1 : -1;
			temp *= bd_time[i] - bd_time[i - 1] >= 0 ? 1 : -1;
			if (temp < 0) check = 1;
			if (llabs(time_diff[i] - time_diff[i - 1]) > ALLOWED_OFFSET * interval) check2 = false;
		}
		if (check == 0 && i < nb_timeline - 1 && tv_time[i + 1] != -1) {
			temp = tv_time[i + 1] - tv_time[i] >= 0 ? 1 : -1;
			temp *= bd_time[i + 1] - bd_time[i] >= 0 ? 1 : -1;
			if (temp < 0) check = 2;
			if (llabs(time_diff[i] - time_diff[i + 1]) > ALLOWED_OFFSET * interval && !check2) check = 3;
		}
		switch (check) {
		case 1:
			feedback += str_vfmt(lang_pack.get_text(Lang_Type::Notif, 1), 
				str_vfmt(lang_pack.get_text(Lang_Type::Match_Sub_Warning, 4), i + 1));//"\n警告：第***行可能存在匹配错误：与前一行次序不一致！"
			break;
		case 2:
			feedback += str_vfmt(lang_pack.get_text(Lang_Type::Notif, 1), 
				str_vfmt(lang_pack.get_text(Lang_Type::Match_Sub_Warning, 5), i + 1));//"\n警告：第***行可能存在匹配错误：与后一行次序不一致！"
			break;
		case 3:
			feedback += str_vfmt(lang_pack.get_text(Lang_Type::Notif, 1), 
				str_vfmt(lang_pack.get_text(Lang_Type::Match_Sub_Warning, 6), i + 1));//"\n警告：第***行可能存在匹配错误：与前后行时差不一致！"
			break;
		}
		check_feedbacks[i] = check;
	}
	//write subtitle
	for (int64_t i = 0; i < nb_timeline; i++) {
		if (tv_time[i] != -1) {
			if (tv_time[i] >= 0) {
				int64_t duration = timeline_list[i].duration();
				timeline_list[i].start(bd_time[i]);
				timeline_list[i].end(bd_time[i] + duration);
				if (i < nb_timeline - 1 && timeline_list[i].end() > bd_time[i + 1]
					&& (timeline_list[i].end() - bd_time[i + 1]) <= overlap_interval) {
					timeline_list[i].end(bd_time[i + 1]);
					feedback += str_vfmt(lang_pack.get_text(Lang_Type::Notif, 0), 
						str_vfmt(lang_pack.get_text(Lang_Type::Match_Sub_Info, 3), i + 1, i + 2));//"\n信息：第***行和第***行发生微小重叠，已自动修正。";
				}
			}
			else {
				timeline_list[i].start(timeline_list[-static_cast<int64_t>(tv_time[i]) - 2].start());
				timeline_list[i].end(timeline_list[-static_cast<int64_t>(tv_time[i]) - 2].end());
			}
			int start = static_cast<int>(round(static_cast<double>(timeline_list[i].start()) * f2t));
			int end = static_cast<int>(round(static_cast<double>(timeline_list[i].end()) * f2t));
			string replacetext;
			std::string_view former_text;
			switch (sub_type) {
			case Sub_Type::ASS:
				replacetext = std::format("{}{},{},", timeline_list[i].head(), cs_to_time(start), cs_to_time(end));
				break;
			case Sub_Type::SRT:
				replacetext = std::format("\n{} --> {}\n", cs_to_time(start), cs_to_time(end));
				break;
			default:
				break;
			}
			former_text = timeline_list[i].former_text();
			content = content.replace(content.find(former_text), former_text.size(), replacetext);
		}
		else {
			timeline_list[i].start(-1);
			timeline_list[i].end(-1);
		}
	}
	//write file
	fstream output_file(output_path.data(), ios::out | ios::trunc);
	if (!output_file.is_open()) {
		feedback += str_vfmt(lang_pack.get_text(Lang_Type::Notif, 2), lang_pack.get_text(Lang_Type::Match_Sub_Error, 2));//"\n错误：打开输出字幕文件失败!"
		return Match_Core_Return::Sub_Out_Open_Err;
	}
	if (!(output_file << head + content)) {
		feedback += str_vfmt(lang_pack.get_text(Lang_Type::Notif, 2), lang_pack.get_text(Lang_Type::Match_Sub_Error, 3));//"\n错误：写入字幕文件失败!"
		return Match_Core_Return::Sub_Write_Err;
	}
	if (prog_single)prog_single(Prog_Mode::Sub, 1.0);
	return Match_Core_Return::Success;
}
Match_Core_Return Match::Match::output()
{
	std::string output_path;
	std::string_view sub_filename = sub_path.substr(0, sub_path.find_last_of('.'));
	switch (sub_type) {
	case Sub_Type::ASS:
		output_path.reserve(sub_filename.size() + 13);
		output_path = std::format("{}{}", sub_filename, ".matched.ass");
		break;
	case Sub_Type::SRT:
		output_path.reserve(sub_filename.size() + 13);
		output_path = std::format("{}{}", sub_filename, ".matched.srt");
		break;
	default:
		output_path.reserve(sub_path.size() + 9);
		if (sub_path.find_last_of('.') != std::string::npos) {
			output_path = std::format("{}{}{}", sub_filename, ".matched",
				sub_path.substr(sub_path.find_last_of('.')));
		}
		else {
			output_path = std::format("{}{}", sub_filename, ".matched");
		}
		break;
	}
	return output(output_path);
}

int64_t Match::Match::get_nb_timeline() const
{
	return nb_timeline;
}
int64_t Match::Match::get_timeline(const size_t& line, const Timeline_Time_Type& type)
{
	switch (type) {
	case Timeline_Time_Type::Start_Time:
		return timeline_list[line].start();
		break;
	case Timeline_Time_Type::End_Time:
		return timeline_list[line].end();
		break;
	default:
		break;
	}
	return 0;
}
std::string_view Match::Match::get_feedback()
{
	return feedback;
}

std::string Match::Match::cs_to_time(const int &cs0)
{
	int hh, mm, ss, cs;
	cs = cs0;
	hh = cs / 360000;
	cs -= hh * 360000;
	mm = cs / 6000;
	cs -= mm * 6000;
	ss = cs / 100;
	cs -= ss * 100;
	std::string timeout;
	switch (sub_type) {
	case Sub_Type::ASS:
		timeout += std::to_string(hh) + ":";
		if (mm < 10)timeout += "0";
		timeout += std::to_string(mm) + ":";
		if (ss < 10)timeout += "0";
		timeout += std::to_string(ss) + ".";
		if (cs < 10)timeout += "0";
		timeout += std::to_string(cs);
		break;
	case Sub_Type::SRT:
		if (hh < 10)timeout += "0";
		timeout += std::to_string(hh) + ":";
		if (mm < 10)timeout += "0";
		timeout += std::to_string(mm) + ":";
		if (ss < 10)timeout += "0";
		timeout += std::to_string(ss) + ",";
		if (cs < 10)timeout += "0";
		timeout += std::to_string(cs) + "0";
		break;
	default:
		break;
	}
	return timeout;
}
int Match::Match::time_to_cs(const std::string_view &time) const
{
	auto sv_to_i = [](const std::string_view& sv) {
		int re = 0;
		for (auto c : sv)re = re * 10 + (c - '0');
		return re;
	};
	int cs = 0;
	switch (sub_type) {
	case Sub_Type::ASS:
		cs += sv_to_i(time.substr(0, 1)) * 360000;
		cs += sv_to_i(time.substr(2, 2)) * 6000;
		cs += sv_to_i(time.substr(5, 2)) * 100;
		cs += sv_to_i(time.substr(8, 2));
		break;
	case Sub_Type::SRT:
		cs += sv_to_i(time.substr(0, 2)) * 360000;
		cs += sv_to_i(time.substr(3, 2)) * 6000;
		cs += sv_to_i(time.substr(6, 2)) * 100;
		cs += sv_to_i(time.substr(9, 2));
		break;
	default:
		break;
	}
	cs += sub_offset;
	return cs;
}

void Match::Match::sub_prog_back()
{
	if (prog_single) {
		matched_line_cnt++;
		double temp = matched_line_cnt / static_cast<double>(nb_timeline);
		if (temp >= prog_val + MIN_AWARE_PROG_VAL || temp == 0) {
			prog_val = temp;
			prog_single(Prog_Mode::Sub, prog_val);
		}
	}
}

int Match::Match::get_node_sum(Spec_Node& node)
{
	int sum = node.get_sum();
	if (sum == std::numeric_limits<int>::min())sum = cal_node_sum(node);
	return sum;
}
char Match::Match::cal_node_max_v(Spec_Node& node)
{
	char* data = node.getdata();
	char max_v = *data;
	for (int i = 1; i < node.size(); i++)
		if (data[i] > max_v)max_v = data[i];
	return max_v;
}
int Match::Match::cal_node_sum(Spec_Node& node)
{
	char* data = node.getdata();
	int sum = 0;
	for (int i = 0; i < node.size(); i++)sum += data[i];
	node.set_sum(sum);
	return sum;
}

char Match::Match_SSE::cal_node_max_v(Spec_Node& node)
{
	char* data = node.getdata();
	__m128i vec = _mm_load_si128(reinterpret_cast<__m128i*>(data));
	for (int i = 1; i < node.size() / 16; i++) {
		data += 16;
		vec = _mm_max_epi8(vec, _mm_load_si128(reinterpret_cast<__m128i*>(data)));
	}
	char max_v = m128i_i8(vec, 0);
	for (int i = 1; i < 16; i++)max_v = std::max(max_v, m128i_i8(vec, 1));
	return max_v;
}
int Match::Match_SSE::cal_node_sum(Spec_Node& node)
{
	char* data = node.getdata();
	__m128i sum_vec = _mm_setzero_si128();
	for (int i = 0; i < node.size() / 16; i++) {
		__m128i vec = _mm_load_si128(reinterpret_cast<__m128i*>(data));
		__m128i temp16 = _mm_cvtepi8_epi16(vec);
		vec = _mm_bsrli_si128(vec, 64);
		temp16 = _mm_add_epi16(temp16, _mm_cvtepi8_epi16(vec));
		__m128i temp32 = _mm_cvtepi16_epi32(temp16);
		temp16 = _mm_bsrli_si128(temp16, 64);
		temp32 = _mm_add_epi32(temp32, _mm_cvtepi16_epi32(temp16));
		sum_vec = _mm_add_epi32(sum_vec, temp32);
		data += 16;
	}
	int sum = 0;
	for (int i = 0; i < 4; i++)sum += m128i_i32(sum_vec, i);
	node.set_sum(sum);
	return sum;
}

char Match::Match_AVX2::cal_node_max_v(Spec_Node& node)
{
	char* data = node.getdata();
	__m256i vec = _mm256_load_si256(reinterpret_cast<__m256i*>(data));
	for (int i = 1; i < node.size() / 32; i++) {
		data += 32;
		vec = _mm256_max_epi8(vec, _mm256_load_si256(reinterpret_cast<__m256i*>(data)));
	}
	char max_v = m256i_i8(vec, 0);
	for (int i = 1; i < 32; i++)max_v = std::max(max_v, m256i_i8(vec, i));
	return max_v;
}
int Match::Match_AVX2::cal_node_sum(Spec_Node& node)
{
	char* data = node.getdata();
	__m256i sum_vec = _mm256_setzero_si256();
	for (int i = 0; i < node.size() / 32; i++) {
		__m256i vec = _mm256_load_si256(reinterpret_cast<__m256i*>(data));
		__m128i temp8_1 = _mm256_extracti128_si256(vec, 0);
		__m128i temp8_2 = _mm256_extracti128_si256(vec, 1);
		__m256i temp16 = _mm256_add_epi16(_mm256_cvtepi8_epi16(temp8_1), _mm256_cvtepi8_epi16(temp8_2));
		__m128i temp16_1 = _mm256_extracti128_si256(temp16, 0);
		__m128i temp16_2 = _mm256_extracti128_si256(temp16, 1);
		__m256i temp32 = _mm256_add_epi32(_mm256_cvtepi16_epi32(temp16_1), _mm256_cvtepi16_epi32(temp16_2));
		sum_vec = _mm256_add_epi32(sum_vec, temp32);
		data += 32;
	}
	int sum = 0;
	for (int i = 0; i < 8; i++)sum += m256i_i32(sum_vec, i);
	node.set_sum(sum);
	return sum;
}

int Match::Match::cal_se_delta(const int64_t& bd_time_start, const size_t& se_start, const size_t& se_end, BDSearch& bd_se, const int32_t& tv_samp_avg, const std::array<Samp_Info, TV_MAXIMUN_CNT + TV_MINIMUN_CNT>& tv_samp_arr)
{
	int64_t bd_time = bd_time_start;
	std::array<int32_t, TV_MAXIMUN_CNT + TV_MINIMUN_CNT> bd_samp_arr;
	for (size_t se_index = se_start; se_index < se_end; se_index++) {
		int64_t delta = 0, bd_samp_sum = 0;
		for (int i = 0; i < tv_samp_arr.size(); i++) {
			int bd_samp_line_sum = get_node_sum(bd_fft_data[tv_samp_arr[i].ch][bd_time + tv_samp_arr[i].time]);
			bd_samp_arr[i] = bd_samp_line_sum;
			delta += std::abs(bd_samp_line_sum - static_cast<int32_t>(tv_samp_arr[i].val));
			bd_samp_sum += bd_samp_line_sum;
		}
		int32_t bd_samp_avg = static_cast<int32_t>(bd_samp_sum / tv_samp_arr.size());
		for (int i = 0; i < tv_samp_arr.size(); i++)
			delta += std::abs((tv_samp_arr[i].val - tv_samp_avg) - (bd_samp_arr[i] - bd_samp_avg)) * 8;
		bd_se.assign(se_index, bd_time, delta);
		bd_time += interval;
	}
	return 0;
}

int Match::Match::sync_match_res(Diffa_t& diffa, Se_Re& feedback, const int& min_cnfrm_num, const int64_t& bd_start, const int64_t& check_field, const int64_t& sum)
{
	if (sum < feedback[0])feedback = { sum, bd_start };
	if (feedback[0] < diffa[0])diffa = { feedback[0], feedback[1], min_cnfrm_num };
	else if (llabs(bd_start - diffa[1]) <= check_field)diffa[2]--;
	if (diffa[2] <= 0)return -1;
	return 0;
}

int Match::Match::cal_diff(const int64_t& tv_start, const size_t& se_start, const size_t& se_end, const int64_t& duration, const int& min_cnfrm_num,
	const int64_t& check_field, const BDSearch& bd_se, Diffa_t& diffa, Se_Re& re)
{
	if (diffa[2] <= 0)return -1;
	Se_Re feedback{ std::numeric_limits<int64_t>::max(), 0 };
	int64_t sum = 0;
	const char* tv_data[Decode::MAX_CHANNEL_CNT], * bd_data[Decode::MAX_CHANNEL_CNT];
	for (size_t se_index = se_start; se_index < se_end; se_index++) {
		int64_t bd_start = bd_se.read(se_index);
		sum = 0;
		for (int i = 0; i < ch; i++) {
			tv_data[i] = tv_fft_data[i][tv_start].getdata();
			bd_data[i] = bd_fft_data[i][bd_start].getdata();
		}
		for (int64_t i = 0; i <= duration; i++) {
			for (int j = 0; j < ch; j++) {
				for (int k = 0; k < fft_size; k++) {
					sum += llabs(static_cast<int64_t>(tv_data[j][k]) - static_cast<int64_t>(bd_data[j][k])) 
						* (static_cast<int64_t>(tv_data[j][k]) + 129LL);
				}
				tv_data[j] += fft_size;
				bd_data[j] += fft_size;
			}
			if (sum > diffa[0])break;
		}
		if (sync_match_res(diffa, feedback, min_cnfrm_num, bd_start, check_field, sum) < 0) {
			re = std::move(feedback);
			return -1;
		}
	}
	re = std::move(feedback);
	return 0;
}
int Match::Match_SSE::cal_diff(const int64_t& tv_start, const size_t& se_start, const size_t& se_end, const int64_t& duration, const int& min_cnfrm_num,
	const int64_t check_field, const BDSearch& bd_se, Diffa_t& diffa, Se_Re& re)
{
	if (diffa[2] <= 0)return -1;
	Se_Re feedback{ std::numeric_limits<int64_t>::max(), 0 };
	int64_t sum = 0;
	int vector_cnt = fft_size / 8;
	const char* tv_data[Decode::MAX_CHANNEL_CNT], * bd_data[Decode::MAX_CHANNEL_CNT];
	__m128i tv_vector, bd_vector, diff_vector[2];
	__m128i sum_vector[2] = { _mm_setzero_si128(),_mm_setzero_si128() };
	const __m128i w1_vector = _mm_set1_epi16(129);
	for (size_t se_index = se_start; se_index < se_end; se_index++) {
		int64_t bd_start = bd_se.read(se_index);
		sum = 0;
		for (int i = 0; i < ch; i++) {
			tv_data[i] = tv_fft_data[i][tv_start].getdata();
			bd_data[i] = bd_fft_data[i][bd_start].getdata();
		}
		for (int64_t i = 0; i <= duration; i++) {
			sum_vector[0] = _mm_setzero_si128();
			sum_vector[1] = _mm_setzero_si128();
			for (int j = 0; j < ch; j++) {
				for (int k = 0; k < vector_cnt; k++) {
					tv_vector = _mm_cvtepi8_epi16(_mm_load_si128(reinterpret_cast<const __m128i*>(tv_data[j])));
					bd_vector = _mm_cvtepi8_epi16(_mm_load_si128(reinterpret_cast<const __m128i*>(bd_data[j])));
					diff_vector[0] = _mm_mullo_epi16(_mm_abs_epi16(_mm_sub_epi16(tv_vector, bd_vector)),
						_mm_add_epi16(tv_vector, w1_vector));
					diff_vector[1] = _mm_mulhi_epi16(_mm_abs_epi16(_mm_sub_epi16(tv_vector, bd_vector)),
						_mm_add_epi16(tv_vector, w1_vector));
					sum_vector[0] = _mm_add_epi32(_mm_unpacklo_epi16(diff_vector[0], diff_vector[1]), sum_vector[0]);
					sum_vector[1] = _mm_add_epi32(_mm_unpackhi_epi16(diff_vector[0], diff_vector[1]), sum_vector[1]);
					tv_data[j] += 8;
					bd_data[j] += 8;
				}
			}
			sum_vector[0] = _mm_add_epi32(sum_vector[0], sum_vector[1]);
			sum_vector[0] = _mm_add_epi32(_mm_srli_epi64(sum_vector[0], 32), sum_vector[0]);
			sum += static_cast<int64_t>(_mm_extract_epi32(sum_vector[0], 0)) + 
				static_cast<int64_t>(_mm_extract_epi32(sum_vector[0], 2));
			if (sum > diffa[0])break;
		}
		if (sync_match_res(diffa, feedback, min_cnfrm_num, bd_start, check_field, sum) < 0) {
			re = std::move(feedback);
			return -1;
		}
	}
	re = std::move(feedback);
	return 0;
}
int Match::Match_AVX2::cal_diff(const int64_t& tv_start, const size_t& se_start, const size_t& se_end, const int64_t& duration, const int& min_cnfrm_num,
	const int64_t& check_field, const BDSearch& bd_se, Diffa_t& diffa, Se_Re& re)
{
	if (diffa[2] <= 0)return -1;
	Se_Re feedback{ std::numeric_limits<int64_t>::max(), 0 };
	int64_t sum = 0;
	int vector_cnt = fft_size / 32;
	const __m256i* tv_data[Decode::MAX_CHANNEL_CNT], * bd_data[Decode::MAX_CHANNEL_CNT];
	__m256i tv_vector[2], bd_vector[2], diff_vector[4];
	__m256i sum_vector[4] = { _mm256_setzero_si256(), _mm256_setzero_si256(), _mm256_setzero_si256(), _mm256_setzero_si256() };
	__m128i sum_vector_128[4], tv_data_128[2], bd_data_128[2];
	const __m256i w1_vector = _mm256_set1_epi16(129);
	for (size_t se_index = se_start; se_index < se_end; se_index++) {
		int64_t bd_start = bd_se.read(se_index);
		sum = 0;
		for (int i = 0; i < ch; i++) {
			tv_data[i] = reinterpret_cast<__m256i*>(tv_fft_data[i][tv_start].getdata());
			bd_data[i] = reinterpret_cast<__m256i*>(bd_fft_data[i][bd_start].getdata());
		}
		for (int64_t i = 0; i <= duration; i++) {
			sum_vector[0] = _mm256_setzero_si256();
			sum_vector[1] = _mm256_setzero_si256();
			sum_vector[2] = _mm256_setzero_si256();
			sum_vector[3] = _mm256_setzero_si256();
			for (int j = 0; j < ch; j++) {
				for (int k = 0; k < vector_cnt; k++) {
					tv_vector[0] = _mm256_load_si256(tv_data[j]);
					bd_vector[0] = _mm256_load_si256(bd_data[j]);
					tv_data_128[0] = _mm256_extracti128_si256(tv_vector[0], 0);
					tv_data_128[1] = _mm256_extracti128_si256(tv_vector[0], 1);
					bd_data_128[0] = _mm256_extracti128_si256(bd_vector[0], 0);
					bd_data_128[1] = _mm256_extracti128_si256(bd_vector[0], 1);
					tv_vector[0] = _mm256_cvtepi8_epi16(tv_data_128[0]);
					tv_vector[1] = _mm256_cvtepi8_epi16(tv_data_128[1]);
					bd_vector[0] = _mm256_cvtepi8_epi16(bd_data_128[0]);
					bd_vector[1] = _mm256_cvtepi8_epi16(bd_data_128[1]);
					diff_vector[0] = _mm256_mullo_epi16(_mm256_abs_epi16(_mm256_sub_epi16(tv_vector[0], bd_vector[0])),
						_mm256_add_epi16(tv_vector[0], w1_vector));
					diff_vector[1] = _mm256_mulhi_epi16(_mm256_abs_epi16(_mm256_sub_epi16(tv_vector[0], bd_vector[0])),
						_mm256_add_epi16(tv_vector[0], w1_vector));
					diff_vector[2] = _mm256_mullo_epi16(_mm256_abs_epi16(_mm256_sub_epi16(tv_vector[1], bd_vector[1])),
						_mm256_add_epi16(tv_vector[1], w1_vector));
					diff_vector[3] = _mm256_mulhi_epi16(_mm256_abs_epi16(_mm256_sub_epi16(tv_vector[1], bd_vector[1])),
						_mm256_add_epi16(tv_vector[1], w1_vector));
					sum_vector[0] = _mm256_add_epi32(_mm256_unpacklo_epi16(diff_vector[0], diff_vector[1]), sum_vector[0]);
					sum_vector[1] = _mm256_add_epi32(_mm256_unpackhi_epi16(diff_vector[0], diff_vector[1]), sum_vector[1]);
					sum_vector[2] = _mm256_add_epi32(_mm256_unpacklo_epi16(diff_vector[2], diff_vector[3]), sum_vector[2]);
					sum_vector[3] = _mm256_add_epi32(_mm256_unpackhi_epi16(diff_vector[2], diff_vector[3]), sum_vector[3]);
					tv_data[j]++;
					bd_data[j]++;
				}
			}
			sum_vector[0] = _mm256_add_epi32(sum_vector[0], sum_vector[1]);
			sum_vector[0] = _mm256_add_epi32(_mm256_srli_epi64(sum_vector[0], 32), sum_vector[0]);
			sum_vector[2] = _mm256_add_epi32(sum_vector[2], sum_vector[3]);
			sum_vector[2] = _mm256_add_epi32(_mm256_srli_epi64(sum_vector[2], 32), sum_vector[2]);
			sum_vector_128[0] = _mm256_extracti128_si256(sum_vector[0], 0);
			sum_vector_128[1] = _mm256_extracti128_si256(sum_vector[0], 1);
			sum_vector_128[2] = _mm256_extracti128_si256(sum_vector[2], 0);
			sum_vector_128[3] = _mm256_extracti128_si256(sum_vector[2], 1);
			sum_vector[0] = _mm256_cvtepi32_epi64(sum_vector_128[0]);
			sum_vector[1] = _mm256_cvtepi32_epi64(sum_vector_128[1]);
			sum_vector[2] = _mm256_cvtepi32_epi64(sum_vector_128[2]);
			sum_vector[3] = _mm256_cvtepi32_epi64(sum_vector_128[3]);
			sum_vector[0] = _mm256_add_epi64(sum_vector[0], sum_vector[1]);
			sum_vector[2] = _mm256_add_epi64(sum_vector[2], sum_vector[3]);
			sum_vector[0] = _mm256_add_epi64(sum_vector[0], sum_vector[2]);
			sum += _mm256_extract_epi64(sum_vector[0], 0) + _mm256_extract_epi64(sum_vector[0], 2);
			if (sum > diffa[0])break;
		}
		if (sync_match_res(diffa, feedback, min_cnfrm_num, bd_start, check_field, sum) < 0) {
			re = std::move(feedback);
			return -1;
		}
	}
	re = std::move(feedback);
	return 0;
}
