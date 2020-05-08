#pragma once
#include<string>
#include<vector>

#include "targetver.h"

enum class Lang_Type { General, Core, Decoder, Match_Sub };

class language_pack {
public:
	std::string get_text(const Lang_Type& type, const int& index);
private:
	std::vector<std::string> general_texts = { u8"\r\n", u8"  ", u8"->", u8"秒", u8", " };
	std::vector<std::string> match_core_texts = { u8"TV文件：", u8"BD文件：", u8"   响度：", u8"dB" , u8"\r\n解码时间：", u8"\r\n字幕文件：" ,
		u8"\r\n匹配时间：" };
	std::vector<std::string> decoder_texts = { u8"\r\n无法打开文件！", u8"\r\n无法读取文件流信息！", u8"\r\n无音频流！", u8"\r\n无法找到音频的对应解码器！",
		u8"\r\n无法创建音频的解码器信息！", u8"\r\n无法获取音频的解码器信息！", u8"\r\n无法打开音频的对应解码器！", u8"\r\n无法打开要写入的PCM文件！",
		u8"\r\n无法构建重采样环境！", u8"\r\n无法初始化重采样环境！", u8"\r\n无法为音频帧分配内存！", u8"\r\n无法提交音频至解码器！", u8"\r\n音频解码出错！",
		u8"\r\n无法计算音频数据大小！", u8"\r\n无法为重采样数据分配内存！", u8"\r\n重采样错误！", u8"Hz -> ", u8"Hz", u8"    声道：", u8"    总帧数：",
		u8"    格式：", u8"\r\n采样位数：", u8"bit -> ", u8"bit    采样率：", u8"    采样格式：", u8"    延迟：",u8"ms", u8"\r\n输出解码音频：", u8"音轨编号：",
		u8"    音频编码：" };
	std::vector<std::string> match_texts = { u8"\r\n读取字幕文件失败!", u8"\r\n输入字幕文件无效！", u8"\r\n信息：第", u8"行为注释，将不作处理。",
		u8"行时长为零，将不作处理。", u8"\r\n警告：第", u8"行时长过长，将不作处理。", u8"行超过音频长度，将不作处理。", u8"行声音过小，将不作处理。",
		u8"\r\n信息：使用快速匹配。", u8"\r\nAverage Found Index = ", u8"%    ", u8"Max Found Index= ", u8"%\r\nMax Found Line= ", u8"    Max Delta= ",
		u8"行（与第", u8"行时间相同）可能存在匹配错误!", u8"行可能存在匹配错误：与前一行次序不一致！", u8"行可能存在匹配错误：与后一行次序不一致！",
		u8"行可能存在匹配错误：与前后行时差不一致！", u8"行和第", u8"行发生微小重叠，已自动修正。", u8"\r\n打开输出字幕文件失败!", u8"\r\n写入字幕文件失败!", 
		u8"\r\nDiffa Consistency = " };
};
