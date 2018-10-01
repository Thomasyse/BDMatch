#pragma once
#include "datastruct.h"
#include <msclr\marshal_cppstd.h>  
#include <string>
#include <complex>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
extern "C" {
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

namespace BDMatch{
	using namespace System;
	using namespace System;
	using namespace System::Collections::Concurrent;
	using namespace System::Collections::Generic;
	using namespace System::Threading::Tasks;
	using namespace System::Data;
	using namespace msclr::interop;
	using namespace DataStruct;

	public delegate void ProgressCallback(int type, double val = 0);

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

	class Normalization 
	{
	public:
		Normalization(uint8_t ** const &audiodata0, double ** &normalized_samples0, double ** &seqs0, int &nb_last_seq,
			const int &realch0, const int &filech0, const int &nb_samples0, const int &sampletype0, const int &FFTnum0,
			const int &vol_mode0, double &total_vol0, const double &vol_coef0);
		virtual int getshiftf();
	protected:
		uint8_t **audiodata = nullptr;
		const int &realch;
		const int &filech;
		const int &nb_samples;
		const int &sampletype;
		const int &FFTnum;
		int &nb_last;
		double ** &seqs;
		double **&normalized_samples;
		const int &vol_mode;
		double &total_vol;
		const double &vol_coef;
	};

	class Normalizationavx2:public Normalization
	{
	public:
		Normalizationavx2(uint8_t ** const &audiodata0, double ** &normalized_samples0, double ** &seqs0, int &nb_last_seq,
			const int &realch0, const int &filech0, const int &nb_samples0, const int &sampletype0, const int &FFTnum0,
			const int &vol_mode0, double &total_vol0, const double &vol_coef0)
			:Normalization(audiodata0, normalized_samples0, seqs0, nb_last_seq, realch0, filech0, nb_samples0, sampletype0, FFTnum0,
				vol_mode0, total_vol0, vol_coef0) {}
		int getshiftf();
	};

	ref class Decode
	{
	public:
		Decode(String^ filename0, int FFTnum0, bool outputpcm0, int mindb0, int resamprate0, int progtype0,
			List<Task^>^ tasks0, System::Threading::CancellationToken canceltoken0, fftw_plan plan0, int ISAMode0, ProgressCallback^ progback0);
		~Decode();
		void decodeaudio();
		String^ getfeedback();
		String^ getfilename();
		int getreturn();
		int getfftsampnum();
		int getmilisecnum();
		int getchannels();
		int getsamprate();
		int getFFTnum();
		double get_avg_vol();
		bool getaudioonly();
		node** getfftdata();
		char** getfftspec();
		int set_vol_mode(const int &input);
		int set_vol_coef(const double &input);
		ProgressCallback^ progback = nullptr;
	private:
		bool add_fft_task(node** &fftdata, fftw_plan &p, double **&sample_seq, const int &FFTnum,
			const double&c_mindb, const int &ISAMode, const int &filech, const int &nb_fft_samples);
		void subprogback(int type, double val);
		int clearfftdata();
		int clearffmpeg();
		String^ filename;
		String^ feedback;
		node** fftdata = nullptr;
		char** fft_spec = nullptr;
		List<Task^>^ tasks = nullptr;
		System::Threading::CancellationToken canceltoken;
		fftw_plan plan;
		int FFTnum = 512;
		int out_bitdepth = 0;
		int audiostream = 0;
		int milisecnum = 0;
		int fftsampnum = 0;
		int efftnum = 0;
		int samplerate = 0;
		int resamprate = 0;
		int mindb = 0;
		int channels = 0;
		int returnval = -100;
		int progtype = 0;
		int decodednum = 0;
		long long start_time = 0;
		int ISAMode = 0;
		double c_mindb = 0.0;
		double progval = 0.0;
		double total_vol = 0.0;
		double vol_coef = 0.0;
		int vol_mode = -1;
		bool outputpcm = false;
		bool audioonly = false;
		FFmpeg *ffmpeg = nullptr;
	};

	class FFTCal
	{
	public:
		FFTCal(node** & nodes0, fftw_plan &p0, double**& in0,const int &FFTnum0,
			const double &c_mindb0, const int &filech0, const int &fft_index0, const int &nb_fft0);
		~FFTCal();
		virtual void FFT();
		virtual int FD8(double* inseq, node* outseq);
	protected:
		int fft_index = 0;
		int nb_fft = 0;
		int FFTnum = 0;
		int filech = 0;
		double c_mindb = 0;
		node** nodes = nullptr;
		fftw_plan p = nullptr;
		double** in = nullptr;
	};

	class FFTCalsse :public FFTCal
	{
	public:
		FFTCalsse(node** & nodes0, fftw_plan &p0, double**& in0, const int &FFTnum0,
			const double &c_mindb0, const int &filech0, const int &fft_index0, const int &nb_fft0)
			:FFTCal(nodes0, p0, in0, FFTnum0, c_mindb0, filech0, fft_index0, nb_fft0) {}
		void FFT();
		int FD8(double* inseq, node* outseq);
	};

	class FFTCalavx :public FFTCal
	{
	public:
		FFTCalavx(node** & nodes0, fftw_plan &p0, double**& in0, const int &FFTnum0,
			const double &c_mindb0, const int &filech0, const int &fft_index0, const int &nb_fft0)
			:FFTCal(nodes0, p0, in0, FFTnum0, c_mindb0, filech0, fft_index0, nb_fft0) {}
		void FFT();
		int FD8(double* inseq, node* outseq);
	};

	ref class FFTC
	{
	public:
		FFTC(node** & fftdata, fftw_plan &p, double**& in, const int& FFTnum, const double &c_mindb,
			const int &ISAMode, const int &filech, const int &fft_index, const int &nb_fft0, ProgressCallback^ progback0);
		~FFTC();
		void FFT();
	private:
		FFTCal * fftcal = nullptr;
		int nb_fft = 0;
		ProgressCallback^ progback = nullptr;
	};

}

