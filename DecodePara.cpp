#include "DecodePara.h"

using namespace BDMatch;

BDMatch::Decode::Decode(String^ filename0, int FFTnum0, bool outputpcm0, int mindb0, int resamprate0, int progtype0,
	List<Task^>^ tasks0, System::Threading::CancellationToken canceltoken0, fftw_plan plan0, ProgressCallback^ progback0)
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
}

BDMatch::Decode::~Decode()
{
	clearfftdata();
	if (ffmpeg) { 
		delete ffmpeg; 
		ffmpeg = nullptr;
	}
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
		if (filefm->streams[j]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
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
		filename = filefm->filename;
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
	double shiftf = 0;
	fftdata = new std::vector<std::vector<node*>>(2, std::vector<node*>(efftnum));
	fftsampnum = 0;
	sample_seqs = new double*[2];
	sample_seqs[0] = (double*)fftw_malloc(sizeof(double)*FFTnum);
	sample_seqs[1] = (double*)fftw_malloc(sizeof(double)*FFTnum);
	int nb_last_seq = 0;
	decoded_frame = NULL;
	int realch = 1;
	String ^chfmt = "Packed";
	bool isplanar = av_sample_fmt_is_planar(codecfm->sample_fmt);
	if (isplanar) {
		realch = codecfm->channels;
		chfmt = "Planar";
	}
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
					nb_samples =
						av_rescale_rnd(decoded_frame->nb_samples, resamprate, samplerate, AV_ROUND_ZERO);
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
				//处理数据
				double **normalized_samples = getshiftf(audiodata, realch, codecfm->channels, nb_samples, sampletype);
				if (isplanar) sample_seqs = fetch_data_planar(normalized_samples, sample_seqs, nb_last_seq, nb_samples, codecfm->channels);//平面音频
				else sample_seqs = fetch_data_linear(normalized_samples, sample_seqs, nb_last_seq, nb_samples, codecfm->channels);//线性音频
				if (sample_seqs == nullptr) {
					for (int ch = 0; ch < realch; ch++)delete[] normalized_samples[ch];
					delete[] normalized_samples;
					if (dst_data)
						av_freep(&dst_data[0]);
					av_freep(&dst_data);
					av_frame_free(&decoded_frame);
					av_free_packet(packet);
					delete ffmpeg;
					ffmpeg = nullptr;
					return;
				}
				for (int ch = 0; ch < realch; ch++)delete[] normalized_samples[ch];
				delete[] normalized_samples;
				if (dst_data)
					av_freep(&dst_data[0]);
				av_freep(&dst_data);
			}
			av_frame_free(&decoded_frame);
		}
		av_free_packet(packet);
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

double** BDMatch::Decode::getshiftf(uint8_t ** audiodata, const int &realch, const int &filech, const int &nb_samples, const int &sampletype)
{
	double **result = new double*[realch];
	int length = nb_samples * filech / realch;
	for (int ch = 0; ch < realch; ch++) {
		double *shiftd = new double[length];
		if (sampletype == 1) {
			float *tempf = reinterpret_cast<float *>(audiodata[ch]);
			for (int i = 0; i < length; i++)shiftd[i] = static_cast<double>(tempf[i]);
		}
		else if (sampletype == 2) {
			double *tempd = reinterpret_cast<double *>(audiodata[ch]);
			for (int i = 0; i < length; i++)shiftd[i] = tempd[i];
		}
		else if (sampletype == 8) {
			for (int i = 0; i < length; i++)shiftd[i] = static_cast<double>(audiodata[ch][i]) / 255.0;
		}
		else if (sampletype == 16) {
			short *temps = reinterpret_cast<short *>(audiodata[ch]);
			for (int i = 0; i < length; i++)shiftd[i] = static_cast<double>(temps[i]) / 32767.0;
		}
		else if (sampletype == 24) {
			int *tempi = reinterpret_cast<int *>(audiodata[ch]);
			for (int i = 0; i < length; i++)shiftd[i] = static_cast<double>(tempi[i] >> 8) / 8388607.0;
		}
		else if (sampletype == 32) {
			int *tempi2 = reinterpret_cast<int *>(audiodata[ch]);
			for (int i = 0; i < length; i++)shiftd[i] = static_cast<double>(tempi2[i]) / 2147483647.0;
		}
		else if (sampletype == 64) {
			long long *templ = reinterpret_cast<long long *>(audiodata[ch]);
			for (int i = 0; i < length; i++)shiftd[i] = static_cast<double>(templ[i]) / static_cast<double>(9223372036854775807);
		}
		result[ch] = shiftd;
	}
	return result;
}

bool BDMatch::Decode::add_fft_task(double *sample_seq, const int &ch)
{
	(*fftdata)[ch][fftsampnum] = new node(FFTnum / 2);
	FFTC^ fftc = gcnew FFTC((*fftdata)[ch][fftsampnum], plan, sample_seq, mindb,
		gcnew ProgressCallback(this, &Decode::subprogback));
	Task^ task = gcnew Task(gcnew Action(fftc, &FFTC::FFT), canceltoken);
	if (task->Status != System::Threading::Tasks::TaskStatus::Canceled)task->Start();
	else {
		fftc->~FFTC();
		return false;
	}
	tasks->Add(task);
	return true;
}

double ** BDMatch::Decode::fetch_data_planar(double **& data, double ** seqs, int & nb_last_seq, const int & nb_samples, const int & ch)
{
	int nb_fft_smp = (nb_last_seq + nb_samples) / FFTnum;
	int smp_start = FFTnum - nb_last_seq;
	for (int i = 0; i < ch; i++) {
		for (int j = nb_last_seq, k = 0; k < smp_start; j++, k++) {
			seqs[i][j] = data[i][k];
		}
	}
	if (nb_fft_smp < 1)
		for (int i = 0; i < ch; i++) {
			for (int j = nb_last_seq; j < FFTnum; j++) {
				seqs[i][j] = 0;
			}
		}
	for (int i = 0; i < ch; i++) {
		if (!add_fft_task(seqs[i], i))return nullptr;
		seqs[i] = (double*)fftw_malloc(sizeof(double)*FFTnum);
	}
	fftsampnum++;
	for (int n = 0; n < nb_fft_smp - 1; n++) {
		for (int i = 0; i < ch; i++) {
			for (int j = 0, k = smp_start; j < FFTnum; j++, k++) {
				seqs[i][j] = data[i][k];
			}
		}
		smp_start += FFTnum;
		for (int i = 0; i < ch; i++) {
			if (!add_fft_task(seqs[i], i))return nullptr;
			seqs[i] = (double*)fftw_malloc(sizeof(double)*FFTnum);
		}
		fftsampnum++;
	}
	if (nb_fft_smp < 1)nb_last_seq = 0;
	else {
		nb_last_seq = nb_samples + nb_last_seq - nb_fft_smp * FFTnum;;
		for (int i = 0; i < ch; i++) {
			for (int j = 0, k = smp_start; j < nb_last_seq; j++, k++) {
				seqs[i][j] = data[i][k];
			}
		}
	}
	return seqs;
}

double ** BDMatch::Decode::fetch_data_linear(double **& data, double ** seqs, int & nb_last_seq, const int & nb_samples, const int & ch)
{
	int nb_fft_smp = (nb_last_seq + nb_samples) / FFTnum;
	int smp_start_ch = (FFTnum - nb_last_seq)*ch;
	int FFTnum_ch = FFTnum * ch;
	for (int j = nb_last_seq, k = 0; k < smp_start_ch; j++) {
		for (int i = 0; i < ch; i++, k++) {
			seqs[i][j] = data[0][k];
		}
	}
	if (nb_fft_smp < 1)
		for (int i = 0; i < ch; i++) {
			for (int j = nb_last_seq; j < FFTnum; j++) {
				seqs[i][j] = 0;
			}
		}
	for (int i = 0; i < ch; i++) {
		if (!add_fft_task(seqs[i], i))return nullptr;
		seqs[i] = (double*)fftw_malloc(sizeof(double)*FFTnum);
	}
	fftsampnum++;
	for (int n = 0; n < nb_fft_smp - 1; n++) {
		for (int j = 0, k = smp_start_ch; j < FFTnum; j++) {
			for (int i = 0; i < ch; i++, k++) {
				seqs[i][j] = data[0][k];
			}
		}
		smp_start_ch += FFTnum_ch;
		for (int i = 0; i < ch; i++) {
			if (!add_fft_task(seqs[i], i))return nullptr;
			seqs[i] = (double*)fftw_malloc(sizeof(double)*FFTnum);
		}
		fftsampnum++;
	}
	if (nb_fft_smp < 1)nb_last_seq = 0;
	else {
		nb_last_seq = nb_samples + nb_last_seq - nb_fft_smp * FFTnum;
		for (int j = 0, k = smp_start_ch; j < nb_last_seq; j++) {
			for (int i = 0; i < ch; i++, k++) {
				seqs[i][j] = data[0][k];
			}
		}
	}
	return seqs;
}

void BDMatch::Decode::subprogback(int type, double val)
{
	decodednum++;
	double temp = decodednum / static_cast<double>(efftnum) / channels;
	if (temp >= progval + 0.02 || temp == 0) {
		progval = temp;
		progback(progtype, progval);
	}
}

BDMatch::FFmpeg::~FFmpeg()
{
	if (sample_seqs) {
		for (int ch = 0; ch < 2; ch++)if (sample_seqs[ch]) {
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
		av_free_packet(packet);
		packet = NULL;
	}
	if (codecfm)avcodec_close(codecfm);
	if (filefm)avformat_close_input(&filefm);
	if (swr_ctx)swr_close(swr_ctx);
}

BDMatch::FFTC::FFTC(node* fftseq0, fftw_plan p0, double* in0, int mindb0, ProgressCallback^ progback0)
{
	fftseq = fftseq0;
	FFTnum = fftseq0->size() * 2;
	progback = progback0;
	mindb = mindb0;
	p = p0;
	in = in0;
}

BDMatch::FFTC::~FFTC()
{
	if(out)fftw_free(out);
	if(in)fftw_free(in);
	p = nullptr;
	in = nullptr;
	out = nullptr;
}

void BDMatch::FFTC::FFT()
{
	using namespace std;
	out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*FFTnum);
	fftw_execute_dft_r2c(p, in, out);
	for (int i = 0; i < FFTnum / 2; i++) {
		double real = *(out + i)[0];
		double imag = *(out + i)[1];
		in[i] = real * real + imag * imag;
	}
	FD8(in, fftseq);
	fftw_free(out);
	fftw_free(in);
	in = nullptr;
	out = nullptr;
	progback(1, 0);
	return;
}

int BDMatch::FFTC::FD8(double *inseq, node* outseq)
{
	char *out = outseq->getdata();
	for (int i = 0; i < outseq->size(); i++) {
		double addx = inseq[i];
		addx = 10.0 * log10(addx);
		addx -= 15.0;
		addx *= 256.0 / (15.0 - static_cast<double>(mindb));
		addx += 127.0;
		addx = min(addx, 127.0);
		addx = max(addx, -128.0);
		out[i] = static_cast<char>(addx);
	}
	out = nullptr;
	return 0;
}
