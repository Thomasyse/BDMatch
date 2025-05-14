#pragma once
#include "datastruct.h"
#include <memory>
#include <ranges>
#include <stop_token>
#include "language_pack.h"
#include <immintrin.h>
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

	template <typename T, int type = 0>
	concept Normalizable = (type == 0 || (std::is_same<T, int>::value && type == 24)) && 
		requires (T v) {
			static_cast<double>(v);
			std::numeric_limits<T>::max();
		}
	;

	enum class Vol_Mode { None, Dec_And_Cal, Cal_Only };
	constexpr int64_t AVX_VALID_MASK_64 = 1ULL << 63;

	struct FFmpeg
	{
	public:
		~FFmpeg();
		AVFormatContext * filefm = nullptr;
		AVCodecContext *codecfm = nullptr;
		const AVCodec *codec = nullptr;
		AVPacket *packet = nullptr;
		uint8_t **dst_data = nullptr;
		AVFrame *decoded_frame = nullptr;
		double **sample_seqs = nullptr;
		struct SwrContext *swr_ctx = nullptr;
		int real_ch = 0;
	};

	inline static double m256d_f64(__m256d& vec, const int& index);

	class Decode
	{
	public:
		Decode(const Language_Pack &lang_pack0, std::stop_source &stop_src0);
		virtual ~Decode();
		int load_settings(const int &fft_num0, const bool &output_pcm0, const int &min_db0, 
			const int &resamp_rate0, const Prog_Mode &prog_type0, fftw_plan plan0, const prog_func &prog_single0 = nullptr);
		Match_Core_Return initialize(const std::string_view &file_name0); // UTF-8 required
		Match_Core_Return decode_audio();
		std::string_view get_feedback();
		std::string_view get_file_name() const;
		Match_Core_Return get_return() const;
		int64_t get_fft_samp_num() const;
		int64_t get_centi_sec() const;
		int get_channels() const;
		int get_samp_rate() const;
		int get_fft_num() const;
		bool get_audio_only() const;
		DataStruct::Spec_Node** get_fft_data();
		char** get_fft_spec();
		double get_avg_vol() const;
		int set_vol_mode(const Vol_Mode &input);
		int set_vol_coef(const double &input);
		int clear_ffmpeg();
	protected:
		char* av_err_to_str(const int &err_code);
		Match_Core_Return error_return(const Match_Core_Return &err_re);
		void sub_prog_back(double val);
		int clear_fft_data();
		int clear_normalized_samples(double** normalized_samples);
		virtual int move_data(double* src, double* dst, const size_t& size);
		template <typename T, int type = 0>
			requires Normalizable<T, type>
		int transfer_audio_data_planar_base(uint8_t** const audio_data, double** const normalized_samples, double** const seqs,
			const int& nb_last, const int& nb_last_next, const int& length, auto&& convert_func) requires requires (T v) { { convert_func(v) }->std::same_as<double>; };
		template <typename T, int type = 0>
			requires Normalizable<T, type>
		int transfer_audio_data_planar(uint8_t** const audio_data, double** const normalized_samples, double** const seqs, 
			const int& nb_last, const int& nb_last_next, const int& length);
		template <typename T, int type = 0>
			requires Normalizable<T, type>
		int transfer_audio_data_packed_base(uint8_t** const audio_data, double** const normalized_samples, double** const seqs,
			const int& nb_last, const int& nb_last_next, const int& length, auto&& convert_func) requires requires (T v) { { convert_func(v) }->std::same_as<double>; };
		template <typename T, int type = 0>
			requires Normalizable<T, type>
		int transfer_audio_data_packed(uint8_t** const audio_data, double** const normalized_samples, double** const seqs,
			const int& nb_last, const int& nb_last_next, const int& length);
		virtual int normalize(uint8_t ** const &audio_data, double ** &normalized_samples, double ** &seqs, 
			int &nb_last, const int &nb_samples);
		int FFT(DataStruct::Spec_Node** nodes, double** in, const int64_t& fft_index, const int& nb_fft);
		virtual int FD8(double* inseq, DataStruct::Spec_Node* outseq);
		std::stop_source& stop_src;//multithreading cancel source
		//language pack
		const Language_Pack& lang_pack;
		//settings
		int fft_num = 512;
		int min_db = -14;
		bool output_pcm = false;
		Vol_Mode vol_mode = Vol_Mode::None;
		int resamp_rate = 0;
		fftw_plan plan = nullptr;
		//fft data
		DataStruct::Spec_Node** fft_data = nullptr;
		char** fft_spec = nullptr;
		DataStruct::Spec_Node* fft_data_mem = nullptr;
		char* fft_spec_mem = nullptr;
		char av_err_buf[AV_ERROR_MAX_STRING_SIZE] = { 0 };
		//audio info
		std::string_view file_name; // UTF-8 required
		int out_bit_depth = 0;
		int audio_stream = 0;
		int64_t centi_sec = 0;
		int64_t fft_samp_num = 0;
		int64_t e_fft_num = 0;
		int channels = 0;//audio channels
		int data_channels = 0;//audio data channels
		int real_ch = 0;//Planar: channels, Linear: 1
		int64_t start_time = 0;
		bool audio_only = false;
		//sample info
		int sample_type = 0;
		int sample_rate = 0;
		//progress bar and return
		prog_func prog_single = nullptr;//func_ptr for progress bar
		std::string feedback;
		Match_Core_Return return_val = Match_Core_Return::Invalid;
		Prog_Mode prog_type = Prog_Mode::Setting;
		std::atomic<int64_t> decoded_num = 0;
		//fix audio volume
		volatile double prog_val = 0.0;
		double total_vol = 0.0;
		double vol_coef = 0.0;
		double c_min_db = 0.0;
		//ffmpeg
		FFmpeg *ffmpeg = nullptr;
	};

	class Decode_SSE :public Decode {
	public:
		Decode_SSE(const Language_Pack& lang_pack0, std::stop_source& stop_src0)
			:Decode(lang_pack0, stop_src0) {}
		virtual int FD8(double* inseq, DataStruct::Spec_Node* outseq);
	};

	class Decode_AVX :public Decode {
	public:
		Decode_AVX(const Language_Pack& lang_pack0, std::stop_source& stop_src0)
			:Decode(lang_pack0, stop_src0) {}
		int move_data(double* src, double* dst, const size_t& size);
		int transfer_audio_data_planar_float(uint8_t** const audio_data, double** const normalized_samples, double** const seqs,
			const int& nb_last, const int& nb_last_next, const int& length, const int& nb_samples);
		template <typename T, size_t vec_stride = 2>
			requires Normalizable<T>
		int transfer_audio_data_packed_base_avx(uint8_t** const audio_data, double** const normalized_samples, double** const seqs,
			const int& nb_last, const int& length, const int& nb_samples, auto&& transfer_avx, auto&& convert_func)
			requires requires (T* v, __m256d res[vec_stride]) {
				transfer_avx(v, res);
				{ convert_func(*v) }->std::same_as<double>;
		};
		int transfer_audio_data_packed_float(uint8_t** const audio_data, double** const normalized_samples, double** const seqs,
			const int& nb_last, const int& length, const int& nb_samples);
		virtual int normalize(uint8_t** const& audio_data, double**& normalized_samples, double**& seqs,
			int& nb_last, const int& nb_samples);
		virtual int FD8(double* inseq, DataStruct::Spec_Node* outseq);
	};

	class Decode_AVX2 :public Decode_AVX {
	public:
		Decode_AVX2(const Language_Pack& lang_pack0, std::stop_source& stop_src0)
			:Decode_AVX(lang_pack0, stop_src0) {}
		int transfer_audio_data_packed_int16(uint8_t** const audio_data, double** const normalized_samples, double** const seqs,
			const int& nb_last, const int& length, const int& nb_samples);
		int transfer_audio_data_packed_int24(uint8_t** const audio_data, double** const normalized_samples, double** const seqs,
			const int& nb_last, const int& length, const int& nb_samples);
		int transfer_audio_data_packed_int32(uint8_t** const audio_data, double** const normalized_samples, double** const seqs,
			const int& nb_last, const int& length, const int& nb_samples);
		virtual int normalize(uint8_t ** const &audio_data, double ** &normalized_samples, double ** &seqs,
			int &nb_last, const int &nb_samples);
		virtual int FD8(double* inseq, DataStruct::Spec_Node* outseq);
	};

	template<typename T, int type>
		requires Normalizable<T, type>
	inline int Decode::transfer_audio_data_planar_base(uint8_t** const audio_data, double** const normalized_samples, double** const seqs, 
		const int& nb_last, const int& nb_last_next, const int& length, auto&& convert_func) requires requires (T v) { { convert_func(v) }->std::same_as<double>; }
	{
		int seq_index = 0;
		if (length > 0) {
			for (int ch = 0; ch < channels; ch++)move_data(seqs[ch], normalized_samples[ch], nb_last);
		}
		else seq_index = nb_last;
		for (int ch = 0; ch < channels; ch++) {
			T* data_ptr = reinterpret_cast<T*>(audio_data[ch]);
			int k = 0;
			for (int i = nb_last; i < length; i++, k++)normalized_samples[ch][i] = convert_func(data_ptr[k]);
			for (int i = seq_index; i < nb_last_next; i++, k++)seqs[ch][i] = convert_func(data_ptr[k]);
		}
		return 0;
	}

	template<typename T, int type>
		requires Normalizable<T, type>
	inline int Decode::Decode::transfer_audio_data_planar(uint8_t** const audio_data, double** const normalized_samples, double** const seqs,
		const int& nb_last, const int& nb_last_next, const int& length)
	{
		transfer_audio_data_planar_base<T>(audio_data, normalized_samples, seqs, nb_last, nb_last_next, length, [](const T& val) { return static_cast<double>(val) / static_cast<double>(std::numeric_limits<T>::max()); });
		return 0;
	}
	template<> inline int Decode::Decode::transfer_audio_data_planar<double>(uint8_t** const audio_data, double** const normalized_samples, double** const seqs,
		const int& nb_last, const int& nb_last_next, const int& length) {
		transfer_audio_data_planar_base<double>(audio_data, normalized_samples, seqs, nb_last, nb_last_next, length, [](const double& val) { return val; });
		return 0;
	}
	template<> inline int Decode::Decode::transfer_audio_data_planar<float>(uint8_t** const audio_data, double** const normalized_samples, double** const seqs,
		const int& nb_last, const int& nb_last_next, const int& length) {
		transfer_audio_data_planar_base<float>(audio_data, normalized_samples, seqs, nb_last, nb_last_next, length, [](const float& val) { return static_cast<double>(val); });
		return 0;
	}
	template<> inline int Decode::Decode::transfer_audio_data_planar<int32_t, 24>(uint8_t** const audio_data, double** const normalized_samples, double** const seqs,
		const int& nb_last, const int& nb_last_next, const int& length) {
		transfer_audio_data_planar_base<int32_t>(audio_data, normalized_samples, seqs, nb_last, nb_last_next, length, [](const int32_t& val) { return static_cast<double>(val >> 8) / 8388607.0; });
		return 0;
	}

	template<typename T, int type>
		requires Normalizable<T, type>
	inline int Decode::transfer_audio_data_packed_base(uint8_t** const audio_data, double** const normalized_samples, double** const seqs, 
		const int& nb_last, const int& nb_last_next, const int& length, auto&& convert_func) requires requires (T v) { { convert_func(v) }->std::same_as<double>; }
	{
		T* data_ptr = reinterpret_cast<T*>(audio_data[0]);
		int index = 0, seq_index = 0;
		if (length > 0) {
			for (int ch = 0; ch < channels; ch++)move_data(seqs[ch], normalized_samples[ch], nb_last);
		}
		else seq_index = nb_last;
		for (int i = nb_last; i < length; i++)
			for (int ch = 0; ch < channels; ch++, index++)
				normalized_samples[ch][i] = convert_func(data_ptr[index]);
		for (int i = seq_index; i < nb_last_next; i++)
			for (int ch = 0; ch < channels; ch++, index++)
				seqs[ch][i] = convert_func(data_ptr[index]);
		return 0;
	}
	template<typename T, int type>
		requires Normalizable<T, type>
	int inline Decode::Decode::transfer_audio_data_packed(uint8_t** const audio_data, double** const normalized_samples, double** const seqs,
		const int& nb_last, const int& nb_last_next, const int& length)
	{
		transfer_audio_data_packed_base<T>(audio_data, normalized_samples, seqs, nb_last, nb_last_next, length, [](const T& val) { return static_cast<double>(val) / static_cast<double>(std::numeric_limits<T>::max()); });
		return 0;
	}
	template<> inline int Decode::Decode::transfer_audio_data_packed<float>(uint8_t** const audio_data, double** const normalized_samples, double** const seqs,
		const int& nb_last, const int& nb_last_next, const int& length) {
		transfer_audio_data_packed_base<float>(audio_data, normalized_samples, seqs, nb_last, nb_last_next, length, [](const float& val) { return static_cast<double>(val); });
		return 0;
	}
	template<> inline int Decode::Decode::transfer_audio_data_packed<double>(uint8_t** const audio_data, double** const normalized_samples, double** const seqs,
		const int& nb_last, const int& nb_last_next, const int& length) {
		transfer_audio_data_packed_base<double>(audio_data, normalized_samples, seqs, nb_last, nb_last_next, length, [](const double& val) { return val; });
		return 0;
	}
	template<> inline int Decode::Decode::transfer_audio_data_packed<int32_t, 24>(uint8_t** const audio_data, double** const normalized_samples, double** const seqs,
		const int& nb_last, const int& nb_last_next, const int& length) {
		transfer_audio_data_packed_base<int32_t>(audio_data, normalized_samples, seqs, nb_last, nb_last_next, length, [](const int32_t& val) { return static_cast<double>(val >> 8) / 8388607.0; });
		return 0;
	}

	template<typename T, size_t vec_stride>
		requires Normalizable<T>
	inline int Decode_AVX::transfer_audio_data_packed_base_avx(uint8_t** const audio_data, double** const normalized_samples, double** const seqs, 
		const int& nb_last, const int& length, const int& nb_samples, auto&& transfer_avx, auto&& convert_func)
		requires requires (T* v, __m256d res[vec_stride]) {
			transfer_avx(v, res);
			{ convert_func(*v) }->std::same_as<double>;
	}
	{
		using std::ranges::iota_view;
		constexpr int VEC_SIZE = sizeof(__m256d) / sizeof(double);
		constexpr int DATA_STRIDE = VEC_SIZE * vec_stride;
		int index = 0, seq_index = 0;
		if (length > 0) {
			for (int ch = 0; ch < channels; ch++)move_data(seqs[ch], normalized_samples[ch], nb_last);
			index = nb_last;
		}
		else seq_index = nb_last;
		int ch = 0;
		__m256d avx_store_buffer[MAX_CHANNEL_CNT];
		int store_buffer_size = 0;
		auto store_packed = [&ch, &channels = this->channels](double** dst, const double& val, int& index) {
			dst[ch++][index] = val;
			if (ch == channels) {
				ch = 0;
				index++;
			}
		};
		auto drain_avx_store_buffer = [&store_buffer_size, &avx_store_buffer, &ch, &store_packed, &channels = this->channels](double** dst, int& index) {
			if (store_buffer_size == 0)return;
			int avx_cnt = store_buffer_size * VEC_SIZE / channels;
			int remainder = store_buffer_size * VEC_SIZE;
			double* buffer_data_ptr = reinterpret_cast<double*>(avx_store_buffer);
			if (avx_cnt > 1) {
				remainder -= avx_cnt * channels;
				__m256i mask = _mm256_set_epi64x(0, avx_cnt >= 3 ? AVX_VALID_MASK_64 : 0, AVX_VALID_MASK_64, AVX_VALID_MASK_64);
				int ch_store = ch;
				for (int ch_cnt = 0; ch_cnt < channels; ch_cnt++) {
					__m128i vindex = _mm_set_epi32(channels * 3 + ch_cnt, channels * 2 + ch_cnt, channels + ch_cnt, ch_cnt);
					__m256d ch_data = _mm256_mask_i32gather_pd(_mm256_setzero_pd(), buffer_data_ptr, vindex, _mm256_castsi256_pd(mask), 8);
					_mm256_maskstore_pd(dst[ch_store++] + index, mask, ch_data);
					if (ch_store == channels)ch_store = 0;
				}
				index += avx_cnt;
				buffer_data_ptr += avx_cnt * channels;
			}
			for (int i = 0; i < remainder; i++)store_packed(dst, *(buffer_data_ptr++), index);
			store_buffer_size = 0;
		};
		auto store_packed_avx = [&store_buffer_size, &avx_store_buffer, &ch, &channels = this->channels](double** dst, __m256d& data, int& index) {
			avx_store_buffer[store_buffer_size++] = data;
			if (store_buffer_size == channels) {
				store_buffer_size = 0;
				int ch_store = ch;
				for (int ch_cnt = 0; ch_cnt < channels; ch_cnt++) {
					__m128i vindex = _mm_set_epi32(channels * 3 + ch_cnt, channels * 2 + ch_cnt, channels + ch_cnt, ch_cnt);
					__m256d ch_data = _mm256_i32gather_pd(reinterpret_cast<double*>(avx_store_buffer), vindex, 8);
					_mm256_storeu_pd(dst[ch_store++] + index, ch_data);
					if (ch_store == channels)ch_store = 0;
				}
				index += VEC_SIZE;
			}
		};
		T* data_ptr = reinterpret_cast<T*>(audio_data[0]);
		int avx_length = nb_samples * channels / DATA_STRIDE;
		int avx_last = (nb_samples * channels) % DATA_STRIDE;
		int nb_to_norm = std::max(length - nb_last, 0);
		int threshold = nb_to_norm * channels / DATA_STRIDE;
		int remainder = nb_to_norm * channels % DATA_STRIDE;
		__m256d tr_res[vec_stride] = { _mm256_setzero_pd(), _mm256_setzero_pd() };
		for (int i = 0; i < threshold; i++) {
			transfer_avx(data_ptr, tr_res);
			for (size_t j : iota_view{ 0ULL, vec_stride })store_packed_avx(normalized_samples, tr_res[j], index);
			data_ptr += DATA_STRIDE;
		}
		drain_avx_store_buffer(normalized_samples, index);
		if (threshold < avx_length) {
			if (remainder) {
				transfer_avx(data_ptr, tr_res);
				for (size_t j : iota_view{ 0ULL, vec_stride * VEC_SIZE }) {
					double out = m256d_f64(tr_res[j / VEC_SIZE], j % VEC_SIZE);
					if (index < length)store_packed(normalized_samples, out, index);
					else store_packed(seqs, out, seq_index);
				}
				data_ptr += DATA_STRIDE;
				threshold++;
			}
			for (int i = threshold; i < avx_length; i++) {
				transfer_avx(data_ptr, tr_res);
				for (size_t j : iota_view{ 0ULL, vec_stride })store_packed_avx(seqs, tr_res[j], seq_index);
				data_ptr += DATA_STRIDE;
			}
			drain_avx_store_buffer(seqs, seq_index);
		}
		for (int i = 0; i < avx_last; i++) {
			double out = convert_func(*(data_ptr++));
			if (index < length)store_packed(normalized_samples, out, index);
			else store_packed(seqs, out, seq_index);
		}
		return 0;
	}

}