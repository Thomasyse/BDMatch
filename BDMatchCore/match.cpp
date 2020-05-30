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

Match::timeline::timeline(const int64_t& start0, const int64_t& end0, const bool& iscom0,
	const std::string &head0, const std::string &text0)
{
	start_ = start0;
	end_ = end0;
	iscom_ = iscom0;
	head_ = head0;
	former_text_ = text0;
}
int64_t Match::timeline::start()
{
	return start_;
}
int64_t Match::timeline::end()
{
	return end_;
}
int64_t Match::timeline::duration()
{
	return end_ - start_;
}
bool Match::timeline::iscom()
{
	return iscom_;
}
std::string Match::timeline::head()
{
	return head_;
}
std::string Match::timeline::former_text()
{
	return former_text_;
}
int Match::timeline::start(const int64_t&start0)
{
	start_ = start0;
	return 0;
}
int Match::timeline::end(const int64_t&end0)
{
	end_ = end0;
	return 0;
}
int Match::timeline::iscom(const bool &iscom0)
{
	iscom_ = iscom0;
	return 0;
}
int Match::timeline::head(std::string &head0)
{
	head_ = head0;
	return 0;
}

Match::bdsearch::bdsearch()
{
}
int Match::bdsearch::reserve(const int &num)
{
	bditem.reserve(num);
	return 0;
}
int Match::bdsearch::push(const int64_t& time, const int64_t& diff)
{
	bditem.emplace_back(std::array<int64_t, 2>({ time, diff }));
	return 0;
}
int64_t Match::bdsearch::read(const size_t &pos)
{
	return bditem[pos][0];
}
int64_t Match::bdsearch::find(const int &searchnum, const int &retype)
{
	int index = 0;
	for (auto &i : bditem) {
		if (i[0] == searchnum) {
			if (!retype)return index;
			else return i[1];
		};
		index++;
	}
	if (index >= bditem.size())index = -1;
	return index;
}
int Match::bdsearch::sort()
{
	std::sort(bditem.begin(), bditem.end(), [](std::array<int64_t, 2> &a, std::array<int64_t, 2>&b) {
		return a[1] < b[1];
	});
	return 0;
}
size_t Match::bdsearch::size()
{
	return bditem.size();
}
int Match::bdsearch::clear()
{
	bditem.clear();
	return 0;
}


Match::se_re::se_re()
{
}
int64_t& Match::se_re::operator[](const int & index)
{
	return data[index];
}
Match::se_re::se_re(se_re &in)
{
	data[0] = in[0];
	data[1] = in[1];
}
int Match::se_re::init()
{
	data[0] = 922372036854775808;
	data[1] = 0;
	return 0;
}


Match::Match::Match(language_pack& lang_pack0, std::shared_ptr<std::atomic_flag> keep_processing0)
	:keep_processing(keep_processing0), lang_pack(lang_pack0) {
}

Match::Match::~Match()
{
	if (search_result)delete[] search_result;
	search_result = nullptr;
}

int Match::Match::load_settings(const int & min_check_num0, const int & find_field0, const int &sub_offset0,
	const int &max_length0,
	const bool & fast_match0, const bool & debug_mode0, const prog_func &prog_single0)
{
	min_check_num = min_check_num0;
	find_field = find_field0;
	sub_offset = sub_offset0;
	max_length = max_length0;
	fast_match = fast_match0;
	debug_mode = debug_mode0;
	prog_single = prog_single0;
	return 0;
}

int Match::Match::load_decode_info(node ** const & tv_fft_data0, node ** const & bd_fft_data0,
	const int & tv_ch0, const int & bd_ch0, const int64_t& tv_fft_samp_num0, const int64_t& bd_fft_samp_num0,
	const int64_t& tv_milisec0, const int64_t& bd_milisec0, const int & tv_samp_rate,
	const std::string & tv_file_name0, const std::string & bd_file_name0, 
	const bool & bd_audio_only0)
{
	tv_fft_data = tv_fft_data0;
	bd_fft_data = bd_fft_data0;
	fft_size = tv_fft_data[0][0].size();
	tv_fft_samp_num = tv_fft_samp_num0;
	bd_fft_samp_num = bd_fft_samp_num0;
	rightshift = static_cast<int>(log2(fft_size) + 1.0);//search parameter
	//conversion paras between time and fft data
	t2f = static_cast<double>(tv_samp_rate) / (static_cast<double>(fft_size)* 2.0 * 100.0);
	f2t = static_cast<double>(fft_size) * 2.0 * 100.0 / static_cast<double>(tv_samp_rate);
	//intervals
	interval = static_cast<int>(t2f);
	if (interval < 1) {
		interval = 1;
	}
	overlap_interval = static_cast<int>(ceil(t2f));
	//channels
	tv_ch = tv_ch0;
	bd_ch = bd_ch0;
	ch = min(tv_ch, bd_ch);
	//time of audio
	tv_milisec = tv_milisec0;
	bd_milisec = bd_milisec0;
	//file names
	tv_file_name = tv_file_name0;
	bd_file_name = bd_file_name0;
	//search parameters
	find_range = static_cast<int>(round(static_cast<double>(find_field) * 100.0 * t2f));
	//multithreading parameters
	nb_threads = std::thread::hardware_concurrency();
	nb_per_task = min(25ULL, 2ULL * find_range / nb_threads / interval);
	nb_tasks = static_cast<size_t>(ceil(static_cast<double>(2LL * find_range / interval) / static_cast<double>(nb_per_task)));
	//other info
	bd_audio_only = bd_audio_only0;
	return 0;
}

int Match::Match::load_sub(const std::string &sub_path0)
{
	sub_path = sub_path0;
	std::string ext_name = sub_path.find_last_of('.') != std::string::npos ? 
		sub_path.substr(sub_path.find_last_of('.')) : "";
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
	return 0;
}
int Match::Match::load_srt()
{
	using std::ios, std::regex, std::ifstream, std::smatch, std::string;
	feedback = "";
	ifstream tv_srt_file(sub_path, ios::binary | ios::ate);
	if (!tv_srt_file.is_open()) {
		feedback += lang_pack.get_text(Lang_Type::Match_Sub, 0);//"\r\n读取字幕文件失败!"
		return -1;
	}
	auto srt_file_size = tv_srt_file.tellg();
	tv_sub_text.resize(srt_file_size, '\0');
	tv_srt_file.seekg(0);
	tv_srt_file.read(&tv_sub_text[0], srt_file_size);
	tv_srt_file.close();
	while (tv_sub_text.find("\r\n") != string::npos)tv_sub_text = tv_sub_text.replace(tv_sub_text.find("\r\n"), 2, "\n");
	if (tv_sub_text.find(" --> ") == std::string::npos) {
		tv_sub_text = "";
		feedback += lang_pack.get_text(Lang_Type::Match_Sub, 1);//"\r\n输入字幕文件无效！"
		return -1;
	}
	head = "";
	content = tv_sub_text;
	tv_sub_text = "";
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
	return 0;
}
int Match::Match::load_ass()
{
	using std::ios, std::regex, std::ifstream, std::smatch, std::string;
	feedback = "";
	ifstream tv_ass_file(sub_path, ios::binary | ios::ate);
	if (!tv_ass_file.is_open()) {
		feedback += lang_pack.get_text(Lang_Type::Match_Sub, 0);//"\r\n读取字幕文件失败!"
		return -1;
	}
	auto ass_file_size = tv_ass_file.tellg();
	tv_sub_text.resize(ass_file_size, '\0');
	tv_ass_file.seekg(0);
	tv_ass_file.read(&tv_sub_text[0], ass_file_size);
	tv_ass_file.close();
	while (tv_sub_text.find("\r\n") != string::npos)tv_sub_text = tv_sub_text.replace(tv_sub_text.find("\r\n"), 2, "\n");
	size_t event_pos = tv_sub_text.find("\n[Events]\n");
	if (event_pos == std::string::npos) {
		tv_sub_text = "";
		feedback += lang_pack.get_text(Lang_Type::Match_Sub, 1);//"\r\n输入字幕文件无效！"
		return -1;
	}
	event_pos += 2;
	head = tv_sub_text.substr(0, event_pos);
	content = tv_sub_text.substr(event_pos);
	tv_sub_text = "";
	regex regex_audio_file("Audio ((File)|(URI)): .*?\\n");
	regex regex_video_file("Video File: .*?\\n");
	head = regex_replace(head, regex_audio_file,
		"Audio File: " + bd_file_name + "\n");
	if (bd_audio_only)head = regex_replace(head, regex_video_file, "");
	else head = regex_replace(head, regex_video_file,
		"Video File: " + bd_file_name + "\n");
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
	return 0;
}
int Match::Match::add_timeline(const int64_t& start, const int64_t& end, const bool& iscom, const std::string& header, const std::string& text)
{
	timeline_list.emplace_back(timeline(start, end, iscom, header, text));
	if (iscom) {
		tv_time.emplace_back(-1);
		bd_time.emplace_back(-1);
		timeline_list.back().start(-1);
		timeline_list.back().end(-1);
		feedback += lang_pack.get_text(Lang_Type::Match_Sub, 2) + std::to_string(nb_timeline) +
			lang_pack.get_text(Lang_Type::Match_Sub, 3);//"\r\n信息：第***行为注释，将不作处理。"
		return -1;
	}
	if (end <= start) {
		tv_time.emplace_back(-1);
		bd_time.emplace_back(-1);
		timeline_list.back().start(-1);
		timeline_list.back().end(-1);
		feedback += lang_pack.get_text(Lang_Type::Match_Sub, 2) + std::to_string(nb_timeline) +
			lang_pack.get_text(Lang_Type::Match_Sub, 4);//"\r\n信息：第***行时长为零，将不作处理。"
		return -1;
	}
	if (double(end) - double(start) > max_length * 100.0 * t2f) {
		tv_time.emplace_back(-1);
		bd_time.emplace_back(-1);
		timeline_list.back().start(-1);
		timeline_list.back().end(-1);
		feedback += lang_pack.get_text(Lang_Type::Match_Sub, 5) + std::to_string(nb_timeline) +
			lang_pack.get_text(Lang_Type::Match_Sub, 6);//"\r\n警告：第***行时长过长，将不作处理。"
		return -1;
	}
	if (end >= tv_fft_samp_num || (end - find_range) > bd_fft_samp_num) {
		tv_time.emplace_back(-1);
		bd_time.emplace_back(-1);
		timeline_list.back().start(-1);
		timeline_list.back().end(-1);
		feedback += lang_pack.get_text(Lang_Type::Match_Sub, 5) + std::to_string(nb_timeline) +
			lang_pack.get_text(Lang_Type::Match_Sub, 7);//"\r\n警告：第***行超过音频长度，将不作处理。"
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
		feedback += lang_pack.get_text(Lang_Type::Match_Sub, 5) + std::to_string(nb_timeline) +
			lang_pack.get_text(Lang_Type::Match_Sub, 8);//"\r\n警告：第***行声音过小，将不作处理。"
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

int Match::Match::match()
{
	feedback = "";
	//multithreading
	std::vector<std::function<int()>> tasks;
	tasks.reserve(nb_tasks);
	fixed_thread_pool pool(nb_threads, keep_processing);
	//search vars
	search_result = new se_re[nb_tasks];
	//fast matching parameters
	int64_t offset = 0; int64_t fivesec = 0; int64_t lastlinetime = 0;
	if (fast_match) {
		fivesec = static_cast<int64_t>(500 * t2f);
		feedback += lang_pack.get_text(Lang_Type::Match_Sub, 9);//"\r\n信息：使用快速匹配。"
	}
	for (int64_t i = 0; i < nb_timeline; i++) {
		if (tv_time[i] >= 0) {
			if (fast_match && offset && lastlinetime > fivesec && tv_time[i - 1] > 0 && llabs(tv_time[i] - lastlinetime) < fivesec) {
				bd_time[i] = tv_time[i] + offset;
				lastlinetime = tv_time[i];
				continue;
			}
			int64_t findstart = static_cast<int64_t>(tv_time[i] - find_range);
			int64_t findend = static_cast<int64_t>(tv_time[i] + find_range);
			duration = timeline_list[i].duration();
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
					if (tv_fft_data[0][j + tv_time[i]].sum() > tvmax[k] || j == 0) {
						for (int m = tvmax_num - 1; m > k; m--) {
							tvmax[m] = tvmax[m - 1];
							tvmaxtime[m] = tvmaxtime[m - 1];
						}
						tvmax[k] = tv_fft_data[0][j + tv_time[i]].sum();
						tvmaxtime[k] = j;
						break;
					}
				}
				for (int k = 0; k < tvmin_num; k++) {
					if (tv_fft_data[0][j + tv_time[i]].sum() < tvmin[k] || j == 0) {
						for (int m = tvmin_num - 1; m > k; m--) {
							tvmin[m] = tvmin[m - 1];
							tvmintime[m] = tvmintime[m - 1];
						}
						tvmin[k] = tv_fft_data[0][j + tv_time[i]].sum();
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
				delta = delta >> rightshift;
				//bd_se.push(bdtimein, delta);
				if (delta < 600)bd_se.push(bdtimein, delta);
			}
			bd_se.sort();
			//accurate match
			diffa[0] = std::numeric_limits<int64_t>::max();
			diffa[1] = 0;
			int min_check_num_cal = min_check_num;
			if (duration <= 75 * overlap_interval)min_check_num_cal = find_num;
			else if (fast_match)min_check_num_cal = min_check_num / 2 * 3;
			int64_t check_field = min_check_num_cal * interval;
			diffa[2] = min_check_num_cal;
			se_re *se_re_ptr = search_result;
			for (size_t j = 0; j < nb_tasks; j++) {
				size_t se_start = j * nb_per_task;
				se_re_ptr->init();
				tasks.emplace_back(std::bind(&Match::caldiff, this, tv_time[i], se_start, min(se_start + nb_per_task, bd_se.size()),
					min_check_num_cal, check_field, se_re_ptr));
				se_re_ptr++;
			}
			pool.execute_batch(tasks);
			pool.wait();
			tasks.clear();
			int64_t minsum = std::numeric_limits<int64_t>::max();
			int besttime = 0;
			for (size_t j = 0; j < nb_tasks; j++) {
				int64_t sum = search_result[j][0];
				if (sum < minsum) {
					besttime = static_cast<int>(search_result[j][1]);
					minsum = sum;
				}
			}
			bd_time[i] = besttime;

			//for debug->
			/*
			volatile std::string besttimestr = cs2time(besttime * f2t);
			volatile int bestfind = bd_se.find(besttime, 0);
			std::string time = "0:03:09.35";
			int fftindex = static_cast<int>(time2cs(time) * t2f);
			fftindex += 1;
			volatile int pos1 = bd_se.find(fftindex, 0);
			volatile int64_t lsfeedback = -1;
			volatile int task_id = static_cast<int>(ceil(pos1 / double(nb_per_task)) - 1.0);
			if (pos1 > 0)lsfeedback = search_result[task_id][0];
			volatile std::string besttimestr2 = cs2time(search_result[task_id][1] * f2t);
			volatile int pos2 = bd_se.find(search_result[task_id][1], 0);
			*/
			//cal debug info
			if (debug_mode) {
				deb_info.nb_line++;
				int64_t delta1 = bd_se.find(besttime, 1);
				if (delta1 > deb_info.max_delta)deb_info.max_delta = delta1;
				feedback += lang_pack.get_text(Lang_Type::General, 0) + std::to_string(i + 1)
					+ lang_pack.get_text(Lang_Type::General, 4) + std::to_string(delta1);//"\r\n i, delta"
				if (minsum == diffa[0])deb_info.diffa_consis++;
				else feedback += lang_pack.get_text(Lang_Type::General, 4) + std::to_string(minsum)
						+ lang_pack.get_text(Lang_Type::General, 4) + std::to_string(diffa[0]);//", minsum, diffa[0]"
				double foundindex = bd_se.find(besttime, 0) / (double)find_num;
				deb_info.ave_index = deb_info.ave_index + foundindex;
				if (foundindex > deb_info.max_index && duration > 75 * interval) {
					deb_info.max_index = foundindex;
					deb_info.max_line = i + 1;
				}
			}
			//
			if (fast_match) {
				offset = bd_time[i] - tv_time[i];
				lastlinetime = tv_time[i];
			}
		}
		if (keep_processing && !keep_processing->test_and_set()) {
			keep_processing->clear();
			delete[] search_result;
			search_result = nullptr;
			return -2;
		}
		if (prog_single)prog_single(3, (i + 1) / static_cast<double>(nb_timeline));
	}
	delete[] search_result;
	search_result = nullptr;
	//output debug info
	if (debug_mode) {
		deb_info.ave_index *= 100.0 / static_cast<double>(deb_info.nb_line);
		deb_info.max_index *= 100.0;
		deb_info.diffa_consis *= 100.0 / static_cast<double>(deb_info.nb_line);
		feedback += lang_pack.get_text(Lang_Type::Match_Sub, 10) + std::to_string(deb_info.ave_index) +
			lang_pack.get_text(Lang_Type::Match_Sub, 11) + //"\r\nAverage Found Index = ***%    "
			lang_pack.get_text(Lang_Type::Match_Sub, 12) + std::to_string(deb_info.max_index) +
			lang_pack.get_text(Lang_Type::Match_Sub, 13) + //"Max Found Index= ***%\r\nMax Found Line= "
			std::to_string(deb_info.max_line) + lang_pack.get_text(Lang_Type::Match_Sub, 14) +
			std::to_string(deb_info.max_delta) +//"***    Max Delta= ***"
			lang_pack.get_text(Lang_Type::Match_Sub, 24) + std::to_string(deb_info.diffa_consis) +
			lang_pack.get_text(Lang_Type::Match_Sub, 11);//"\r\nDiffa Consistency = ***%    "
	}
	return 0;
}

int Match::Match::output(const std::string &output_path)
{
	using std::vector, std::string, std::fstream, std::ios;
	feedback = "";
	//check feedbacks
	vector<int64_t>time_diff(nb_timeline);
	vector<int>check_feedbacks(nb_timeline);
	for (int64_t i = 0; i < nb_timeline; i++) {
		if (tv_time[i] >= 0)time_diff.push_back(bd_time[i] - tv_time[i]);
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
				feedback += lang_pack.get_text(Lang_Type::Match_Sub, 5) + std::to_string(i + 1) +
				lang_pack.get_text(Lang_Type::Match_Sub, 15) + std::to_string(line_num + 1) + 
				lang_pack.get_text(Lang_Type::Match_Sub, 16);//"\r\n警告：第***行（与第***行时间相同）可能存在匹配错误!";
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
			feedback += lang_pack.get_text(Lang_Type::Match_Sub, 5) + std::to_string(i + 1) +
				lang_pack.get_text(Lang_Type::Match_Sub, 17);//"\r\n警告：第***行可能存在匹配错误：与前一行次序不一致！"
			break;
		case 2:
			feedback += lang_pack.get_text(Lang_Type::Match_Sub, 5) + std::to_string(i + 1) + 
				lang_pack.get_text(Lang_Type::Match_Sub, 18);//"\r\n警告：第***行可能存在匹配错误：与后一行次序不一致！"
			break;
		case 3:
			feedback += lang_pack.get_text(Lang_Type::Match_Sub, 5) + std::to_string(i + 1) + 
				lang_pack.get_text(Lang_Type::Match_Sub, 19);//"\r\n警告：第***行可能存在匹配错误：与前后行时差不一致！"
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
					feedback += lang_pack.get_text(Lang_Type::Match_Sub, 2) + std::to_string(i + 1) +
						lang_pack.get_text(Lang_Type::Match_Sub, 20) +	std::to_string(i + 2) +
						lang_pack.get_text(Lang_Type::Match_Sub, 21);//"\r\n信息：第***行和第***行发生微小重叠，已自动修正。";
				}
			}
			else {
				timeline_list[i].start(timeline_list[-static_cast<int64_t>(tv_time[i]) - 2].start());
				timeline_list[i].end(timeline_list[-static_cast<int64_t>(tv_time[i]) - 2].end());
			}
			int start = static_cast<int>(round(static_cast<double>(timeline_list[i].start()) * f2t));
			int end = static_cast<int>(round(static_cast<double>(timeline_list[i].end()) * f2t));
			string replacetext, former_text;
			switch (sub_type) {
			case Sub_Type::ASS:
				replacetext = timeline_list[i].head() + cs2time(start) + "," + cs2time(end) + ",";
				break;
			case Sub_Type::SRT:
				replacetext = "\n" + cs2time(start) + " --> " + cs2time(end) + "\n";
			default:
				break;
			}
			former_text = timeline_list[i].former_text();
			content = content.replace(content.find(former_text), former_text.length(), replacetext);
		}
		else {
			timeline_list[i].start(-1);
			timeline_list[i].end(-1);
		}
	}
	//write file
	fstream output_file(output_path, ios::out | ios::trunc);
	if (!output_file.is_open()) {
		feedback += lang_pack.get_text(Lang_Type::Match_Sub, 22);//"\r\n打开输出字幕文件失败!"
		return -1;
	}
	if (!(output_file << head + content)) {
		feedback += lang_pack.get_text(Lang_Type::Match_Sub, 23);//"\r\n写入字幕文件失败!"
		return -1;
	}
	if (prog_single)prog_single(3, 1.0);
	return 0;
}
int Match::Match::output()
{
	std::string output_path;
	switch (sub_type) {
	case Sub_Type::ASS:
		output_path = sub_path.substr(0, sub_path.find_last_of('.')) + ".matched.ass";
		break;
	case Sub_Type::SRT:
		output_path = sub_path.substr(0, sub_path.find_last_of('.')) + ".matched.srt";
		break;
	default:
		output_path = sub_path.find_last_of('.') != std::string::npos ?
			sub_path.substr(0, sub_path.find_last_of('.')) + ".matched" + sub_path.substr(sub_path.find_last_of('.'))
			: sub_path + ".matched";
		break;
	}
	output(output_path);
	return 0;
}

int64_t Match::Match::get_nb_timeline()
{
	return nb_timeline;
}
int64_t Match::Match::get_timeline(const int& line, const Timeline_Time_Type& type)
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
std::string Match::Match::get_feedback()
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
	std::string timeout = "";
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
int Match::Match::time2cs(const std::string &time)
{
	using std::stoi;
	int cs = 0;
	switch (sub_type) {
	case Sub_Type::ASS:
		cs += stoi(time.substr(0, 1)) * 360000;
		cs += stoi(time.substr(2, 2)) * 6000;
		cs += stoi(time.substr(5, 2)) * 100;
		cs += stoi(time.substr(8, 2));
		break;
	case Sub_Type::SRT:
		cs += stoi(time.substr(0, 2)) * 360000;
		cs += stoi(time.substr(3, 2)) * 6000;
		cs += stoi(time.substr(6, 2)) * 100;
		cs += stoi(time.substr(9, 2));
		break;
	default:
		break;
	}
	cs += sub_offset;
	return cs;
}

int Match::Match::caldiff(const int64_t tv_start, const size_t se_start, const size_t se_end, const int min_check_num,
	const int64_t check_field, se_re* re)
{
	se_re feedback;
	if (diffa[2] <= 0) {
		*re = feedback;
		return -1;
	}
	int64_t sum = 0;
	char* tvdata[8], * bddata[8];
	for (size_t seindex = se_start; seindex < se_end; seindex++) {
		int64_t bd_start = bd_se.read(seindex);
		sum = 0;
		for (int i = 0; i < ch; i++) {
			tvdata[i] = tv_fft_data[i][tv_start].getdata();
			bddata[i] = bd_fft_data[i][bd_start].getdata();
		}
		for (int64_t i = 0; i <= duration; i++) {
			for (int j = 0; j < ch; j++) {
				for (int k = 0; k < fft_size; k++) {
					sum += llabs(static_cast<int64_t>(tvdata[j][k]) - static_cast<int64_t>(bddata[j][k])) 
						* (static_cast<int64_t>(tvdata[j][k]) + 129LL);
				}
				tvdata[j] += fft_size;
				bddata[j] += fft_size;
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
			diffa[2] = min_check_num;
		}
		else if (llabs(bd_start - diffa[1]) <= check_field) diffa[2]--;
		if (diffa[2] <= 0) {
			*re = feedback;
			return -1;
		}
	}
	*re = feedback;
	return 0;
}
int Match::Match_SSE::caldiff(const int64_t tv_start, const size_t se_start, const size_t se_end, const int min_check_num,
	const int64_t check_field, se_re* re)
{
	se_re feedback;
	if (diffa[2] <= 0) {
		*re = feedback;
		return -1;
	}
	int64_t sum = 0;
	int vectornum = fft_size / 8;
	char* tvdata[8], * bddata[8];
	__m128i tvvector, bdvector, difvector[2];
	__m128i sumvector[2] = { _mm_setzero_si128(),_mm_setzero_si128() };
	const __m128i w1vector = _mm_set1_epi16(129);
	for (size_t seindex = se_start; seindex < se_end; seindex++) {
		int64_t bd_start = bd_se.read(seindex);
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
					tvvector = _mm_cvtepi8_epi16(_mm_load_si128(reinterpret_cast<__m128i*>(tvdata[j])));
					bdvector = _mm_cvtepi8_epi16(_mm_load_si128(reinterpret_cast<__m128i*>(bddata[j])));
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
			diffa[2] = min_check_num;
		}
		else if (llabs(bd_start - diffa[1]) <= check_field) diffa[2]--;
		if (diffa[2] <= 0) {
			*re = feedback;
			return -1;
		}
	}
	*re = feedback;
	return 0;
}
int Match::Match_AVX2::caldiff(const int64_t tv_start, const size_t se_start, const size_t se_end, const int min_check_num,
	const int64_t check_field, se_re* re)
{
	se_re feedback;
	if (diffa[2] <= 0) {
		*re = feedback;
		return -1;
	}
	int64_t sum = 0;
	int vectornum = fft_size / 16;
	__m128i* tvdata[8], * bddata[8];
	__m256i tvvector, bdvector, difvector[2];
	__m256i sumvector[2] = { _mm256_setzero_si256(),_mm256_setzero_si256() };
	__m128i sumvector8[2];
	const __m256i w1vector = _mm256_set1_epi16(129);
	for (size_t seindex = se_start; seindex < se_end; seindex++) {
		int64_t bd_start = bd_se.read(seindex);
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
			diffa[2] = min_check_num;
		}
		else if (llabs(bd_start - diffa[1]) <= check_field) diffa[2]--;
		if (diffa[2] <= 0) {
			*re = feedback;
			return -1;
		}
	}
	*re = feedback;
	return 0;
}
