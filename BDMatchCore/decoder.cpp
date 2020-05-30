#include "headers/decoder.h"
#include <immintrin.h>
#include <algorithm>
#include <fstream>
#include "headers/multithreading.h"

//#define _NO_SVML_ // define if _mm(256)_log10_pd is not supported

constexpr double MaxdB = 20.0;

namespace Decode {
	inline double m256d_f64(const __m256d& vec, const int& index)
	{
#ifndef _CLI_ // Windows
		return vec.m256d_f64[index];
#else // Other platforms
		return reinterpret_cast<const double*>(&vec)[index];
#endif 
	}

	inline __m128d _mm_log10_pd_cmpt(const __m128d& vec)
	{
#ifndef _NO_SVML_ // Platforms which support _mm_log10_pd
		return _mm_log10_pd(vec);
#else // Other platforms
		__m128d re_vec = vec;
		double* re_d = reinterpret_cast<double*>(&re_vec);
		re_d[0] = log10(re_d[0]);
		re_d[1] = log10(re_d[1]);
		return re_vec;
#endif 
	}

	inline __m256d _mm256_log10_pd_cmpt(const __m256d& vec)
	{
#ifndef _NO_SVML_ // Platforms which support _mm256_log10_pd
		return _mm256_log10_pd(vec);
#else // Other platforms
		__m256d re_vec = vec;
		double* re_d = reinterpret_cast<double*>(&re_vec);
		re_d[0] = log10(re_d[0]);
		re_d[1] = log10(re_d[1]);
		re_d[2] = log10(re_d[2]);
		re_d[3] = log10(re_d[3]);
		return re_vec;
#endif 
	}
}

Decode::FFmpeg::~FFmpeg()
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

Decode::Decode::Decode(language_pack& lang_pack0, std::shared_ptr<std::atomic_flag> keep_processing0)
	:keep_processing(keep_processing0), lang_pack(lang_pack0) {
}
Decode::Decode::~Decode()
{
	clear_fft_data();
	if (ffmpeg) {
		delete ffmpeg;
		ffmpeg = nullptr;
	}
	plan = nullptr;
}

int Decode::Decode::load_settings(const int &fft_num0, const bool &output_pcm0,
	const int &min_db0, const int &resamp_rate0, const int &prog_type0, fftw_plan plan0, const prog_func &prog_single0)
{
	fft_num = fft_num0;
	output_pcm = output_pcm0;
	min_db = min_db0;
	resamp_rate = resamp_rate0;
	prog_type = prog_type0;
	plan = plan0;
	prog_single = prog_single0;
	return 0;
}

int Decode::Decode::initialize(const std::string & file_name0)
{
	file_name = file_name0;
	ffmpeg = new FFmpeg;
	ffmpeg->filefm = NULL;//文件格式;
	if (avformat_open_input(&ffmpeg->filefm, file_name.c_str(), NULL, NULL) != 0) {//获取文件格式
		feedback = lang_pack.get_text(Lang_Type::Decoder, 0);//"\r\n无法打开文件！"
		return_val = -1;
		keep_processing->clear();
		return return_val;
	}
	if (avformat_find_stream_info(ffmpeg->filefm, NULL) < 0) {//获取文件内音视频流的信息
		feedback = lang_pack.get_text(Lang_Type::Decoder, 1);//"\r\n无法读取文件流信息！"
		return_val = -1;
		keep_processing->clear();
		return return_val;
	}
	ffmpeg->codecfm = NULL;//编码格式
	ffmpeg->codec = NULL;//解码器
	unsigned int j;
	// Find the first audio stream
	audio_stream = -1;
	for (j = 0; j < ffmpeg->filefm->nb_streams; j++)//找到音频对应的stream
		if (ffmpeg->filefm->streams[j]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			audio_stream = j;
			break;
		}
	if (audio_stream == -1)
	{
		feedback = lang_pack.get_text(Lang_Type::Decoder, 2);// "\r\n无音频流！"
		return_val = -2;
		keep_processing->clear();
		return return_val; // Didn't find a audio stream
	}
	else if (audio_stream == 0) {
		audio_only = true;
	}
	milisec = static_cast<int64_t>(ffmpeg->filefm->duration / 10000);
	ffmpeg->codec = avcodec_find_decoder(ffmpeg->filefm->streams[audio_stream]->codecpar->codec_id);//寻找解码器
	if (!ffmpeg->codec)
	{
		feedback = lang_pack.get_text(Lang_Type::Decoder, 3);//"\r\n无法找到音频的对应解码器！"
		return_val = -3;
		keep_processing->clear();
		return return_val; // Codec not found codec
	}
	ffmpeg->codecfm = avcodec_alloc_context3(ffmpeg->codec);//音频的编码信息
	if (!ffmpeg->codecfm)
	{
		feedback = lang_pack.get_text(Lang_Type::Decoder, 4);//"\r\n无法创建音频的解码器信息！";
		return_val = -3;
		keep_processing->clear();
		return return_val; // Failed to allocate the codec context
	}
	int getcodecpara = avcodec_parameters_to_context(ffmpeg->codecfm, ffmpeg->filefm->streams[audio_stream]->codecpar);
	if (getcodecpara < 0) {
		feedback = lang_pack.get_text(Lang_Type::Decoder, 5);//"\r\n无法获取音频的解码器信息！"
		return_val = -3;
		keep_processing->clear();
		return return_val; // Failed to allocate the codec context
	}

	feedback = lang_pack.get_text(Lang_Type::Decoder, 28) + std::to_string(audio_stream) +//"音轨编号：**"
		lang_pack.get_text(Lang_Type::Decoder, 29) + ffmpeg->codec->long_name;//"    音频编码：****"

	if (avcodec_open2(ffmpeg->codecfm, ffmpeg->codec, NULL) < 0)//将两者结合以便在下面的解码函数中调用pInCodec中的对应解码函数
	{
		feedback = lang_pack.get_text(Lang_Type::Decoder, 6);//"\r\n无法打开音频的对应解码器！"
		return_val = -3;
		keep_processing->clear();
		return return_val; // Could not open codec
	}
	//延迟
	start_time = ffmpeg->filefm->streams[audio_stream]->start_time;
	//采样率
	sample_rate = ffmpeg->codecfm->sample_rate;
	//声道
	channels = ffmpeg->codecfm->channels;
	//vol
	total_vol = 0.0;
	return 0;
}
int Decode::Decode::decode_audio() {
	using namespace DataStruct;

	//multithreading
	int nb_threads = std::thread::hardware_concurrency();
	fixed_thread_pool pool(nb_threads, keep_processing);
	//重采样变量1
	bool resamp = false;
	if (resamp_rate > 0 && sample_rate != resamp_rate)resamp = true;
	//准备写入文件
	std::fstream pcm;
	short pcmh = 0;
	int pcmh32 = 0;
	std::string filename;
	if (output_pcm) {//写入wav头信息
		filename = ffmpeg->filefm->url;
		filename += ".wav";
		pcm.open(filename, std::ios::out | std::ios::trunc | std::ios::binary);
		if (!pcm.is_open()) {
			feedback = lang_pack.get_text(Lang_Type::Decoder, 7);//"\r\n无法打开要写入的PCM文件！"
			return_val = -4;
			keep_processing->clear();
			return return_val;
		}
		pcm.write("RIFF", 4);
		pcm.write(reinterpret_cast<char*>(&pcmh), 4);//file size
		pcm.write("WAVE", 4);
		pcm.write("fmt ", 4);
		pcmh32 = 16;
		pcm.write(reinterpret_cast<char*>(&pcmh32), 4);//16
		if (ffmpeg->codecfm->sample_fmt == AV_SAMPLE_FMT_FLT || ffmpeg->codecfm->sample_fmt == AV_SAMPLE_FMT_DBL ||
			ffmpeg->codecfm->sample_fmt == AV_SAMPLE_FMT_FLTP || ffmpeg->codecfm->sample_fmt == AV_SAMPLE_FMT_DBLP)
			pcmh = 3;
		else pcmh = 1;
		pcm.write(reinterpret_cast<char*>(&pcmh), 2);//pcm(1) IEEE float(3)
		pcmh = channels;
		pcm.write(reinterpret_cast<char*>(&pcmh), 2);//channels stereo(2)
		pcmh32 = resamp ? resamp_rate : sample_rate;
		pcm.write(reinterpret_cast<char*>(&pcmh32), 4);//sample rate
		int byterate;
		if (!resamp)byterate = av_get_bytes_per_sample(ffmpeg->codecfm->sample_fmt)*channels*sample_rate;
		else byterate = av_get_bytes_per_sample(ffmpeg->codecfm->sample_fmt)*channels*resamp_rate;
		pcm.write(reinterpret_cast<char*>(&byterate), 4);//byte per sec
		pcmh = 4;
		pcm.write(reinterpret_cast<char*>(&pcmh), 2);//byte per sample * channels
		pcmh = 16;
		pcm.write(reinterpret_cast<char*>(&pcmh), 2);//bit per sample
		pcm.write("data", 4);//data
		pcmh = 0;
		pcm.write(reinterpret_cast<char*>(&pcmh), 4);//pcm size
	}
	//确定输出位深
	switch (ffmpeg->codecfm->sample_fmt) {
	case AV_SAMPLE_FMT_U8:
		sample_type = 8;
		break;
	case AV_SAMPLE_FMT_U8P:
		sample_type = 8;
		break;
	case AV_SAMPLE_FMT_S16:
		sample_type = 16;
		break;
	case AV_SAMPLE_FMT_S16P:
		sample_type = 16;
		break;
	case AV_SAMPLE_FMT_S32:
		sample_type = 32;
		break;
	case AV_SAMPLE_FMT_S32P:
		sample_type = 32;
		break;
	case AV_SAMPLE_FMT_S64:
		sample_type = 64;
		break;
	case AV_SAMPLE_FMT_S64P:
		sample_type = 64;
		break;
	default:
		break;
	}
	if (ffmpeg->codecfm->sample_fmt == AV_SAMPLE_FMT_FLT || ffmpeg->codecfm->sample_fmt == AV_SAMPLE_FMT_FLTP) sample_type = 1;
	else if (ffmpeg->codecfm->sample_fmt == AV_SAMPLE_FMT_DBL || ffmpeg->codecfm->sample_fmt == AV_SAMPLE_FMT_DBLP) sample_type = 2;
	//采样变量
	int64_t samplenum = 0;
	c_min_db = 256.0 / (MaxdB - static_cast<double>(min_db));
	//重采样变量2
	ffmpeg->dst_data = nullptr;
	int dst_linesize;
	//对音频解码(加重采样)
	if (resamp) {
		samplenum = static_cast<int>(ceil(resamp_rate / 1000.0*ffmpeg->filefm->duration / 1000.0));
		ffmpeg->swr_ctx = swr_alloc();
		if (!ffmpeg->swr_ctx) {
			feedback = lang_pack.get_text(Lang_Type::Decoder, 8);//"\r\n无法构建重采样环境！"
			return_val = -8;
			if (output_pcm)pcm.close();
			keep_processing->clear();
			return return_val;
		}
		//重采样选项
		av_opt_set_int(ffmpeg->swr_ctx, "in_channel_layout", ffmpeg->codecfm->channel_layout, 0);
		av_opt_set_int(ffmpeg->swr_ctx, "in_sample_rate", sample_rate, 0);
		av_opt_set_sample_fmt(ffmpeg->swr_ctx, "in_sample_fmt", ffmpeg->codecfm->sample_fmt, 0);

		av_opt_set_int(ffmpeg->swr_ctx, "out_channel_layout", ffmpeg->codecfm->channel_layout, 0);
		av_opt_set_int(ffmpeg->swr_ctx, "out_sample_rate", resamp_rate, 0);
		av_opt_set_sample_fmt(ffmpeg->swr_ctx, "out_sample_fmt", ffmpeg->codecfm->sample_fmt, 0);
		/* initialize the resampling context */
		if ((swr_init(ffmpeg->swr_ctx)) < 0) {
			feedback = lang_pack.get_text(Lang_Type::Decoder, 9);//"\r\n无法初始化重采样环境！"
			return_val = -8;
			if (output_pcm)pcm.close();
			keep_processing->clear();
			return return_val;
		}
	}
	else {
		samplenum = static_cast<int64_t>(ceil(sample_rate / 1000.0 * ffmpeg->filefm->duration / 1000.0));
	}
	e_fft_num = static_cast<int64_t>(ceil(samplenum / static_cast<double>(fft_num)));//estimated_fft_num
	//查询音频封装格式
	ffmpeg->packet = av_packet_alloc();
	std::string chfmt = "Packed";
	real_ch = 1;
	bool isplanar = av_sample_fmt_is_planar(ffmpeg->codecfm->sample_fmt);
	if (isplanar) {
		real_ch = channels;
		chfmt = "Planar";
	}
	//为频谱数据分配内存
	if (vol_mode != 1) {
		data_channels = std::min(channels, 8);
		int spectrum_size = fft_num / 2;
		fft_data = new node*[data_channels];
		fft_spec = new char*[data_channels];
		fft_data_mem = new node[data_channels * size_t(e_fft_num)];
		fft_spec_mem = new char[data_channels * size_t(e_fft_num) * spectrum_size];
		for (int i = 0; i < data_channels; i++) {
			fft_data[i] = fft_data_mem + i * size_t(e_fft_num);
			fft_spec[i] = fft_spec_mem + i * size_t(e_fft_num) * spectrum_size;
			char* index = fft_spec[i];
			for (int64_t j = 0; j < e_fft_num; j++, index += spectrum_size) {
				fft_data[i][j].init_data(spectrum_size, index);
			}
		}
	}
	fft_samp_num = 0;
	ffmpeg->sample_seqs = new double*[channels];
	for (int i = 0; i < channels; i++) {
		ffmpeg->sample_seqs[i] = (double*)fftw_malloc(sizeof(double)*fft_num);
	}
	int nb_last_seq = 0;
	ffmpeg->decoded_frame = nullptr;
	while (av_read_frame(ffmpeg->filefm, ffmpeg->packet) >= 0)//file中调用对应格式的packet获取函数
	{
		if (ffmpeg->packet->stream_index == audio_stream)//如果是音频
		{
			int data_size = 0;
			if (!ffmpeg->decoded_frame) {
				if (!(ffmpeg->decoded_frame = av_frame_alloc())) {
					feedback = lang_pack.get_text(Lang_Type::Decoder, 10);//"\r\n无法为音频帧分配内存！"
					return_val = -5;
					if (output_pcm)pcm.close();
					keep_processing->clear();
					return return_val;
				}
			}
			int ret = 0;
			ret = avcodec_send_packet(ffmpeg->codecfm, ffmpeg->packet);
			if (ret < 0) {
				feedback = lang_pack.get_text(Lang_Type::Decoder, 11);//"\r\n无法提交音频至解码器！"
				return_val = -5;
				if (output_pcm)pcm.close();
				keep_processing->clear();
				return return_val;
			}
			int len = 0;
			while (len >= 0) {
				len = avcodec_receive_frame(ffmpeg->codecfm, ffmpeg->decoded_frame);
				if (len == AVERROR(EAGAIN) || len == AVERROR_EOF)
					break;
				else if (len < 0) {
					feedback = lang_pack.get_text(Lang_Type::Decoder, 12);//"\r\n音频解码出错！"
					return_val = -7;
					if (output_pcm)pcm.close();
					keep_processing->clear();
					return return_val;
				}
				data_size = av_get_bytes_per_sample(ffmpeg->codecfm->sample_fmt);
				if (data_size < 0) {
					/* This should not occur, checking just for paranoia */
					feedback = lang_pack.get_text(Lang_Type::Decoder, 13);//"\r\n无法计算音频数据大小！"
					return_val = -8;
					if (output_pcm)pcm.close();
					keep_processing->clear();
					return return_val;
				}
				//从frame中获取数据
				if (sample_type == 32) {
					if (ffmpeg->codecfm->bits_per_raw_sample == 24)sample_type = 24;
				}
				int nb_samples;
				uint8_t **audiodata;
				//重采样
				if (resamp) {
					nb_samples = static_cast<int>(
						av_rescale_rnd(ffmpeg->decoded_frame->nb_samples, resamp_rate, sample_rate, AV_ROUND_ZERO));
					ret = av_samples_alloc_array_and_samples(&ffmpeg->dst_data, &dst_linesize, channels,
						nb_samples, ffmpeg->codecfm->sample_fmt, 0);
					if (ret < 0) {
						feedback = lang_pack.get_text(Lang_Type::Decoder, 14);//"\r\n无法为重采样数据分配内存！"
						if (output_pcm)pcm.close();
						return_val = -9;
						keep_processing->clear();
						return return_val;
					}
					ret = swr_convert(ffmpeg->swr_ctx, ffmpeg->dst_data, nb_samples,
						(const uint8_t **)ffmpeg->decoded_frame->extended_data, ffmpeg->decoded_frame->nb_samples);
					if (ret < 0) {
						feedback = lang_pack.get_text(Lang_Type::Decoder, 15);//"\r\n重采样错误！"
						if (output_pcm)pcm.close();
						return_val = -10; 
						keep_processing->clear();
						return return_val;
					}
					audiodata = ffmpeg->dst_data;
				}
				else {
					nb_samples = ffmpeg->decoded_frame->nb_samples;
					audiodata = ffmpeg->decoded_frame->extended_data;
				}
				//处理数据
				double **normalized_samples = nullptr;
				int nb_fft_sample = normalize(audiodata, normalized_samples, ffmpeg->sample_seqs, nb_last_seq, nb_samples);
				if (keep_processing && !keep_processing->test_and_set()) {
					keep_processing->clear();
					return_val = -6;
					if (ffmpeg->dst_data)
						av_freep(&ffmpeg->dst_data[0]);
					av_freep(&ffmpeg->dst_data);
					av_frame_free(&ffmpeg->decoded_frame);
					av_packet_unref(ffmpeg->packet);
					delete ffmpeg;
					ffmpeg = nullptr;
					return return_val;
				}
				if (vol_mode != 1) {
					if (nb_fft_sample > 0) {
						int64_t fft_index = fft_samp_num;
						if (fft_index < e_fft_num) {
							if (fft_index + nb_fft_sample > e_fft_num)nb_fft_sample = static_cast<int>(e_fft_num - fft_index);
							fft_samp_num += nb_fft_sample;
							pool.execute(std::bind(&Decode::Decode::FFT, this, fft_data, normalized_samples, fft_index, nb_fft_sample));
						}
						else {
							delete[] normalized_samples;
							normalized_samples = nullptr;
						}
					}
				}
				//输出pcm数据
				if (output_pcm) {
					if (!isplanar) {
						data_size *= channels;
						pcm.write(reinterpret_cast<char*>(audiodata[0]), size_t(nb_samples) * size_t(data_size));
					}
					else {
						for (int i = 0; i < nb_samples; i++)
							for (int ch = 0; ch < real_ch; ch++)
								pcm.write(reinterpret_cast<char*>(audiodata[ch] + size_t(i) * size_t(data_size)), data_size);
					}
				}
				if (ffmpeg->dst_data)
					av_freep(&ffmpeg->dst_data[0]);
				av_freep(&ffmpeg->dst_data);
			}
			av_frame_free(&ffmpeg->decoded_frame);
		}
		av_packet_unref(ffmpeg->packet);
	}
	//数据存入pcmdata
	int count = ffmpeg->codecfm->frame_number;
	int bit_depth_raw = ffmpeg->codecfm->bits_per_raw_sample;
	out_bit_depth = av_get_bytes_per_sample(ffmpeg->codecfm->sample_fmt) * 8;
	//
	pool.wait();
	//
	std::string samp_rate_info = "";
	if (resamp)samp_rate_info = std::to_string(sample_rate) + lang_pack.get_text(Lang_Type::Decoder, 16)
		+ std::to_string(resamp_rate) + lang_pack.get_text(Lang_Type::Decoder, 17);//"***Hz -> ***Hz"
	else samp_rate_info = std::to_string(sample_rate) + lang_pack.get_text(Lang_Type::Decoder, 17);//"***Hz"
	std::string samp_format_info = av_get_sample_fmt_name(ffmpeg->codecfm->sample_fmt);
	//samp_format_info = samp_format_info.replace(samp_format_info.find("AV_SAMPLE_FMT_"), 14, "");
	feedback += lang_pack.get_text(Lang_Type::Decoder, 18) + std::to_string(channels) + lang_pack.get_text(Lang_Type::Decoder, 19) + std::to_string(count)
		+ lang_pack.get_text(Lang_Type::Decoder, 20) + chfmt +    //"    声道：**    总帧数：***    格式：****"
		lang_pack.get_text(Lang_Type::Decoder, 21) + std::to_string(bit_depth_raw) + lang_pack.get_text(Lang_Type::Decoder, 22) +
		std::to_string(out_bit_depth) + lang_pack.get_text(Lang_Type::Decoder, 23) + samp_rate_info
		+ lang_pack.get_text(Lang_Type::Decoder, 24) + samp_format_info;//"\r\n采样位数：**bit -> **bit    采样率：***   采样格式：****"
	if (start_time != 0 && !audio_only)feedback += lang_pack.get_text(Lang_Type::Decoder, 25) +
		std::to_string(start_time) + lang_pack.get_text(Lang_Type::Decoder, 26);//"    延迟：***ms"
	//补充wav头信息
	if (output_pcm) {
		long pcmfilesize = static_cast<long>(pcm.tellp()) - 8;
		pcm.seekp(32);
		pcmh = av_get_bytes_per_sample(ffmpeg->codecfm->sample_fmt)*channels;
		pcm.write(reinterpret_cast<char*>(&pcmh), 2);//byte per sample * channels
		pcm.seekp(34);
		pcmh = out_bit_depth;
		pcm.write(reinterpret_cast<char*>(&pcmh), 2);//bit per sample
		pcm.seekp(4);
		pcm.write(reinterpret_cast<char*>(&pcmfilesize), 4);//file size
		pcm.seekp(40);
		pcmfilesize -= 36;
		pcm.write(reinterpret_cast<char*>(&pcmfilesize), 4);//pcm size
		pcm.close();
		feedback += lang_pack.get_text(Lang_Type::Decoder, 27) + file_name;//"\r\n输出解码音频：****"
	}
	//释放内存
	return_val = 0;
	clear_ffmpeg();
	return 0;
}

std::string Decode::Decode::get_feedback()
{
	return feedback;
}
std::string Decode::Decode::get_file_name()
{
	return file_name;
}
int Decode::Decode::get_return()
{
	return return_val;
}
int64_t Decode::Decode::get_fft_samp_num()
{
	return fft_samp_num;
}
int64_t Decode::Decode::get_milisec()
{
	return milisec;
}
int Decode::Decode::get_channels()
{
	return data_channels;
}
int Decode::Decode::get_samp_rate()
{
	return sample_rate;
}
int Decode::Decode::get_fft_num()
{
	return fft_num;
}
bool Decode::Decode::get_audio_only()
{
	return audio_only;
}
DataStruct::node ** Decode::Decode::get_fft_data()
{
	return fft_data;
}
char ** Decode::Decode::get_fft_spec()
{
	return fft_spec;
}

double Decode::Decode::get_avg_vol()
{
	return total_vol / static_cast<double>(e_fft_num);
}
int Decode::Decode::set_vol_mode(const int &input)
{
	vol_mode = input;
	return 0;
}
int Decode::Decode::set_vol_coef(const double &input)
{
	vol_coef = input;
	return 0;
}

void Decode::Decode::sub_prog_back(int type, double val)
{
	if (prog_single) {
		decoded_num += static_cast<int>(val);
		double temp = decoded_num / static_cast<double>(e_fft_num);
		if (temp >= prog_val + 0.02 || temp == 0) {
			prog_val = temp;
			prog_single(prog_type, prog_val);
		}
	}
}

int Decode::Decode::clear_fft_data()
{
	delete[] fft_data_mem;
	fft_data_mem = nullptr;
	delete[] fft_spec_mem;
	fft_spec_mem = nullptr;
	delete[] fft_data;
	fft_data = nullptr;
	delete[] fft_spec;
	fft_spec = nullptr;
	return 0;
}
int Decode::Decode::clear_ffmpeg()
{
	if (ffmpeg) {
		delete ffmpeg;
		ffmpeg = nullptr;
	}
	return 0;
}



int Decode::Decode::normalize(uint8_t ** const &audiodata, double **&normalized_samples, double **& seqs, int &nb_last, const int &nb_samples)
{
	int nb_last_next = (nb_samples + nb_last) % fft_num;
	int length = nb_samples + nb_last - nb_last_next;
	int nb_fft_samples = length / fft_num;
	if (length > 0) {
		normalized_samples = new double*[channels];
		for (int ch = 0; ch < channels; ch++)normalized_samples[ch] = (double*)fftw_malloc(sizeof(double)*length);
	}
	if (real_ch == channels) 
		switch (sample_type)
		{
		case 1:
			transfer_audio_data_planar<float>(audiodata, normalized_samples, seqs, nb_last, nb_last_next, length);
			break;
		case 2:
			transfer_audio_data_planar<double>(audiodata, normalized_samples, seqs, nb_last, nb_last_next, length);
			break;
		case 8:
			transfer_audio_data_planar<char>(audiodata, normalized_samples, seqs, nb_last, nb_last_next, length);
			break;
		case 16:
			transfer_audio_data_planar<short>(audiodata, normalized_samples, seqs, nb_last, nb_last_next, length);
			break;
		case 24:
			transfer_audio_data_planar<int, 24>(audiodata, normalized_samples, seqs, nb_last, nb_last_next, length);
			break;
		case 32:
			transfer_audio_data_planar<int>(audiodata, normalized_samples, seqs, nb_last, nb_last_next, length);
			break;
		case 64:
			transfer_audio_data_planar<int64_t>(audiodata, normalized_samples, seqs, nb_last, nb_last_next, length);
			break;
		default:
			break;
		}
	else
		switch (sample_type)
		{
		case 1:
			transfer_audio_data_packed<float>(audiodata, normalized_samples, seqs, nb_last, nb_last_next, length);
			break;
		case 2:
			transfer_audio_data_packed<double>(audiodata, normalized_samples, seqs, nb_last, nb_last_next, length);
			break;
		case 8:
			transfer_audio_data_packed<char>(audiodata, normalized_samples, seqs, nb_last, nb_last_next, length);
			break;
		case 16:
			transfer_audio_data_packed<short>(audiodata, normalized_samples, seqs, nb_last, nb_last_next, length);
			break;
		case 24:
			transfer_audio_data_packed<int, 24>(audiodata, normalized_samples, seqs, nb_last, nb_last_next, length);
			break;
		case 32:
			transfer_audio_data_packed<int>(audiodata, normalized_samples, seqs, nb_last, nb_last_next, length);
			break;
		case 64:
			transfer_audio_data_packed<int64_t>(audiodata, normalized_samples, seqs, nb_last, nb_last_next, length);
			break;
		default:
			break;
		}
	nb_last = nb_last_next;
	//响度计算和匹配
	if (vol_mode >= 0 && length > 0) {
		if (vol_coef != 0.0)
			for (int ch = 0; ch < channels; ch++)
				for (int i = 0; i < length; i++)
					normalized_samples[ch][i] *= vol_coef;
		double sum = 0.0;
		for (int ch = 0; ch < channels; ch++)
			for (int i = 0; i < length; i++)
				sum += normalized_samples[ch][i] * normalized_samples[ch][i];
		total_vol += sum / fft_num / channels;
		if (vol_mode == 1) {
			for (int ch = 0; ch < channels; ch++) {
				fftw_free(normalized_samples[ch]);
				normalized_samples[ch] = nullptr;
			}
			delete[] normalized_samples;
		}
	}
	return nb_fft_samples;
}
int Decode::Decode::FFT(DataStruct::node ** nodes, double ** in, int fft_index, const int nb_fft)
{
	int fi_m = nb_fft * fft_num;
	fftw_complex* out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fft_num);
	for (int fi = 0; fi < fi_m; fi += fft_num) {
		for (int ch = 0; ch < data_channels; ch++) {
			fftw_execute_dft_r2c(plan, in[ch] + fi, out);
			FD8(reinterpret_cast<double*>(out), nodes[ch] + fft_index);
		}
		fft_index++;
	}
	fftw_free(out);
	for (int ch = 0; ch < channels; ch++) {
		fftw_free(in[ch]);
		in[ch] = nullptr;
	}
	delete[] in;
	in = nullptr;
	out = nullptr;
	sub_prog_back(prog_type, static_cast<double>(nb_fft));
	return 0;
}
int Decode::Decode::FD8(double * inseq, DataStruct::node * outseq)
{
	char *out = outseq->getdata();
	for (int i = 0; i < fft_num; i += 2) {
		double addx = inseq[i] * inseq[i] + inseq[i + 1] * inseq[i + 1];
		addx = 10.0 * log10(addx);
		addx -= MaxdB;
		addx *= c_min_db;
		addx += static_cast<double>(std::numeric_limits<char>::max());
		addx = std::min(addx, static_cast<double>(std::numeric_limits<char>::max()));
		addx = std::max(addx, static_cast<double>(std::numeric_limits<char>::min()));
		addx = round(addx);
		*(out++) = static_cast<char>(addx);
	}
	out = nullptr;
	return 0;
}


int Decode::Decode_SSE::FD8(double * inseq, DataStruct::node * outseq)
{
	char *out = outseq->getdata();
	__m128d const10 = _mm_set1_pd(10.0);
	__m128d const_maxdb = _mm_set1_pd(MaxdB);
	__m128d const_mindb = _mm_set1_pd(c_min_db);
	__m128d const127 = _mm_set1_pd(static_cast<double>(std::numeric_limits<char>::max()));
	__m128d constm128 = _mm_set1_pd(static_cast<double>(std::numeric_limits<char>::min()));
	for (int i = 0; i < fft_num / 4; i++) {
		__m128d seq1 = _mm_load_pd(inseq);
		__m128d seq2 = _mm_load_pd(inseq + 2);
		seq1 = _mm_mul_pd(seq1, seq1);
		seq2 = _mm_mul_pd(seq2, seq2);
		__m128d temp = _mm_hadd_pd(seq1, seq2);
		temp = _mm_log10_pd_cmpt(temp);
		temp = _mm_mul_pd(temp, const10);
		temp = _mm_sub_pd(temp, const_maxdb);
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


int Decode::Decode_AVX::transfer_audio_data_planar_float(uint8_t** const audiodata,
	double** const normalized_samples, double** const seqs, 
	const int& nb_last, const int& nb_last_next, const int& length, const int& nb_samples)
{
	int index = 0, index2 = 0;
	if (length > 0) {
		for (int ch = 0; ch < channels; ch++)
			for (int i = 0; i < nb_last; i++)normalized_samples[ch][i] = seqs[ch][i];
		index = nb_last;
	}
	else index2 = nb_last;
	for (int ch = 0; ch < channels; ch++) {
		int avxlength = nb_samples / 8;
		int avx_last = nb_samples % 8;
		int nb_to_norm = std::max(length - nb_last, 0);
		int threshold = nb_to_norm / 8;
		int remainder = nb_to_norm % 8;
		float* tempf = reinterpret_cast<float*>(audiodata[ch]);
		double* tempd = nullptr;
		if (normalized_samples)tempd = normalized_samples[ch] + index;
		double* temp_seq = seqs[ch] + index2;
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
					if (j < remainder) *(tempd++) = m256d_f64(tempd_1, j);
					else *(temp_seq++) = m256d_f64(tempd_1, j);
				remainder -= 4;
				for (int j = 0; j < 4; j++)
					if (j < remainder) *(tempd++) = m256d_f64(tempd_2, j);
					else *(temp_seq++) = m256d_f64(tempd_2, j);
				tempf += 8;
				threshold++;
			}
			for (int i = threshold; i < avxlength; i++) {
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
	return 0;
}
int Decode::Decode_AVX::transfer_audio_data_packed_float(uint8_t** const audiodata, 
	double** const normalized_samples, double** const seqs, 
	const int& nb_last, const int& length, const int& nb_samples)
{
	int index = 0, index2 = 0;
	if (length > 0) {
		for (int ch = 0; ch < channels; ch++)
			for (int i = 0; i < nb_last; i++)normalized_samples[ch][i] = seqs[ch][i];
		index = nb_last;
	}
	else index2 = nb_last;
	int ch = 0;
	float* tempf = reinterpret_cast<float*>(audiodata[0]);
	int avxlength = nb_samples * channels / 8;
	int avx_last = (nb_samples * channels) % 8;
	int nb_to_norm = std::max(length - nb_last, 0);
	int threshold = nb_to_norm * channels / 8;
	int remainder = nb_to_norm * channels % 8;
	for (int i = 0; i < threshold; i++) {
		__m256 temp256 = _mm256_load_ps(tempf);
		__m128 temp128_1 = _mm256_extractf128_ps(temp256, 0);
		__m128 temp128_2 = _mm256_extractf128_ps(temp256, 1);
		__m256d tempd_1 = _mm256_cvtps_pd(temp128_1);
		__m256d tempd_2 = _mm256_cvtps_pd(temp128_2);
		double out;
		for (int j = 0; j < 4; j++) {
			out = m256d_f64(tempd_1, j);
			normalized_samples[ch++][index] = out;
			if (ch == channels) {
				ch = 0;
				index++;
			}
		}
		for (int j = 0; j < 4; j++) {
			out = m256d_f64(tempd_2, j);
			normalized_samples[ch++][index] = out;
			if (ch == channels) {
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
				if (j < 4)out = m256d_f64(tempd_1, j);
				else out = m256d_f64(tempd_2, j - 4);
				if (index < length) {
					normalized_samples[ch++][index] = out;
					if (ch == channels) {
						ch = 0;
						index++;
					}
				}
				else {
					seqs[ch++][index2] = out;
					if (ch == channels) {
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
				out = m256d_f64(tempd_1, j);
				seqs[ch++][index2] = out;
				if (ch == channels) {
					ch = 0;
					index2++;
				}
			}
			for (int j = 0; j < 4; j++) {
				out = m256d_f64(tempd_2, j);
				seqs[ch++][index2] = out;
				if (ch == channels) {
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
			if (ch == channels) {
				ch = 0;
				index++;
			}
		}
		else {
			seqs[ch++][index2] = static_cast<double>(*(tempf++));
			if (ch == channels) {
				ch = 0;
				index2++;
			}
		}
	}
	return 0;
}

int Decode::Decode_AVX::normalize(uint8_t** const& audiodata, double**& normalized_samples, double**& seqs, int& nb_last, const int& nb_samples)
{
	int nb_last_next = (nb_samples + nb_last) % fft_num;
	int length = nb_samples + nb_last - nb_last_next;
	int nb_fft_samples = length / fft_num;
	if (length > 0) {
		normalized_samples = new double* [channels];
		for (int ch = 0; ch < channels; ch++)normalized_samples[ch] = (double*)fftw_malloc(sizeof(double) * length);
	}
	if (real_ch == channels)
		switch (sample_type)
		{
		case 1:
			transfer_audio_data_planar_float(audiodata, normalized_samples, seqs, nb_last, nb_last_next, length, nb_samples);
			break;
		case 2:
			transfer_audio_data_planar<double>(audiodata, normalized_samples, seqs, nb_last, nb_last_next, length);
			break;
		case 8:
			transfer_audio_data_planar<char>(audiodata, normalized_samples, seqs, nb_last, nb_last_next, length);
			break;
		case 16:
			transfer_audio_data_planar<short>(audiodata, normalized_samples, seqs, nb_last, nb_last_next, length);
			break;
		case 24:
			transfer_audio_data_planar<int, 24>(audiodata, normalized_samples, seqs, nb_last, nb_last_next, length);
			break;
		case 32:
			transfer_audio_data_planar<int>(audiodata, normalized_samples, seqs, nb_last, nb_last_next, length);
			break;
		case 64:
			transfer_audio_data_planar<int64_t>(audiodata, normalized_samples, seqs, nb_last, nb_last_next, length);
			break;
		default:
			break;
		}
	else
		switch (sample_type)
		{
		case 1:
			transfer_audio_data_packed_float(audiodata, normalized_samples, seqs, nb_last, length, nb_samples);
			break;
		case 2:
			transfer_audio_data_packed<double>(audiodata, normalized_samples, seqs, nb_last, nb_last_next, length);
			break;
		case 8:
			transfer_audio_data_packed<char>(audiodata, normalized_samples, seqs, nb_last, nb_last_next, length);
			break;
		case 16:
			transfer_audio_data_packed<short>(audiodata, normalized_samples, seqs, nb_last, length, nb_samples);
			break;
		case 24:
			transfer_audio_data_packed<int, 24>(audiodata, normalized_samples, seqs, nb_last, length, nb_samples);
			break;
		case 32:
			transfer_audio_data_packed<int>(audiodata, normalized_samples, seqs, nb_last, length, nb_samples);
			break;
		case 64:
			transfer_audio_data_packed<int64_t>(audiodata, normalized_samples, seqs, nb_last, nb_last_next, length);
			break;
		default:
			break;
		}
	nb_last = nb_last_next;
	//响度计算和匹配
	if (vol_mode >= 0 && length > 0) {
		int avxlength = length / 4;
		if (vol_coef != 0.0) {
			__m256d vol_vect = _mm256_set1_pd(vol_coef);
			for (int ch = 0; ch < channels; ch++) {
				double* tempd = normalized_samples[ch];
				for (int i = 0; i < avxlength; i++) {
					__m256d temp256 = _mm256_load_pd(tempd);
					temp256 = _mm256_mul_pd(temp256, vol_vect);
					_mm256_store_pd(tempd, temp256);
					tempd += 4;
				}
			}
		}
		__m256d sum256 = _mm256_set1_pd(0.0);
		for (int ch = 0; ch < channels; ch++) {
			double* tempd = normalized_samples[ch];
			for (int i = 0; i < avxlength; i++) {
				__m256d temp256 = _mm256_load_pd(tempd);
				temp256 = _mm256_mul_pd(temp256, temp256);
				sum256 = _mm256_add_pd(temp256, sum256);
				tempd += 4;
			}
		}
		total_vol += (m256d_f64(sum256, 0) + m256d_f64(sum256, 1) + m256d_f64(sum256, 2) + m256d_f64(sum256, 3)) / fft_num / channels;
		if (vol_mode == 1) {
			for (int ch = 0; ch < channels; ch++) {
				fftw_free(normalized_samples[ch]);
				normalized_samples[ch] = nullptr;
			}
			delete[] normalized_samples;
		}
	}
	return nb_fft_samples;
}
int Decode::Decode_AVX::FD8(double* inseq, DataStruct::node* outseq)
{
	char* out = outseq->getdata();
	__m256d const10 = _mm256_set1_pd(10.0);
	__m256d const_maxdb = _mm256_set1_pd(MaxdB);
	__m256d const_mindb = _mm256_set1_pd(c_min_db);
	__m256d const127 = _mm256_set1_pd(static_cast<double>(std::numeric_limits<char>::max())); // 127
	__m256d constm128 = _mm256_set1_pd(static_cast<double>(std::numeric_limits<char>::min())); // -128
	for (int i = 0; i < fft_num / 8; i++) {
		__m256d seq1 = _mm256_load_pd(inseq);
		__m256d seq2 = _mm256_load_pd(inseq + 4);
		seq1 = _mm256_mul_pd(seq1, seq1);
		seq2 = _mm256_mul_pd(seq2, seq2);
		__m256d temp = _mm256_hadd_pd(seq1, seq2);
		temp = _mm256_log10_pd_cmpt(temp);
		temp = _mm256_mul_pd(temp, const10);
		temp = _mm256_sub_pd(temp, const_maxdb);
		temp = _mm256_mul_pd(temp, const_mindb);
		temp = _mm256_add_pd(temp, const127);
		temp = _mm256_max_pd(temp, constm128);
		temp = _mm256_min_pd(temp, const127);
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


int Decode::Decode_AVX2::transfer_audio_data_packed_int16(uint8_t** const audiodata, 
	double** const normalized_samples, double** const seqs, 
	const int& nb_last, const int& length, const int& nb_samples)
{
	int index = 0, index2 = 0;
	if (length > 0) {
		for (int ch = 0; ch < channels; ch++)
			for (int i = 0; i < nb_last; i++)normalized_samples[ch][i] = seqs[ch][i];
		index = nb_last;
	}
	else index2 = nb_last;
	int ch = 0;
	__m128i* temp128 = reinterpret_cast<__m128i*>(audiodata[0]);
	short* temps = reinterpret_cast<short*>(audiodata[0]);
	__m256d const16 = _mm256_set1_pd(1.0 / 32767.0);
	int avxlength = nb_samples * channels / 8;
	int avx_last = (nb_samples * channels) % 8;
	int nb_to_norm = std::max(length - nb_last, 0);
	int threshold = nb_to_norm * channels / 8;
	int remainder = nb_to_norm * channels % 8;
	for (int i = 0; i < threshold; i++) {
		__m128i temp16 = _mm_load_si128(temp128);
		__m256i temp32 = _mm256_cvtepi16_epi32(temp16);
		__m128i temp32_1 = _mm256_extracti128_si256(temp32, 0);
		__m128i temp32_2 = _mm256_extracti128_si256(temp32, 1);
		__m256d tempd_1 = _mm256_cvtepi32_pd(temp32_1);
		__m256d tempd_2 = _mm256_cvtepi32_pd(temp32_2);
		tempd_1 = _mm256_mul_pd(tempd_1, const16);
		tempd_2 = _mm256_mul_pd(tempd_2, const16);
		double out;
		for (int j = 0; j < 4; j++) {
			out = m256d_f64(tempd_1, j);
			normalized_samples[ch++][index] = out;
			if (ch == channels) {
				ch = 0;
				index++;
			}
		}
		for (int j = 0; j < 4; j++) {
			out = m256d_f64(tempd_2, j);
			normalized_samples[ch++][index] = out;
			if (ch == channels) {
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
			tempd_1 = _mm256_mul_pd(tempd_1, const16);
			tempd_2 = _mm256_mul_pd(tempd_2, const16);
			for (int j = 0; j < 8; j++) {
				double out;
				if (j < 4)out = m256d_f64(tempd_1, j);
				else out = m256d_f64(tempd_2, j - 4);
				if (index < length) {
					normalized_samples[ch++][index] = out;
					if (ch == channels) {
						ch = 0;
						index++;
					}
				}
				else {
					seqs[ch++][index2] = out;
					if (ch == channels) {
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
			tempd_1 = _mm256_mul_pd(tempd_1, const16);
			tempd_2 = _mm256_mul_pd(tempd_2, const16);
			double out;
			for (int j = 0; j < 4; j++) {
				out = m256d_f64(tempd_1, j);
				seqs[ch++][index2] = out;
				if (ch == channels) {
					ch = 0;
					index2++;
				}
			}
			for (int j = 0; j < 4; j++) {
				out = m256d_f64(tempd_2, j);
				seqs[ch++][index2] = out;
				if (ch == channels) {
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
			if (ch == channels) {
				ch = 0;
				index++;
			}
		}
		else {
			seqs[ch++][index2] = static_cast<double>(*(temps++)) / 32767.0;
			if (ch == channels) {
				ch = 0;
				index2++;
			}
		}
	}
	return 0;
}
int Decode::Decode_AVX2::transfer_audio_data_packed_int24(uint8_t** const audiodata, 
	double** const normalized_samples, double** const seqs, 
	const int& nb_last, const int& length, const int& nb_samples)
{
	int index = 0, index2 = 0;
	if (length > 0) {
		for (int ch = 0; ch < channels; ch++)
			for (int i = 0; i < nb_last; i++)normalized_samples[ch][i] = seqs[ch][i];
		index = nb_last;
	}
	else index2 = nb_last;
	int ch = 0;
	int* tempi = reinterpret_cast<int*>(audiodata[0]);
	__m256i* temp256 = reinterpret_cast<__m256i*>(audiodata[0]);
	__m256d const24 = _mm256_set1_pd(1.0 / 8388607.0);
	int avxlength = nb_samples * channels / 8;
	int avx_last = (nb_samples * channels) % 8;
	int nb_to_norm = std::max(length - nb_last, 0);
	int threshold = nb_to_norm * channels / 8;
	int remainder = nb_to_norm * channels % 8;
	for (int i = 0; i < threshold; i++) {
		__m256i temp32 = _mm256_load_si256(temp256);
		temp32 = _mm256_srai_epi32(temp32, 8);
		__m128i temp32_1 = _mm256_extracti128_si256(temp32, 0);
		__m128i temp32_2 = _mm256_extracti128_si256(temp32, 1);
		__m256d tempd_1 = _mm256_cvtepi32_pd(temp32_1);
		__m256d tempd_2 = _mm256_cvtepi32_pd(temp32_2);
		tempd_1 = _mm256_mul_pd(tempd_1, const24);
		tempd_2 = _mm256_mul_pd(tempd_2, const24);
		double out;
		for (int j = 0; j < 4; j++) {
			out = m256d_f64(tempd_1, j);
			normalized_samples[ch++][index] = out;
			if (ch == channels) {
				ch = 0;
				index++;
			}
		}
		for (int j = 0; j < 4; j++) {
			out = m256d_f64(tempd_2, j);
			normalized_samples[ch++][index] = out;
			if (ch == channels) {
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
			tempd_1 = _mm256_mul_pd(tempd_1, const24);
			tempd_2 = _mm256_mul_pd(tempd_2, const24);
			for (int j = 0; j < 8; j++) {
				double out;
				if (j < 4)out = m256d_f64(tempd_1, j);
				else out = m256d_f64(tempd_2, j - 4);
				if (index < length) {
					normalized_samples[ch++][index] = out;
					if (ch == channels) {
						ch = 0;
						index++;
					}
				}
				else {
					seqs[ch++][index2] = out;
					if (ch == channels) {
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
			tempd_1 = _mm256_mul_pd(tempd_1, const24);
			tempd_2 = _mm256_mul_pd(tempd_2, const24);
			double out;
			for (int j = 0; j < 4; j++) {
				out = m256d_f64(tempd_1, j);
				seqs[ch++][index2] = out;
				if (ch == channels) {
					ch = 0;
					index2++;
				}
			}
			for (int j = 0; j < 4; j++) {
				out = m256d_f64(tempd_2, j);
				seqs[ch++][index2] = out;
				if (ch == channels) {
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
			if (ch == channels) {
				ch = 0;
				index++;
			}
		}
		else {
			seqs[ch++][index2] = static_cast<double>(*(tempi++) >> 8) / 8388607.0;
			if (ch == channels) {
				ch = 0;
				index2++;
			}
		}
	}
	return 0;
}
int Decode::Decode_AVX2::transfer_audio_data_packed_int32(uint8_t** const audiodata, 
	double** const normalized_samples, double** const seqs, 
	const int& nb_last, const int& length, const int& nb_samples)
{
	int index = 0, index2 = 0;
	if (length > 0) {
		for (int ch = 0; ch < channels; ch++)
			for (int i = 0; i < nb_last; i++)normalized_samples[ch][i] = seqs[ch][i];
		index = nb_last;
	}
	else index2 = nb_last;
	int ch = 0;
	int* tempi32 = reinterpret_cast<int*>(audiodata[0]);
	__m256i* temp256 = reinterpret_cast<__m256i*>(audiodata[0]);
	__m256d const32 = _mm256_set1_pd(1.0 / static_cast<double>(std::numeric_limits<int>::max()));
	int avxlength = nb_samples * channels / 8;
	int avx_last = (nb_samples * channels) % 8;
	int nb_to_norm = std::max(length - nb_last, 0);
	int threshold = nb_to_norm * channels / 8;
	int remainder = nb_to_norm * channels % 8;
	for (int i = 0; i < threshold; i++) {
		__m256i temp32 = _mm256_load_si256(temp256);
		__m128i temp32_1 = _mm256_extracti128_si256(temp32, 0);
		__m128i temp32_2 = _mm256_extracti128_si256(temp32, 1);
		__m256d tempd_1 = _mm256_cvtepi32_pd(temp32_1);
		__m256d tempd_2 = _mm256_cvtepi32_pd(temp32_2);
		tempd_1 = _mm256_mul_pd(tempd_1, const32);
		tempd_2 = _mm256_mul_pd(tempd_2, const32);
		double out;
		for (int j = 0; j < 4; j++) {
			out = m256d_f64(tempd_1, j);
			normalized_samples[ch++][index] = out;
			if (ch == channels) {
				ch = 0;
				index++;
			}
		}
		for (int j = 0; j < 4; j++) {
			out = m256d_f64(tempd_2, j);
			normalized_samples[ch++][index] = out;
			if (ch == channels) {
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
			tempd_1 = _mm256_mul_pd(tempd_1, const32);
			tempd_2 = _mm256_mul_pd(tempd_2, const32);
			for (int j = 0; j < 8; j++) {
				double out;
				if (j < 4)out = m256d_f64(tempd_1, j);
				else out = m256d_f64(tempd_2, j - 4);
				if (index < length) {
					normalized_samples[ch++][index] = out;
					if (ch == channels) {
						ch = 0;
						index++;
					}
				}
				else {
					seqs[ch++][index2] = out;
					if (ch == channels) {
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
			tempd_1 = _mm256_mul_pd(tempd_1, const32);
			tempd_2 = _mm256_mul_pd(tempd_2, const32);
			double out;
			for (int j = 0; j < 4; j++) {
				out = m256d_f64(tempd_1, j);
				seqs[ch++][index2] = out;
				if (ch == channels) {
					ch = 0;
					index2++;
				}
			}
			for (int j = 0; j < 4; j++) {
				out = m256d_f64(tempd_2, j);
				seqs[ch++][index2] = out;
				if (ch == channels) {
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
			normalized_samples[ch++][index] = static_cast<double>(*(tempi32++)) / static_cast<double>(std::numeric_limits<int>::max());
			if (ch == channels) {
				ch = 0;
				index++;
			}
		}
		else {
			seqs[ch++][index2] = static_cast<double>(*(tempi32++)) / static_cast<double>(std::numeric_limits<int>::max());
			if (ch == channels) {
				ch = 0;
				index2++;
			}
		}
	}
	return 0;
}

int Decode::Decode_AVX2::normalize(uint8_t** const& audiodata, double**& normalized_samples, double**& seqs, int& nb_last, const int& nb_samples)
{
	int nb_last_next = (nb_samples + nb_last) % fft_num;
	int length = nb_samples + nb_last - nb_last_next;
	int nb_fft_samples = length / fft_num;
	if (length > 0) {
		normalized_samples = new double*[channels];
		for (int ch = 0; ch < channels; ch++)normalized_samples[ch] = (double*)fftw_malloc(sizeof(double)*length);
	}
	if (real_ch == channels)
		switch (sample_type)
		{
		case 1:
			transfer_audio_data_planar_float(audiodata, normalized_samples, seqs, nb_last, nb_last_next, length, nb_samples);
			break;
		case 2:
			transfer_audio_data_planar<double>(audiodata, normalized_samples, seqs, nb_last, nb_last_next, length);
			break;
		case 8:
			transfer_audio_data_planar<char>(audiodata, normalized_samples, seqs, nb_last, nb_last_next, length);
			break;
		case 16:
			transfer_audio_data_planar<short>(audiodata, normalized_samples, seqs, nb_last, nb_last_next, length);
			break;
		case 24:
			transfer_audio_data_planar<int, 24>(audiodata, normalized_samples, seqs, nb_last, nb_last_next, length);
			break;
		case 32:
			transfer_audio_data_planar<int>(audiodata, normalized_samples, seqs, nb_last, nb_last_next, length);
			break;
		case 64:
			transfer_audio_data_planar<int64_t>(audiodata, normalized_samples, seqs, nb_last, nb_last_next, length);
			break;
		default:
			break;
		}
	else
		switch (sample_type)
		{
		case 1:
			transfer_audio_data_packed_float(audiodata, normalized_samples, seqs, nb_last, length, nb_samples);
			break;
		case 2:
			transfer_audio_data_packed<double>(audiodata, normalized_samples, seqs, nb_last, nb_last_next, length);
			break;
		case 8:
			transfer_audio_data_packed<char>(audiodata, normalized_samples, seqs, nb_last, nb_last_next, length);
			break;
		case 16:
			transfer_audio_data_packed_int16(audiodata, normalized_samples, seqs, nb_last, length, nb_samples);
			break;
		case 24:
			transfer_audio_data_packed_int24(audiodata, normalized_samples, seqs, nb_last, length, nb_samples);
			break;
		case 32:
			transfer_audio_data_packed_int32(audiodata, normalized_samples, seqs, nb_last, length, nb_samples);
			break;
		case 64:
			transfer_audio_data_packed<int64_t>(audiodata, normalized_samples, seqs, nb_last, nb_last_next, length);
			break;
		default:
			break;
		}
	nb_last = nb_last_next;
	//响度计算和匹配
	if (vol_mode >= 0 && length > 0) {
		int avxlength = length / 4;
		if (vol_coef != 0.0) {
			__m256d vol_vect = _mm256_set1_pd(vol_coef);
			for (int ch = 0; ch < channels; ch++) {
				double *tempd = normalized_samples[ch];
				for (int i = 0; i < avxlength; i++) {
					__m256d temp256 = _mm256_load_pd(tempd);
					temp256 = _mm256_mul_pd(temp256, vol_vect);
					_mm256_store_pd(tempd, temp256);
					tempd += 4;
				}
			}
		}
		__m256d sum256 = _mm256_set1_pd(0.0);
		for (int ch = 0; ch < channels; ch++) {
			double *tempd = normalized_samples[ch];
			for (int i = 0; i < avxlength; i++) {
				__m256d temp256 = _mm256_load_pd(tempd);
				sum256 = _mm256_fmadd_pd(temp256, temp256, sum256);
				tempd += 4;
			}
		}
		total_vol += (m256d_f64(sum256, 0) + m256d_f64(sum256, 1) + m256d_f64(sum256, 2) + m256d_f64(sum256, 3)) / fft_num / channels;
		if (vol_mode == 1) {
			for (int ch = 0; ch < channels; ch++) {
				fftw_free(normalized_samples[ch]);
				normalized_samples[ch] = nullptr;
			}
			delete[] normalized_samples;
		}
	}
	return nb_fft_samples;
}
int Decode::Decode_AVX2::FD8(double * inseq, DataStruct::node * outseq)
{
	char *out = outseq->getdata();
	__m256d const10 = _mm256_set1_pd(10.0);
	__m256d const_maxdb = _mm256_set1_pd(MaxdB);
	__m256d const_mindb = _mm256_set1_pd(c_min_db);
	__m256d const127 = _mm256_set1_pd(static_cast<double>(std::numeric_limits<char>::max())); // 127
	__m256d constm128 = _mm256_set1_pd(static_cast<double>(std::numeric_limits<char>::min())); // -128
	for (int i = 0; i < fft_num / 8; i++) {
		__m256d seq1 = _mm256_load_pd(inseq);
		__m256d seq2 = _mm256_load_pd(inseq + 4);
		seq1 = _mm256_mul_pd(seq1, seq1);
		seq2 = _mm256_mul_pd(seq2, seq2);
		__m256d temp = _mm256_hadd_pd(seq1, seq2);
		temp = _mm256_log10_pd_cmpt(temp);
		temp = _mm256_fmsub_pd(temp, const10, const_maxdb);
		temp = _mm256_fmadd_pd(temp, const_mindb, const127);
		temp = _mm256_max_pd(temp, constm128);
		temp = _mm256_min_pd(temp, const127);
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
