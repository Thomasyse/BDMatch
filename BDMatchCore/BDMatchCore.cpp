#include "headers/BDMatchCore.h"
#include "headers/multithreading.h"
#include <time.h>

//A sample of command line usage
/*
int match{
	std::atomic_flag *keep_processing = new std::atomic_flag;//cancel token
	BDMatchCore *match_core = new BDMatchCore(keep_processing);
	prog_func prog_ptr = nullptr;//function pointer to show progress: prog_func(int phase(0-3), double percent);
	feedback_func feedback_ptr = nullptr;//function pointer to show feedback: feedback_func(const char* feedback_string);
	match_core->load_interface(prog_ptr, feedback_ptr);
	//Setteings(default value as below):
	int isa_mode = 3;//ISA Mode:0 for none, 1 for SSE/SSE2/SSE4.1, 2 for AVX, 3 for AVX2.
	int fft_num = 512;//FFT data size, larger for speed, smaller for precision
	int min_db = -14;//volume(db) threshold for ignoring
	bool output_pcm = false;//whether to output decoded wave
	bool parallel_decode = false;//whether to decode paralleling, recommended true for Disk with good performance
	bool vol_match = false;//whether to match the volume of BD to TV, recommended true for large volume difference between TV and BD
	int min_check_num = 100;//check times for match results, larger for precision, smaller for speed
	int find_field = 10;//range for searching the results(-xx -> +xx), with the unit of second
	int sub_offset = 0;//offset of the timeline of the ass file, with the unit of centisecond
	int max_length = 20;//the max length of the timeline to be matched, with the unit of second
	bool match_ass = true;//whether to match the ass
	bool fast_match = false;//whether to match fast but perhaps lose some pecision
	bool debug_mode = false;//whether to show some debug info
	match_core->load_settings(isa_mode,fft_num,min_db,output_pcm, parallel_decode, vol_match,
		min_check_num, find_field, sub_offset, max_length,
		match_ass, fast_match, debug_mode);
	//decode
	const char *tv_path = "TV file path";
	const char *bd_path = "BD file path";
	const char *sub_path = "subtitle file path";
	const char *output_path = "output ASS file path";//"" for auto rename.
	int re = 0;
	re = match_core->decode(tv_path, bd_path);
	if (re < 0) return re;
	if (match_ass) {
		re = match_core->match_1(sub_path);
		if (re < 0) return re;
		re = match_core->match_2(output_path);
		if (re < 0) return re;
		match_core->clear_match();
	}
	if (re < 0) {
		if (re == -2)return -6;
		else return -5;
	}
	delete keep_processing;
	return 0;
}
*/

BDMatchCore::BDMatchCore()
{
}
BDMatchCore::~BDMatchCore()
{
}

int BDMatchCore::clear_data()
{
	tv_decode.reset(nullptr);
	bd_decode.reset(nullptr);
	match.reset(nullptr);
	return 0;
}

int BDMatchCore::load_interface(const prog_func & prog_back0, const feedback_func & feed_func0)
{
	prog_back = prog_back0;
	feed_func = feed_func0;
	return 0;
}

int BDMatchCore::load_settings(const int &isa_mode0, const int &fft_num0, const int &min_db0,
	const bool &output_pcm0, const bool &parallel_decode0, const bool &vol_match0, 
	const int &min_check_num0, const int &find_field0, const int &sub_offset0, const int &max_length0,
	const bool &match_ass0, const bool &fast_match0, const bool &debug_mode0)
{
	isa_mode = isa_mode0;
	fft_num = fft_num0;
	min_db = min_db0;
	output_pcm = output_pcm0;
	vol_match = vol_match0;
	parallel_decode = parallel_decode0;
	min_check_num = min_check_num0;
	find_field = find_field0;
	sub_offset = sub_offset0;
	max_length = max_length0;
	match_ass = match_ass0;
	fast_match = fast_match0;
	debug_mode = debug_mode0;
	return 0;
}

int BDMatchCore::decode(const char* tv_path0, const char* bd_path0)
{
	std::string tv_path = tv_path0;
	std::string bd_path = bd_path0;
	if (prog_back)prog_back(0, 0);
	long start = clock();//开始计时
	//fftw设置
	double* in = (double*)fftw_malloc(sizeof(double)*fft_num);
	fftw_complex* out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*fft_num);
	fftw_plan plan = fftw_plan_dft_r2c_1d(fft_num, in, out, FFTW_MEASURE);
	fftw_free(in);
	fftw_free(out);
	//multithreading
	fixed_thread_pool pool(2, keep_processing);
	//解码TV文件
	if (isa_mode == 3)tv_decode.reset(new Decode::Decode_AVX2(lang_pack, keep_processing));
	else if (isa_mode == 2)tv_decode.reset(new Decode::Decode_AVX(lang_pack, keep_processing));
	else if (isa_mode == 1)tv_decode.reset(new Decode::Decode_SSE(lang_pack, keep_processing));
	else tv_decode.reset(new Decode::Decode(lang_pack, keep_processing));
	if (vol_match) tv_decode->set_vol_mode(0);
	tv_decode->load_settings(fft_num, output_pcm, min_db, 0, 1, plan, prog_back);
	int re = 0;
	re = tv_decode->initialize(tv_path);
	if (re < 0) {
		fftw_destroy_plan(plan);
		feedback_tv(tv_path);
		return re; 
	}
	pool.execute(std::bind(&Decode::Decode::decode_audio, tv_decode.get()));
	if (!parallel_decode) {
		pool.wait();
		re = tv_decode->get_return();
		if (re < 0) {
			fftw_destroy_plan(plan);
			feedback_tv(tv_path);
			return re;
		}
	}
	//解码BD文件
	double bd_pre_avg_vol = 0.0;
	if (isa_mode == 3)bd_decode.reset(new Decode::Decode_AVX2(lang_pack, keep_processing));
	else if (isa_mode == 2)bd_decode.reset(new Decode::Decode_AVX(lang_pack, keep_processing));
	else if (isa_mode == 1)bd_decode.reset(new Decode::Decode_SSE(lang_pack, keep_processing));
	else bd_decode.reset(new Decode::Decode(lang_pack, keep_processing));
	if (vol_match) bd_decode->set_vol_mode(1);
	bd_decode->load_settings(fft_num, output_pcm, min_db, tv_decode->get_samp_rate(), 2, plan, prog_back);
	re = bd_decode->initialize(bd_path);
	if (re < 0) {
		keep_processing->clear();
		fftw_destroy_plan(plan);
		feedback_bd(bd_path, bd_pre_avg_vol);
		return re;
	}
	if (keep_processing->test_and_set())pool.execute(std::bind(&Decode::Decode::decode_audio, bd_decode.get()));
	else keep_processing->clear();
	pool.wait();
	if (tv_decode->get_return() < 0) {
		re = tv_decode->get_return();
		fftw_destroy_plan(plan);
		feedback_tv(tv_path);
		return re;
	}
	else if (bd_decode->get_return() < 0) {
		re = bd_decode->get_return();
		fftw_destroy_plan(plan);
		feedback_bd(bd_path, bd_pre_avg_vol);
		return re;
	}
	if (vol_match) {
		bd_decode->set_vol_mode(0);
		bd_pre_avg_vol = bd_decode->get_avg_vol();
		double vol_coef = sqrt(tv_decode->get_avg_vol() / bd_decode->get_avg_vol());
		if (vol_coef < 1.00)vol_coef *= 1.49;
		else vol_coef /= 1.49;
		bd_decode->set_vol_coef(vol_coef);
		re = bd_decode->initialize(bd_path);
		if (keep_processing->test_and_set())pool.execute(std::bind(&Decode::Decode::decode_audio, bd_decode.get())); 
		else keep_processing->clear();
		pool.wait();
		if (bd_decode->get_return() < 0) {
			re = bd_decode->get_return();
			fftw_destroy_plan(plan);
			feedback_bd(bd_path, bd_pre_avg_vol);
			return re;
		}
	}
	//输出解码时间
	long end = clock();
	double spend = (double(end) - double(start)) / (double)CLOCKS_PER_SEC;
	feedback_tv(tv_path);
	feedback_bd(bd_path, bd_pre_avg_vol);
	feedback_time(spend);
	fftw_destroy_plan(plan);
	return 0;
}

int BDMatchCore::match_1(const char *sub_path0)
{
	std::string sub_path = sub_path0;
	if (match_ass) {
		if (!keep_processing->test_and_set()) {
			keep_processing->clear();
			return -2;
		}
		if (isa_mode == 0)match.reset(new Matching::Match(lang_pack, keep_processing));
		else if (isa_mode <= 2)match.reset(new Matching::Match_SSE(lang_pack, keep_processing));
		else if (isa_mode == 3)match.reset(new Matching::Match_AVX2(lang_pack, keep_processing));
		int re = 0;
		match->load_settings(min_check_num, find_field, sub_offset, max_length,
			fast_match, debug_mode, prog_back);
		match->load_decode_info(tv_decode->get_fft_data(), bd_decode->get_fft_data(),
			tv_decode->get_channels(), bd_decode->get_channels(), tv_decode->get_fft_samp_num(), bd_decode->get_fft_samp_num(),
			tv_decode->get_milisec(), bd_decode->get_milisec(), tv_decode->get_samp_rate(),
			tv_decode->get_file_name(), bd_decode->get_file_name(), bd_decode->get_audio_only());
		re = match->load_sub(sub_path);
		if (feed_func) {
			std::string tmp = lang_pack.get_text(Lang_Type::Core, 5);
			feed_func(tmp.c_str(), tmp.length());//"\r\n字幕文件："
			feed_func(sub_path.substr(sub_path.find_last_of("\\") + 1).c_str(), -1);
		}
		feedback_match();
		if (re < 0) return re;
	}
	else {
		prog_back(3, 0);
	}
	return 0;
}
int BDMatchCore::match_2(const char *output_path0)
{
	std::string output_path = output_path0;
	int re = 0;
	if (match_ass) {
		re = match->match();
		feedback_match();
		if (!keep_processing->test_and_set()) {
			keep_processing->clear();
			return -2;
		}
		else if (re < 0) {
			return re;
		}
		if (output_path == "")re = match->output();
		else {
			re = match->output(output_path);
		}
		feedback_match();
		if (re < 0) return re;
	}
	return 0;
}

int BDMatchCore::clear_match()
{
	match.reset(nullptr);
	return 0;
}

size_t BDMatchCore::get_nb_timeline()
{
	if (match)return match->get_nb_timeline();
	else return 0;
}
int BDMatchCore::get_timeline(const int & index, const int & type)
{
	if (match)return match->get_timeline(index, type);
	else return -1;
}

int BDMatchCore::get_decode_info(const Decode_File & file, const Decode_Info & type)
{
	Decode::Decode *decode_ptr;
	if (file == Decode_File::BD_Decode)decode_ptr = bd_decode.get();
	else decode_ptr = tv_decode.get();
	if (!decode_ptr)return -1;
	switch (type) {
	case Decode_Info::Channels:
		return decode_ptr->get_channels();
		break;
	case Decode_Info::FFT_Samp_Num:
		return decode_ptr->get_fft_samp_num();
		break;
	case Decode_Info::Milisec:
		return decode_ptr->get_milisec();
		break;
	case Decode_Info::Samp_Rate:
		return decode_ptr->get_samp_rate();
		break;
	case Decode_Info::FFT_Num:
		return decode_ptr->get_fft_num();
		break;
	default:
		break;
	}
	return 0;
}
char ** BDMatchCore::get_decode_spec(const Decode_File & file)
{
	Decode::Decode *decode_ptr;
	if (file == Decode_File::TV_Decode)decode_ptr = tv_decode.get();
	else decode_ptr = bd_decode.get();
	if (!decode_ptr)return nullptr;
	else return decode_ptr->get_fft_spec();
}

int BDMatchCore::initialize_cancel_token()
{
	keep_processing = std::make_shared<std::atomic_flag>();
	keep_processing->test_and_set();
	return 0;
}
int BDMatchCore::start_process()
{
	keep_processing->test_and_set();
	return 0;
}
int BDMatchCore::stop_process()
{
	keep_processing->clear();
	return 0;
}

int BDMatchCore::feedback_tv(const std::string& tv_path)
{
	if (feed_func) {
		std::string feedback = "";
		feedback = lang_pack.get_text(Lang_Type::General, 0) + lang_pack.get_text(Lang_Type::Core, 0) +
			lang_pack.get_text(Lang_Type::General, 1);//"\r\nTV文件：  "
		feed_func(feedback.c_str(), feedback.length());
		feed_func(tv_path.substr(tv_path.find_last_of("\\") + 1).c_str(), -1);
		feedback = lang_pack.get_text(Lang_Type::General, 0) + tv_decode->get_feedback();//"\r\n tv_feedback"
		if (vol_match) {
			std::string vol = lang_pack.to_u16string(10.0 * log10(tv_decode->get_avg_vol()));
			vol = vol.substr(0, vol.find_last_of('.') + 6);
			feedback += lang_pack.get_text(Lang_Type::Core, 2) + vol + lang_pack.get_text(Lang_Type::Core, 3);//"   响度：**.*dB"
		}
		feed_func(feedback.c_str(), feedback.length());
	}
	return 0;
}
int BDMatchCore::feedback_bd(const std::string& bd_path, const double& bd_pre_avg_vol)
{
	if (feed_func) {
		std::string feedback = "";
		feedback = lang_pack.get_text(Lang_Type::General, 0) + lang_pack.get_text(Lang_Type::Core, 1) + 
			lang_pack.get_text(Lang_Type::General, 1);//"\r\nBD文件：  "
		feed_func(feedback.c_str(), feedback.length());
		feed_func(bd_path.substr(bd_path.find_last_of("\\") + 1).c_str(), -1);
		feedback = lang_pack.get_text(Lang_Type::General, 0) + bd_decode->get_feedback();//"\r\n bd_feedback"
		if (vol_match) {
			std::string vol = lang_pack.to_u16string(10.0 * log10(bd_pre_avg_vol));
			vol = vol.substr(0, vol.find_last_of('.') + 6);
			std::string vol2 = lang_pack.to_u16string(10.0 * log10(bd_decode->get_avg_vol()));
			vol2 = vol2.substr(0, vol2.find_last_of('.') + 6);
			feedback += lang_pack.get_text(Lang_Type::Core, 2) + vol + lang_pack.get_text(Lang_Type::General, 2) +
				vol2 + lang_pack.get_text(Lang_Type::Core, 3);//"   响度：**.*->**.*dB"
		}
		feed_func(feedback.c_str(), feedback.length());
	}
	return 0;
}
int BDMatchCore::feedback_time(const double& spend)
{
	if (feed_func) {
		std::string feedback = "";
		std::string spend_str = lang_pack.to_u16string(spend);
		spend_str = spend_str.substr(0, spend_str.find_last_of('.') + 8);
		feedback += lang_pack.get_text(Lang_Type::General, 0) + lang_pack.get_text(Lang_Type::Core, 4) +
			spend_str + lang_pack.get_text(Lang_Type::General, 3);//"\r\n解码时间：**.***秒"
		feed_func(feedback.c_str(), feedback.length());
	}
	return 0;
}
int BDMatchCore::feedback_match()
{
	std::string feedback = match->get_feedback();
	if (feed_func)feed_func(feedback.c_str(), feedback.length());
	return 0;
}



