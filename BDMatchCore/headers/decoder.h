#pragma once
#include "datastruct.h"
#include <atomic>
#include <string>
#include <memory>
#include "language_pack.h"
extern"C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <libavutil/frame.h>
#include <libavutil/mem.h>
#include "libswscale/swscale.h"
#include <libavutil/channel_layout.h>
#include "libavutil/md5.h"
#include "libavutil/opt.h"

#include "libavfilter/avfilter.h"
#include "libavfilter/buffersink.h"
#include "libavfilter/buffersrc.h"

#include <libswresample/swresample.h>
#include <fftw3.h>
}

namespace Decode {

	typedef void(__stdcall *prog_func)(int, double);

	struct FFmpeg
	{
	public:
		~FFmpeg();
		AVFormatContext * filefm = NULL;
		AVCodecContext *codecfm = NULL;
		AVCodec *codec = NULL;
		AVPacket *packet = NULL;
		uint8_t **dst_data = NULL;
		AVFrame *decoded_frame = NULL;
		double **sample_seqs = nullptr;
		struct SwrContext *swr_ctx = NULL;
	};
	
	class Decode
	{
	public:
		Decode(language_pack& lang_pack0, std::shared_ptr<std::atomic_flag> keep_processing0 = nullptr);
		~Decode();
		int load_settings(const int &fft_num0, const bool &output_pcm0, const int &min_db0, 
			const int &resamp_rate0, const int &prog_type0,	fftw_plan plan0, const prog_func &prog_single0 = nullptr);
		int initialize(const std::string &file_name0);
		int decode_audio();
		std::string get_feedback();
		std::string get_file_name();
		int get_return();
		int get_fft_samp_num();
		int get_milisec();
		int get_channels();
		int get_samp_rate();
		int get_fft_num();
		bool get_audio_only();
		DataStruct::node** get_fft_data();
		char** get_fft_spec();
		double get_avg_vol();
		int set_vol_mode(const int &input);
		int set_vol_coef(const double &input);
	protected:
		void sub_prog_back(int type, double val);
		int clear_fft_data();
		int clear_ffmpeg();
		virtual int normalize(uint8_t ** const &audiodata, double ** &normalized_samples, double ** &seqs, 
			int &nb_last, const int &nb_samples);
		virtual int FFT(DataStruct::node** nodes, double** in, int fft_index, const int nb_fft);
		virtual int FD8(double* inseq, DataStruct::node* outseq);
		std::shared_ptr<std::atomic_flag> const keep_processing;//multithreading cancel token
		//language pack
		language_pack& lang_pack;
		//settings
		int fft_num = 512;
		int min_db = -14;
		bool output_pcm = false;
		int vol_mode = -1;
		int resamp_rate = 0;
		fftw_plan plan = nullptr;
		//fft data
		DataStruct::node** fft_data = nullptr;
		char** fft_spec = nullptr;
		//audio info
		std::string file_name;
		int out_bit_depth = 0;
		int audio_stream = 0;
		int milisec = 0;
		int fft_samp_num = 0;
		int e_fft_num = 0;
		int channels = 0;//audio channels
		int data_channels = 0;//audio data channels
		int real_ch = 0;//Planar: channels, Linear: 1
		long long start_time = 0;
		bool audio_only = false;
		//sample info
		int sample_type = 0;
		int sample_rate = 0;
		//progress bar and return
		prog_func prog_single = nullptr;//func_ptr for progress bar
		std::string feedback = "";
		int return_val = -100;
		int prog_type = 0;
		int decoded_num = 0;
		//fix audio volume
		double prog_val = 0.0;
		double total_vol = 0.0;
		double vol_coef = 0.0;
		double c_min_db = 0.0;
		//ffmpeg
		FFmpeg *ffmpeg = nullptr;
	};

	class Decode_SSE :public Decode {
	public:
		Decode_SSE(language_pack& lang_pack0, std::shared_ptr<std::atomic_flag> keep_processing0 = nullptr)
			:Decode(lang_pack0, keep_processing0) {}
		int FFT(DataStruct::node** nodes, double** in, int fft_index, const int nb_fft);
		int FD8(double* inseq, DataStruct::node* outseq);
	};

	class Decode_AVX :public Decode {
	public:
		Decode_AVX(language_pack& lang_pack0, std::shared_ptr<std::atomic_flag> keep_processing0 = nullptr)
			:Decode(lang_pack0, keep_processing0) {}
		int FFT(DataStruct::node** nodes, double** in, int fft_index, const int nb_fft);
		int FD8(double* inseq, DataStruct::node* outseq);
	};

	class Decode_AVX2 :public Decode {
	public:
		Decode_AVX2(language_pack& lang_pack0, std::shared_ptr<std::atomic_flag> keep_processing0 = nullptr)
			:Decode(lang_pack0, keep_processing0) {}
		int normalize(uint8_t ** const &audiodata, double ** &normalized_samples, double ** &seqs,
			int &nb_last, const int &nb_samples);
		int FFT(DataStruct::node** nodes, double** in, int fft_index, const int nb_fft);
		int FD8(double* inseq, DataStruct::node* outseq);
	};

}

