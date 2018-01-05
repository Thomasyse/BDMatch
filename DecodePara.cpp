#include "DecodePara.h"

#define progmaxnum 150
using namespace BDMatch;

BDMatch::Decode::Decode(String^ filename0, int FFTnum0, bool outputpcm0, int mindb0, int resamprate0, List<Task^>^ tasks0, fftw_plan plan0,
	ProgressCallback^ progback0, ProgMaxCallback^ progmax0)
{
	filename = filename0;
	FFTnum = FFTnum0;
	outputpcm = outputpcm0;
	progback = progback0;
	progmax = progmax0;
	mindb = mindb0;
	resamprate = resamprate0;
	tasks = tasks0;
	plan = plan0;
}

void BDMatch::Decode::decodeaudio()
{
	using namespace System::IO;
	using namespace Node;
	using namespace System::Threading;

	AVFormatContext *filefm = NULL;//�ļ���ʽ
	std::string filestr = marshal_as<std::string>(filename->ToString());
	if (avformat_open_input(&filefm, filestr.c_str(), NULL, NULL) != 0) {//��ȡ�ļ���ʽ
		feedback = "�޷����ļ���";
		returnval = -1;
		return;
	}
	if (avformat_find_stream_info(filefm, NULL) < 0) {//��ȡ�ļ�������Ƶ������Ϣ
		feedback = "�޷���ȡ�ļ�����Ϣ��";
		returnval = -1;
		return;
	}
	AVCodecContext *codecfm = NULL;//�����ʽ
	AVCodec *codec = NULL;//������
	unsigned int j;
	// Find the first audio stream
	audiostream = -1;
	for (j = 0; j<filefm->nb_streams; j++)//�ҵ���Ƶ��Ӧ��stream
		if (filefm->streams[j]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			audiostream = j;
			break;
		}
	if (audiostream == -1)
	{
		feedback = "����Ƶ����";
		returnval = -2;
		return; // Didn't find a audio stream
	}
	milisecnum = static_cast<int>(ceil(filefm->duration / 10000));
	codec = avcodec_find_decoder(filefm->streams[audiostream]->codecpar->codec_id);//Ѱ�ҽ�����
	if (!codec)
	{
		feedback = "�޷��ҵ���Ƶ�Ķ�Ӧ��������";
		returnval = -3;
		return; // Codec not found codec
	}
	codecfm = avcodec_alloc_context3(codec);//��Ƶ�ı�����Ϣ
	if (!codecfm)
	{
		feedback = "�޷�������Ƶ�Ľ�������Ϣ��";
		returnval = -3;
		return; // Failed to allocate the codec context
	}
	int getcodecpara = avcodec_parameters_to_context(codecfm, filefm->streams[audiostream]->codecpar);
	if (getcodecpara < 0) {
		feedback = "�޷���ȡ��Ƶ�Ľ�������Ϣ��";
		returnval = -3;
		return; // Failed to allocate the codec context
	}

	feedback = "�����ţ�" + (audiostream).ToString() +
		"    ��Ƶ���룺" + marshal_as<String^>(codec->long_name);

	if (avcodec_open2(codecfm, codec, NULL) < 0)//�����߽���Ա�������Ľ��뺯���е���pInCodec�еĶ�Ӧ���뺯��
	{
		feedback = "�޷�����Ƶ�Ķ�Ӧ��������";
		returnval = -3;
		return; // Could not open codec
	}
	
	//�ز�������1
	samplerate = codecfm->sample_rate;
	bool resamp = false;
	if (resamprate > 0 && samplerate != resamprate)resamp = true;
	//׼��д���ļ�
	FILE* pcm = NULL;
	int sampletype = 0;
	Int16 pcmh = 0;
	int pcmh32 = 0;
	std::string filename;
	if (outputpcm) {//д��wavͷ��Ϣ
		filename = filefm->filename;
		filename += ".wav";
		pcm = fopen(filename.c_str(), "wb");
		if (pcm == NULL) {
			feedback = "�޷���Ҫд���PCM�ļ���";
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
	//ȷ�����λ��
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
	//��������
	int samplecount = 0, samplenum = 0;
	channels = codecfm->channels;
	//�ز�������2
	uint8_t **dst_data = NULL;
	int dst_linesize;
	struct SwrContext *swr_ctx;
	bool setsampnum = false;
	//����Ƶ����(���ز���)
	if (resamp) {
		samplenum = static_cast<int>(ceil(samplerate / 1000.0*filefm->duration / 1000.0));
		swr_ctx = swr_alloc();
		if (!swr_ctx) {
			feedback = "�޷������ز���������";
			returnval = -8;
			return;
		}
		//�ز���ѡ��
		av_opt_set_int(swr_ctx, "in_channel_layout", codecfm->channel_layout, 0);
		av_opt_set_int(swr_ctx, "in_sample_rate", samplerate, 0);
		av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt", codecfm->sample_fmt, 0);

		av_opt_set_int(swr_ctx, "out_channel_layout", codecfm->channel_layout, 0);
		av_opt_set_int(swr_ctx, "out_sample_rate", resamprate, 0);
		av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt", codecfm->sample_fmt, 0);
		/* initialize the resampling context */
		if ((swr_init(swr_ctx)) < 0) {
			feedback = "�޷���ʼ���ز���������";
			returnval = -8;
			return;
		}
	}
	else {
		samplenum = static_cast<int>(ceil(samplerate / 1000.0*filefm->duration / 1000.0));
	}
	efftnum = static_cast<int>(ceil(samplenum / float(FFTnum)));

	AVPacket *packet = av_packet_alloc();
	double shiftf = 0;
	progmax(progmaxnum);
	fftdata = new std::vector<std::vector<node*>>(2, std::vector<node*>(efftnum));
	fftsampnum = 0;
	noded* sample_seq_l = new noded(FFTnum);
	noded* sample_seq_r = new noded(FFTnum);
	AVFrame *decoded_frame = NULL;
	int realch = 1;
	String ^chfmt = "Packed";
	bool isplanar = av_sample_fmt_is_planar(codecfm->sample_fmt);
	int tempsize = 0;
	if (isplanar) {
		realch = codecfm->channels;
		chfmt = "Planar";
		tempsize = av_get_bytes_per_sample(codecfm->sample_fmt);
	}
	else tempsize = av_get_bytes_per_sample(codecfm->sample_fmt)*codecfm->channels;
	uint8_t *temp = new uint8_t[tempsize];
	while (av_read_frame(filefm, packet) >= 0)//file�е��ö�Ӧ��ʽ��packet��ȡ����
	{
		if (packet->stream_index == audiostream)//�������Ƶ
		{
			int data_size = 0, i = 0, ch = 0;
			if (!decoded_frame) {
				if (!(decoded_frame = av_frame_alloc())) {
					feedback = "�޷�Ϊ��Ƶ֡�����ڴ棡";
					returnval = -5;
					if (outputpcm)fclose(pcm);
					return;
				}
			}
			int ret = 0;
			ret = avcodec_send_packet(codecfm, packet);
			if (ret < 0) {
				feedback = "�޷��ύ��Ƶ����������";
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
					feedback = "��Ƶ�������";
					returnval = -6;
					if (outputpcm)fclose(pcm);
					return;
				}
				data_size = av_get_bytes_per_sample(codecfm->sample_fmt);
				if (data_size < 0) {
					/* This should not occur, checking just for paranoia */
					feedback = "�޷�������Ƶ���ݴ�С��";
					returnval = -7;
					if (outputpcm)fclose(pcm);
					return;
				}
				//��frame�л�ȡ����
				if (sampletype == 32) {
					if (codecfm->bits_per_raw_sample == 24)sampletype = 24;
				}
				int nb_samples;
				uint8_t **audiodata;
				if (resamp) {
					nb_samples =
						av_rescale_rnd(decoded_frame->nb_samples, resamprate, samplerate, AV_ROUND_UP);
					ret = av_samples_alloc_array_and_samples(&dst_data, &dst_linesize, channels,
						nb_samples, codecfm->sample_fmt, 0);
					if (!setsampnum) {
						samplenum = static_cast<int>(ceil(samplenum / double(decoded_frame->nb_samples)*nb_samples));
						efftnum = static_cast<int>(ceil(samplenum / float(FFTnum)));
						delete fftdata;
						fftdata = new std::vector<std::vector<node*>>(2, std::vector<node*>(efftnum));;
						setsampnum = true;
					}
					if (ret < 0) {
						feedback = "�޷�Ϊ�ز������ݷ����ڴ棡";
						if (outputpcm)fclose(pcm);
						returnval = -9;
						return;
					}
					ret = swr_convert(swr_ctx, dst_data, nb_samples, 
						(const uint8_t **)decoded_frame->extended_data, decoded_frame->nb_samples);
					if (ret < 0) {
						feedback = "�ز�������";
						if (outputpcm)fclose(pcm);
						returnval = -10;
						return;
					}
					audiodata = dst_data;
				}
				else
				{
					nb_samples = decoded_frame->nb_samples;
					audiodata = decoded_frame->extended_data;
				}
				if (!isplanar)data_size *= codecfm->channels;
				for (i = 0; i < nb_samples; i++) {
					for (ch = 0; ch < realch; ch++) {
						for (int j = 0; j < data_size; j++) {
							*(temp + j) = *(audiodata[ch] + data_size * i + j);
						}
						if (outputpcm)fwrite(audiodata[ch] + data_size * i, 1, data_size, pcm);//���pcm����

						if (!isplanar) {//������Ƶ
							shiftf = getshiftf(temp, sampletype, 0);
							sample_seq_l->add(shiftf);
							if (codecfm->channels > 1) {
								shiftf = getshiftf(temp, sampletype, data_size / codecfm->channels);
								sample_seq_r->add(shiftf);
							}
						}
						else {//ƽ����Ƶ
							if (ch == 0) {
								shiftf = getshiftf(temp, sampletype, 0);
								sample_seq_l->add(shiftf);
							}
							if (ch == 1) {
								shiftf = getshiftf(temp, sampletype, 0);
								sample_seq_r->add(shiftf);
							}
						}
					}
					if (samplecount == samplenum - 1) {
						for (int i = sample_seq_l->gethead(); i < FFTnum; i++) {
							sample_seq_l->add(0);
							if (codecfm->channels > 1)sample_seq_r->add(0);
						}
					}
					if (sample_seq_l->gethead() == 0 && samplecount > 0) {
						(*fftdata)[0][fftsampnum] = new node(FFTnum / 2);
						double* in = (double*)fftw_malloc(sizeof(double)*FFTnum);
						for (int i = 0; i < FFTnum; i++) {
							*(in + i) = sample_seq_l->read0(i);
						}
						FFTC^ fftcl = gcnew FFTC((*fftdata)[0][fftsampnum], plan, in, mindb,
							gcnew ProgressCallback(this, &Decode::subprogback));
						Task^ taskl = gcnew Task(gcnew Action(fftcl, &FFTC::FFT));
						taskl->Start();
						tasks->Add(taskl);
						//int a = fftdata[0, fftsampnum]->sum()/FFTnum;//������
						fftsampnum++;
					}
					if (sample_seq_r->gethead() == 0 && samplecount > 0 && codecfm->channels > 1) {
						(*fftdata)[1][fftsampnum - 1] = new node(FFTnum / 2);
						double* in = (double*)fftw_malloc(sizeof(double)*FFTnum);
						for (int i = 0; i < FFTnum; i++) {
							*(in + i) = sample_seq_r->read0(i);
						}
						FFTC^ fftcr = gcnew FFTC((*fftdata)[1][fftsampnum - 1], plan, in, mindb,
							gcnew ProgressCallback(this, &Decode::subprogback));
						Task^ taskr = gcnew Task(gcnew Action(fftcr, &FFTC::FFT));
						taskr->Start();
						tasks->Add(taskr);
					}
					samplecount++;
				}
				if (dst_data)
					av_freep(&dst_data[0]);
				av_freep(&dst_data);
			}
			av_frame_free(&decoded_frame);
		}
		av_free_packet(packet);
	}
	//���ݴ���pcmdata
	int count = codecfm->frame_number;
	int bit_depth_raw = codecfm->bits_per_raw_sample;
	out_bitdepth = av_get_bytes_per_sample(codecfm->sample_fmt) * 8;
	String^ samprateinfo = "";
	if (resamp)samprateinfo = samplerate.ToString() + "Hz -> " + resamprate.ToString() + "Hz";
	else samprateinfo = samplerate.ToString() + "Hz";
	feedback += "\r\n����λ����" + bit_depth_raw.ToString() + "bit -> " + (out_bitdepth).ToString() +
		"bit    �����ʣ�" + samprateinfo +"    ������" + channels.ToString() +
		"\r\n��֡����" + count.ToString() + "    ��ʽ��" + chfmt + "    ������ʽ��"
		+ marshal_as<String^>(av_get_sample_fmt_name(codecfm->sample_fmt))->Replace("AV_SAMPLE_FMT_", "")->ToUpper();
	//����wavͷ��Ϣ
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
		feedback += "\r\n���������Ƶ��" + marshal_as<String^>(filename);
	}
	//�ͷ��ڴ�
	delete[] temp;
	temp = nullptr;
	delete sample_seq_l;
	sample_seq_l = nullptr;
	delete sample_seq_r;
	sample_seq_r = nullptr;
	av_frame_free(&decoded_frame);
	av_free_packet(packet);
	avcodec_close(codecfm);
	avformat_close_input(&filefm);
	return;
}

String ^ BDMatch::Decode::getfeedback()
{
	return feedback;
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

std::vector<std::vector<node*>>* BDMatch::Decode::getfftdata()
{
	return fftdata;
}

double BDMatch::Decode::getshiftf(uint8_t * temp, int sampletype, int start)
{
	long long shift64 = 0;
	double shiftd = 0;
	switch (sampletype) {
	case 1:
		shiftd = static_cast<double>(*(float *)(temp + start));
		break;
	case 2:
		shiftd = *(double *)(temp + start);
		break;
	case 8:
		shiftd = *(temp + start) / static_cast<double>(255);
		break;
	default:
		if (sampletype == 16)shift64 = static_cast<long long>(*(INT16 *)(temp + start));
		else if (sampletype == 24 || sampletype == 32)shift64 = static_cast<long long>(*(int *)(temp + start));
		else shift64 = *(long long *)(temp + start);
		long long ss;
		if (sampletype == 24) ss = (1 << 31) - 1;
		else ss = (1 << (sampletype - 1)) - 1;
		shiftd = shift64 / static_cast<double>(ss);
		break;
	}
	return shiftd;
}

void BDMatch::Decode::subprogback()
{
	static int count = 0;
	static int maxnum = efftnum / progmaxnum * channels;
	if (++count > maxnum) {
		count = 0;
		progback();
	}
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

void BDMatch::FFTC::FFT()
{
	using namespace Node;
	using namespace std;

	fftw_complex* out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*FFTnum);
	fftw_execute_dft_r2c(p, in, out);
	for (int i = 0; i < FFTnum / 2; i++) {
		double real = *(out + i)[0];
		double imag = *(out + i)[1];
		*(in + i) = real * real + imag * imag;
	}
	FD8(in, fftseq);
	fftw_free(in);
	fftw_free(out);
	p = nullptr;
	in = nullptr;
	out = nullptr;
	progback();
	return;
}

int BDMatch::FFTC::FD8(double *inseq, Node::node* outseq)
{
	using namespace Node;
	for (int i = 0; i < outseq->size(); i++) {
		double addx = *(inseq + i);
		addx = 10 * log10(addx);
		addx -= 15;
		addx *= 2 * 128 / (15 - mindb);
		addx += 128;
		addx = min(addx, 127);
		addx = max(addx, -128);
		outseq->add(static_cast<char>(addx));
	}
	return 0;
}


