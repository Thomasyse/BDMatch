#include "headers/language_pack.h"

std::string_view Language_Pack::get_text(const Lang_Type& type, const size_t& index)
{
	std::u8string_view re;
	switch (type) {
	case Lang_Type::General:
		re = general_texts[index];
		break;
	case Lang_Type::Notif:
		re = notif_texts[index];
		break;
	case Lang_Type::Core:
		re = match_core_texts[index];
		break;
	case Lang_Type::Decoder_Info:
		re = decoder_info_texts[index];
		break;
	case Lang_Type::Decoder_Error:
		re = decoder_error_texts[index];
		break;
	case Lang_Type::Match_Sub_Info:
		re = match_info_texts[index];
		break;
	case Lang_Type::Match_Sub_Warning:
		re = match_warning_texts[index];
		break;
	case Lang_Type::Match_Sub_Error:
		re = match_error_texts[index];
		break;
	case Lang_Type::Match_Sub_Debug:
		re = match_debug_texts[index];
		break;
	default:
		break;
	}
	return std::string_view(reinterpret_cast<const char*>(re.data()));
}
