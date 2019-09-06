#include "headers/language_pack.h"

std::string language_pack::get_text(const Lang_Type& type, const int& index)
{
	std::u16string str = u"";
	switch (type) {
	case General:
		str = general_texts[index];
		break;
	case Core:
		str = match_core_texts[index];
		break;
	case Decoder:
		str = decoder_texts[index];
		break;
	case Match_ASS:
		str = match_texts[index];
		break;
	default:
		break;
	}
	return u16_to_s(str);
}

std::string language_pack::u16_to_s(std::u16string& str)
{
	const char* str_ptr = reinterpret_cast<const char*>(str.c_str());
	size_t len = str.length();
	std::string re = "";
	re.append(str_ptr, len * 2);
	return re;
}

