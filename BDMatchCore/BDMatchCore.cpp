#include "headers/BDMatchCore.h"
#include "headers/multithreading.h"

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

int BDMatchCore::set_language(const char* language0)
{
	std::string lang_str(language0);
	std::transform(lang_str.begin(), lang_str.end(), lang_str.begin(), ::tolower);
	if (lang_str == "zh-cn" || lang_str == "zh-hans-cn")lang_pack = { Language::zh_CN };
	else lang_pack = { Language::en_US };

	return 0;
}

int BDMatchCore::load_interface(const prog_func & prog_back0, const feedback_func & feed_func0)
{
	prog_back = prog_back0;
	feed_func = feed_func0;
	return 0;
}

int BDMatchCore::load_settings(const ISA_Mode &isa_mode0, const int &fft_num0, const int &min_db0,
	const bool &output_pcm0, const bool &parallel_decode0, const bool &vol_match0, 
	const int &min_cnfrm_num0, const int &search_range0, const int &sub_offset0, const int &max_length0,
	const bool &match_ass0, const bool &fast_match0, const bool &debug_mode0)
{
	isa_mode = isa_mode0;
	fft_num = fft_num0;
	min_db = min_db0;
	output_pcm = output_pcm0;
	vol_match = vol_match0;
	parallel_decode = parallel_decode0;
	min_cnfrm_num = min_cnfrm_num0;
	search_range = search_range0;
	sub_offset = sub_offset0;
	max_length = max_length0;
	match_ass = match_ass0;
	fast_match = fast_match0;
	debug_mode = debug_mode0;
	return 0;
}

Match_Core_Return BDMatchCore::decode(const char* tv_path0, const char* bd_path0)
{
	std::string_view tv_path(tv_path0), bd_path(bd_path0);
	if (prog_back)prog_back(Prog_Mode::Setting, 0);
	clock_start = std::chrono::high_resolution_clock::now();//开始计时
	//fftw设置
	double* in = (double*)fftw_malloc(sizeof(double)*fft_num);
	fftw_complex* out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*fft_num);
	fftw_plan plan = fftw_plan_dft_r2c_1d(fft_num, in, out, FFTW_MEASURE);
	fftw_free(in);
	fftw_free(out);
	//multithreading
	fixed_thread_pool pool(2, *stop_src);
	//解码TV文件
	switch (isa_mode) {
	case ISA_Mode::AVX2_FMA:
		tv_decode.reset(new Decode::Decode_AVX2(lang_pack, *stop_src));
		break;
	case ISA_Mode::AVX:
		tv_decode.reset(new Decode::Decode_AVX(lang_pack, *stop_src));
		break;
	case ISA_Mode::SSE:
		tv_decode.reset(new Decode::Decode_SSE(lang_pack, *stop_src));
		break;
	case ISA_Mode::No_SIMD:
	default:
		tv_decode.reset(new Decode::Decode(lang_pack, *stop_src));
		break;
	}
	if (vol_match)tv_decode->set_vol_mode(Decode::Vol_Mode::Dec_And_Cal);
	tv_decode->load_settings(fft_num, output_pcm, min_db, 0, Prog_Mode::TV, plan, prog_back);
	Match_Core_Return re = Match_Core_Return::Success;
	re = tv_decode->initialize(tv_path);
	if (re != Match_Core_Return::Success) {
		fftw_destroy_plan(plan);
		feedback_tv(tv_path);
		return re; 
	}
	pool.execute([&tv_decode = this->tv_decode]() { tv_decode->decode_audio(); });
	if (!parallel_decode) {
		pool.wait();
		re = tv_decode->get_return();
		if (re != Match_Core_Return::Success) {
			fftw_destroy_plan(plan);
			feedback_tv(tv_path);
			return re;
		}
	}
	//解码BD文件
	double bd_pre_avg_vol = 0.0;
	switch (isa_mode) {
	case ISA_Mode::AVX2_FMA:
		bd_decode.reset(new Decode::Decode_AVX2(lang_pack, *stop_src));
		break;
	case ISA_Mode::AVX:
		bd_decode.reset(new Decode::Decode_AVX(lang_pack, *stop_src));
		break;
	case ISA_Mode::SSE:
		bd_decode.reset(new Decode::Decode_SSE(lang_pack, *stop_src));
		break;
	case ISA_Mode::No_SIMD:
	default:
		bd_decode.reset(new Decode::Decode(lang_pack, *stop_src));
		break;
	}
	if (vol_match)bd_decode->set_vol_mode(Decode::Vol_Mode::Cal_Only);
	bd_decode->load_settings(fft_num, output_pcm, min_db, tv_decode->get_samp_rate(), Prog_Mode::BD, plan, prog_back);
	re = bd_decode->initialize(bd_path);
	if (re != Match_Core_Return::Success) {
		stop_src->request_stop();
		fftw_destroy_plan(plan);
		feedback_bd(bd_path, bd_pre_avg_vol);
		return re;
	}
	if (!stop_src->stop_requested())pool.execute([&bd_decode = this->bd_decode]() { bd_decode->decode_audio(); });
	pool.wait();
	if (tv_decode->get_return() != Match_Core_Return::Success) {
		re = tv_decode->get_return();
		fftw_destroy_plan(plan);
		feedback_tv(tv_path);
		return re;
	}
	else if (bd_decode->get_return() != Match_Core_Return::Success) {
		re = bd_decode->get_return();
		fftw_destroy_plan(plan);
		feedback_bd(bd_path, bd_pre_avg_vol);
		return re;
	}
	if (vol_match) {
		bd_decode->clear_ffmpeg();
		bd_decode->set_vol_mode(Decode::Vol_Mode::Dec_And_Cal);
		bd_pre_avg_vol = bd_decode->get_avg_vol();
		double vol_coef = sqrt(tv_decode->get_avg_vol() / bd_decode->get_avg_vol());
		if (vol_coef < 1.00)vol_coef *= 1.49;
		else vol_coef /= 1.49;
		bd_decode->set_vol_coef(vol_coef);
		re = bd_decode->initialize(bd_path);
		if (!stop_src->stop_requested())pool.execute([&bd_decode = this->bd_decode]() { bd_decode->decode_audio(); });
		pool.wait();
		if (bd_decode->get_return() != Match_Core_Return::Success) {
			re = bd_decode->get_return();
			fftw_destroy_plan(plan);
			feedback_bd(bd_path, bd_pre_avg_vol);
			return re;
		}
	}
	feedback_tv(tv_path);
	feedback_bd(bd_path, bd_pre_avg_vol);
	feedback_time(Procedure::Decode);
	fftw_destroy_plan(plan);
	return Match_Core_Return::Success;
}

Match_Core_Return BDMatchCore::match_1(const char *sub_path0, const char* encoded_tv_path, const char* encoded_bd_path)
{
	if (match_ass) {
		std::string_view sub_path(sub_path0);
		// read tv and bd paths
		std::string_view tv_file_name, bd_file_name;
		if (encoded_tv_path)tv_file_name = encoded_tv_path;
		else tv_file_name = tv_decode->get_file_name().data();
		if (encoded_bd_path)bd_file_name = encoded_bd_path;
		else bd_file_name = bd_decode->get_file_name().data();
		//
		if (stop_src->stop_requested()) {
			return Match_Core_Return::User_Stop;
		}
		clock_start = std::chrono::high_resolution_clock::now();//开始计时
		switch (isa_mode) {
		case ISA_Mode::AVX2_FMA:
			match.reset(new Match::Match_AVX2(lang_pack, *stop_src));
			break;
		case ISA_Mode::AVX:
		case ISA_Mode::SSE:
			match.reset(new Match::Match_SSE(lang_pack, *stop_src));
			break;
		case ISA_Mode::No_SIMD:
		default:
			match.reset(new Match::Match(lang_pack, *stop_src));
			break;
		}
		Match_Core_Return re = Match_Core_Return::Success;
		match->load_settings(min_cnfrm_num, search_range, sub_offset, max_length,
			fast_match, debug_mode, prog_back);
		match->load_decode_info(tv_decode->get_fft_data(), bd_decode->get_fft_data(),
			tv_decode->get_channels(), bd_decode->get_channels(), tv_decode->get_fft_samp_num(), bd_decode->get_fft_samp_num(),
			tv_decode->get_centi_sec(), bd_decode->get_centi_sec(), tv_decode->get_samp_rate(),
			tv_file_name, bd_file_name, bd_decode->get_audio_only());
		re = match->load_sub(sub_path);
		if (feed_func) {
			std::string_view tmp = lang_pack.get_text(Lang_Type::Core, 4);
			feed_func(tmp.data(), tmp.length());//"\n字幕文件："
			feed_func(sub_path.substr(sub_path.find_last_of("\\") + 1).data(), -1);
		}
		feedback_match();
		if (re != Match_Core_Return::Success) return re;
	}
	else {
		prog_back(Prog_Mode::Sub, 0);
	}
	return Match_Core_Return::Success;
}
Match_Core_Return BDMatchCore::match_2(const char *output_path0)
{
	std::string output_path = output_path0;
	Match_Core_Return re = Match_Core_Return::Success;
	if (match_ass) {
		re = match->match();
		feedback_match();
		if (stop_src->stop_requested()) {
			return Match_Core_Return::User_Stop;
		}
		else if (re != Match_Core_Return::Success) {
			return re;
		}
		if (output_path.empty())re = match->output();
		else {
			re = match->output(output_path);
		}
		feedback_match();
		feedback_time(Procedure::Match);
		if (re != Match_Core_Return::Success) return re;
	}
	return Match_Core_Return::Success;
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
int64_t BDMatchCore::get_timeline(const size_t & index, const Match::Timeline_Time_Type& type)
{
	if (match)return match->get_timeline(index, type);
	else return -1;
}

int64_t BDMatchCore::get_decode_info(const Decode::Decode_File & file, const Decode::Decode_Info & type)
{
	Decode::Decode *decode_ptr;
	if (file == Decode::Decode_File::BD_Decode)decode_ptr = bd_decode.get();
	else decode_ptr = tv_decode.get();
	if (!decode_ptr)return -1;
	switch (type) {
	case Decode::Decode_Info::Channels:
		return decode_ptr->get_channels();
		break;
	case Decode::Decode_Info::FFT_Samp_Num:
		return decode_ptr->get_fft_samp_num();
		break;
	case Decode::Decode_Info::Centi_Sec:
		return decode_ptr->get_centi_sec();
		break;
	case Decode::Decode_Info::Samp_Rate:
		return decode_ptr->get_samp_rate();
		break;
	case Decode::Decode_Info::FFT_Num:
		return decode_ptr->get_fft_num();
		break;
	}
	return 0;
}
char ** BDMatchCore::get_decode_spec(const Decode::Decode_File & file)
{
	Decode::Decode* decode_ptr = nullptr;
	switch (file) {
	case Decode::Decode_File::TV_Decode:
		decode_ptr = tv_decode.get();
		break;
	case Decode::Decode_File::BD_Decode:
		decode_ptr = bd_decode.get();
		break;
	}
	if (!decode_ptr)return nullptr;
	else return decode_ptr->get_fft_spec();
}

int BDMatchCore::start_process()
{
	stop_src.reset(new std::stop_source);
	return 0;
}
int BDMatchCore::stop_process()
{
	stop_src->request_stop();
	return 0;
}

int BDMatchCore::feedback_tv(const std::string_view& tv_path)
{
	if (feed_func) {
		std::string feedback;
		feedback += std::format("{}{}", lang_pack.get_text(Lang_Type::Core, 0),
			lang_pack.get_text(Lang_Type::General, 1));//"\nTV文件：  "
		feed_func(feedback.c_str(), feedback.length());
		std::string_view tv_file_name = tv_path.substr(tv_path.find_last_of("\\") + 1);
		feed_func(reinterpret_cast<const char*>(tv_file_name.data()), tv_file_name.size());
		feedback.clear();
		std::string_view decoder_feedback = tv_decode->get_feedback();
		feedback.reserve(decoder_feedback.size() + 3);
		feedback += std::format("\n{}", decoder_feedback);//"\n tv_feedback"
		if (vol_match) {
			double vol = 10.0 * log10(tv_decode->get_avg_vol());
			feedback += str_vfmt(lang_pack.get_text(Lang_Type::Core, 2), std::format("{:3f}", vol));//"   响度：**.*dB"
		}
		feed_func(feedback.c_str(), feedback.length());
	}
	return 0;
}
int BDMatchCore::feedback_bd(const std::string_view& bd_path, const double& bd_pre_avg_vol)
{
	if (feed_func) {
		std::string feedback;
		feedback += std::format("{}{}", lang_pack.get_text(Lang_Type::Core, 1),
			lang_pack.get_text(Lang_Type::General, 1));//"\nBD文件：  "
		feed_func(feedback.c_str(), feedback.length());
		std::string_view bd_file_name = bd_path.substr(bd_path.find_last_of("\\") + 1);
		feed_func(reinterpret_cast<const char*>(bd_file_name.data()), bd_file_name.size());
		feedback.clear();
		std::string_view decoder_feedback = bd_decode->get_feedback();
		feedback.reserve(decoder_feedback.size() + 3);
		feedback += std::format("\n{}", decoder_feedback);//"\n bd_feedback"
		if (vol_match) {
			double vol = 10.0 * log10(bd_pre_avg_vol);
			double vol2 = 10.0 * log10(bd_decode->get_avg_vol());
			feedback += str_vfmt(lang_pack.get_text(Lang_Type::Core, 2), std::format("{:3f}{}{:3f}", vol, lang_pack.get_text(Lang_Type::General, 2), vol2));//"   响度：**.*->**.*dB"
		}
		feed_func(feedback.c_str(), feedback.length());
	}
	return 0;
}
int BDMatchCore::feedback_time(const Procedure& proc)
{
	if (feed_func) {
		auto clock_end = std::chrono::high_resolution_clock::now();
		double spend = std::chrono::duration<double>(clock_end - clock_start).count();
		std::string feedback;
		feedback += str_vfmt(lang_pack.get_text(Lang_Type::Core, static_cast<int>(proc)), 
			spend, lang_pack.get_text(Lang_Type::General, 3));//"\n解码（/匹配）时间：**.***秒"
		feed_func(feedback.c_str(), feedback.length());
	}
	return 0;
}
int BDMatchCore::feedback_match()
{
	std::string_view feedback = match->get_feedback();
	if (feed_func)feed_func(feedback.data(), feedback.size());
	return 0;
}
