// No need for platform other than Windows
#include "include/BDMatchCoreAPI.h"
#include "headers/BDMatchCore.h"
#include "include/instructionset.h"

std::unique_ptr<BDMatchCore> match_core;
const InstructionSet::InstructionSet_Internal InstructionSet::CPU_Rep;
int ISA_mode = 0;
std::string CPU_Brand = "";

BDMatchCore_API int BDMatchCoreAPI::new_BDMatchCore()
{
	if (!match_core) {
		match_core.reset(new BDMatchCore);
		cal_ISA_mode();
		CPU_Brand = InstructionSet::Brand();
	}
	return 0;
}

BDMatchCore_API int BDMatchCoreAPI::clear_data()
{
	return match_core->clear_data();
}

BDMatchCore_API int BDMatchCoreAPI::load_interface(const prog_func& prog_back, const feedback_func& feed_func)
{
	return match_core->load_interface(prog_back, feed_func);
}

BDMatchCore_API int BDMatchCoreAPI::load_settings(const int& isa_mode0, const int& fft_num, const int& min_db,
	const bool& output_pcm, const bool& parallel_decode, const bool& vol_match, 
	const int& min_check_num, const int& find_field, const int& sub_offset, const int& max_length,
	const bool& match_ass, const bool& fast_match, const bool& debug_mode)
{
	if (isa_mode0 >= 0)ISA_mode = isa_mode0;
	return match_core->load_settings(ISA_mode, fft_num, min_db, 
		output_pcm, parallel_decode, vol_match, 
		min_check_num, find_field, sub_offset, max_length,
		match_ass, fast_match, debug_mode);
}

BDMatchCore_API int BDMatchCoreAPI::decode(const char* tv_path, const char* bd_path)
{
	return match_core->decode(tv_path, bd_path);
}

BDMatchCore_API int BDMatchCoreAPI::match_1(const char* sub_path)
{
	return match_core->match_1(sub_path);
}

BDMatchCore_API int BDMatchCoreAPI::match_2(const char* output_path)
{
	return match_core->match_2(output_path);
}

BDMatchCore_API int BDMatchCoreAPI::clear_match()
{
	return match_core->clear_match();
}

BDMatchCore_API size_t BDMatchCoreAPI::get_nb_timeline()
{
	return match_core->get_nb_timeline();
}

BDMatchCore_API int BDMatchCoreAPI::get_timeline(const int& index, const Match::Timeline_Time_Type& type)
{
	return match_core->get_timeline(index, type);
}

BDMatchCore_API int BDMatchCoreAPI::get_decode_info(const Decode::Decode_File& file, const Decode::Decode_Info& type)
{
	return match_core->get_decode_info(file, type);
}

BDMatchCore_API char** BDMatchCoreAPI::get_decode_spec(const Decode::Decode_File& file)
{
	return match_core->get_decode_spec(file);
}

BDMatchCore_API int BDMatchCoreAPI::search_ISA_mode()
{
	return ISA_mode;
}

BDMatchCore_API const char* BDMatchCoreAPI::get_CPU_brand()
{
	return CPU_Brand.c_str();
}

BDMatchCore_API int BDMatchCoreAPI::start_process()
{
	return match_core->start_process();
}

BDMatchCore_API int BDMatchCoreAPI::stop_process()
{
	return match_core->stop_process();
}

int BDMatchCoreAPI::cal_ISA_mode()
{
	if (InstructionSet::AVX2() && InstructionSet::AVX())ISA_mode = 3;
	else if (InstructionSet::AVX())ISA_mode = 2;
	else if (InstructionSet::SSE41() && InstructionSet::SSE2() && InstructionSet::SSSE3())ISA_mode = 1;
	else ISA_mode = 0;
	return 0;
}

