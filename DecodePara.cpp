#include "DecodePara.h"

using namespace BDMatch;

BDMatch::Decode::Decode(String^ filename0, int FFTnum0, bool outputpcm0, int mindb0, int resamprate0, int progtype0,
	List<Task^>^ tasks0, System::Threading::CancellationToken canceltoken0, fftw_plan plan0, int ISAMode0, ProgressCallback^ progback0)
{
	filename = filename0;
	FFTnum = FFTnum0;
	outputpcm = outputpcm0;
	progback = progback0;
	mindb = mindb0;
	resamprate = resamprate0;
	progtype = progtype0;
	tasks = tasks0;
	canceltoken = canceltoken0;
	plan = plan0;
	ISAMode = ISAMode0;
}

BDMatch::Decode::~Decode()
{
	clearfftdata();
	if (ffmpeg) { 
		delete ffmpeg; 
		ffmpeg = nullptr;
	}
	plan = nullptr;
}

void BDMatch::Decode::decodeaudio()
{
	using namespace System::IO;
	using namespace System::Threading;
	ffmpeg = new FFmpeg;
	AVFormatContext *&filefm = ffmpeg->filefm;
	AVCodecContext *&codecfm = ffmpeg->codecfm;
	AVCodec *&codec = ffmpeg->codec;
	AVPacket *&packet = ffmpeg->packet;
	uint8_t **&dst_data = ffmpeg->dst_data;
	AVFrame *&decoded_frame = ffmpeg->decoded_frame;
	struct SwrContext *&swr_ctx = ffmpeg->swr_ctx;
	double **&sample_seqs = ffmpeg->sample_seqs;

	filefm = NULL;//文件格式
	std::string filestr = marshal_as<std::string>(filename->ToString());
	if (avformat_open_input(&filefm, filestr.c_str(), NULL, NULL) != 0) {//获取文件格式
		feedback = "无法打开文件！";
		returnval = -1;
		return;
	}
	if (avformat_find_stream_info(filefm, NULL) < 0) {//获取文件内音视频流的信息
		feedback = "无法读取文件流信息！";
		returnval = -1;
		return;
	}
	codecfm = NULL;//编码格式
	codec = NULL;//解码器
	unsigned int j;
	// Find the first audio stream
	audiostream = -1;
	for (j = 0; j<filefm->nb_streams; j++)//找到音频对应的stream
		if (filefm->streams[j]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			audiostream = j;
			break;
		}
	if (audiostream == -1)
	{
		feedback = "无音频流！";
		returnval = -2;
		return; // Didn't find a audio stream
	}
	else if (audiostream == 0) {
		audioonly = true;
	}
	milisecnum = static_cast<int>(ceil(filefm->duration / 10000));
	codec = avcodec_find_decoder(filefm->streams[audiostream]->codecpar->codec_id);//寻找解码器
	if (!codec)
	{
		feedback = "无法找到音频的对应解码器！";
		returnval = -3;
		return; // Codec not found codec
	}
	codecfm = avcodec_alloc_context3(codec);//音频的编码信息
	if (!codecfm)
	{
		feedback = "无法创建音频的解码器信息！";
		returnval = -3;
		return; // Failed to allocate the codec context
	}
	int getcodecpara = avcodec_parameters_to_context(codecfm, filefm->streams[audiostream]->codecpar);
	if (getcodecpara < 0) {
		feedback = "无法获取音频的解码器信息！";
		returnval = -3;
		return; // Failed to allocate the codec context
	}

	feedback = "音轨编号：" + (audiostream).ToString() +
		"    音频编码：" + marshal_as<String^>(codec->long_name);

	if (avcodec_open2(codecfm, codec, NULL) < 0)//将两者结合以便在下面的解码函数中调用pInCodec中的对应解码函数
	{
		feedback = "无法打开音频的对应解码器！";
		returnval = -3;
		return; // Could not open codec
	}
	//延迟
	start_time = filefm->streams[audiostream]->start_time;
	//采样率
	samplerate = codecfm->sample_rate;
	//重采样变量1
	bool resamp = false;
	if (resamprate > 0 && samplerate != resamprate)resamp = true;
	//准备写入文件
	FILE* pcm = NULL;
	int sampletype = 0;
	Int16 pcmh = 0;
	int pcmh32 = 0;
	std::string filename;
	if (outputpcm) {//写入wav头信息
		filename = filefm->url;
		filename += ".wav";
		pcm = fopen(filename.c_str(), "wb");
		if (pcm == NULL) {
			feedback = "无法打开要写入的PCM文件！";
			returnval = -4;
			return;
		}
		fwrite("RIFF", 4, 1, pcm);
		fwrite(&pcmh, 4, 1, pcm);//file size
		fwrite("WAVE", 4, 1, pcm);
		fwrite("fmt ", 4, 1, pcm);
		pcmh32 = 16;
		fwrite(&pcmh32, 4, 1, pcm);//16
		if (codecfm->sample_fmt == AV_SAMPLE_FMT_FLT || codecfm->sample_fmt == AV_SAMPLE_FMT_DBL ||
			codecfm->sample_fmt == AV_SAMPLE_FMT_FLTP || codecfm->sample_fmt == AV_SAMPLE_FMT_DBLP)
			pcmh = 3;
		else pcmh = 1;
		fwrite(&pcmh, 2, 1, pcm);//pcm(1) IEEE float(3)
		pcmh = codecfm->channels;
		fwrite(&pcmh, 2, 1, pcm);//channels stereo(2)
		pcmh32 = resamp ? resamprate : samplerate;
		fwrite(&pcmh32, 4, 1, pcm);//sample rate
		int byterate = av_get_bytes_per_sample(codecfm->sample_fmt)*codecfm->channels*codecfm->sample_rate;
		fwrite(&byterate, 4, 1, pcm);//byte per sec
		pcmh = 4;
		fwrite(&pcmh, 2, 1, pcm);//byte per sample * channels
		pcmh = 16;
		fwrite(&pcmh, 2, 1, pcm);//bit per sample
		fwrite("data", 4, 1, pcm);//data
		pcmh = 0;
		fwrite(&pcmh, 4, 1, pcm);//pcm size
	}
	//确定输出位深
	switch (codecfm->sample_fmt) {
	case AV_SAMPLE_FMT_U8:
		sampletype = 8;
		break;
	case AV_SAMPLE_FMT_U8P:
		sampletype = 8;
		break;
	case AV_SAMPLE_FMT_S16:
		sampletype = 16;
		break;
	case AV_SAMPLE_FMT_S16P:
		sampletype = 16;
		break;
	case AV_SAMPLE_FMT_S32:
		sampletype = 32;
		break;
	case AV_SAMPLE_FMT_S32P:
		sampletype = 32;
		break;
	case AV_SAMPLE_FMT_S64:
		sampletype = 64;
		break;
	case AV_SAMPLE_FMT_S64P:
		sampletype = 64;
		break;
	}
	if (codecfm->sample_fmt == AV_SAMPLE_FMT_FLT || codecfm->sample_fmt == AV_SAMPLE_FMT_FLTP) sampletype = 1;
	else if (codecfm->sample_fmt == AV_SAMPLE_FMT_DBL || codecfm->sample_fmt == AV_SAMPLE_FMT_DBLP) sampletype = 2;
	//采样变量
	int samplecount = 0, samplenum = 0;
	int fftnum = FFTnum;
	c_mindb = 256.0 / (15.0 - static_cast<double>(mindb));
	double c_mindb0 = c_mindb;
	int isamode = ISAMode;
	channels = codecfm->channels;
	//重采样变量2
	dst_data = NULL;
	int dst_linesize;
	//对音频解码(加重采样)
	if (resamp) {
		samplenum = static_cast<int>(ceil(resamprate / 1000.0*filefm->duration / 1000.0));
		swr_ctx = swr_alloc();
		if (!swr_ctx) {
			feedback = "无法构建重采样环境！";
			returnval = -8;
			return;
		}
		//重采样选项
		av_opt_set_int(swr_ctx, "in_channel_layout", codecfm->channel_layout, 0);
		av_opt_set_int(swr_ctx, "in_sample_rate", samplerate, 0);
		av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt", codecfm->sample_fmt, 0);

		av_opt_set_int(swr_ctx, "out_channel_layout", codecfm->channel_layout, 0);
		av_opt_set_int(swr_ctx, "out_sample_rate", resamprate, 0);
		av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt", codecfm->sample_fmt, 0);
		/* initialize the resampling context */
		if ((swr_init(swr_ctx)) < 0) {
			feedback = "无法初始化重采样环境！";
			returnval = -8;
			return;
		}
	}
	else {
		samplenum = static_cast<int>(ceil(samplerate / 1000.0*filefm->duration / 1000.0));
	}
	efftnum = static_cast<int>(ceil(samplenum / float(FFTnum)));//estimated_fft_num

	packet = av_packet_alloc();
	int realch = 1;
	String ^chfmt = "Packed";
	bool isplanar = av_sample_fmt_is_planar(codecfm->sample_fmt);
	if (isplanar) {
		realch = codecfm->channels;
		chfmt = "Planar";
	}
	fftdata = new std::vector<std::vector<node*>>(min(codecfm->channels, 2), std::vector<node*>(efftnum));
	std::vector<std::vector<node*>>* fftdata0 = fftdata;
	fftw_plan plan0 = plan;
	fftsampnum = 0;
	sample_seqs = new double*[codecfm->channels];
	for (int i = 0; i < codecfm->channels; i++) {
		sample_seqs[i] = (double*)fftw_malloc(sizeof(double)*FFTnum);
	}
	int nb_last_seq = 0;
	decoded_frame = NULL;
	while (av_read_frame(filefm, packet) >= 0)//file中调用对应格式的packet获取函数
	{
		if (packet->stream_index == audiostream)//如果是音频
		{
			int data_size = 0;
			if (!decoded_frame) {
				if (!(decoded_frame = av_frame_alloc())) {
					feedback = "无法为音频帧分配内存！";
					returnval = -5;
					if (outputpcm)fclose(pcm);
					return;
				}
			}
			int ret = 0;
			ret = avcodec_send_packet(codecfm, packet);
			if (ret < 0) {
				feedback = "无法提交音频至解码器！";
				returnval = -5;
				if (outputpcm)fclose(pcm);
				return;
			}
			int len = 0;
			while (len >= 0) {
				len = avcodec_receive_frame(codecfm, decoded_frame);
				if (len == AVERROR(EAGAIN) || len == AVERROR_EOF)
					break;
				else if (len < 0) {
					feedback = "音频解码出错！";
					returnval = -6;
					if (outputpcm)fclose(pcm);
					return;
				}
				data_size = av_get_bytes_per_sample(codecfm->sample_fmt);
				if (data_size < 0) {
					/* This should not occur, checking just for paranoia */
					feedback = "无法计算音频数据大小！";
					returnval = -7;
					if (outputpcm)fclose(pcm);
					return;
				}
				//从frame中获取数据
				if (sampletype == 32) {
					if (codecfm->bits_per_raw_sample == 24)sampletype = 24;
				}
				int nb_samples;
				uint8_t **audiodata;
				//重采样
				if (resamp) {
					nb_samples = static_cast<int>(
						av_rescale_rnd(decoded_frame->nb_samples, resamprate, samplerate, AV_ROUND_ZERO));
					ret = av_samples_alloc_array_and_samples(&dst_data, &dst_linesize, channels,
						nb_samples, codecfm->sample_fmt, 0);
					if (ret < 0) {
						feedback = "无法为重采样数据分配内存！";
						if (outputpcm)fclose(pcm);
						returnval = -9;
						return;
					}
					ret = swr_convert(swr_ctx, dst_data, nb_samples, 
						(const uint8_t **)decoded_frame->extended_data, decoded_frame->nb_samples);
					if (ret < 0) {
						feedback = "重采样错误！";
						if (outputpcm)fclose(pcm);
						returnval = -10;
						return;
					}
					audiodata = dst_data;
				}
				else {
					nb_samples = decoded_frame->nb_samples;
					audiodata = decoded_frame->extended_data;
				}				
				//处理数据
				Normalization *normlz;
				double **normalized_samples;
				if (ISAMode == 3) normlz = new Normalizationavx2(audiodata, normalized_samples, sample_seqs, nb_last_seq, realch, codecfm->channels, nb_samples, sampletype, fftnum);
				else normlz = new Normalization(audiodata, normalized_samples, sample_seqs, nb_last_seq, realch, codecfm->channels, nb_samples, sampletype, fftnum);
				int nb_sample_fft= normlz->getshiftf();
				delete normlz;
				bool result = add_fft_task(fftdata0, plan0, normalized_samples, fftnum, c_mindb0, isamode, min(2, codecfm->channels), nb_sample_fft);
				if (!result) {
					if (dst_data)
						av_freep(&dst_data[0]);
					av_freep(&dst_data);
					av_frame_free(&decoded_frame);
					av_packet_unref(packet);
					delete ffmpeg;
					ffmpeg = nullptr;
					return;
				}
				//输出pcm数据
				if (outputpcm) {
					if (!isplanar) {
						data_size *= codecfm->channels;
						fwrite(audiodata[0], nb_samples, data_size, pcm);
					}
					else {
						for (int i = 0; i < nb_samples; i++)
							for (int ch = 0; ch < realch; ch++)
								fwrite(audiodata[ch] + i * data_size, 1, data_size, pcm);
					}
				}
				if (dst_data)
					av_freep(&dst_data[0]);
				av_freep(&dst_data);
			}
			av_frame_free(&decoded_frame);
		}
		av_packet_unref(packet);
	}
	//数据存入pcmdata
	int count = codecfm->frame_number;
	int bit_depth_raw = codecfm->bits_per_raw_sample;
	out_bitdepth = av_get_bytes_per_sample(codecfm->sample_fmt) * 8;
	String^ samprateinfo = "";
	if (resamp)samprateinfo = samplerate.ToString() + "Hz -> " + resamprate.ToString() + "Hz";
	else samprateinfo = samplerate.ToString() + "Hz";
	feedback += "    声道：" + channels.ToString() + "    总帧数：" + count.ToString() + "    格式：" + chfmt +
		"\r\n采样位数：" + bit_depth_raw.ToString() + "bit -> " + (out_bitdepth).ToString() + "bit    采样率：" + samprateinfo
		+ "    采样格式："
		+ marshal_as<String^>(av_get_sample_fmt_name(codecfm->sample_fmt))->Replace("AV_SAMPLE_FMT_", "")->ToUpper();
	if (start_time != 0)feedback += "    延迟：" + start_time.ToString() + "ms";
	//补充wav头信息
	if (outputpcm) {
		long pcmfilesize = ftell(pcm) - 8;
		fseek(pcm, 32L, SEEK_SET);
		pcmh = av_get_bytes_per_sample(codecfm->sample_fmt)*channels;
		fwrite(&pcmh, 2, 1, pcm);//byte per sample * channels
		fseek(pcm, 34L, SEEK_SET);
		pcmh = out_bitdepth;
		fwrite(&pcmh, 2, 1, pcm);//bit per sample
		fseek(pcm, 4L, SEEK_SET);
		fwrite(&pcmfilesize, 4, 1, pcm);//file size
		fseek(pcm, 40L, SEEK_SET);
		pcmfilesize -= 36;
		fwrite(&pcmfilesize, 4, 1, pcm);//pcm size
		fclose(pcm);
		feedback += "\r\n输出解码音频：" + marshal_as<String^>(filename);
	}
	//释放内存
	returnval = 0;
	delete ffmpeg;
	ffmpeg = nullptr;
	return;
}

String ^ BDMatch::Decode::getfeedback()
{
	return feedback;
}
String ^ BDMatch::Decode::getfilename()
{
	return filename;
}

int BDMatch::Decode::getreturn()
{
	return returnval;
}
int BDMatch::Decode::getfftsampnum()
{
	return fftsampnum;
}
int BDMatch::Decode::getmilisecnum()
{
	return milisecnum;
}
int BDMatch::Decode::getchannels()
{
	return channels;
}
int BDMatch::Decode::getsamprate()
{
	return samplerate;
}
int BDMatch::Decode::getFFTnum()
{
	return FFTnum;
}
bool BDMatch::Decode::getaudioonly()
{
	return audioonly;
}

int BDMatch::Decode::clearfftdata()
{
	for (int i = 0; i < fftsampnum; i++) {
		if ((*fftdata)[0][i] != nullptr) {
			delete (*fftdata)[0][i];
			(*fftdata)[0][i] = nullptr;
		}
		if ((*fftdata)[1][i] != nullptr) {
			delete (*fftdata)[1][i];
			(*fftdata)[1][i] = nullptr;
		}
	}
	delete fftdata;
	fftdata = nullptr;
	return 0;
}

std::vector<std::vector<node*>>* BDMatch::Decode::getfftdata()
{
	return fftdata;
}

bool BDMatch::Decode::add_fft_task(std::vector<std::vector<node*>>* &fftdata, fftw_plan &p, double **sample_seq, const int &FFTnum,
	const double&c_mindb, const int &ISAMode, const int &filech, const int &nb_fft_samples)
{
	if (nb_fft_samples <= 0)return true;
	int fft_index = fftsampnum;
	fftsampnum += nb_fft_samples;
	FFTC^ fftc = gcnew FFTC(fftdata, p, sample_seq, FFTnum, c_mindb, ISAMode, filech, fft_index, nb_fft_samples,
		gcnew ProgressCallback(this, &Decode::subprogback));
	Task^ task = gcnew Task(gcnew Action(fftc, &FFTC::FFT), canceltoken);
	if(!task->IsCanceled)task->Start();
	else return false;
	tasks->Add(task);
	return true;
}

void BDMatch::Decode::subprogback(int type, double val)
{
	decodednum += static_cast<int>(val);
	double temp = decodednum / static_cast<double>(efftnum);
	if (temp >= progval + 0.02 || temp == 0) {
		progval = temp;
		progback(progtype, progval);
	}
}

BDMatch::FFmpeg::~FFmpeg()
{
	if (sample_seqs) {
		for (int ch = 0; ch < codecfm->channels; ch++)if (sample_seqs[ch]) {
			fftw_free(sample_seqs[ch]);
			sample_seqs[ch] = nullptr;
		}
		delete[] sample_seqs;
		sample_seqs = nullptr;
	}
	if (dst_data)
		av_freep(&dst_data[0]);
	av_freep(&dst_data);
	if (decoded_frame)av_frame_free(&decoded_frame);
	if (packet) {
		av_packet_unref(packet);
		packet = NULL;
	}
	if (codecfm)avcodec_close(codecfm);
	if (filefm)avformat_close_input(&filefm);
	if (swr_ctx)swr_close(swr_ctx);
}

BDMatch::FFTCal::FFTCal(std::vector<std::vector<node*>>*& nodes0, fftw_plan &p0, double**& in0, const int &FFTnum0,
	const double &c_mindb0, const int &filech0, const int &fft_index0, const int &nb_fft0)
{
	nodes = nodes0;
	p = p0;
	FFTnum = FFTnum0;
	c_mindb = c_mindb0;
	in = in0;
	filech = filech0;
	fft_index = fft_index0;
	nb_fft = nb_fft0;
}

BDMatch::FFTCal::~FFTCal()
{
	if (in) {
		for (int ch = 0; ch < filech; ch++)
			if (in[ch]) {
				fftw_free(in[ch]);
				in[ch] = nullptr;
			}
		delete[] in;
	}
	in = nullptr;
	p = nullptr;
	nodes = nullptr;
}
#pragma unmanaged 
void BDMatch::FFTCal::FFT()
{
	using namespace std;
	if (!nodes || !p)return;
	fftw_complex* out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*FFTnum);
	double *out_d = (double*)fftw_malloc(sizeof(double)*FFTnum / 2);
	int fi_m = nb_fft * FFTnum;
	for (int fi = 0; fi < fi_m; fi += FFTnum) {
		for (int ch = 0; ch < filech; ch++) {
			fftw_execute_dft_r2c(p, in[ch] + fi, out);
			for (int i = 0; i < FFTnum / 2; i++) {
				double real = *(out + i)[0];
				double imag = *(out + i)[1];
				out_d[i] = real * real + imag * imag;
			}
			FD8(out_d, (*nodes)[ch][fft_index]);
		}
		fft_index++;
	}
	fftw_free(out);
	fftw_free(out_d);
	for (int ch = 0; ch < filech; ch++) {
		fftw_free(in[ch]);
		in[ch] = nullptr;
	}
	delete[] in;
	in = nullptr;
	out = nullptr;
	out_d = nullptr;
	return;
}

int BDMatch::FFTCal::FD8(double* inseq, node*& outseq)
{
	char *out = outseq->getdata();
	for (int i = 0; i < outseq->size(); i++) {
		double addx = inseq[i];
		addx = 10.0 * log10(addx);
		addx -= 15.0;
		addx *= c_mindb;
		addx += 127.0;
		addx = min(addx, 127.0);
		addx = max(addx, -128.0);
		addx = round(addx);
		out[i] = static_cast<char>(addx);
	}
	out = nullptr;
	return 0;
}

void BDMatch::FFTCalsse::FFT()
{
	using namespace std;
	if (!nodes || !p)return;
	fftw_complex* out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*FFTnum);
	int fi_m = nb_fft * FFTnum;
	for (int fi = 0; fi < fi_m; fi += FFTnum) {
		for (int ch = 0; ch < filech; ch++) {
			fftw_execute_dft_r2c(p, in[ch] + fi, out);
			FD8(reinterpret_cast<double*>(out), (*nodes)[ch][fft_index]);
		}
		fft_index++;
	}
	fftw_free(out);
	for (int ch = 0; ch < filech; ch++) {
		fftw_free(in[ch]);
		in[ch] = nullptr;
	}
	delete[] in;
	in = nullptr;
	out = nullptr;
	return;
}

int BDMatch::FFTCalsse::FD8(double* inseq, node *& outseq)
{
	char *out = outseq->getdata();
	__m128d const10 = _mm_set1_pd(10.0);
	__m128d const15 = _mm_set1_pd(15.0);
	__m128d const_mindb = _mm_set1_pd(c_mindb);
	__m128d const127 = _mm_set1_pd(127.0);
	__m128d constm128 = _mm_set1_pd(-128.0);
	for (int i = 0; i < FFTnum / 4; i++) {
		__m128d seq1 = _mm_load_pd(inseq);
		__m128d seq2 = _mm_load_pd(inseq + 2);
		seq1 = _mm_mul_pd(seq1, seq1);
		seq2 = _mm_mul_pd(seq2, seq2);
		__m128d temp = _mm_hadd_pd(seq1, seq2);
		temp.m128d_f64[0] = log10(temp.m128d_f64[0]);
		temp.m128d_f64[1] = log10(temp.m128d_f64[1]);
		temp = _mm_mul_pd(temp, const10);
		temp = _mm_sub_pd(temp, const15);
		temp = _mm_mul_pd(temp, const_mindb);
		temp = _mm_add_pd(temp, const127);
		temp = _mm_min_pd(temp, const127);
		temp = _mm_max_pd(temp, constm128);
		temp = _mm_round_pd(temp, _MM_FROUND_TO_NEAREST_INT);
		__m128i temp_int = _mm_cvtpd_epi32(temp);
		out[0] = static_cast<char>(_mm_extract_epi8(temp_int, 0));
		out[1] = static_cast<char>(_mm_extract_epi8(temp_int, 4));
		inseq += 4;
		out += 2;
	}
	out = nullptr;
	return 0;
}

void BDMatch::FFTCalavx::FFT()
{
	using namespace std;
	if (!nodes || !p)return;
	int fi_m = nb_fft * FFTnum;
	fftw_complex* out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*FFTnum);
	for (int fi = 0; fi < fi_m; fi += FFTnum) {
		for (int ch = 0; ch < filech; ch++) {
			fftw_execute_dft_r2c(p, in[ch] + fi, out);
			FD8(reinterpret_cast<double*>(out), (*nodes)[ch][fft_index]);
		}
		fft_index++;
	}
	fftw_free(out);
	for (int ch = 0; ch < filech; ch++) {
		fftw_free(in[ch]);
		in[ch] = nullptr;
	}
	delete[] in;
	in = nullptr;
	out = nullptr;
	return;
}

int BDMatch::FFTCalavx::FD8(double* inseq, node *& outseq)
{
	outseq = new node(FFTnum / 2);
	char *out = outseq->getdata();
	__m256d const10 = _mm256_set1_pd(10.0);
	__m256d const15 = _mm256_set1_pd(15.0);
	__m256d const_mindb = _mm256_set1_pd(c_mindb);
	__m256d const127 = _mm256_set1_pd(127.0);
	__m256d constm128 = _mm256_set1_pd(-128.0);
	for (int i = 0; i < FFTnum / 8; i++) {
		__m256d seq1 = _mm256_load_pd(inseq);
		__m256d seq2 = _mm256_load_pd(inseq + 4);
		seq1 = _mm256_mul_pd(seq1, seq1);
		seq2 = _mm256_mul_pd(seq2, seq2);
		__m256d temp = _mm256_hadd_pd(seq1, seq2);
		temp.m256d_f64[0] = log10(temp.m256d_f64[0]);
		temp.m256d_f64[1] = log10(temp.m256d_f64[1]);
		temp.m256d_f64[2] = log10(temp.m256d_f64[2]);
		temp.m256d_f64[3] = log10(temp.m256d_f64[3]);
		temp = _mm256_mul_pd(temp, const10);
		temp = _mm256_sub_pd(temp, const15);
		temp = _mm256_mul_pd(temp, const_mindb);
		temp = _mm256_add_pd(temp, const127);
		temp = _mm256_min_pd(temp, const127);
		temp = _mm256_max_pd(temp, constm128);
		temp = _mm256_round_pd(temp, _MM_FROUND_TO_NEAREST_INT);
		__m128i temp_int = _mm256_cvtpd_epi32(temp);
		out[0] = static_cast<char>(_mm_extract_epi8(temp_int, 0));
		out[2] = static_cast<char>(_mm_extract_epi8(temp_int, 4));
		out[1] = static_cast<char>(_mm_extract_epi8(temp_int, 8));
		out[3] = static_cast<char>(_mm_extract_epi8(temp_int, 12));
		inseq += 8;
		out += 4;
	}
	out = nullptr;
	return 0;
}
#pragma managed

BDMatch::FFTC::FFTC(std::vector<std::vector<node*>>*& fftdata, fftw_plan &p, double**& in, const int& FFTnum, const double &c_mindb,
	const int &ISAMode, const int &filech, const int &fft_index, const int &nb_fft0, ProgressCallback^ progback0)
{
	if (ISAMode >= 2) fftcal = new FFTCalavx(fftdata, p, in, FFTnum, c_mindb, filech, fft_index, nb_fft0);
	else  if (ISAMode == 1) fftcal = new FFTCalsse(fftdata, p, in, FFTnum, c_mindb, filech, fft_index, nb_fft0);
	else fftcal = new FFTCal(fftdata, p, in, FFTnum, c_mindb, filech, fft_index, nb_fft0);
	progback = progback0;
	nb_fft = nb_fft0;
}

BDMatch::FFTC::~FFTC()
{
	if (fftcal) {
		delete fftcal;
		fftcal = nullptr;
	}
}

void BDMatch::FFTC::FFT()
{
	fftcal->FFT();
	delete fftcal;
	fftcal = nullptr;
	progback(1, nb_fft);
	return;
}

BDMatch::Normalization::Normalization(uint8_t ** const &audiodata0, double ** &normalized_samples0, double ** &seqs0, int &nb_last_seq,
	const int &realch0, const int &filech0, const int &nb_samples0, const int &sampletype0, const int &FFTnum0)
	:audiodata(audiodata0), normalized_samples(normalized_samples0), seqs(seqs0), nb_last(nb_last_seq),
	realch(realch0), filech(filech0), nb_samples(nb_samples0), sampletype(sampletype0), FFTnum(FFTnum0)
{
}

#pragma unmanaged
int BDMatch::Normalization::getshiftf()
{
	normalized_samples = new double*[filech];
	int nb_last_next = (nb_samples + nb_last) % FFTnum;
	int length = nb_samples + nb_last - nb_last_next;
	int nb_fft_samples = length / FFTnum;
	for (int ch = 0; ch < filech; ch++)normalized_samples[ch] = (double*)fftw_malloc(sizeof(double)*length);
	if (realch == filech) {
		if (sampletype == 1)
			for (int ch = 0; ch < filech; ch++) {
				float *tempf = reinterpret_cast<float *>(audiodata[ch]);
				for (int i = 0; i < nb_last; i++)normalized_samples[ch][i] = seqs[ch][i];
				int k = 0;
				for (int i = nb_last; i < length; i++, k++)normalized_samples[ch][i] = static_cast<double>(tempf[k]);
				for (int i = 0; i < nb_last_next; i++, k++)seqs[ch][i] = static_cast<double>(tempf[k]);
			}
		else if (sampletype == 2)
			for (int ch = 0; ch < filech; ch++) {
				double *tempd = reinterpret_cast<double *>(audiodata[ch]);
				for (int i = 0; i < nb_last; i++)normalized_samples[ch][i] = seqs[ch][i];
				int k = 0;
				for (int i = nb_last; i < length; i++, k++)normalized_samples[ch][i] = tempd[k];
				for (int i = 0; i < nb_last_next; i++, k++)seqs[ch][i] = tempd[k];
			}
		else if (sampletype == 8)
			for (int ch = 0; ch < filech; ch++) {
				int k = 0;
				for (int i = nb_last; i < length; i++, k++)normalized_samples[ch][i] = static_cast<double>(audiodata[ch][k]) / 255.0;
				for (int i = 0; i < nb_last_next; i++, k++)seqs[ch][i] = static_cast<double>(audiodata[ch][k]) / 255.0;
			}
		else if (sampletype == 16) 
			for (int ch = 0; ch < filech; ch++) {
				short *temps = reinterpret_cast<short *>(audiodata[ch]);
				for (int i = 0; i < nb_last; i++)normalized_samples[ch][i] = seqs[ch][i];
				int k = 0;
				for (int i = nb_last; i < length; i++, k++)normalized_samples[ch][i] = static_cast<double>(temps[k]) / 32767.0;
				for (int i = 0; i < nb_last_next; i++, k++)seqs[ch][i] = static_cast<double>(temps[k]) / 32767.0;
			}
		else if (sampletype == 24) 
			for (int ch = 0; ch < filech; ch++) {
				int *tempi = reinterpret_cast<int *>(audiodata[ch]);
				for (int i = 0; i < nb_last; i++)normalized_samples[ch][i] = seqs[ch][i];
				int k = 0;
				for (int i = nb_last; i < length; i++, k++)normalized_samples[ch][i] = static_cast<double>(tempi[k] >> 8) / 8388607.0;
				for (int i = 0; i < nb_last_next; i++, k++)seqs[ch][i] = static_cast<double>(tempi[k] >> 8) / 8388607.0;
			}
		else if (sampletype == 32) 
			for (int ch = 0; ch < filech; ch++) {
				int *tempi2 = reinterpret_cast<int *>(audiodata[ch]);
				for (int i = 0; i < nb_last; i++)normalized_samples[ch][i] = seqs[ch][i];
				int k = 0;
				for (int i = nb_last; i < length; i++, k++)normalized_samples[ch][i] = static_cast<double>(tempi2[k]) / 2147483647.0;
				for (int i = 0; i < nb_last_next; i++, k++)seqs[ch][i] = static_cast<double>(tempi2[k]) / 2147483647.0;
			}
		else if (sampletype == 64) 
			for (int ch = 0; ch < filech; ch++) {
				long long *templ = reinterpret_cast<long long *>(audiodata[ch]);
				for (int i = 0; i < nb_last; i++)normalized_samples[ch][i] = seqs[ch][i];
				int k = 0;
				for (int i = nb_last; i < length; i++, k++)normalized_samples[ch][i] = static_cast<double>(templ[k]) / static_cast<double>(9223372036854775807);
				for (int i = 0; i < nb_last_next; i++, k++)seqs[ch][i] = static_cast<double>(templ[k]) / static_cast<double>(9223372036854775807);
			}
	}
	else {
		if (sampletype == 1) {
			float *tempf = reinterpret_cast<float *>(audiodata[0]);
			for (int ch = 0; ch < filech; ch++)
				for (int i = 0; i < nb_last; i++)normalized_samples[ch][i] = seqs[ch][i];
			int k = 0;
			for (int i = nb_last; i < length; i++)
				for (int ch = 0; ch < filech; ch++, k++) {
					normalized_samples[ch][i] = static_cast<double>(tempf[k]);
				}
			for (int i = 0; i < nb_last_next; i++)
				for (int ch = 0; ch < filech; ch++, k++) {
					seqs[ch][i] = static_cast<double>(tempf[k]);
				}
		}
		else if (sampletype == 2) {
			double *tempd = reinterpret_cast<double *>(audiodata[0]);
			for (int ch = 0; ch < filech; ch++)
				for (int i = 0; i < nb_last; i++)normalized_samples[ch][i] = seqs[ch][i];
			int k = 0;
			for (int i = nb_last; i < length; i++)
				for (int ch = 0; ch < filech; ch++, k++) {
					normalized_samples[ch][i] = tempd[k];
				}
			for (int i = 0; i < nb_last_next; i++)
				for (int ch = 0; ch < filech; ch++, k++) {
					seqs[ch][i] = tempd[k];
				}
		}
		else if (sampletype == 8) {
			for (int ch = 0; ch < filech; ch++)
				for (int i = 0; i < nb_last; i++)normalized_samples[ch][i] = seqs[ch][i];
			int k = 0;
			for (int i = nb_last; i < length; i++)
				for (int ch = 0; ch < filech; ch++, k++) {
					normalized_samples[ch][i] = static_cast<double>(audiodata[0][k]) / 255.0;
				}
			for (int i = 0; i < nb_last_next; i++)
				for (int ch = 0; ch < filech; ch++, k++) {
					seqs[ch][i] = static_cast<double>(audiodata[0][k]) / 255.0;
				}
		}
		else if (sampletype == 16) {
			short *temps = reinterpret_cast<short *>(audiodata[0]);
			for (int ch = 0; ch < filech; ch++)
				for (int i = 0; i < nb_last; i++)normalized_samples[ch][i] = seqs[ch][i];
			int k = 0;
			for (int i = nb_last; i < length; i++)
				for (int ch = 0; ch < filech; ch++, k++) {
					normalized_samples[ch][i] = static_cast<double>(temps[k]) / 32767.0;
				}
			for (int i = 0; i < nb_last_next; i++)
				for (int ch = 0; ch < filech; ch++, k++) {
					seqs[ch][i] = static_cast<double>(temps[k]) / 32767.0;
				}
		}
		else if (sampletype == 24) {
			int *tempi = reinterpret_cast<int *>(audiodata[0]);
			for (int ch = 0; ch < filech; ch++)
				for (int i = 0; i < nb_last; i++)normalized_samples[ch][i] = seqs[ch][i];
			int k = 0;
			for (int i = nb_last; i < length; i++)
				for (int ch = 0; ch < filech; ch++, k++) {
					normalized_samples[ch][i] = static_cast<double>(tempi[k] >> 8) / 8388607.0;
				}
			for (int i = 0; i < nb_last_next; i++)
				for (int ch = 0; ch < filech; ch++, k++) {
					seqs[ch][i] = static_cast<double>(tempi[k] >> 8) / 8388607.0;
				}
		}
		else if (sampletype == 32) {
			int *tempi2 = reinterpret_cast<int *>(audiodata[0]);
			for (int ch = 0; ch < filech; ch++)
				for (int i = 0; i < nb_last; i++)normalized_samples[ch][i] = seqs[ch][i];
			int k = 0;
			for (int i = nb_last; i < length; i++)
				for (int ch = 0; ch < filech; ch++, k++) {
					normalized_samples[ch][i] = static_cast<double>(tempi2[k]) / 2147483647.0;
				}
			for (int i = 0; i < nb_last_next; i++)
				for (int ch = 0; ch < filech; ch++, k++) {
					seqs[ch][i] = static_cast<double>(tempi2[k]) / 2147483647.0;
				}
		}
		else if (sampletype == 64) {
			long long *templ = reinterpret_cast<long long *>(audiodata[0]);
			for (int ch = 0; ch < filech; ch++)
				for (int i = 0; i < nb_last; i++)normalized_samples[ch][i] = seqs[ch][i];
			int k = 0;
			for (int i = nb_last; i < length; i++)
				for (int ch = 0; ch < filech; ch++, k++) {
					normalized_samples[ch][i] = static_cast<double>(templ[k]) / static_cast<double>(9223372036854775807);
				}
			for (int i = 0; i < nb_last_next; i++)
				for (int ch = 0; ch < filech; ch++, k++) {
					seqs[ch][i] = static_cast<double>(templ[k]) / static_cast<double>(9223372036854775807);
				}
		}
	}
	nb_last = nb_last_next;
	return nb_fft_samples;
}

int BDMatch::Normalizationavx2::getshiftf()
{
	normalized_samples = new double*[filech];
	int nb_last_next = (nb_samples + nb_last) % FFTnum;
	int length = nb_samples + nb_last - nb_last_next;
	int nb_fft_samples = length / FFTnum;
	if (nb_fft_samples <= 0)return 0;
	for (int ch = 0; ch < filech; ch++)normalized_samples[ch] = (double*)fftw_malloc(sizeof(double)*length);
	if (realch == filech) {
		if (sampletype == 1)
			for (int ch = 0; ch < filech; ch++) {
				for (int i = 0; i < nb_last; i++)normalized_samples[ch][i] = seqs[ch][i];
				int avxlength = nb_samples / 8;
				int avx_last = nb_samples % 8;
				int threshold = (length - nb_last) / 8;
				int remainder = (length - nb_last) % 8;
				float *tempf = reinterpret_cast<float *>(audiodata[ch]);
				double *tempd = normalized_samples[ch] + nb_last;
				double *temp_seq = seqs[ch];
				for (int i = 0; i < threshold; i++) {
					__m256 temp256 = _mm256_load_ps(tempf);
					__m128 temp128_1 = _mm256_extractf128_ps(temp256, 0);
					__m128 temp128_2 = _mm256_extractf128_ps(temp256, 1);
					_mm256_storeu_pd(tempd, _mm256_cvtps_pd(temp128_1));
					tempd += 4;
					_mm256_storeu_pd(tempd, _mm256_cvtps_pd(temp128_2));
					tempd += 4;
					tempf += 8;
				}
				if (threshold < avxlength) {
					if (remainder) {
						__m256 temp256 = _mm256_load_ps(tempf);
						__m128 temp128_1 = _mm256_extractf128_ps(temp256, 0);
						__m128 temp128_2 = _mm256_extractf128_ps(temp256, 1);
						__m256d tempd_1 = _mm256_cvtps_pd(temp128_1);
						__m256d tempd_2 = _mm256_cvtps_pd(temp128_2);
						for (int j = 0; j < 4; j++)
							if (j < remainder) *(tempd++) = tempd_1.m256d_f64[j];
							else *(temp_seq++) = tempd_1.m256d_f64[j];
						remainder -= 4;
						for (int j = 0; j < 4; j++)
							if (j < remainder) *(tempd++) = tempd_2.m256d_f64[j];
							else *(temp_seq++) = tempd_2.m256d_f64[j];
						tempf += 8;
						threshold++;
					}
					for (int i = threshold; i < avxlength; i++){
						__m256 temp256 = _mm256_load_ps(tempf);
						__m128 temp128_1 = _mm256_extractf128_ps(temp256, 0);
						__m128 temp128_2 = _mm256_extractf128_ps(temp256, 1);
						_mm256_storeu_pd(temp_seq, _mm256_cvtps_pd(temp128_1));
						temp_seq += 4;
						_mm256_storeu_pd(temp_seq, _mm256_cvtps_pd(temp128_2));
						temp_seq += 4;
						tempf += 8;
					}
				}
				if (avx_last > 0) {
					int threshold2 = avx_last - nb_last_next;
					for (int i = 0; i < avx_last; i++)
						if (i < threshold2)*(tempd++) = static_cast<double>(*(tempf++));
						else *(temp_seq++) = static_cast<double>(*(tempf++));
				}
			}
		else if (sampletype == 2)
			for (int ch = 0; ch < filech; ch++) {
				double *tempd = reinterpret_cast<double *>(audiodata[ch]);
				for (int i = 0; i < nb_last; i++)normalized_samples[ch][i] = seqs[ch][i];
				int k = 0;
				for (int i = nb_last; i < length; i++, k++)normalized_samples[ch][i] = tempd[k];
				for (int i = 0; i < nb_last_next; i++, k++)seqs[ch][i] = tempd[k];
			}
		else if (sampletype == 8)
			for (int ch = 0; ch < filech; ch++) {
				int k = 0;
				for (int i = nb_last; i < length; i++, k++)normalized_samples[ch][i] = static_cast<double>(audiodata[ch][k]) / 255.0;
				for (int i = 0; i < nb_last_next; i++, k++)seqs[ch][i] = static_cast<double>(audiodata[ch][k]) / 255.0;
			}
		else if (sampletype == 16)
			for (int ch = 0; ch < filech; ch++) {
				short *temps = reinterpret_cast<short *>(audiodata[ch]);
				for (int i = 0; i < nb_last; i++)normalized_samples[ch][i] = seqs[ch][i];
				int k = 0;
				for (int i = nb_last; i < length; i++, k++)normalized_samples[ch][i] = static_cast<double>(temps[k]) / 32767.0;
				for (int i = 0; i < nb_last_next; i++, k++)seqs[ch][i] = static_cast<double>(temps[k]) / 32767.0;
			}
		else if (sampletype == 24)
			for (int ch = 0; ch < filech; ch++) {
				int *tempi = reinterpret_cast<int *>(audiodata[ch]);
				for (int i = 0; i < nb_last; i++)normalized_samples[ch][i] = seqs[ch][i];
				int k = 0;
				for (int i = nb_last; i < length; i++, k++)normalized_samples[ch][i] = static_cast<double>(tempi[k] >> 8) / 8388607.0;
				for (int i = 0; i < nb_last_next; i++, k++)seqs[ch][i] = static_cast<double>(tempi[k] >> 8) / 8388607.0;
			}
		else if (sampletype == 32)
			for (int ch = 0; ch < filech; ch++) {
				int *tempi2 = reinterpret_cast<int *>(audiodata[ch]);
				for (int i = 0; i < nb_last; i++)normalized_samples[ch][i] = seqs[ch][i];
				int k = 0;
				for (int i = nb_last; i < length; i++, k++)normalized_samples[ch][i] = static_cast<double>(tempi2[k]) / 2147483647.0;
				for (int i = 0; i < nb_last_next; i++, k++)seqs[ch][i] = static_cast<double>(tempi2[k]) / 2147483647.0;
			}
		else if (sampletype == 64)
			for (int ch = 0; ch < filech; ch++) {
				long long *templ = reinterpret_cast<long long *>(audiodata[ch]);
				for (int i = 0; i < nb_last; i++)normalized_samples[ch][i] = seqs[ch][i];
				int k = 0;
				for (int i = nb_last; i < length; i++, k++)normalized_samples[ch][i] = static_cast<double>(templ[k]) / static_cast<double>(9223372036854775807);
				for (int i = 0; i < nb_last_next; i++, k++)seqs[ch][i] = static_cast<double>(templ[k]) / static_cast<double>(9223372036854775807);
			}
	}
	else {
		if (sampletype == 1) {
			for (int ch = 0; ch < filech; ch++)
				for (int i = 0; i < nb_last; i++)normalized_samples[ch][i] = seqs[ch][i];
			int index = nb_last;
			int index2 = 0;
			int ch = 0;
			float *tempf = reinterpret_cast<float *>(audiodata[0]);
			int avxlength = nb_samples * filech / 8;
			int avx_last = (nb_samples * filech) % 8;
			int threshold = (length - nb_last) * filech / 8;
			int remainder = (length - nb_last) * filech % 8;
			for (int i = 0; i < threshold; i++) {
				__m256 temp256 = _mm256_load_ps(tempf); 
				__m128 temp128_1 = _mm256_extractf128_ps(temp256, 0);
				__m128 temp128_2 = _mm256_extractf128_ps(temp256, 1);
				__m256d tempd_1 = _mm256_cvtps_pd(temp128_1);
				__m256d tempd_2 = _mm256_cvtps_pd(temp128_2);
				double out;
				for (int j = 0; j < 4; j++) {
					out = tempd_1.m256d_f64[j];
					normalized_samples[ch++][index] = out;
					if (ch == filech) {
						ch = 0;
						index++;
					}
				}
				for (int j = 0; j < 4; j++) {
					out = tempd_2.m256d_f64[j];
					normalized_samples[ch++][index] = out;
					if (ch == filech) {
						ch = 0;
						index++;
					}
				}
				tempf += 8;
			}
			if (threshold < avxlength) {
				if (remainder) {
					__m256 temp256 = _mm256_load_ps(tempf);
					__m128 temp128_1 = _mm256_extractf128_ps(temp256, 0);
					__m128 temp128_2 = _mm256_extractf128_ps(temp256, 1);
					__m256d tempd_1 = _mm256_cvtps_pd(temp128_1);
					__m256d tempd_2 = _mm256_cvtps_pd(temp128_2);
					for (int j = 0; j < 8; j++) {
						double out;
						if (j < 4)out = tempd_1.m256d_f64[j];
						else out = tempd_2.m256d_f64[j - 4];
						if (index < length) {
							normalized_samples[ch++][index] = out;
							if (ch == filech) {
								ch = 0;
								index++;
							}
						}
						else {
							seqs[ch++][index2] = out;
							if (ch == filech) {
								ch = 0;
								index2++;
							}
						}
					}
					tempf += 8;
					threshold++;
				}
				for (int i = threshold; i < avxlength; i++) {
					__m256 temp256 = _mm256_load_ps(tempf);
					__m128 temp128_1 = _mm256_extractf128_ps(temp256, 0);
					__m128 temp128_2 = _mm256_extractf128_ps(temp256, 1);
					__m256d tempd_1 = _mm256_cvtps_pd(temp128_1);
					__m256d tempd_2 = _mm256_cvtps_pd(temp128_2);
					double out;
					for (int j = 0; j < 4; j++) {
						out = tempd_1.m256d_f64[j];
						seqs[ch++][index2] = out;
						if (ch == filech) {
							ch = 0;
							index2++;
						}
					}
					for (int j = 0; j < 4; j++) {
						out = tempd_2.m256d_f64[j];
						seqs[ch++][index2] = out;
						if (ch == filech) {
							ch = 0;
							index2++;
						}
					}
					tempf += 8;
				}
			}
			for (int i = 0; i < avx_last; i++) {
				if (index < length) {
					normalized_samples[ch++][index] = static_cast<double>(*(tempf++));
					if (ch == filech) {
						ch = 0;
						index++;
					}
				}
				else {
					seqs[ch++][index2] = static_cast<double>(*(tempf++));
					if (ch == filech) {
						ch = 0;
						index2++;
					}
				}
			}
		}
		else if (sampletype == 2) {
			double *tempd = reinterpret_cast<double *>(audiodata[0]);
			for (int ch = 0; ch < filech; ch++)
				for (int i = 0; i < nb_last; i++)normalized_samples[ch][i] = seqs[ch][i];
			int k = 0;
			for (int i = nb_last; i < length; i++)
				for (int ch = 0; ch < filech; ch++, k++) {
					normalized_samples[ch][i] = tempd[k];
				}
			for (int i = 0; i < nb_last_next; i++)
				for (int ch = 0; ch < filech; ch++, k++) {
					seqs[ch][i] = tempd[k];
				}
		}
		else if (sampletype == 8) {
			for (int ch = 0; ch < filech; ch++)
				for (int i = 0; i < nb_last; i++)normalized_samples[ch][i] = seqs[ch][i];
			int k = 0;
			for (int i = nb_last; i < length; i++)
				for (int ch = 0; ch < filech; ch++, k++) {
					normalized_samples[ch][i] = static_cast<double>(audiodata[0][k]) / 255.0;
				}
			for (int i = 0; i < nb_last_next; i++)
				for (int ch = 0; ch < filech; ch++, k++) {
					seqs[ch][i] = static_cast<double>(audiodata[0][k]) / 255.0;
				}
		}
		else if (sampletype == 16) {
			for (int ch = 0; ch < filech; ch++)
				for (int i = 0; i < nb_last; i++)normalized_samples[ch][i] = seqs[ch][i];
			int index = nb_last;
			int index2 = 0;
			int ch = 0;
			__m128i *temp128 = reinterpret_cast<__m128i *>(audiodata[0]);
			short *temps = reinterpret_cast<short *>(audiodata[0]);
			__m256d const16 = _mm256_set1_pd(32767.0);
			int avxlength = nb_samples * filech / 8;
			int avx_last = (nb_samples * filech) % 8;
			int threshold = (length - nb_last) * filech / 8;
			int remainder = (length - nb_last) * filech % 8;
			for (int i = 0; i < threshold; i++) {
				__m128i temp16 = _mm_load_si128(temp128);
				__m256i temp32 = _mm256_cvtepi16_epi32(temp16);
				__m128i temp32_1 = _mm256_extracti128_si256(temp32, 0);
				__m128i temp32_2 = _mm256_extracti128_si256(temp32, 1);
				__m256d tempd_1 = _mm256_cvtepi32_pd(temp32_1);
				__m256d tempd_2 = _mm256_cvtepi32_pd(temp32_2);
				tempd_1 = _mm256_div_pd(tempd_1, const16);
				tempd_2 = _mm256_div_pd(tempd_2, const16);
				double out;
				for (int j = 0; j < 4; j++) {
					out = tempd_1.m256d_f64[j];
					normalized_samples[ch++][index] = out;
					if (ch == filech) {
						ch = 0;
						index++;
					}
				}
				for (int j = 0; j < 4; j++) {
					out = tempd_2.m256d_f64[j];
					normalized_samples[ch++][index] = out;
					if (ch == filech) {
						ch = 0;
						index++;
					}
				}
				temp128++;
				temps += 8;
			}
			if (threshold < avxlength) {
				if (remainder) {
					__m128i temp16 = _mm_load_si128(temp128);
					__m256i temp32 = _mm256_cvtepi16_epi32(temp16);
					__m128i temp32_1 = _mm256_extracti128_si256(temp32, 0);
					__m128i temp32_2 = _mm256_extracti128_si256(temp32, 1);
					__m256d tempd_1 = _mm256_cvtepi32_pd(temp32_1);
					__m256d tempd_2 = _mm256_cvtepi32_pd(temp32_2);
					tempd_1 = _mm256_div_pd(tempd_1, const16);
					tempd_2 = _mm256_div_pd(tempd_2, const16);
					for (int j = 0; j < 8; j++) {
						double out;
						if (j < 4)out = tempd_1.m256d_f64[j];
						else out = tempd_2.m256d_f64[j - 4];
						if (index < length) {
							normalized_samples[ch++][index] = out;
							if (ch == filech) {
								ch = 0;
								index++;
							}
						}
						else {
							seqs[ch++][index2] = out;
							if (ch == filech) {
								ch = 0;
								index2++;
							}
						}
					}
					temp128++;
					temps += 8;
					threshold++;
				}
				for (int i = threshold; i < avxlength; i++) {
					__m128i temp16 = _mm_load_si128(temp128);
					__m256i temp32 = _mm256_cvtepi16_epi32(temp16);
					__m128i temp32_1 = _mm256_extracti128_si256(temp32, 0);
					__m128i temp32_2 = _mm256_extracti128_si256(temp32, 1);
					__m256d tempd_1 = _mm256_cvtepi32_pd(temp32_1);
					__m256d tempd_2 = _mm256_cvtepi32_pd(temp32_2);
					tempd_1 = _mm256_div_pd(tempd_1, const16);
					tempd_2 = _mm256_div_pd(tempd_2, const16);
					double out;
					for (int j = 0; j < 4; j++) {
						out = tempd_1.m256d_f64[j];
						seqs[ch++][index2] = out;
						if (ch == filech) {
							ch = 0;
							index2++;
						}
					}
					for (int j = 0; j < 4; j++) {
						out = tempd_2.m256d_f64[j];
						seqs[ch++][index2] = out;
						if (ch == filech) {
							ch = 0;
							index2++;
						}
					}
					temp128++;
					temps += 8;
				}
			}
			for (int i = 0; i < avx_last; i++) {
				if (index < length) {
					normalized_samples[ch++][index] = static_cast<double>(*(temps++)) / 32767.0;
					if (ch == filech) {
						ch = 0;
						index++;
					}
				}
				else {
					seqs[ch++][index2] = static_cast<double>(*(temps++)) / 32767.0;
					if (ch == filech) {
						ch = 0;
						index2++;
					}
				}
			}
		}
		else if (sampletype == 24) {
			for (int ch = 0; ch < filech; ch++)
				for (int i = 0; i < nb_last; i++)normalized_samples[ch][i] = seqs[ch][i];
			int index = nb_last;
			int index2 = 0;
			int ch = 0;
			int *tempi = reinterpret_cast<int *>(audiodata[0]);
			__m256i *temp256 = reinterpret_cast<__m256i *>(audiodata[0]);
			__m256d const24 = _mm256_set1_pd(8388607.0);
			int avxlength = nb_samples * filech / 8;
			int avx_last = (nb_samples * filech) % 8;
			int threshold = (length - nb_last) * filech / 8;
			int remainder = (length - nb_last) * filech % 8;
			for (int i = 0; i < threshold; i++) {
				__m256i temp32 = _mm256_load_si256(temp256);
				temp32 = _mm256_srai_epi32(temp32, 8);
				__m128i temp32_1 = _mm256_extracti128_si256(temp32, 0);
				__m128i temp32_2 = _mm256_extracti128_si256(temp32, 1);
				__m256d tempd_1 = _mm256_cvtepi32_pd(temp32_1);
				__m256d tempd_2 = _mm256_cvtepi32_pd(temp32_2);
				tempd_1 = _mm256_div_pd(tempd_1, const24);
				tempd_2 = _mm256_div_pd(tempd_2, const24);
				double out;
				for (int j = 0; j < 4; j++) {
					out = tempd_1.m256d_f64[j];
					normalized_samples[ch++][index] = out;
					if (ch == filech) {
						ch = 0;
						index++;
					}
				}
				for (int j = 0; j < 4; j++) {
					out = tempd_2.m256d_f64[j];
					normalized_samples[ch++][index] = out;
					if (ch == filech) {
						ch = 0;
						index++;
					}
				}
				temp256++;
				tempi += 8;
			}
			if (threshold < avxlength) {
				if (remainder) {
					__m256i temp32 = _mm256_load_si256(temp256);
					temp32 = _mm256_srai_epi32(temp32, 8);
					__m128i temp32_1 = _mm256_extracti128_si256(temp32, 0);
					__m128i temp32_2 = _mm256_extracti128_si256(temp32, 1);
					__m256d tempd_1 = _mm256_cvtepi32_pd(temp32_1);
					__m256d tempd_2 = _mm256_cvtepi32_pd(temp32_2);
					tempd_1 = _mm256_div_pd(tempd_1, const24);
					tempd_2 = _mm256_div_pd(tempd_2, const24);
					for (int j = 0; j < 8; j++) {
						double out;
						if (j < 4)out = tempd_1.m256d_f64[j];
						else out = tempd_2.m256d_f64[j - 4];
						if (index < length) {
							normalized_samples[ch++][index] = out;
							if (ch == filech) {
								ch = 0;
								index++;
							}
						}
						else {
							seqs[ch++][index2] = out;
							if (ch == filech) {
								ch = 0;
								index2++;
							}
						}
					}
					temp256++;
					tempi += 8;
					threshold++;
				}
				for (int i = threshold; i < avxlength; i++) {
					__m256i temp32 = _mm256_load_si256(temp256);
					temp32 = _mm256_srai_epi32(temp32, 8);
					__m128i temp32_1 = _mm256_extracti128_si256(temp32, 0);
					__m128i temp32_2 = _mm256_extracti128_si256(temp32, 1);
					__m256d tempd_1 = _mm256_cvtepi32_pd(temp32_1);
					__m256d tempd_2 = _mm256_cvtepi32_pd(temp32_2);
					tempd_1 = _mm256_div_pd(tempd_1, const24);
					tempd_2 = _mm256_div_pd(tempd_2, const24);
					double out;
					for (int j = 0; j < 4; j++) {
						out = tempd_1.m256d_f64[j];
						seqs[ch++][index2] = out;
						if (ch == filech) {
							ch = 0;
							index2++;
						}
					}
					for (int j = 0; j < 4; j++) {
						out = tempd_2.m256d_f64[j];
						seqs[ch++][index2] = out;
						if (ch == filech) {
							ch = 0;
							index2++;
						}
					}
					temp256++;
					tempi += 8;
				}
			}
			for (int i = 0; i < avx_last; i++) {
				if (index < length) {
					normalized_samples[ch++][index] = static_cast<double>(*(tempi++) >> 8) / 8388607.0;
					if (ch == filech) {
						ch = 0;
						index++;
					}
				}
				else {
					seqs[ch++][index2] = static_cast<double>(*(tempi++) >> 8) / 8388607.0;
					if (ch == filech) {
						ch = 0;
						index2++;
					}
				}
			}
		}
		else if (sampletype == 32) {
			for (int ch = 0; ch < filech; ch++)
				for (int i = 0; i < nb_last; i++)normalized_samples[ch][i] = seqs[ch][i];
			int index = nb_last;
			int index2 = 0;
			int ch = 0;
			int *tempi32 = reinterpret_cast<int *>(audiodata[0]);
			__m256i *temp256 = reinterpret_cast<__m256i *>(audiodata[0]);
			__m256d const32 = _mm256_set1_pd(2147483647.0);
			int avxlength = nb_samples * filech / 8;
			int avx_last = (nb_samples * filech) % 8;
			int threshold = (length - nb_last) * filech / 8;
			int remainder = (length - nb_last) * filech % 8;
			for (int i = 0; i < threshold; i++) {
				__m256i temp32 = _mm256_load_si256(temp256);
				__m128i temp32_1 = _mm256_extracti128_si256(temp32, 0);
				__m128i temp32_2 = _mm256_extracti128_si256(temp32, 1);
				__m256d tempd_1 = _mm256_cvtepi32_pd(temp32_1);
				__m256d tempd_2 = _mm256_cvtepi32_pd(temp32_2);
				tempd_1 = _mm256_div_pd(tempd_1, const32);
				tempd_2 = _mm256_div_pd(tempd_2, const32);
				double out;
				for (int j = 0; j < 4; j++) {
					out = tempd_1.m256d_f64[j];
					normalized_samples[ch++][index] = out;
					if (ch == filech) {
						ch = 0;
						index++;
					}
				}
				for (int j = 0; j < 4; j++) {
					out = tempd_2.m256d_f64[j];
					normalized_samples[ch++][index] = out;
					if (ch == filech) {
						ch = 0;
						index++;
					}
				}
				temp256++;
				tempi32 += 8;
			}
			if (threshold < avxlength) {
				if (remainder) {
					__m256i temp32 = _mm256_load_si256(temp256);
					__m128i temp32_1 = _mm256_extracti128_si256(temp32, 0);
					__m128i temp32_2 = _mm256_extracti128_si256(temp32, 1);
					__m256d tempd_1 = _mm256_cvtepi32_pd(temp32_1);
					__m256d tempd_2 = _mm256_cvtepi32_pd(temp32_2);
					tempd_1 = _mm256_div_pd(tempd_1, const32);
					tempd_2 = _mm256_div_pd(tempd_2, const32);
					for (int j = 0; j < 8; j++) {
						double out;
						if (j < 4)out = tempd_1.m256d_f64[j];
						else out = tempd_2.m256d_f64[j - 4];
						if (index < length) {
							normalized_samples[ch++][index] = out;
							if (ch == filech) {
								ch = 0;
								index++;
							}
						}
						else {
							seqs[ch++][index2] = out;
							if (ch == filech) {
								ch = 0;
								index2++;
							}
						}
					}
					temp256++;
					tempi32 += 8;
					threshold++;
				}
				for (int i = threshold; i < avxlength; i++) {
					__m256i temp32 = _mm256_load_si256(temp256);
					__m128i temp32_1 = _mm256_extracti128_si256(temp32, 0);
					__m128i temp32_2 = _mm256_extracti128_si256(temp32, 1);
					__m256d tempd_1 = _mm256_cvtepi32_pd(temp32_1);
					__m256d tempd_2 = _mm256_cvtepi32_pd(temp32_2);
					tempd_1 = _mm256_div_pd(tempd_1, const32);
					tempd_2 = _mm256_div_pd(tempd_2, const32);
					double out;
					for (int j = 0; j < 4; j++) {
						out = tempd_1.m256d_f64[j];
						seqs[ch++][index2] = out;
						if (ch == filech) {
							ch = 0;
							index2++;
						}
					}
					for (int j = 0; j < 4; j++) {
						out = tempd_2.m256d_f64[j];
						seqs[ch++][index2] = out;
						if (ch == filech) {
							ch = 0;
							index2++;
						}
					}
					temp256++;
					tempi32 += 8;
				}
			}
			for (int i = 0; i < avx_last; i++) {
				if (index < length) {
					normalized_samples[ch++][index] = static_cast<double>(*(tempi32++)) / 2147483647.0;
					if (ch == filech) {
						ch = 0;
						index++;
					}
				}
				else {
					seqs[ch++][index2] = static_cast<double>(*(tempi32++)) / 2147483647.0;
					if (ch == filech) {
						ch = 0;
						index2++;
					}
				}
			}
		}
		else if (sampletype == 64) {
			long long *templ = reinterpret_cast<long long *>(audiodata[0]);
			for (int ch = 0; ch < filech; ch++)
				for (int i = 0; i < nb_last; i++)normalized_samples[ch][i] = seqs[ch][i];
			int k = 0;
			for (int i = nb_last; i < length; i++)
				for (int ch = 0; ch < filech; ch++, k++) {
					normalized_samples[ch][i] = static_cast<double>(templ[k]) / static_cast<double>(9223372036854775807);
				}
			for (int i = 0; i < nb_last_next; i++)
				for (int ch = 0; ch < filech; ch++, k++) {
					seqs[ch][i] = static_cast<double>(templ[k]) / static_cast<double>(9223372036854775807);
				}
		}
	}
	nb_last = nb_last_next;
	return nb_fft_samples;
}
#pragma managed