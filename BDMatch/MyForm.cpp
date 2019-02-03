#include "MyForm.h"
#pragma unmanaged
#include "instructionset.h"
#pragma managed
#include <msclr\marshal_cppstd.h >

#define appversion "1.5.2"
#define secpurple 45
#define setintnum 5
#define MaxdB 20.0

#define cut_off "\r\n----------------------------------------------------------------------------------------------"

const InstructionSet::InstructionSet_Internal InstructionSet::CPU_Rep;
using namespace BDMatch;
[STAThreadAttribute]
int main(array<System::String^>^args)
{
	Application::EnableVisualStyles();
	MyForm^ myform = gcnew MyForm();
	Application::Run(myform);
	return 0;
}

int BDMatch::MyForm::match(String^ asstext, String^ tvtext, String^ bdtext)
{
	using namespace System::IO;
	using namespace System::Text;
	using namespace System::Runtime::InteropServices;
	using namespace msclr::interop;

	//进度条
	ProgressCallback^ prog = gcnew ProgressCallback(this, &MyForm::progsingle);
	GCHandle gch1 = GCHandle::Alloc(prog);
	IntPtr prog_ip = Marshal::GetFunctionPointerForDelegate(prog);
	prog_func prog_ptr = static_cast<prog_func>(prog_ip.ToPointer());
	//日志
	FeedbackCallback^ feed_back = gcnew FeedbackCallback(this, &MyForm::feedback);
	GCHandle gch2 = GCHandle::Alloc(feed_back);
	IntPtr feedback_ip = Marshal::GetFunctionPointerForDelegate(feed_back);
	feedback_func feedback_ptr = static_cast<feedback_func>(feedback_ip.ToPointer());
	GC::Collect();

	drawpre();
		
	progsingle(0, 0);
	
	int re = 0;
	match_core->load_interface(prog_ptr, feedback_ptr);
	//settings
	int isa_mode = ISAMode;
	int fft_num = Setting->fft_num;
	int min_db = Setting->min_find_db;
	bool output_pcm = Setting->output_pcm;
	bool parallel_decode = Setting->parallel_decode;
	bool vol_match = Setting->vol_match;
	int min_check_num = Setting->min_check_num;
	int find_field = Setting->find_field;
	int ass_offset = Setting->ass_offset;
	int max_length = Setting->max_length;
	bool match_ass = Setting->match_ass;
	bool fast_match = Setting->fast_match;
	bool debug_mode0 = debug_mode;
	match_core->load_settings(isa_mode,fft_num,min_db,output_pcm, parallel_decode, vol_match,
		min_check_num, find_field, ass_offset, max_length,
		match_ass, fast_match, debug_mode0);
	//decode
	marshal_context convert;
	const char *tv_path = convert.marshal_as<const char*>(tvtext);
	const char *bd_path = convert.marshal_as<const char*>(bdtext);
	const char *ass_path = convert.marshal_as<const char*>(asstext);
	const char *output_path0 = convert.marshal_as<const char*>(output_path);
	re = match_core->decode(tv_path, bd_path);
	if (re < 0)return re;
	if (Setting->match_ass) {
		re = writeass(ass_path, output_path0);
	}
	else {
		progsingle(3, 0);
	}
	drawpre(re);
	if (re < 0) {
		if (re == -2)return -6;
		else return -5;
	}
	gch1.Free();
	gch2.Free();
	progsingle(0, 0);
	return 0;
}

int BDMatch::MyForm::writeass(const char* ass_path, const char* output_path)
{
	//
	int re = 0;
	re = match_core->match_1(ass_path);
	if (re < 0) return re;
	int nb_timeline = match_core->get_nb_timeline();
	//绘图相关
	if (Setting->draw) {
		tvdraw.timelist = gcnew array<int, 2>(nb_timeline, 2);
		bddraw.timelist = gcnew array<int, 2>(nb_timeline, 2);
		tvdraw.linenum = nb_timeline;
		bddraw.linenum = nb_timeline;
		LineSel->Maximum = nb_timeline;
		for (int i = 0; i < nb_timeline; i++) {
			tvdraw.timelist[i, 0] = match_core->get_timeline(i, Matching::Timeline_Start_Time);
			tvdraw.timelist[i, 1] = match_core->get_timeline(i, Matching::Timeline_End_Time);
		}
	}
	//
	re = match_core->match_2(output_path);
	if (re < 0) {
		return re;
	}
	//绘图相关
	if (Setting->draw) {
		for (int i = 0; i < nb_timeline; i++) {
			bddraw.timelist[i, 0] = match_core->get_timeline(i, Matching::Timeline_Start_Time);
			bddraw.timelist[i, 1] = match_core->get_timeline(i, Matching::Timeline_End_Time);
		}
	}
	match_core->clear_match();
	progsingle(3, 1);
	return 0;
}

int BDMatch::MyForm::drawpre()
{
	match_core->clear_data();
	tvdraw.spec = nullptr;
	bddraw.spec = nullptr;
	ViewSel->Enabled = false;
	LineSel->Enabled = false;
	ChSelect->Enabled = false;
	TimeRoll->Enabled = false;
	ViewSel->SelectedIndex = 0;
	ChSelect->SelectedIndex = 0;
	LineSel->Value = 1;
	tvdraw.ch = 0;
	bddraw.ch = 0;
	tvdraw.milisec = 0;
	bddraw.milisec = 0;
	tvdraw.linenum = 0;
	bddraw.linenum = 0;
	tvdraw.ttf = 1.0;
	bddraw.ttf = 1.0;
	tvdraw.fftnum = 0;
	bddraw.fftnum = 0;
	tvdraw.timelist = nullptr;
	bddraw.timelist = nullptr;
	ChartTime->Text = "";
	if (!Setting->draw)setrows();
	return 0;
}
int BDMatch::MyForm::drawpre(const int &re)
{
	if (Setting->draw && !re) {
		tvdraw.spec = match_core->get_decode_spec(TV_Decode);
		bddraw.spec = match_core->get_decode_spec(BD_Decode);
		tvdraw.num = match_core->get_decode_info(TV_Decode, FFT_Samp_Num);
		bddraw.num = match_core->get_decode_info(BD_Decode, FFT_Samp_Num);
		tvdraw.ch = match_core->get_decode_info(TV_Decode, Channels);
		bddraw.ch = match_core->get_decode_info(BD_Decode, Channels);
		tvdraw.milisec = match_core->get_decode_info(TV_Decode, Milisec);
		bddraw.milisec = match_core->get_decode_info(BD_Decode, Milisec);
		tvdraw.fftnum = match_core->get_decode_info(TV_Decode, FFT_Num);
		bddraw.fftnum = match_core->get_decode_info(BD_Decode, FFT_Num);
		tvdraw.ttf = match_core->get_decode_info(TV_Decode, Samp_Rate) / (static_cast<double>(tvdraw.fftnum) * 100.0);
		bddraw.ttf = match_core->get_decode_info(TV_Decode, Samp_Rate) / (static_cast<double>(bddraw.fftnum) * 100.0);
		ViewSel->SelectedIndex = 0;
		ChSelect->SelectedIndex = 0;
		ChSelect->Enabled = true;
		TimeRoll->Maximum = max(tvdraw.milisec, bddraw.milisec);
		TimeRoll->Value = 0;
		TimeRoll->Enabled = true;
		if (Setting->match_ass)ViewSel->Enabled = true;
		setrows();
		drawchart();
	}
	else
	{
		if (!re) {
			match_core->clear_data();
		}
		tvdraw.spec = nullptr;
		bddraw.spec = nullptr;
		tvdraw.num = 0;
		bddraw.num = 0;
	}
	return 0;
}
int BDMatch::MyForm::drawchart()
{
	Match->Enabled = false;
	int maxsampnum = max(tvdraw.num, bddraw.num);
	int milisec = max(tvdraw.milisec, bddraw.milisec);
	int offset = 0;
	if (milisec < 10000) offset = 150;
	else if (milisec < 100000)offset = 250;
	else offset = 350;
	TimeRoll->TickFrequency = offset;
	TimeRoll->LargeChange = offset;
	TimeRoll->SmallChange = offset / 4;
	int tvstart = 0, tvend = 0, bdstart = 0;
	if (ViewSel->SelectedIndex == 0) {
		tvstart = static_cast<int>(round((TimeRoll->Value - offset)* tvdraw.ttf));
		tvend = static_cast<int>(round((TimeRoll->Value + offset)* tvdraw.ttf));
		bdstart = static_cast<int>(round((TimeRoll->Value - offset)* bddraw.ttf));
		ChartTime->Text = mstotime(TimeRoll->Value);
	}
	else {
		if (tvdraw.timelist[static_cast<int>(LineSel->Value) - 1, 1] - tvdraw.timelist[static_cast<int>(LineSel->Value) - 1, 0]
	> 2 * offset) {
			tvstart = tvdraw.timelist[static_cast<int>(LineSel->Value) - 1, 0];
			tvend = tvdraw.timelist[static_cast<int>(LineSel->Value) - 1, 1];
			bdstart = bddraw.timelist[static_cast<int>(LineSel->Value) - 1, 0];
		}
		else {
			tvstart = (tvdraw.timelist[static_cast<int>(LineSel->Value) - 1, 0]
				+ tvdraw.timelist[static_cast<int>(LineSel->Value) - 1, 1]) / 2 - offset;
			tvend = tvstart + 2 * offset;
			bdstart = (bddraw.timelist[static_cast<int>(LineSel->Value) - 1, 0]
				+ bddraw.timelist[static_cast<int>(LineSel->Value) - 1, 1]) / 2 - offset;
		}
		if (bddraw.timelist[static_cast<int>(LineSel->Value) - 1, 0] == -1) ChartTime->Text = "未匹配！";
		else ChartTime->Text = mstotime(static_cast<int>(round(tvdraw.timelist[static_cast<int>(LineSel->Value) - 1, 0] / tvdraw.ttf)) + 1)
			+ "\n" + mstotime(static_cast<int>(round(bddraw.timelist[static_cast<int>(LineSel->Value) - 1, 0] / bddraw.ttf)));
	}
	int duration = tvend - tvstart + 1;
	
	Bitmap^ spectrum1 = gcnew Bitmap(duration, tvdraw.fftnum);
	int x;
	int y;
	// Loop through the images pixels to reset color.
	for (x = 0; x < spectrum1->Width; x++)
	{
		int tvinline = 0, bdinline = 0;
		int tvedge = 0, bdedge = 0;
		for (int i = 0; i < tvdraw.linenum; i++) {
			if (tvdraw.timelist[i, 0] >= 0) {
				if (tvdraw.timelist[i, 0] <= x + tvstart && tvdraw.timelist[i, 1] >= x + tvstart) {
					if (i == static_cast<int>(LineSel->Value) - 1 || ViewSel->SelectedIndex == 0) tvinline = 1;
					else if (!tvinline) tvinline = 2;
					if (tvdraw.timelist[i, 0] == x + tvstart || tvdraw.timelist[i, 1] == x + tvstart) {
						if (i == static_cast<int>(LineSel->Value) - 1 || ViewSel->SelectedIndex == 0) {
							if (tvdraw.timelist[i, 0] == x + tvstart)tvedge = 1;
							else if (tvedge != 1)tvedge = 2;
						}
						else if (tvedge != 1 && tvedge != 2)tvedge = 3;
					}
				}
				if (bddraw.timelist[i, 0] <= x + bdstart && bddraw.timelist[i, 1] >= x + bdstart) {
					if (i == static_cast<int>(LineSel->Value) - 1 || ViewSel->SelectedIndex == 0) bdinline = 1;
					else if (!bdinline) bdinline = 2;
					if (bddraw.timelist[i, 0] == x + bdstart || bddraw.timelist[i, 1] == x + bdstart) {
						if (i == static_cast<int>(LineSel->Value) - 1 || ViewSel->SelectedIndex == 0) {
							if (bddraw.timelist[i, 0] == x + bdstart)bdedge = 1;
							else if (bdedge != 1)bdedge = 2;
						}
						else if (bdedge != 1 && bdedge != 2)bdedge = 3;
					}
				}
			}
		}
		for (y = 0; y < spectrum1->Height; y++)
		{
			int color = -128;
			if (y >= bddraw.fftnum / 2) {
				if (0 <= x + bdstart && x + bdstart < bddraw.num)
					color = bddraw.spec[ChSelect->SelectedIndex][(x + bdstart)*bddraw.fftnum / 2 + bddraw.fftnum - y - 1];
			}
			else if (0 <= x + tvstart && x + tvstart < tvdraw.num)
				color = tvdraw.spec[ChSelect->SelectedIndex][(x + tvstart)*tvdraw.fftnum / 2 + tvdraw.fftnum / 2 - y - 1];
			color += 128;
			Color newColor = Color::FromArgb(color / 4, color, color);
			if (y >= Setting->fft_num / 2) {
				if (bdinline)
					switch (bdedge) {
					case 1:
						newColor = Color::FromArgb(0, 255, 0);
						break;
					case 2:
						newColor = Color::FromArgb(255, 255, 0);
						break;
					case 3:
						newColor = Color::FromArgb(180, 180, 180);
						break;
					default:
						switch (bdinline) {
						case 1:
							newColor = Color::FromArgb(color / 4 + 70, color, max(color, 70));
							break;
						case 2:
							newColor = Color::FromArgb(color / 4 + secpurple, color, max(color, secpurple));
							break;
						default:
							newColor = Color::FromArgb(color / 4 + secpurple, color, max(color, secpurple));
							break;
						}
						break;
					}
			}
			else {
				if (tvinline)
					switch (tvedge) {
					case 1:
						newColor = Color::FromArgb(0, 255, 0);
						break;
					case 2:
						newColor = Color::FromArgb(255, 255, 0);
						break;
					case 3:
						newColor = Color::FromArgb(180, 180, 180);
						break;
					default:
						switch (tvinline) {
						case 1:
							newColor = Color::FromArgb(color / 4 + 70, color, max(color, 70));
							break;
						case 2:
							newColor = Color::FromArgb(color / 4 + secpurple, color, max(color, secpurple));
							break;
						default:
							newColor = Color::FromArgb(color / 4 + secpurple, color, max(color, secpurple));
							break;
						}
						break;
					}
			}
			spectrum1->SetPixel(x, y, newColor);
		}
	}
	// Set the PictureBox to display the image.
	Spectrum->Image = spectrum1;
	Match->Enabled = true;
	return 0;
}
String ^ BDMatch::MyForm::mstotime(int ms)
{
	int hh, mm, ss;
	hh = ms / 360000;
	ms -= hh * 360000;
	mm = ms / 6000;
	ms -= mm * 6000;
	ss = ms / 100;
	ms -= ss * 100;
	String^ timeout = "";
	timeout += hh.ToString() + ":";
	if (mm < 10)timeout += "0";
	timeout += mm.ToString() + ":";
	if (ss < 10)timeout += "0";
	timeout += ss.ToString() + ".";
	if (ms < 10)timeout += "0";
	timeout += ms.ToString();
	return timeout;
}

int BDMatch::MyForm::setrows()
{
	if (Setting->draw) {
		AllTablePanel->RowStyles[7]->Height = static_cast<float>(0.4);
		AllTablePanel->RowStyles[8]->Height = static_cast<float>(0.6);
	}
	else
	{
		AllTablePanel->RowStyles[7]->Height = 1;
		AllTablePanel->RowStyles[8]->Height = 0;
	}
	return 0;
}

int BDMatch::MyForm::adddropdown(ComboBox ^ combo, String ^ text)
{
	bool repeat = false;
	for (int i = 0; i < combo->Items->Count; i++) {
		if (combo->GetItemText(combo->Items[i]) == text) {
			repeat = true;
			break;
		}
	}
	if (!repeat) {
		if (combo->Items->Count == combo->MaxDropDownItems) {
			combo->Items->RemoveAt(combo->Items->Count - 1);
		}
		combo->Items->Insert(0, text);
	}
	return 0;
}

int BDMatch::MyForm::loadsettings(String^ path, SettingVals^ settingvals)
{
	using namespace System::IO;
	using namespace System::Text::RegularExpressions;
	setrows();
	if (File::Exists(path)) {
		String^ settext;
		try {
			settext = File::ReadAllText(path);
		}
		finally{
			Regex^ numkey = gcnew Regex("[\\+-]?[0-9]+");
			for (int i = 0; i < 12; i++) {
				SettingType type = static_cast<SettingType>(i);
				Regex^ setkey = gcnew Regex(settingvals->getname(type) + ":[\\+-]?[0-9]+\\r\\n");
				String^ setting = setkey->Match(settext)->Value;
				if (setting != "") {
					int val = int::Parse(numkey->Match(setting)->Value);
					settingvals->setval(type, val);
				}
			}
		}
	}
	return 0;
}
int BDMatch::MyForm::savesettings(String^ path, SettingVals^ settingvals)
{
	using namespace System::IO;
	using namespace System::Text;
	using namespace System::Text::RegularExpressions;
	FileStream^ fs = File::OpenWrite(path);
	try {
		for (int i = 0; i < 12; i++) {
			SettingType type = static_cast<SettingType>(i);
			String^ outstr = settingvals->getname(type) + ":" + settingvals->getval(type).ToString() + "\r\n";
			array<Byte>^info = (gcnew UTF8Encoding(true))->
				GetBytes(outstr);
			fs->Write(info, 0, info->Length);
		}
	}
	finally{
		if (fs)
			delete (IDisposable^)fs;
	}
	return 0;
}

int BDMatch::MyForm::matchinput()
{
	using namespace System::IO;
	using namespace System::Text::RegularExpressions;
	using namespace System::Collections::Concurrent;
	using namespace System::Collections::Generic;

	Result->Text = "";
	searchISA();
	if (Setting->ass_offset != 0)Result->Text += "\r\nASS时间偏置：延后 " + Setting->ass_offset.ToString() + " 厘秒" + cut_off;
	/*
	ASStext->Text = "\"G:\\Movie\\[SFEO-Raws] Haruchika Haruta & Chika (BD 1080P x264 FLAC)\\\
[SFEO-Raws] Haruchika Haruta & Chika - 03 (BD 1080P x264 FLAC).ass\"\
\"G:\\Movie\\[SFEO-Raws] Haruchika Haruta & Chika (BD 1080P x264 FLAC)\\\
[SFEO-Raws] Haruchika Haruta & Chika - 01 (BD 1080P x264 FLAC).ass\"\
\"G:\\Movie\\[SFEO-Raws] Haruchika Haruta & Chika (BD 1080P x264 FLAC)\\\
[SFEO-Raws] Haruchika Haruta & Chika - 02 (BD 1080P x264 FLAC).ass\"";
	BDtext->Text = "\"G:\\Movie\\[SFEO-Raws] Haruchika Haruta & Chika (BD 1080P x264 FLAC)\\\
[SFEO-Raws] Haruchika Haruta & Chika - 03 (BD 1080P x264 FLAC).mkv\"\
\"G:\\Movie\\[SFEO-Raws] Haruchika Haruta & Chika (BD 1080P x264 FLAC)\\\
[SFEO-Raws] Haruchika Haruta & Chika - 01 (BD 1080P x264 FLAC).mkv\"\
\"G:\\Movie\\[SFEO-Raws] Haruchika Haruta & Chika (BD 1080P x264 FLAC)\\\
[SFEO-Raws] Haruchika Haruta & Chika - 02 (BD 1080P x264 FLAC).mkv\"";
	TVtext->Text = "\"G:\\Movie\\[FLsnow&SumiSora][Haruchika][MKV][1080p]\\[FLsnow&SumiSora][Haruchika][03][x265_aac].mkv\"\
\"G:\\Movie\\[FLsnow&SumiSora][Haruchika][MKV][1080p]\\[FLsnow&SumiSora][Haruchika][01][x265_aac].mkv\"\
\"G:\\Movie\\[FLsnow&SumiSora][Haruchika][MKV][1080p]\\[FLsnow&SumiSora][Haruchika][02][x265_aac].mkv\"";
	*/
	String ^ asstext_all = ASStext->Text;
	String ^ tvtext_all = TVtext->Text;
	String ^ bdtext_all = BDtext->Text;
	match_num = fin_match_num = 0;
	matches_num = fin_matches_num = 0;
	if (asstext_all == "" || tvtext_all == "" || bdtext_all == "") {
		matchcontrol(true);
		MessageBox::Show(this, "文件名为空！", "BDMatch", MessageBoxButtons::OK);
		return -1;
	}
	int re = 0;
	if (!asstext_all->Contains("\""))asstext_all = "\"" + asstext_all + "\"";
	if (!tvtext_all->Contains("\""))tvtext_all = "\"" + tvtext_all + "\"";
	if (!bdtext_all->Contains("\""))bdtext_all = "\"" + bdtext_all + "\"";
	String ^ asstext = "";
	String ^ tvtext = "";
	String ^ bdtext = "";
	long start = clock();//开始计时
	//分解多个文件名
	Regex^ quotationkey = gcnew Regex("\".*?\"");
	MatchCollection^ assmatch_all = quotationkey->Matches(asstext_all);
	MatchCollection^ tvmatch_all = quotationkey->Matches(tvtext_all);
	MatchCollection^ bdmatch_all = quotationkey->Matches(bdtext_all);
	matches_num = min(min(assmatch_all->Count, tvmatch_all->Count), bdmatch_all->Count);
	if (assmatch_all->Count == 1 && tvmatch_all->Count == 1 && (bdmatch_all->Count > 1 || bdtext_all->Contains("*"))) {
		Result->Text += "\r\n批量处理将不作声谱图。";
		Setting->draw = false;
		asstext = assmatch_all[0]->Value->Replace("\"", "");
		tvtext = tvmatch_all[0]->Value->Replace("\"", "");
		if (!File::Exists(asstext)) {
			Result->Text += "\r\nASS文件：  " + asstext + "\r\nASS文件不存在！";
			re = -1;
		}
		if (!File::Exists(tvtext)) {
			Result->Text += "\r\nASS文件：  " + asstext + "\r\nTV文件：  " + tvtext +
				"\r\nTV文件不存在！";
			re = -1;
		}
		matches_num = bdmatch_all->Count;
		if (re >= 0) {
			for (int index = 0; index < static_cast<int>(matches_num); index++) {
				bdtext = bdmatch_all[index]->Value->Replace("\"", "");
				fin_match_num = 0;
				if (!bdtext->Contains("*")) {
					if (tvtext == bdtext) {
						Result->Text += "\r\nASS输入：  " + asstext + "\r\n" + "TV和BD文件相同！";
						if (match_num > 0)Result->Text += cut_off;
						re = -1;
						continue;
					}
					Result->Text += "\r\nASS文件：  " + asstext->Substring(asstext->LastIndexOf("\\") + 1) + "\r\n";
					match_num = 1;
					output_path = asstext->Substring(0, asstext->LastIndexOf(".")) + " - " +
						bdtext->Substring(bdtext->LastIndexOf("\\") + 1, bdtext->LastIndexOf(".") - bdtext->LastIndexOf("\\") - 1)
						+ ".matched.ass";
					re = match(asstext, tvtext, bdtext);
					if (re < 0) {
						if (re == -6)	Result->Text += "\r\n\r\n用户中止操作。";
						matchcontrol(true);
						return re;
					}
					fin_match_num = 1;
					if (matches_num > 0)Result->Text += cut_off;
					fin_matches_num++;
				}
				else {
					Regex^ starkey = gcnew Regex("(\\*.*?$)|(\\*.*?\\*)|(^.*?\\*)");
					MatchCollection^ bdmatch = starkey->Matches(bdtext->Replace("*", "**"));
					String^ path = "";
					array<String^>^ files;
					List<String^>^ bdfiles = gcnew List<String^>();
					path = bdtext->Substring(0, bdtext->LastIndexOf("\\"));
					if (Directory::Exists(path))files = Directory::GetFiles(path);
					else {
						Result->Text += "\r\nBD路径：  " + path + " 不存在！\r\n";
						if (matches_num > 0)Result->Text += cut_off;
						re = -2;
						continue;
					}
					Regex^ bdfilekey = gcnew Regex(returnregt(bdtext));
					for (int i = 0; i < files->Length; i++)
						if (bdfilekey->IsMatch(files[i])) bdfiles->Add(files[i]);
					match_num = bdfiles->Count;
					for (int i = 0; i < bdfiles->Count; i++) {
						re = -10;
						int re1 = 0;
						Result->Text += "\r\nASS文件：  " + asstext->Substring(asstext->LastIndexOf("\\") + 1) + "\r\n";
						String ^star_content = bdfiles[i];
						for (int j = 0; j < bdmatch->Count; j++)
							star_content = star_content->Replace(bdmatch[j]->Value->Replace("*", ""), " ");
						if (star_content->StartsWith(" "))star_content = star_content->Substring(1);
						if (star_content->EndsWith(" "))star_content = star_content->Substring(0, star_content->Length - 1);
						output_path = asstext->Substring(0, asstext->LastIndexOf(".")) + " - " +
							star_content + ".matched.ass";
						re1 = match(asstext, tvtext, bdfiles[i]);
						if (re < 0)re = re1;
						if (re1 == -6) {
							Result->Text += "\r\n\r\n用户中止操作。";
							matchcontrol(true);
							return -6;
						}
						Result->Text += cut_off;
						fin_match_num = i + 1;
					}
					fin_matches_num++;
				}
			}
		}
	}
	else {
		output_path = "";
		if (matches_num > 1) {
			Result->Text += "\r\n批量处理将不作声谱图。";
			Setting->draw = false;
		}
		for (int index = 0; index < static_cast<int>(matches_num); index++) {
			asstext = assmatch_all[index]->Value->Replace("\"", "");
			tvtext = tvmatch_all[index]->Value->Replace("\"", "");
			bdtext = bdmatch_all[index]->Value->Replace("\"", "");
			fin_match_num = 0;
			if (tvtext == bdtext) {
				Result->Text += "\r\nASS输入：  " + asstext + "\r\n" + "TV和BD文件相同！";
				if (matches_num > 0)Result->Text += cut_off;
				re = -1;
				continue;
			}
			if (!asstext->Contains("*")) {
				if (!tvtext->Contains("*") && !bdtext->Contains("*")) {
					if (!File::Exists(asstext)) {
						Result->Text += "\r\nASS文件：  " + asstext + "\r\nASS文件不存在！";
						if (matches_num > 0)Result->Text += cut_off;
						re = -1;
						continue;
					}
					if (!File::Exists(tvtext)) {
						Result->Text += "\r\nASS文件：  " + asstext + "\r\nTV文件：  " + tvtext +
							"\r\nTV文件不存在！";
						if (matches_num > 0)Result->Text += cut_off;
						re = -1;
						continue;
					}
					if (!File::Exists(bdtext)) {
						Result->Text += "\r\nASS文件：  " + asstext + "\r\nTV文件：  " + tvtext + "\r\nBD文件：  " + bdtext +
							"\r\nBD文件不存在！";
						if (matches_num > 0)Result->Text += cut_off;
						re = -1;
						continue;
					}
					match_num = 1;
					Result->Text += "\r\nASS文件：  " + asstext->Substring(asstext->LastIndexOf("\\") + 1) + "\r\n";
					re = match(asstext, tvtext, bdtext);
					if (re < 0) {
						if (re == -6)	Result->Text += "\r\n\r\n用户中止操作。";
						matchcontrol(true);
						return re;
					}
					fin_match_num = 1;
					if (matches_num > 0)Result->Text += cut_off;
				}
				else {
					Result->Text += "\r\nASS输入：  " + asstext + "\r\nTV输入：  " + tvtext + "\r\nBD输入：  " + bdtext +
						"\r\n输入格式错误！\r\n";
					if (matches_num > 0)Result->Text += cut_off;
					re = -1;
					continue;
				}
			}
			else {
				//按*分解
				Regex^ starkey = gcnew Regex("(\\*.*?$)|(\\*.*?\\*)|(^.*?\\*)");
				MatchCollection^ assmatch = starkey->Matches(asstext->Replace("*", "**"));
				MatchCollection^ tvmatch = starkey->Matches(tvtext->Replace("*", "**"));
				MatchCollection^ bdmatch = starkey->Matches(bdtext->Replace("*", "**"));
				if (assmatch->Count == tvmatch->Count&& assmatch->Count == bdmatch->Count) {
					//检查合乎命名规则的文件
					String^ path = "";
					array<String^>^ files;
					List<String^>^ assfiles = gcnew List<String^>();
					List<String^>^ tvfiles = gcnew List<String^>();
					List<String^>^ bdfiles = gcnew List<String^>();
					path = asstext->Substring(0, asstext->LastIndexOf("\\"));
					if (Directory::Exists(path))files = Directory::GetFiles(path);
					else {
						Result->Text += "\r\nASS路径：  " + path + " 不存在！\r\n";
						if (matches_num > 0)Result->Text += cut_off;
						re = -2;
						continue;
					}
					Regex^ assfilekey = gcnew Regex(returnregt(asstext));
					for (int i = 0; i < files->Length; i++)
						if (assfilekey->IsMatch(files[i])) assfiles->Add(files[i]);
					path = tvtext->Substring(0, tvtext->LastIndexOf("\\"));
					if (Directory::Exists(path))files = Directory::GetFiles(path);
					else {
						Result->Text += "\r\nTV路径：  " + path + " 不存在！\r\n";
						if (matches_num > 0)Result->Text += cut_off;
						re = -2;
						continue;
					}
					Regex^ tvfilekey = gcnew Regex(returnregt(tvtext));
					for (int i = 0; i < files->Length; i++)
						if (tvfilekey->IsMatch(files[i])) tvfiles->Add(files[i]);
					path = bdtext->Substring(0, bdtext->LastIndexOf("\\"));
					if (Directory::Exists(path))files = Directory::GetFiles(path);
					else {
						Result->Text += "\r\nBD路径：  " + path + " 不存在！\r\n";
						if (matches_num > 0)Result->Text += cut_off;
						re = -2;
						continue;
					}
					Regex^ bdfilekey = gcnew Regex(returnregt(bdtext));
					for (int i = 0; i < files->Length; i++)
						if (bdfilekey->IsMatch(files[i])) bdfiles->Add(files[i]);
					if (Setting->draw == true && matches_num == 1) {
						Result->Text += "\r\n批量处理将不作声谱图。";
						Setting->draw = false;
					}
					match_num = assfiles->Count;
					for (int i = 0; i < assfiles->Count; i++) {
						String^ asskey = assfiles[i];
						for (int j = 0; j < assmatch->Count; j++) {
							asskey = asskey->Replace(assmatch[j]->Value->Replace("*", ""), "|");
						}
						int tvfileindex = -1, bdfileindex = -1;
						for (int j = 0; j < tvfiles->Count; j++) {
							String^ tvkey = tvfiles[j];
							for (int k = 0; k < tvmatch->Count; k++) {
								tvkey = tvkey->Replace(tvmatch[k]->Value->Replace("*", ""), "|");
							}
							if (tvkey == asskey) {
								tvfileindex = j;
								break;
							}
						}
						for (int j = 0; j < bdfiles->Count; j++) {
							String^ bdkey = bdfiles[j];
							for (int k = 0; k < bdmatch->Count; k++) {
								bdkey = bdkey->Replace(bdmatch[k]->Value->Replace("*", ""), "|");
							}
							if (bdkey == asskey) {
								bdfileindex = j;
								break;
							}
						}
						Result->Text += "\r\nASS文件：  " + assfiles[i]->Substring(assfiles[i]->LastIndexOf("\\") + 1) + "\r\n";
						re = -10;
						int re1 = 0;
						if (tvfileindex >= 0 && bdfileindex >= 0) {
							re1 = match(assfiles[i], tvfiles[tvfileindex], bdfiles[bdfileindex]);
							if (re < 0)re = re1;
							if (re1 == -6) {
								Result->Text += "\r\n\r\n用户中止操作。";
								matchcontrol(true);
								return -6;
							}
							Result->Text += cut_off;
						}
						else {
							Result->Text += "\r\n未找到对应的TV或BD文件！";
							Result->Text += cut_off;
						}
						fin_match_num = i + 1;
					}
				}
				else {
					Result->Text += "\r\nASS输入：  " + asstext + "\r\nTV输入：  " + tvtext + "\r\nBD输入：  " + bdtext +
						"\r\n输入格式错误！\r\n";
					if (matches_num > 0)Result->Text += cut_off;
					re = -1;
					continue;
				}
			}
			fin_matches_num++;
		}
	}
	Setting->draw = drawstore;
	//结束计时
	if (fin_matches_num > 0) {
		long end = clock();
		double spend = double(end - start) / (double)CLOCKS_PER_SEC;
		Result->Text += "\r\n总时间：" + spend.ToString() + "秒";
	}
	if (re >= 0) {
		adddropdown(ASStext, ASStext->Text);
		adddropdown(TVtext, TVtext->Text);
		adddropdown(BDtext, BDtext->Text);
		taskbar->ProgressState(TBPFLAG::TBPF_NOPROGRESS);
	}
	else taskbar->ProgressState(TBPFLAG::TBPF_ERROR);
	matchcontrol(true);
	return 0;
}
int BDMatch::MyForm::searchISA()
{
	using namespace msclr::interop;
	Result->Text += marshal_as<String^>(InstructionSet::Brand());
	ISAMode = 0;
	if (InstructionSet::AVX2() && InstructionSet::AVX()) {
		ISAMode = 3;
		Result->Text += "：使用AVX、AVX2指令集加速。";
	}
	else if (InstructionSet::AVX()) {
		ISAMode = 2;
		Result->Text += "：使用SSE2、SSSE3、SSE4.1、AVX指令集加速。";
	}
	else if (InstructionSet::SSE41() && InstructionSet::SSE2() && InstructionSet::SSSE3()) {
		ISAMode = 1;
		Result->Text += "：使用SSE2、SSSE3、SSE4.1指令集加速。";
	}
	else {
		Result->Text += "：不使用增强指令集加速。";
	}
	Result->Text += cut_off;
	return 0;
}
String ^ BDMatch::MyForm::returnregt(String ^ search)
{
	String^ searchregs = search->Replace("\\", "\\\\");
	searchregs = searchregs->Replace("[", "\\[");
	searchregs = searchregs->Replace("]", "\\]");
	searchregs = searchregs->Replace("(", "\\(");
	searchregs = searchregs->Replace(")", "\\)");
	searchregs = searchregs->Replace("{", "\\{");
	searchregs = searchregs->Replace("}", "\\}");
	searchregs = searchregs->Replace("+", "\\+");
	searchregs = searchregs->Replace(".", "\\.");
	searchregs = searchregs->Replace("^", "\\^");
	searchregs = searchregs->Replace("$", "\\$");
	searchregs = searchregs->Replace("*", "(.+?)");
	return searchregs;
}
int BDMatch::MyForm::matchcontrol(bool val)
{
	ASStext->Enabled = val; TVtext->Enabled = val; BDtext->Enabled = val;
	ASSfind->Enabled = val; TVfind->Enabled = val; BDfind->Enabled = val;
	settings->Enabled = val;
	if(val)Match->Text = "匹配";
	else Match->Text = "停止";
	return 0;
}

void BDMatch::MyForm::nullsetform() {
	setform = nullptr;
}
void BDMatch::MyForm::SetVals(SettingType type,int val)
{
	Setting->setval(type, val);
	return System::Void();
}
void BDMatch::MyForm::progsingle(int type, double val)
{
	static double progval[3] = { 0,0,0 };
	static double findfieldpartion = Setting->match_ass ? Setting->find_field * 0.06
		* log10(min(Setting->find_field * 100, Setting->min_check_num)) * 2.0 : 0.0;
	switch (type) {
	case 1:
		progval[0] = val;
		break;
	case 2:
		progval[1] = val;
		break;
	case 3:
		if (val > 0.0 && val < 1.0 && val < progval[2] + 0.02)return System::Void();
		progval[2] = val;
		progval[0] = 1;
		progval[1] = 1;
		break;
	default:
		progval[0] = val;
		progval[1] = val;
		progval[2] = val;
		findfieldpartion = Setting->match_ass ? Setting->find_field * 0.06
			* log10(min(Setting->find_field * 100, Setting->min_check_num)) * 2.0 : 0.0;
		return System::Void();
		break;
	}
	SingleProgress->Value = static_cast<int>(round(SingleProgress->Maximum * ((progval[0] + progval[1]) * 20 + progval[2] * findfieldpartion)
		/ (40 + findfieldpartion)));
	progtotal();
	return System::Void();
}
void BDMatch::MyForm::progtotal()
{
	double val = TotalProgress->Maximum * (fin_matches_num + (fin_match_num
		+ SingleProgress->Value / static_cast<double>(SingleProgress->Maximum)) / static_cast<double>(match_num))
		/ static_cast<double>(matches_num);
	TotalProgress->Value = static_cast<int>(val);
	taskbar->ProgressValue(static_cast<unsigned long long>(val), 
		static_cast<unsigned long long>(TotalProgress->Maximum));
	return System::Void();
}
void BDMatch::MyForm::feedback(const char * input)
{
	using namespace msclr::interop;
	Result->Text += marshal_as<String^>(input);
}


System::Void BDMatch::MyForm::Match_Click(System::Object ^ sender, System::EventArgs ^ e)
{
	using namespace System::Threading::Tasks;
	if (Match->Text == "匹配") {
		if (ASStext->Text == "debug mode") {
			debug_mode = !debug_mode;
			Result->Text = debug_mode ? "调试模式打开。" : "调试模式关闭";
			return System::Void();
		}
		matchcontrol(false);
		drawstore = Setting->draw;
		CancelSource = gcnew System::Threading::CancellationTokenSource();
		keep_processing->test_and_set();
		Task<int>^matchtask = gcnew Task<int>(gcnew Func<int>(this, &MyForm::matchinput), TaskCreationOptions::LongRunning);
		taskbar->ProgressState(TBPFLAG::TBPF_NORMAL);
		matchtask->Start();
	}
	else {
		CancelSource->Cancel();
		keep_processing->clear();
		Setting->draw = drawstore;
		taskbar->ProgressState(TBPFLAG::TBPF_ERROR);
		matchcontrol(true);
	}
	return System::Void();
}

System::Void BDMatch::MyForm::MyForm_Load(System::Object ^ sender, System::EventArgs ^ e)
{
	SingleProgress->CheckForIllegalCrossThreadCalls = false;
	TotalProgress->CheckForIllegalCrossThreadCalls = false;
	About->Text = "v" + appversion;
	loadsettings("settings.ini", Setting);
	TextEditorPanel->Visible = false;
	TextEditorPanel->Dock = System::Windows::Forms::DockStyle::Fill;
	taskbar = new TaskBar(this->Handle.ToPointer());
	return System::Void();
}

System::Void BDMatch::MyForm::MyForm_FormClosing(System::Object ^ sender, System::Windows::Forms::FormClosingEventArgs ^ e)
{
	savesettings("settings.ini", Setting);
	delete taskbar;
	delete keep_processing;
	return System::Void();
}

System::Void BDMatch::MyForm::TVfind_Click(System::Object ^ sender, System::EventArgs ^ e)
{
	using namespace System::IO;
	Stream^ myStream;
	Filebrowse->Title = "获取TV文件";
	Filebrowse->FileName = "";
	Filebrowse->Filter = "Video Files|*.mp4;*.mov;*.mkv;*.avi;*.wmv;*.mpeg;*.mpg;*.qt;*.rmvb;*.flv|All Files|*.*";
	Filebrowse->FilterIndex = 0;
	Filebrowse->RestoreDirectory = true;
	if (Filebrowse->ShowDialog() == System::Windows::Forms::DialogResult::OK)
	{
		if ((myStream = Filebrowse->OpenFile()) != nullptr)
		{
			using namespace System::Text::RegularExpressions;
			if (Filebrowse->FileNames->Length > 1) {
				TVtext->Text = "";
				for (int i = 0; i < Filebrowse->FileNames->Length; i++) {
					TVtext->Text += "\"" + Filebrowse->FileNames[i] + "\"";
				}
			}
			else {
				TVtext->Text = Filebrowse->FileName;
			}
			myStream->Close();
		}
	}
	return System::Void();
}
System::Void BDMatch::MyForm::TVtext_DragEnter(System::Object ^ sender, System::Windows::Forms::DragEventArgs ^ e)
{
	if (e->Data->GetDataPresent(DataFormats::FileDrop) || e->Data->GetDataPresent(DataFormats::StringFormat))
	{
		e->Effect = DragDropEffects::Copy;
	}
	else
	{
		e->Effect = DragDropEffects::None;
	}
	return System::Void();
}
System::Void BDMatch::MyForm::TVtext_DragDrop(System::Object ^ sender, System::Windows::Forms::DragEventArgs ^ e)
{
	using namespace System::Text::RegularExpressions;
	using namespace System::IO;
	if (e->Data->GetDataPresent(DataFormats::FileDrop)) {
		array<String^>^files = (array<String^>^)e->Data->GetData(DataFormats::FileDrop);
		if (files->Length > 1) {
			TVtext->Text = "";
			for (int i = 0; i < files->Length; i++) {
				TVtext->Text += "\"" + files[i] + "\"";
			}
		}
		else {
			TVtext->Text = files[0];
		}
	}
	else
		if (e->Data->GetDataPresent(DataFormats::StringFormat)) {
			String^str = (String^)e->Data->GetData(DataFormats::StringFormat);
			TVtext->Text = str;
		}
	return System::Void();
}

System::Void BDMatch::MyForm::BDfind_Click(System::Object ^ sender, System::EventArgs ^ e)
{
	using namespace System::IO;
	Stream^ myStream;
	Filebrowse->Title = "获取BD文件";
	Filebrowse->FileName = "";
	Filebrowse->Filter = "Video Files|*.mp4;*.mov;*.mkv;*.avi;*.wmv;*.mpeg;*.mpg;*.qt;*.rmvb;*.flv|All Files|*.*";
	Filebrowse->FilterIndex = 0;
	Filebrowse->RestoreDirectory = true;
	if (Filebrowse->ShowDialog() == System::Windows::Forms::DialogResult::OK)
	{
		if ((myStream = Filebrowse->OpenFile()) != nullptr)
		{
			using namespace System::Text::RegularExpressions;
			if (Filebrowse->FileNames->Length > 1) {
				BDtext->Text = "";
				for (int i = 0; i < Filebrowse->FileNames->Length; i++) {
					BDtext->Text += "\"" + Filebrowse->FileNames[i] + "\"";
				}
			}
			else {
				BDtext->Text = Filebrowse->FileName;
			}
			myStream->Close();
		}
	}
	return System::Void();
}
System::Void BDMatch::MyForm::BDtext_DragEnter(System::Object ^ sender, System::Windows::Forms::DragEventArgs ^ e)
{
	if (e->Data->GetDataPresent(DataFormats::FileDrop) || e->Data->GetDataPresent(DataFormats::StringFormat))
	{
		e->Effect = DragDropEffects::Copy;
	}
	else
	{
		e->Effect = DragDropEffects::None;
	}
	return System::Void();
}
System::Void BDMatch::MyForm::BDtext_DragDrop(System::Object ^ sender, System::Windows::Forms::DragEventArgs ^ e)
{
	using namespace System::Text::RegularExpressions;
	using namespace System::IO;
	if (e->Data->GetDataPresent(DataFormats::FileDrop)) {
		array<String^>^files = (array<String^>^)e->Data->GetData(DataFormats::FileDrop);
		if (files->Length > 1) {
			BDtext->Text = "";
			for (int i = 0; i < files->Length; i++) {
				BDtext->Text += "\"" + files[i] + "\"";
			}
		}
		else {
			BDtext->Text = files[0];
		}
	}
	else
		if (e->Data->GetDataPresent(DataFormats::StringFormat)) {
			String^str = (String^)e->Data->GetData(DataFormats::StringFormat);
			BDtext->Text = str;
		}
	return System::Void();
}

System::Void BDMatch::MyForm::ASSfind_Click(System::Object ^ sender, System::EventArgs ^ e)
{
	using namespace System::IO;
	Stream^ myStream;
	Filebrowse->Title = "获取ASS文件";
	Filebrowse->FileName = "";
	Filebrowse->Filter = "ASS Files|*.ass|All Files|*.*";
	Filebrowse->FilterIndex = 0;
	Filebrowse->RestoreDirectory = true;
	if (Filebrowse->ShowDialog() == System::Windows::Forms::DialogResult::OK)
	{
		if ((myStream = Filebrowse->OpenFile()) != nullptr)
		{
			using namespace System::Text::RegularExpressions;
			if (Filebrowse->FileNames->Length > 1) {
				ASStext->Text = "";
				for (int i = 0; i < Filebrowse->FileNames->Length; i++) {
					ASStext->Text += "\"" + Filebrowse->FileNames[i] + "\"";
				}
			}
			else {
				ASStext->Text = Filebrowse->FileName;
			}
			myStream->Close();
		}
	}
	return System::Void();
}
System::Void BDMatch::MyForm::ASStext_DragEnter(System::Object ^ sender, System::Windows::Forms::DragEventArgs ^ e)
{
	if (e->Data->GetDataPresent(DataFormats::FileDrop) || e->Data->GetDataPresent(DataFormats::StringFormat))
	{
		e->Effect = DragDropEffects::Copy;
	}
	else
	{
		e->Effect = DragDropEffects::None;
	}
	return System::Void();
}
System::Void BDMatch::MyForm::ASStext_DragDrop(System::Object ^ sender, System::Windows::Forms::DragEventArgs ^ e)
{
	using namespace System::Text::RegularExpressions;
	using namespace System::IO;
	if (e->Data->GetDataPresent(DataFormats::FileDrop)) {
		array<String^>^files = (array<String^>^)e->Data->GetData(DataFormats::FileDrop);
		if (files->Length > 1) {
			ASStext->Text = "";
			for (int i = 0; i < files->Length; i++) {
				ASStext->Text += "\"" + files[i] + "\"";
			}
		}
		else {
			ASStext->Text = files[0];
		}
	}
	else
		if (e->Data->GetDataPresent(DataFormats::StringFormat)) {
			String^str = (String^)e->Data->GetData(DataFormats::StringFormat);
			ASStext->Text = str;
		}
	return System::Void();
}


System::Void BDMatch::MyForm::About_Click(System::Object ^ sender, System::EventArgs ^ e)
{
	MessageBox::Show(this, "BDMatch\nVersion " + appversion + "\nCopyright (c) 2019, Thomasys\n\nReference:\nFFmpeg 4.1\nFFTW 3.3.7\n" +
		"Matteo Frigo and Steven G. Johnson, Proceedings of the IEEE 93 (2), 216–231 (2005). ", "关于", MessageBoxButtons::OK);
	return System::Void();
}
System::Void BDMatch::MyForm::About_MouseEnter(System::Object ^ sender, System::EventArgs ^ e)
{
	About->Font = gcnew System::Drawing::Font(About->Font->Name, About->Font->Size, FontStyle::Underline);
	return System::Void();
}
System::Void BDMatch::MyForm::About_MouseLeave(System::Object ^ sender, System::EventArgs ^ e)
{
	About->Font = gcnew System::Drawing::Font(About->Font->Name, About->Font->Size, FontStyle::Regular);
	return System::Void();
}
System::Void BDMatch::MyForm::settings_Click(System::Object ^ sender, System::EventArgs ^ e)
{
	if (!setform) {
		setform = gcnew Settings(gcnew SettingCallback(this, &MyForm::SetVals),gcnew NullCallback(this, &MyForm::nullsetform), Setting);
	}
	setform->Show();
	if (!setform->Focused)setform->Focus();
	return System::Void();
}
System::Void BDMatch::MyForm::settings_MouseEnter(System::Object ^ sender, System::EventArgs ^ e)
{
	settings->Font = gcnew System::Drawing::Font(settings->Font->Name, settings->Font->Size, FontStyle::Underline);
	return System::Void();
}
System::Void BDMatch::MyForm::settings_MouseLeave(System::Object ^ sender, System::EventArgs ^ e)
{
	settings->Font = gcnew System::Drawing::Font(settings->Font->Name, settings->Font->Size, FontStyle::Regular);
	return System::Void();
}

System::Void BDMatch::MyForm::ChSelect_SelectedIndexChanged(System::Object ^ sender, System::EventArgs ^ e)
{
	if (ChSelect->Enabled)drawchart();
	return System::Void();
}
System::Void BDMatch::MyForm::ViewSel_SelectedIndexChanged(System::Object ^ sender, System::EventArgs ^ e)
{
	if (ViewSel->Enabled) {
		if (ViewSel->SelectedIndex == 0) {
			TimeRoll->Enabled = true;
			LineSel->Enabled = false;
		}
		else {
			TimeRoll->Enabled = false;
			LineSel->Enabled = true;
		}
		drawchart();
	}
	return System::Void();
}
System::Void BDMatch::MyForm::TimeRoll_Scroll(System::Object ^ sender, System::EventArgs ^ e)
{
	if (TimeRoll->Enabled)drawchart();
	return System::Void();
}
System::Void BDMatch::MyForm::LineSel_ValueChanged(System::Object ^ sender, System::EventArgs ^ e)
{
	if (LineSel->Enabled)drawchart();
	return System::Void();
}

System::Void BDMatch::MyForm::Result_TextChanged(System::Object ^ sender, System::EventArgs ^ e)
{
	Result->SelectionStart = Result->TextLength;
	Result->ScrollToCaret();
	return System::Void();
}

System::Void BDMatch::MyForm::CompleteEdit_Click(System::Object ^ sender, System::EventArgs ^ e)
{
	TextEditorPanel->Visible = false;
	if (EditorLabel->Text == "ASS输入编辑") ASStext->Text = TextEditor->Text->Replace("\r\n", "");
	else if (EditorLabel->Text == "TV输入编辑") TVtext->Text = TextEditor->Text->Replace("\r\n", "");
	else BDtext->Text = TextEditor->Text->Replace("\r\n", "");
	return System::Void();
}
System::Void BDMatch::MyForm::ASSLabel_MouseDoubleClick(System::Object ^ sender, System::Windows::Forms::MouseEventArgs ^ e)
{
	EditorLabel->Text = "ASS输入编辑";
	TextEditor->Text = ASStext->Text->Replace("\"\"", "\"\r\n\"");
	TextEditorPanel->Visible = true;
	return System::Void();
}
System::Void BDMatch::MyForm::TVLabel_MouseDoubleClick(System::Object ^ sender, System::Windows::Forms::MouseEventArgs ^ e)
{
	EditorLabel->Text = "TV输入编辑";
	TextEditor->Text = TVtext->Text->Replace("\"\"", "\"\r\n\"");
	TextEditorPanel->Visible = true;
	return System::Void();
}
System::Void BDMatch::MyForm::BDLabel_MouseDoubleClick(System::Object ^ sender, System::Windows::Forms::MouseEventArgs ^ e)
{
	EditorLabel->Text = "BD输入编辑";
	TextEditor->Text = BDtext->Text->Replace("\"\"", "\"\r\n\"");
	TextEditorPanel->Visible = true;
	return System::Void();
}



