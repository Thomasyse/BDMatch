#include "headers/match.h"
#include "headers/multithreading.h"
#include <fstream>
#include <regex> 
#include <immintrin.h>
#include <cmath>

constexpr int tvmax_num = 12;
constexpr int tvmin_num = 12;
constexpr int allowed_offset = 2;

using namespace DataStruct;
using std::min;
using std::max;

Match::Timeline::Timeline(const int64_t& start0, const int64_t& end0, const bool& iscom0,
	const std::string_view &head0, const std::string_view &text0)
{
	start_ = start0;
	end_ = end0;
	iscom_ = iscom0;
	head_ = head0;
	former_text_ = text0;
}
int64_t Match::Timeline::start()
{
	return start_;
}
int64_t Match::Timeline::end()
{
	return end_;
}
int64_t Match::Timeline::duration()
{
	return end_ - start_;
}
bool Match::Timeline::iscom()
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
int Match::BDSearch::reserve(const int &num)
{
	bd_items.reserve(num);
	return 0;
}
int Match::BDSearch::push(const int64_t& time, const int64_t& diff)
{
	bd_items.push_back({ diff, time });
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
	right_shift = static_cast<int>(log2(fft_size) + 1.0);//search parameter
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
	ch = min(tv_ch, bd_ch);
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
	nb_per_sub_task = std::max(50ULL, std::min(100ULL, search_range_fft / nb_sub_threads / interval));
	nb_sub_tasks = static_cast<size_t>(ceil(static_cast<double>(2LL * search_range_fft / interval) / static_cast<double>(nb_per_sub_task)));
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
Match_Core_Return Match::Match::load_srt()
{
	using std::ios, std::regex, std::ifstream, std::smatch, std::string;
	feedback.clear();
	ifstream tv_srt_file(sub_path.data(), ios::binary | ios::ate);
	if (!tv_srt_file.is_open()) {
		feedback += str_vfmt(lang_pack.get_text(Lang_Type::Notif, 2), lang_pack.get_text(Lang_Type::Match_Sub_Error, 0));//"\n错误：读取字幕文件失败!"
		return Match_Core_Return::Sub_Open_Err;
	}
	auto srt_file_size = tv_srt_file.tellg();
	tv_sub_text.resize(srt_file_size, '\0');
	tv_srt_file.seekg(0);
	tv_srt_file.read(&tv_sub_text[0], srt_file_size);
	tv_srt_file.close();
	while (tv_sub_text.find("\r\n") != string::npos)tv_sub_text = tv_sub_text.replace(tv_sub_text.find("\r\n"), 2, "\n");
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
	smatch timeline_match;
	string temp = content;
	timeline_list.reserve(srt_file_size / 40);
	tv_time.reserve(srt_file_size / 40);
	bd_time.reserve(srt_file_size / 40);
	nb_timeline = 0;
	while (regex_search(temp, timeline_match, timeline_regex)) {
		string match = timeline_match.str();
		string text = match;
		temp = timeline_match.suffix();
		nb_timeline++;
		smatch time_match;
		//time
		int time[2] = { 0,0 };
		for (int i = 0; i < 2; i++) {
			regex_search(match, time_match, time_regex);
			time[i] = time2cs(time_match.str());
			match = time_match.suffix();
			//cs to fft
			time[i] = static_cast<int>(round(static_cast<double>(time[i])* t2f));
		}
		int start = time[0];
		int end = time[1];
		add_timeline(start, end, false, "", text);//neither comment nor header
	}
	return Match_Core_Return::Success;
}
Match_Core_Return Match::Match::load_ass()
{
	using std::ios, std::regex, std::ifstream, std::smatch, std::string;
	feedback.clear();
	ifstream tv_ass_file(sub_path.data(), ios::binary | ios::ate);
	if (!tv_ass_file.is_open()) {
		feedback += str_vfmt(lang_pack.get_text(Lang_Type::Notif, 2), lang_pack.get_text(Lang_Type::Match_Sub_Error, 0));//"\n错误：读取字幕文件失败!"
		return Match_Core_Return::Sub_Open_Err;
	}
	auto ass_file_size = tv_ass_file.tellg();
	tv_sub_text.resize(ass_file_size, '\0');
	tv_ass_file.seekg(0);
	tv_ass_file.read(&tv_sub_text[0], ass_file_size);
	tv_ass_file.close();
	while (tv_sub_text.find("\r\n") != string::npos)tv_sub_text = tv_sub_text.replace(tv_sub_text.find("\r\n"), 2, "\n");
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
	smatch timeline_match;
	string temp = content;
	timeline_list.reserve(ass_file_size / 60);
	tv_time.reserve(ass_file_size / 60);
	bd_time.reserve(ass_file_size / 60);
	nb_timeline = 0;
	while (regex_search(temp, timeline_match, timeline_regex)) {
		string match = timeline_match.str();
		string text = match;
		temp = timeline_match.suffix();
		nb_timeline++;
		smatch header_match, time_match;
		//header
		regex_search(match, header_match, header_regex);
		string header = header_match.str();
		bool iscom = match.find("Comment") == string::npos ? false : true;
		//time
		int time[2] = { 0,0 };
		for (int i = 0; i < 2; i++) {
			regex_search(match, time_match, time_regex);
			time[i] = time2cs(time_match.str());
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
	if (iscom) {
		tv_time.emplace_back(-1);
		bd_time.emplace_back(-1);
		timeline_list.back().start(-1);
		timeline_list.back().end(-1);
		feedback += str_vfmt(lang_pack.get_text(Lang_Type::Notif, 0), 
			str_vfmt(lang_pack.get_text(Lang_Type::Match_Sub_Info, 0), nb_timeline));//"\n信息：第***行为注释，将不作处理。"
		return -1;
	}
	if (end <= start) {
		tv_time.emplace_back(-1);
		bd_time.emplace_back(-1);
		timeline_list.back().start(-1);
		timeline_list.back().end(-1);
		feedback += str_vfmt(lang_pack.get_text(Lang_Type::Notif, 0), 
			str_vfmt(lang_pack.get_text(Lang_Type::Match_Sub_Info, 1), nb_timeline));//"\n信息：第***行时长为零，将不作处理。"
		return -1;
	}
	if (double(end) - double(start) > max_length * 100.0 * t2f) {
		tv_time.emplace_back(-1);
		bd_time.emplace_back(-1);
		timeline_list.back().start(-1);
		timeline_list.back().end(-1);
		feedback += str_vfmt(lang_pack.get_text(Lang_Type::Notif, 1), 
			str_vfmt(lang_pack.get_text(Lang_Type::Match_Sub_Warning, 0), nb_timeline));//"\n警告：第***行时长过长，将不作处理。"
		return -1;
	}
	if (end >= tv_fft_samp_num || (end - search_range_fft) > bd_fft_samp_num) {
		tv_time.emplace_back(-1);
		bd_time.emplace_back(-1);
		timeline_list.back().start(-1);
		timeline_list.back().end(-1);
		feedback += str_vfmt(lang_pack.get_text(Lang_Type::Notif, 1), 
			str_vfmt(lang_pack.get_text(Lang_Type::Match_Sub_Warning, 1), nb_timeline));//"\n警告：第***行超过音频长度，将不作处理。"
		return -1;
	}
	int maxdb = -128;
	for (int64_t j = start; j <= end; j++) {
		if (tv_fft_data[0][j].maxv() > maxdb) {
			maxdb = tv_fft_data[0][j].maxv();
		}
	}
	if (maxdb <= -128) {
		tv_time.emplace_back(-1);
		bd_time.emplace_back(-1);
		timeline_list.back().start(-1);
		timeline_list.back().end(-1);
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
	std::vector<std::function<Match_Core_Return()>> batch_tasks;
	batch_tasks.reserve((nb_timeline / nb_per_batch_task + 1));
	fixed_thread_pool batch_pool(nb_batch_threads, stop_src);

	int64_t batch_start_line = 0;
	while (batch_start_line < nb_timeline) {
		batch_tasks.emplace_back(std::bind(&Match::match_batch_lines, this, batch_start_line, std::min((batch_start_line + nb_per_batch_task), nb_timeline)));
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
		for (int i = 0; i < nb_timeline; i++) {
			const auto &deb_line = deb_info.lines[i];
			if (deb_line.found_index > -2) {
				deb_info.nb_line++;
				if (deb_line.delta_1 > deb_info.max_delta)deb_info.max_delta = deb_line.delta_1;
				feedback += std::format("\n{}, {}", i + 1, deb_line.delta_1);//"\n i, delta"
				if (deb_line.min_diff == deb_line.diffa_0)deb_info.diffa_consis++;
				else feedback += std::format(", {}, {}", deb_line.min_diff, deb_line.diffa_0);//", min_diff, diffa[0]"
				deb_info.ave_index = deb_info.ave_index + deb_line.found_index;
				if (deb_line.found_index > deb_info.max_index && timeline_list[i].duration() > 75 * interval) {
					deb_info.max_index = deb_line.found_index;
					deb_info.max_line = i + 1;
				}
			}
		}
		
		deb_info.ave_index = deb_info.ave_index * 100.0 / static_cast<double>(deb_info.nb_line);
		deb_info.max_index = deb_info.max_index * 100.0;
		deb_info.diffa_consis = deb_info.diffa_consis * 100.0 / static_cast<double>(deb_info.nb_line);
		feedback += std::format("\n{1}{0}{2}\n{3}{0}{4}\n{5}", lang_pack.get_text(Lang_Type::General, 5),
			str_vfmt(lang_pack.get_text(Lang_Type::Match_Sub_Debug, 0), deb_info.ave_index),  // "\nAverage Found Index = ***%    "
			str_vfmt(lang_pack.get_text(Lang_Type::Match_Sub_Debug, 1), deb_info.max_index),  // "Max Found Index = ***%\n"
			str_vfmt(lang_pack.get_text(Lang_Type::Match_Sub_Debug, 2), deb_info.max_line),   // "Max Found Line = ***    "
			str_vfmt(lang_pack.get_text(Lang_Type::Match_Sub_Debug, 3), deb_info.max_delta),  // "Max Delta= ***\n"
			str_vfmt(lang_pack.get_text(Lang_Type::Match_Sub_Debug, 4), deb_info.diffa_consis)// "Diffa Consistency = ***%"
		);
	}
	return Match_Core_Return::Success;
}

Match_Core_Return Match::Match::match_batch_lines(const int64_t start_line, const int64_t end_line)
{
	// multithreading
	std::vector<std::function<int()>> tasks;
	tasks.reserve(nb_sub_tasks);
	fixed_thread_pool sub_task_pool(nb_sub_threads, stop_src);
	// search vars
	BDSearch bd_se;
	std::vector<Se_Re> search_result(nb_sub_tasks, { std::numeric_limits<int64_t>::max() , 0 });
	// fast matching parameters
	int64_t offset = 0; int64_t five_sec = 0; int64_t last_line_time = 0;
	if (fast_match)five_sec = static_cast<int64_t>(500 * t2f);
	std::array<int64_t, 3> diffa = { 0 }; // No need to make atomic
	for (int64_t line_idx = start_line; line_idx < end_line; line_idx++) {
		if (tv_time[line_idx] >= 0) {
			if (fast_match && offset && last_line_time > five_sec && tv_time[line_idx - 1] > 0 && llabs(tv_time[line_idx] - last_line_time) < five_sec) {
				bd_time[line_idx] = tv_time[line_idx] + offset;
				last_line_time = tv_time[line_idx];
				sub_prog_back();
				continue;
			}
			int64_t findstart = static_cast<int64_t>(tv_time[line_idx] - search_range_fft);
			int64_t findend = static_cast<int64_t>(tv_time[line_idx] + search_range_fft);
			int64_t duration = timeline_list[line_idx].duration();
			findstart = max(static_cast<int64_t>(0), findstart);
			findend = static_cast<int64_t>(min(bd_fft_samp_num - duration - 1, findend));
			int find_num = static_cast<int>((findend - findstart) / interval);
			//初筛
			int tvmax[tvmax_num]; int64_t tvmaxtime[tvmax_num];
			int tvmin[tvmin_num]; int64_t tvmintime[tvmin_num];
			for (auto& j : tvmax) j = -128 * fft_size;
			for (auto& j : tvmaxtime) j = 0;
			for (auto& j : tvmin) j = 128 * fft_size;
			for (auto& j : tvmintime) j = 0;
			for (int64_t j = 0; j <= duration; j++) {
				for (int k = 0; k < tvmax_num; k++) {
					if (tv_fft_data[0][j + tv_time[line_idx]].sum() > tvmax[k] || j == 0) {
						for (int m = tvmax_num - 1; m > k; m--) {
							tvmax[m] = tvmax[m - 1];
							tvmaxtime[m] = tvmaxtime[m - 1];
						}
						tvmax[k] = tv_fft_data[0][j + tv_time[line_idx]].sum();
						tvmaxtime[k] = j;
						break;
					}
				}
				for (int k = 0; k < tvmin_num; k++) {
					if (tv_fft_data[0][j + tv_time[line_idx]].sum() < tvmin[k] || j == 0) {
						for (int m = tvmin_num - 1; m > k; m--) {
							tvmin[m] = tvmin[m - 1];
							tvmintime[m] = tvmintime[m - 1];
						}
						tvmin[k] = tv_fft_data[0][j + tv_time[line_idx]].sum();
						tvmintime[k] = j;
						break;
					}
				}
			}
			bd_se.clear();
			for (int j = 0; j <= find_num; j++) {
				int64_t bdtimein = findstart + j * interval;
				int delta = 0;
				for (int k = 0; k < tvmax_num; k++) {
					delta += labs(bd_fft_data[0][bdtimein + tvmaxtime[k]].sum() - tvmax[k]);
				}
				for (int k = 0; k < tvmin_num; k++) {
					delta += labs(bd_fft_data[0][bdtimein + tvmintime[k]].sum() - tvmin[k]);
				}
				delta = delta >> right_shift;
				//bd_se.push(bdtimein, delta);
				if (delta < 600)bd_se.push(bdtimein, delta);
			}
			bd_se.sort();
			//accurate match
			diffa[0] = std::numeric_limits<int64_t>::max();
			diffa[1] = 0;
			int min_cnfrm_num_aply = min_cnfrm_num;
			if (duration <= 75 * overlap_interval)min_cnfrm_num_aply = find_num;
			else if (fast_match)min_cnfrm_num_aply = min_cnfrm_num / 2 * 3;
			int64_t check_field = min_cnfrm_num_aply * interval;
			diffa[2] = min_cnfrm_num_aply;
			for (size_t j = 0; j < nb_sub_tasks; j++) {
				size_t se_start = j * nb_per_sub_task;
				search_result[j][0] = std::numeric_limits<int64_t>::max();
				tasks.emplace_back(std::bind(&Match::caldiff, this, tv_time[line_idx], se_start, min(se_start + nb_per_sub_task, bd_se.size()), duration,
					min_cnfrm_num_aply, check_field, std::ref(bd_se), std::ref(diffa), std::ref(search_result[j])));
			}
			sub_task_pool.execute_batch(tasks);
			sub_task_pool.wait();
			tasks.clear();
			const auto& [min_diff, besttime] = *std::min_element(search_result.begin(), search_result.end(), [](const Se_Re& left, const Se_Re& right) { return left[0] < right[0]; });
			bd_time[line_idx] = besttime;

			//for debug->
			/*
			volatile std::string besttimestr = cs2time(besttime * f2t);
			volatile int bestfind = bd_se.find(besttime, 0);
			std::string time = "0:03:09.35";
			int fftindex = static_cast<int>(time2cs(time) * t2f);
			fftindex += 1;
			volatile int pos1 = bd_se.find(fftindex, 0);
			volatile int64_t lsfeedback = -1;
			volatile int task_id = static_cast<int>(ceil(pos1 / double(nb_per_sub_task)) - 1.0);
			if (pos1 > 0)lsfeedback = search_result[task_id][0];
			volatile std::string besttimestr2 = cs2time(search_result[task_id][1] * f2t);
			volatile int pos2 = bd_se.find(search_result[task_id][1], 0);
			*/
			//cal debug info
			if (debug_mode) {
				auto& deb_line = deb_info.lines[line_idx];
				deb_line.delta_1 = bd_se.find(besttime, 1);
				deb_line.min_diff = min_diff;
				deb_line.diffa_0 = diffa[0];
				deb_line.found_index = bd_se.find(besttime, 0) / (double)find_num;
			}
			//
			if (fast_match) {
				offset = bd_time[line_idx] - tv_time[line_idx];
				last_line_time = tv_time[line_idx];
			}
		}
		if (stop_src.stop_requested()) {
			search_result.clear();
			return Match_Core_Return::User_Stop;
		}
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
			if (llabs(time_diff[i] - time_diff[i - 1]) > allowed_offset * interval) check2 = false;
		}
		if (check == 0 && i < nb_timeline - 1 && tv_time[i + 1] != -1) {
			temp = tv_time[i + 1] - tv_time[i] >= 0 ? 1 : -1;
			temp *= bd_time[i + 1] - bd_time[i] >= 0 ? 1 : -1;
			if (temp < 0) check = 2;
			if (llabs(time_diff[i] - time_diff[i + 1]) > allowed_offset * interval && !check2) check = 3;
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
				replacetext = std::format("{}{},{},", timeline_list[i].head(), cs2time(start), cs2time(end));
				break;
			case Sub_Type::SRT:
				replacetext = std::format("\n{} --> {}\n", cs2time(start), cs2time(end));
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

int64_t Match::Match::get_nb_timeline()
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

std::string Match::Match::cs2time(const int &cs0)
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
int Match::Match::time2cs(const std::string_view &time)
{
	using std::stoi;
	int cs = 0;
	switch (sub_type) {
	case Sub_Type::ASS:
		cs += stoi(time.substr(0, 1).data()) * 360000;
		cs += stoi(time.substr(2, 2).data()) * 6000;
		cs += stoi(time.substr(5, 2).data()) * 100;
		cs += stoi(time.substr(8, 2).data());
		break;
	case Sub_Type::SRT:
		cs += stoi(time.substr(0, 2).data()) * 360000;
		cs += stoi(time.substr(3, 2).data()) * 6000;
		cs += stoi(time.substr(6, 2).data()) * 100;
		cs += stoi(time.substr(9, 2).data());
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

int Match::Match::caldiff(const int64_t tv_start, const size_t se_start, const size_t se_end, const int64_t duration, const int min_cnfrm_num, 
	const int64_t check_field, const BDSearch &bd_se, std::array<int64_t, 3> &diffa, Se_Re &re)
{
	Se_Re feedback{ std::numeric_limits<int64_t>::max(), 0 };
	if (diffa[2] <= 0) {
		re = std::move(feedback);
		return -1;
	}
	int64_t sum = 0;
	const char* tv_data[8], * bd_data[8];
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
		if (sum < feedback[0])
		{
			feedback[0] = sum;
			feedback[1] = bd_start;
		}
		if (feedback[0] < diffa[0]) {
			diffa[0] = feedback[0];
			diffa[1] = feedback[1];
			diffa[2] = min_cnfrm_num;
		}
		else if (llabs(bd_start - diffa[1]) <= check_field) diffa[2]--;
		if (diffa[2] <= 0) {
			re = std::move(feedback);
			return -1;
		}
	}
	re = std::move(feedback);
	return 0;
}
int Match::Match_SSE::caldiff(const int64_t tv_start, const size_t se_start, const size_t se_end, const int64_t duration, const int min_cnfrm_num,
	const int64_t check_field, const BDSearch &bd_se, std::array<int64_t, 3> &diffa, Se_Re &re)
{
	Se_Re feedback{ std::numeric_limits<int64_t>::max(), 0 };
	if (diffa[2] <= 0) {
		re = std::move(feedback);
		return -1;
	}
	int64_t sum = 0;
	int vectornum = fft_size / 8;
	const char* tvdata[8], * bddata[8];
	__m128i tvvector, bdvector, difvector[2];
	__m128i sumvector[2] = { _mm_setzero_si128(),_mm_setzero_si128() };
	const __m128i w1vector = _mm_set1_epi16(129);
	for (size_t se_index = se_start; se_index < se_end; se_index++) {
		int64_t bd_start = bd_se.read(se_index);
		sum = 0;
		for (int i = 0; i < ch; i++) {
			tvdata[i] = tv_fft_data[i][tv_start].getdata();
			bddata[i] = bd_fft_data[i][bd_start].getdata();
		}
		for (int64_t i = 0; i <= duration; i++) {
			sumvector[0] = _mm_setzero_si128();
			sumvector[1] = _mm_setzero_si128();
			for (int j = 0; j < ch; j++) {
				for (int k = 0; k < vectornum; k++) {
					tvvector = _mm_cvtepi8_epi16(_mm_load_si128(reinterpret_cast<const __m128i*>(tvdata[j])));
					bdvector = _mm_cvtepi8_epi16(_mm_load_si128(reinterpret_cast<const __m128i*>(bddata[j])));
					difvector[0] = _mm_mullo_epi16(_mm_abs_epi16(_mm_sub_epi16(tvvector, bdvector)),
						_mm_add_epi16(tvvector, w1vector));
					difvector[1] = _mm_mulhi_epi16(_mm_abs_epi16(_mm_sub_epi16(tvvector, bdvector)),
						_mm_add_epi16(tvvector, w1vector));
					sumvector[0] = _mm_add_epi32(_mm_unpacklo_epi16(difvector[0], difvector[1]), sumvector[0]);
					sumvector[1] = _mm_add_epi32(_mm_unpackhi_epi16(difvector[0], difvector[1]), sumvector[1]);
					tvdata[j] += 8;
					bddata[j] += 8;
				}
			}
			sumvector[0] = _mm_add_epi32(sumvector[0], sumvector[1]);
			sumvector[0] = _mm_add_epi32(_mm_srli_epi64(sumvector[0], 32), sumvector[0]);
			sum += static_cast<int64_t>(_mm_extract_epi32(sumvector[0], 0)) + 
				static_cast<int64_t>(_mm_extract_epi32(sumvector[0], 2));
			if (sum > diffa[0])break;
		}
		if (sum < feedback[0])
		{
			feedback[0] = sum;
			feedback[1] = bd_start;
		}
		if (feedback[0] < diffa[0]) {
			diffa[0] = feedback[0];
			diffa[1] = feedback[1];
			diffa[2] = min_cnfrm_num;
		}
		else if (llabs(bd_start - diffa[1]) <= check_field) diffa[2]--;
		if (diffa[2] <= 0) {
			re = std::move(feedback);
			return -1;
		}
	}
	re = std::move(feedback);
	return 0;
}
int Match::Match_AVX2::caldiff(const int64_t tv_start, const size_t se_start, const size_t se_end, const int64_t duration, const int min_cnfrm_num,
	const int64_t check_field, const BDSearch &bd_se, std::array<int64_t, 3> &diffa, Se_Re &re)
{
	Se_Re feedback{ std::numeric_limits<int64_t>::max(), 0 };
	if (diffa[2] <= 0) {
		re = std::move(feedback);
		return -1;
	}
	int64_t sum = 0;
	int vectornum = fft_size / 16;
	const __m128i* tvdata[8], * bddata[8];
	__m256i tvvector, bdvector, difvector[2];
	__m256i sumvector[2] = { _mm256_setzero_si256(),_mm256_setzero_si256() };
	__m128i sumvector8[2];
	const __m256i w1vector = _mm256_set1_epi16(129);
	for (size_t se_index = se_start; se_index < se_end; se_index++) {
		int64_t bd_start = bd_se.read(se_index);
		sum = 0;
		for (int i = 0; i < ch; i++) {
			tvdata[i] = reinterpret_cast<__m128i*>(tv_fft_data[i][tv_start].getdata());
			bddata[i] = reinterpret_cast<__m128i*>(bd_fft_data[i][bd_start].getdata());
		}
		for (int64_t i = 0; i <= duration; i++) {
			sumvector[0] = _mm256_setzero_si256();
			sumvector[1] = _mm256_setzero_si256();
			for (int j = 0; j < ch; j++) {
				for (int k = 0; k < vectornum; k++) {
					tvvector = _mm256_cvtepi8_epi16(_mm_load_si128(tvdata[j]));
					bdvector = _mm256_cvtepi8_epi16(_mm_load_si128(bddata[j]));
					difvector[0] = _mm256_mullo_epi16(_mm256_abs_epi16(_mm256_sub_epi16(tvvector, bdvector)),
						_mm256_add_epi16(tvvector, w1vector));
					difvector[1] = _mm256_mulhi_epi16(_mm256_abs_epi16(_mm256_sub_epi16(tvvector, bdvector)),
						_mm256_add_epi16(tvvector, w1vector));
					sumvector[0] = _mm256_add_epi32(_mm256_unpacklo_epi16(difvector[0], difvector[1]), sumvector[0]);
					sumvector[1] = _mm256_add_epi32(_mm256_unpackhi_epi16(difvector[0], difvector[1]), sumvector[1]);
					tvdata[j]++;
					bddata[j]++;
				}
			}
			sumvector[0] = _mm256_add_epi32(sumvector[0], sumvector[1]);
			sumvector[0] = _mm256_add_epi32(_mm256_srli_epi64(sumvector[0], 32), sumvector[0]);
			sumvector8[0] = _mm256_extracti128_si256(sumvector[0], 0);
			sumvector8[1] = _mm256_extracti128_si256(sumvector[0], 1);
			sumvector8[0] = _mm_add_epi32(sumvector8[1], sumvector8[0]);
			sumvector[0] = _mm256_cvtepi32_epi64(sumvector8[0]);
			sum += _mm256_extract_epi64(sumvector[0], 0) + _mm256_extract_epi64(sumvector[0], 2);
			if (sum > diffa[0])break;
		}
		if (sum < feedback[0])
		{
			feedback[0] = sum;
			feedback[1] = bd_start;
		}
		if (feedback[0] < diffa[0]) {
			diffa[0] = feedback[0];
			diffa[1] = feedback[1];
			diffa[2] = min_cnfrm_num;
		}
		else if (llabs(bd_start - diffa[1]) <= check_field) diffa[2]--;
		if (diffa[2] <= 0) {
			re = std::move(feedback);
			return -1;
		}
	}
	re = std::move(feedback);
	return 0;
}

