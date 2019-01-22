#include "BDMatchCore.h"
#include "multithreading.h"
#include <time.h>


BDMatchCore::BDMatchCore(std::atomic_flag *keep_processing0)
	:keep_processing(keep_processing0) {
}
BDMatchCore::~BDMatchCore()
{
	clear_data();
}

int BDMatchCore::clear_data()
{
	if (tv_decode)delete tv_decode;
	if (bd_decode)delete bd_decode;
	if (match)delete match;
	tv_decode = nullptr;
	bd_decode = nullptr;
	match = nullptr;
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
	const int &min_check_num0, const int &find_field0, const int &ass_offset0, const int &max_length0,
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
	ass_offset = ass_offset0;
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
	if (isa_mode == 3)tv_decode = new Decode::Decode_AVX2(keep_processing);
	else if(isa_mode == 2)tv_decode = new Decode::Decode_AVX(keep_processing);
	else if (isa_mode == 1)tv_decode = new Decode::Decode_SSE(keep_processing);
	else tv_decode = new Decode::Decode(keep_processing);
	if (vol_match) tv_decode->set_vol_mode(0);
	tv_decode->load_settings(fft_num, output_pcm, min_db, 0, 1, plan, prog_back);
	int re = 0;
	re = tv_decode->initialize(tv_path);
	if (re < 0) {
		clear_data();
		return re;
	}
	pool.execute(std::bind(&Decode::Decode::decodeaudio, tv_decode));
	if (!parallel_decode) {
		pool.wait();
		re = tv_decode->get_return();
		if (re < 0) {
			clear_data();
			fftw_destroy_plan(plan);
			return re;
		}
	}
	//解码BD文件
	if (isa_mode == 3)bd_decode = new Decode::Decode_AVX2(keep_processing);
	else if (isa_mode == 2)bd_decode = new Decode::Decode_AVX(keep_processing);
	else if (isa_mode == 1)bd_decode = new Decode::Decode_SSE(keep_processing);
	else bd_decode = new Decode::Decode(keep_processing);
	if (vol_match) bd_decode->set_vol_mode(1);
	bd_decode->load_settings(fft_num, output_pcm, min_db, tv_decode->get_samp_rate(), 2, plan, prog_back);
	re = bd_decode->initialize(bd_path);
	if (re < 0) {
		keep_processing->clear();
		clear_data();
		return re;
	}
	if (keep_processing->test_and_set())pool.execute(std::bind(&Decode::Decode::decodeaudio, bd_decode));
	else keep_processing->clear();
	pool.wait();
	if (tv_decode->get_return() < 0) {
		re = tv_decode->get_return();
		clear_data();
		fftw_destroy_plan(plan);
		return re;
	}
	else if (bd_decode->get_return() < 0) {
		re = bd_decode->get_return();
		clear_data();
		fftw_destroy_plan(plan);
		return re;
	}
	double bd_pre_avg_vol = 0.0;
	if (vol_match) {
		bd_decode->set_vol_mode(0);
		bd_pre_avg_vol = bd_decode->get_avg_vol();
		double vol_coef = sqrt(tv_decode->get_avg_vol() / bd_decode->get_avg_vol());
		if (vol_coef > 1.00)vol_coef *= 1.49;
		else vol_coef /= 1.50;
		bd_decode->set_vol_coef(vol_coef);
		re = bd_decode->initialize(bd_path);
		if (keep_processing->test_and_set())pool.execute(std::bind(&Decode::Decode::decodeaudio, bd_decode)); 
		else keep_processing->clear();
		pool.wait();
		if (bd_decode->get_return() < 0) {
			re = bd_decode->get_return();
			clear_data();
			fftw_destroy_plan(plan);
			return re;
		}
	}
	//输出解码时间
	long end = clock();
	double spend = double(end - start) / (double)CLOCKS_PER_SEC;
	std::string feedback = "";
	if (feed_func) {
		feedback += "TV文件：  " + tv_path.substr(tv_path.find_last_of("\\") + 1) + "\r\n" + tv_decode->get_feedback();
		if (vol_match) {
			std::string vol = std::to_string(10.0*log10(tv_decode->get_avg_vol()));
			vol = vol.substr(0, vol.find_last_of('.') + 3);
			feedback += "   响度：" + vol + " dB";
		}
		feedback += "\r\nBD文件：  " + bd_path.substr(bd_path.find_last_of("\\") + 1) + "\r\n" + bd_decode->get_feedback();
		if (vol_match) {
			std::string vol = std::to_string(10.0*log10(bd_pre_avg_vol));
			vol = vol.substr(0, vol.find_last_of('.') + 3);
			std::string vol2 = std::to_string(10.0*log10(bd_decode->get_avg_vol()));
			vol2 = vol2.substr(0, vol2.find_last_of('.') + 3);
			feedback += "   响度：" + vol + "->" + vol2 + " dB";
		}
		std::string spend_str = std::to_string(spend);
		spend_str = spend_str.substr(0, spend_str.find_last_of('.') + 4);
		feedback += "\r\n解码时间：" + spend_str + "秒";
		feed_func(feedback.c_str());
	}
	fftw_destroy_plan(plan);
	return 0;
}

int BDMatchCore::match_1(const char *ass_path)
{
	if (match_ass) {
		if (!keep_processing->test_and_set()) {
			keep_processing->clear();
			clear_data();
			return -2;
		}
		if (isa_mode == 0)match = new Matching::Match(keep_processing);
		else if (isa_mode <= 2)match = new Matching::Match_SSE(keep_processing);
		else if (isa_mode == 3)match = new Matching::Match_AVX2(keep_processing);
		int re = 0;
		match->load_settings(min_check_num, find_field, ass_offset, max_length,
			fast_match, debug_mode, prog_back);
		match->load_decode_info(tv_decode->get_fft_data(), bd_decode->get_fft_data(),
			tv_decode->get_channels(), bd_decode->get_channels(), tv_decode->get_fft_samp_num(), bd_decode->get_fft_samp_num(),
			tv_decode->get_milisec(), bd_decode->get_milisec(), tv_decode->get_samp_rate(),
			tv_decode->get_file_name(), bd_decode->get_file_name(), bd_decode->get_audio_only());
		re = match->load_ass(ass_path);
		if (feed_func) feed_func(match->get_feedback().c_str());
		if (re < 0) {
			clear_data();
			return re;
		}
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
		if (feed_func) feed_func(match->get_feedback().c_str());
		if (!keep_processing->test_and_set()) {
			keep_processing->clear();
			clear_data();
			return -2;
		}
		else if (re < 0) {
			keep_processing->clear();
			clear_data();
			return re;
		}
		if (output_path == "")re = match->output();
		else {
			re = match->output(output_path);
		}
		if (feed_func) feed_func(match->get_feedback().c_str());
		if (re < 0) {
			clear_data();
			return re;
		}
	}
	return 0;
}
int BDMatchCore::clear_match()
{
	if (match)delete match;
	match = nullptr;
	return 0;
}

Decode::Decode * BDMatchCore::get_tv_decode()
{
	return tv_decode;
}
Decode::Decode * BDMatchCore::get_bd_decode()
{
	return bd_decode;
}
Matching::Match * BDMatchCore::get_match()
{
	return match;
}
