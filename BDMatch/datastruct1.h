#pragma once

namespace DataStruct1 {
	
	using namespace System;

	public delegate void ProgressCallback(int type, double val = 0);
	public delegate void FeedbackCallback(const char *input);

	enum SettingType { FFTNum, MinCheckNum, FindField, MinFinddB, MaxLength, AssOffset, OutputPCM, Draw, MatchAss, ParallelDecode, FastMatch, VolMatch };

	public ref struct SettingVals {
	public:
		SettingVals();
		SettingVals(SettingVals ^ in);
		String^ getname(const SettingType &type);
		int getval(const SettingType &type);
		int setval(const SettingType &type, int val);
		int fft_num = 512;
		int min_find_db = -12;
		int find_field = 10;
		int max_length = 20;
		int min_check_num = 80;
		int ass_offset = 0;
		bool output_pcm = false;
		bool draw = false;
		bool match_ass = true;
		bool parallel_decode = false;
		bool fast_match = false;
		bool vol_match = false;
	};
}


