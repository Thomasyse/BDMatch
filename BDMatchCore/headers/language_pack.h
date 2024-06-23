#pragma once
#include<string>
#include<string_view>
#include<vector>
#include<format>
#include "targetver.h"

enum class Lang_Type { General, Notif, Core, Decoder_Info, Decoder_Error, Match_Sub_Info, Match_Sub_Warning, Match_Sub_Error, Match_Sub_Debug };
enum class Language { en_US, zh_CN };

class Language_Pack {
public:
	Language_Pack(const Language &language = Language::en_US);
	const std::string_view get_text(const Lang_Type& type, const size_t& index) const;
private:
	std::vector<std::u8string_view> general_texts = { u8"\n", u8"  ", u8"->", u8"秒", u8", ", u8"    " };
	std::vector<std::u8string_view> notif_texts = { u8"\n    信息：{}", u8"\n    警告：{}", u8"\n    错误：{}", u8"\n    错误：{} 错误信息：{}" };
	std::vector<std::u8string_view> match_core_texts = { u8"\nTV文件：", u8"\nBD文件：", u8"   响度：{}dB" , u8"\n解码时间：{:.5f}{}", u8"\n字幕文件：" ,
		u8"\n匹配时间：{:.5f}{}" };
	std::vector<std::u8string_view> decoder_error_texts = { u8"无法打开文件！", u8"无法读取文件流信息！", u8"无音频流！", u8"无法找到音频的对应解码器！",
		u8"无法构建音频的解码器环境！", u8"无法复制音频的解码器参数！", u8"无法打开音频的对应解码器！", u8"无法打开要写入的PCM文件！",
		u8"无法构建重采样环境！", u8"无法初始化重采样环境！", u8"无法为音频帧分配内存！", u8"无法提交音频至解码器！", u8"音频解码出错！",
		u8"无法计算音频数据大小！", u8"无法为重采样数据分配内存！", u8"重采样错误！" };
	std::vector<std::u8string_view> decoder_info_texts = { u8"声道：{}", u8"总帧数：{}", u8"格式：{}",
		u8"采样率：{}Hz", u8"采样位数：{}bit",  u8"采样格式：{}", u8"延迟：{}ms", 
		u8"\n输出解码音频：{}", 
		u8"音轨编号：{}", u8"音频编码：{}" };
	std::vector<std::u8string_view> match_info_texts = { u8"第{}行为注释，将不作处理。",
		u8"第{}行时长为零，将不作处理。", u8"使用快速匹配。", u8"第{}行和第{}行发生微小重叠，已自动修正。" };
	std::vector<std::u8string_view> match_warning_texts = { u8"第{}行时长过长，将不作处理。", u8"第{}行超过音频长度，将不作处理。", 
		u8"第{}行声音过小，将不作处理。", u8"第{}行（与第{}行时间相同）可能存在匹配错误!", u8"第{}行可能存在匹配错误：与前一行次序不一致！", 
		u8"第{}行可能存在匹配错误：与后一行次序不一致！", u8"第{}行可能存在匹配错误：与前后行时差不一致！" };
	std::vector<std::u8string_view> match_error_texts = { u8"读取字幕文件失败!", u8"输入字幕文件无效！", 
		u8"打开输出字幕文件失败!", u8"写入字幕文件失败!" };
	std::vector<std::u8string_view> match_debug_texts = { u8"Average Found Index = {:.6f}%", u8"Max Found Index = {:.6f}%", u8"Max Found Line = {}", 
		u8"Max Delta = {}", u8"Diffa Consistency = {:.6f}%" };
};

template<typename... Args>
std::string str_vfmt(const std::string_view fmt_str, Args&&... args)
{
	return std::vformat(fmt_str, std::make_format_args(args...));
}