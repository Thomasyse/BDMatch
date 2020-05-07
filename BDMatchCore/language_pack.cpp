#include "headers/language_pack.h"

std::string language_pack::get_text(const Lang_Type& type, const int& index)
{
	std::string str = u8"";
	switch (type) {
	case Lang_Type::General:
		str = general_texts[index];
		break;
	case Lang_Type::Core:
		str = match_core_texts[index];
		break;
	case Lang_Type::Decoder:
		str = decoder_texts[index];
		break;
	case Lang_Type::Match_Sub:
		str = match_texts[index];
		break;
	default:
		break;
	}
	return str;
}

