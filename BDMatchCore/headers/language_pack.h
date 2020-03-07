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
	std::vector<std::u16string> general_texts = { u"\r\n", u"  ", u"->", u"秒", u", "};
	std::vector<std::u16string> match_core_texts = { u"TV文件：", u"BD文件：", u"   响度：", u"dB" , u"解码时间：", u"\r\n字幕文件：" };
	std::vector<std::u16string> decoder_texts = { u"\r\n无法打开文件！", u"\r\n无法读取文件流信息！", u"\r\n无音频流！", u"\r\n无法找到音频的对应解码器！",
		u"\r\n无法创建音频的解码器信息！", u"\r\n无法获取音频的解码器信息！", u"\r\n无法打开音频的对应解码器！", u"\r\n无法打开要写入的PCM文件！",
		u"\r\n无法构建重采样环境！", u"\r\n无法初始化重采样环境！", u"\r\n无法为音频帧分配内存！", u"\r\n无法提交音频至解码器！", u"\r\n音频解码出错！",
		u"\r\n无法计算音频数据大小！", u"\r\n无法为重采样数据分配内存！", u"\r\n重采样错误！", u"Hz -> ", u"Hz", u"    声道：", u"    总帧数：",
		u"    格式：", u"\r\n采样位数：", u"bit -> ", u"bit    采样率：", u"    采样格式：", u"    延迟：",u"ms", u"\r\n输出解码音频：", u"音轨编号：",
		u"    音频编码：" };
	std::vector<std::u16string> match_texts = { u"\r\n读取字幕文件失败!", u"\r\n输入字幕文件无效！", u"\r\n信息：第", u"行为注释，将不作处理。",
		u"行时长为零，将不作处理。", u"\r\n警告：第", u"行时长过长，将不作处理。", u"行超过音频长度，将不作处理。", u"行声音过小，将不作处理。",
		u"\r\n信息：使用快速匹配。", u"\r\nAverage Found Index = ", u"%    ", u"Max Found Index= ", u"%\r\nMax Found Line= ", u"    Max Delta= ",
		u"行（与第", u"行时间相同）可能存在匹配错误!", u"行可能存在匹配错误：与前一行次序不一致！", u"行可能存在匹配错误：与后一行次序不一致！",
		u"行可能存在匹配错误：与前后行时差不一致！", u"行和第", u"行发生微小重叠，已自动修正。", u"\r\n打开输出字幕文件失败!", u"\r\n写入字幕文件失败!",
		u"\r\n匹配时间：", u"\r\nDiffa Consistency = " };
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
