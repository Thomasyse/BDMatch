/*
CLI,cpp
A simple example of command line usage of BDMatchCore
*/
//#define _CLI_

#ifdef _CLI_
#include <iostream>
#include "headers/BDMatchCore.h"

constexpr const char* version = "1.1.5";

void print(const char* in, const int64_t len) {
	std::cout << in;
}

template<typename T>
class setting {
public:
	setting(const T& _val, const char* _flag, const char* _tip)
		:val(_val), flag(_flag), tip(_tip) {}
	T val;
	std::string flag, tip;
};

int main(int argc, char* argv[]) {
	//Setteings(default value as below):
	std::vector<setting<bool>> bool_settings = { 
		{false, "-opcm", "output decoded wave file"},
		{false, "-padec", "parallel decode, recommended for disk with good performance"},
		{false, "-volma", "match the volume of BD to TV, strongly recommended for large volume difference between TV and BD"}, 
		{false, "-nmasub", "not to match the subtitle"}, 
		{false, "-fastma", "match with calculation of some lines, fast but perhaps lose some pecision"}, 
		{false, "-debug", "show some debug info"} };
	std::vector<setting<int>> int_settings = {
		{3, "-isa", "ISA mode:0 for no SIMD, 1 for SSE/SSE2/SSE4.1, 2 for AVX, 3 for AVX2 (default: 3)"},
		{512, "-fftn", "FFT window size, larger for speed, smaller for precision(uncertain), should be a power of 2 (default: 512)"},
		{-14, "-mindb", "volume(db) threshold for noise filter(default: -14)"},
		{100, "-mincfn", "confirm times for match results, larger(ie. 10000) for precision, smaller for speed (default: 100)"},
		{10, "-range", "range for searching the results(-xx -> +xx), with the unit of second (default: 10)"},
		{0, "-offset", "offset of the timeline of the subtitle, with the unit of centisecond (default: 0)"},
		{20, "-maxlen", "the max length of the timeline to be matched, with the unit of second (default: 20)"} };
		{0, "-lang", "Language (en-US: 0, zh-CN: 1)" }
};
	//help
	std::string help_s = "--help";
	if (argc == 2) {
		if (argc == 2 && std::string(argv[1]) == help_s) {
			std::cout << "Usage:" << std::endl <<
				"bdmatch TV_file BD_file subtitle_file [Options]" << std::endl <<
				"Verision:" << std::endl << "bdmatch -v" << std::endl <<
				"Options:" << std::endl;
			for (auto& i : bool_settings) std::cout << "\t" << i.flag << "\t\t" << i.tip << std::endl;
			for (auto& i : int_settings) std::cout << "\t" << i.flag << " <arg>\t" << i.tip << std::endl;
			std::cout << "\t" << "-o <file>\toutput subtitle file path" << std::endl;
			return 0;
		}
		else if (std::string(argv[1]) == "-v") {
			std::cout << "BDMatchCore" << std::endl << "Version " << version << std::endl <<
				"Copyright (c) 2024, Thomasys" << std::endl;
			return 0;
		}
	}
	if (argc < 3) {
		std::cout << "insufficient inputs, type 'bdmatch " << help_s << "' for help." << std::endl;
		return 0;
	}
	//file paths
	const char* tv_path = argv[1];//TV file path
	const char* bd_path = argv[2];//BD file path
	const char* sub_path = argv[3];//subtitle file path
	const char* output_path = "";//output ASS file path, "" for auto rename.
	//read argv
	for (int i = 4; i < argc; i++) {
		bool fin = false;
		std::string str = argv[i];
		for (auto& s : bool_settings)
			if (str == s.flag) {
				s.val = true;
				fin = true;
			}
		if (fin)continue;
		for (auto& s : int_settings)
			if (str == s.flag) {
				if (++i < argc)s.val = std::atoi(argv[i]);
				else {
					std::cout << "option value required, type 'bdmatch " << help_s << "' for help." << std::endl;
					return 0;
				}
				fin = true;
			}
		if (str == "-o") {
			if (++i < argc)output_path = argv[i];
			else {
				std::cout << "option value required, type 'bdmatch " << help_s << "' for help." << std::endl;
				return 0;
			}
			fin = true;
		}
		if (!fin) {
			std::cout << "unsupported options, type 'bdmatch " << help_s << "' for help." << std::endl;
			return 0;
		}
	}
	//set up BDMatchCore
	BDMatchCore* match_core = new BDMatchCore;
	prog_func prog_ptr = nullptr;//function pointer to show progress: prog_func(int phase(0-3), double percent);
	feedback_func feedback_ptr = print;//function pointer to show feedback: feedback_func(const char* feedback_string);
	if (int_settings[7].val == 1)match_core->set_language("zh-CN");
	else match_core->set_language("en-US");
	match_core->load_interface(prog_ptr, feedback_ptr);
	match_core->load_settings(static_cast<ISA_Mode>(int_settings[0].val), int_settings[1].val, int_settings[2].val,
		bool_settings[0].val, bool_settings[1].val, bool_settings[2].val,
		int_settings[3].val, int_settings[4].val, int_settings[5].val, int_settings[6].val,
		!bool_settings[3].val, bool_settings[4].val, bool_settings[5].val);
	match_core->start_process();
	//decode
	int re = 0;
	re = match_core->decode(tv_path, bd_path);
	if (re < 0) return re;
	if (!bool_settings[3].val) {
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
	std::cout << std::endl;
	delete match_core;
	return 0;
}

#endif // _CLI_
