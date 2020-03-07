#pragma once
#include<string>
#include<vector>

enum class Lang_Type { General, Core, Decoder, Match_Sub };

class language_pack {
public:
	std::string get_text(const Lang_Type &type, const int &index);
	std::string u16_to_s(std::u16string& str);
	template<typename T>
	std::string to_u16string(T&& input);//only support ASCII characters!!
	template<>
	std::string to_u16string<const char*>(const char*&& input);//only support ASCII characters!!
	template<>
	std::string to_u16string<std::string>(std::string&& input);//only support ASCII characters!!
private:
	std::vector<std::u16string> general_texts = { u"\r\n", u"  ", u"->", u"��", u", "};
	std::vector<std::u16string> match_core_texts = { u"TV�ļ���", u"BD�ļ���", u"   ��ȣ�", u"dB" , u"����ʱ�䣺", u"\r\n��Ļ�ļ���" };
	std::vector<std::u16string> decoder_texts = { u"\r\n�޷����ļ���", u"\r\n�޷���ȡ�ļ�����Ϣ��", u"\r\n����Ƶ����", u"\r\n�޷��ҵ���Ƶ�Ķ�Ӧ��������",
		u"\r\n�޷�������Ƶ�Ľ�������Ϣ��", u"\r\n�޷���ȡ��Ƶ�Ľ�������Ϣ��", u"\r\n�޷�����Ƶ�Ķ�Ӧ��������", u"\r\n�޷���Ҫд���PCM�ļ���",
		u"\r\n�޷������ز���������", u"\r\n�޷���ʼ���ز���������", u"\r\n�޷�Ϊ��Ƶ֡�����ڴ棡", u"\r\n�޷��ύ��Ƶ����������", u"\r\n��Ƶ�������",
		u"\r\n�޷�������Ƶ���ݴ�С��", u"\r\n�޷�Ϊ�ز������ݷ����ڴ棡", u"\r\n�ز�������", u"Hz -> ", u"Hz", u"    ������", u"    ��֡����",
		u"    ��ʽ��", u"\r\n����λ����", u"bit -> ", u"bit    �����ʣ�", u"    ������ʽ��", u"    �ӳ٣�",u"ms", u"\r\n���������Ƶ��", u"�����ţ�",
		u"    ��Ƶ���룺" };
	std::vector<std::u16string> match_texts = { u"\r\n��ȡ��Ļ�ļ�ʧ��!", u"\r\n������Ļ�ļ���Ч��", u"\r\n��Ϣ����", u"��Ϊע�ͣ�����������",
		u"��ʱ��Ϊ�㣬����������", u"\r\n���棺��", u"��ʱ������������������", u"�г�����Ƶ���ȣ�����������", u"��������С������������",
		u"\r\n��Ϣ��ʹ�ÿ���ƥ�䡣", u"\r\nAverage Found Index = ", u"%    ", u"Max Found Index= ", u"%\r\nMax Found Line= ", u"    Max Delta= ",
		u"�У����", u"��ʱ����ͬ�����ܴ���ƥ�����!", u"�п��ܴ���ƥ�������ǰһ�д���һ�£�", u"�п��ܴ���ƥ��������һ�д���һ�£�",
		u"�п��ܴ���ƥ�������ǰ����ʱ�һ�£�", u"�к͵�", u"�з���΢С�ص������Զ�������", u"\r\n�������Ļ�ļ�ʧ��!", u"\r\nд����Ļ�ļ�ʧ��!",
		u"\r\nƥ��ʱ�䣺", u"\r\nDiffa Consistency = " };
};

template<typename T>
inline std::string language_pack::to_u16string(T&& input)//only support ASCII characters!!
{
	std::string str = std::to_string(input);
	std::string re = "";
	for (auto& i : str) {
		re += i;
		re += '\0';
	}
	return re;
}
template<>
inline std::string language_pack::to_u16string<const char*>(const char*&& input)//only support ASCII characters!!
{
	const char* ptr = input;
	std::string re = "";
	while (*ptr) {
		re += *ptr;
		re += '\0';
		ptr++;
	}
	return re;
}
template<>
inline std::string language_pack::to_u16string<std::string>(std::string&& input)//only support ASCII characters!!
{
	std::string re = "";
	for (auto& i : input) {
		re += i;
		re += '\0';
	}
	return re;
}
