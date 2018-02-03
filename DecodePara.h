#pragma once
#include "datastruct.h"
#include <msclr\marshal_cppstd.h>  
#include <string>
#include <complex>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
extern "C" {
#include "libavcodec\avcodec.h"
#include "libavformat\avformat.h"
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <libavutil/frame.h>
#include <libavutil/mem.h>
#include "libswscale\swscale.h"
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

	struct FFmpeg {
	public:
		~FFmpeg();
		AVFormatContext * filefm = NULL;
		AVCodecContext *codecfm = NULL;
		AVCodec *codec = NULL;
		AVPacket *packet = NULL;
		uint8_t *temp = NULL;
		uint8_t **dst_data = NULL;
		noded* sample_seq_l = nullptr;
		noded* sample_seq_r = nullptr;
		AVFrame *decoded_frame = NULL;
		struct SwrContext *swr_ctx = NULL;
	};

	ref class Decode
	{
	public:
		Decode(String^ filename0, int FFTnum0, bool outputpcm0, int mindb0, int resamprate0, int progtype0,
			List<Task^>^ tasks0, System::Threading::CancellationToken canceltoken0, fftw_plan plan0, ProgressCallback^ progback0);
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
		double getsampleratio();
		std::vector<std::vector<node*>>* getfftdata();
		ProgressCallback^ progback = nullptr;
	private:
		double getshiftf(uint8_t * temp, int &sampletype, const int &start);
		void subprogback(int type, double val);
		int clearfftdata();
		String^ filename;
		String^ feedback;
		std::vector<std::vector<node*>>* fftdata = nullptr;
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
		int returnval = 0;
		int progtype = 0;
		int decodednum = 0;
		double sampleratio = 1.0;
		double progval = 0.0;
		bool outputpcm = false;
		
		FFmpeg *ffmpeg = nullptr;
	};

	ref class FFTC
	{
	public:
		FFTC(node* fftseq0, fftw_plan p0, double* in0, int mindb0, ProgressCallback^ progback0);
		~FFTC();
		void FFT();
	private:
		int FFTnum;
		int mindb;
		ProgressCallback^ progback = nullptr;
		node* fftseq = nullptr;
		fftw_plan p = nullptr;
		fftw_complex* out = nullptr;
		double *in = nullptr;
		int FD8(double* inseq, node* outseq);
	};
}

