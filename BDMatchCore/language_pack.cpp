#include "headers/language_pack.h"

Language_Pack::Language_Pack(const Language& language)
{
	switch (language) {
	case Language::zh_CN:
		break;
	default:
		general_texts = { u8"\n", u8"  ", u8"->", u8" Sec", u8", ", u8"    " };
		notif_texts = { u8"\n    Info: {}", u8"\n    Warning: {}", u8"\n    Error: {}", u8"\n    Error: {} Err Msg: {}" };
		match_core_texts = { u8"\nTV File: ", u8"\nBD File: ", u8"   Loudness: {} dB" , u8"\nDecoding Time: {:.5f}{}", u8"\nSub File: " ,
			u8"\nMatching time: {:.5f}{}" };
		decoder_error_texts = { u8"Failed to open file!", u8"Failed to read file stream information!", u8"Didn't find a audio stream!", u8"Codec not found!",
			u8"Failed to allocate the codec context!", u8"Failed to copy codec parameters to decoder context!", u8"Failed to open codec!", u8"Failed to open PCM file for writing!",
			u8"Failed to allocate resampling context!", u8"Failed to initialize resampling context!", u8"Could not allocate frame!", u8"Error submitting a packet for decoding!", u8"Error during decoding!",
			u8"Failed to calculate audio data size!", u8"Could not allocate destination samples!", u8"Error while converting!" };
		decoder_info_texts = { u8"Channel: {}", u8"Total Frame Number: {}", u8"Format: {}",
			u8"Sampling Rate: {} Hz", u8"Sampling Bits: {} bit",  u8"Sampling Format: {}", u8"Delay: {} ms",
			u8"\nOutput decoded audio: {}",
			u8"Track No: {}", u8"Audio Encoding: {}" };
		match_info_texts = { u8"Line {} is a comment and will not be processed.",
			u8"Line {} with zero duration will not be processed.", u8"Fast Matching On.", u8"A slight overlap between lines {} and {} has been automatically corrected." };
		match_warning_texts = { u8"Line {} is too long and will not be processed.", u8"Line {} exceeds the audio length and will not be processed.",
			u8"Line {} is too quiet to be processed.", u8"Possible matching error in line {} (same time as line {})!", u8"Possible matching error in line {}: The order with the previous line is inconsistent!",
			u8"Possible matching error in line {}: The order with the following line is inconsistent!", u8"Possible matching error in line {}: The time differences with the previous and following lines are inconsistent!" };
		match_error_texts = { u8"Failed to read subtitle file!", u8"Original subtitle file is invalid!",
			u8"Failed to open the output subtitle file!", u8"Failed to write the subtitle file!" };
		match_debug_texts = { u8"Average Found Index = {:.6f}%", u8"Max Found Index = {:.6f}%", u8"Max Found Line = {}",
			u8"Max Delta = {}", u8"Diffa Consistency = {:.6f}%" };
		break;
	}
}

const std::string_view Language_Pack::get_text(const Lang_Type& type, const size_t& index) const
{
	const std::u8string_view *re = nullptr;
	switch (type) {
	case Lang_Type::General:
		re = &general_texts[index];
		break;
	case Lang_Type::Notif:
		re = &notif_texts[index];
		break;
	case Lang_Type::Core:
		re = &match_core_texts[index];
		break;
	case Lang_Type::Decoder_Info:
		re = &decoder_info_texts[index];
		break;
	case Lang_Type::Decoder_Error:
		re = &decoder_error_texts[index];
		break;
	case Lang_Type::Match_Sub_Info:
		re = &match_info_texts[index];
		break;
	case Lang_Type::Match_Sub_Warning:
		re = &match_warning_texts[index];
		break;
	case Lang_Type::Match_Sub_Error:
		re = &match_error_texts[index];
		break;
	case Lang_Type::Match_Sub_Debug:
		re = &match_debug_texts[index];
		break;
	default:
		break;
	}
	return re ? std::string_view(reinterpret_cast<const char*>(re->data())) : "";
}
