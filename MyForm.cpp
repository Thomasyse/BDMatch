#include "MyForm.h"
#define appversion "1.1.0"
#define tvmaxnum 6
#define secpurple 45
#define setintnum 5

using namespace BDMatch;
[STAThreadAttribute]
int main(array<System::String^>^args)
{
	av_register_all();
	Application::EnableVisualStyles();
	MyForm^ myform = gcnew MyForm();
	Application::Run(myform);
	return 0;
}

const InstructionSet::InstructionSet_Internal InstructionSet::CPU_Rep;

int BDMatch::MyForm::match(String^ asstext, String^ tvtext, String^ bdtext)
{
	using namespace System::IO;
	using namespace System::Threading;
	using namespace System::Threading::Tasks;
	using namespace System::Collections::Concurrent;
	using namespace System::Collections::Generic;
	using namespace System::Text;

	drawpre();
		
	progsingle(0, 0);
	List<Task^>^ decodetasks = gcnew List<Task^>();
	long start = clock();//开始计时
	double* in = (double*)fftw_malloc(sizeof(double)*Setting->FFTnum);
	fftw_complex* out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*Setting->FFTnum);
	fftw_plan plan = fftw_plan_dft_r2c_1d(Setting->FFTnum, in, out, FFTW_MEASURE);
	fftw_free(in);
	fftw_free(out);
	Decode^ tvdecode = gcnew Decode(tvtext, Setting->FFTnum, Setting->outputpcm, Setting->minfinddb, 0, 1,
		decodetasks, CancelSource->Token, plan, gcnew ProgressCallback(this, &MyForm::progsingle));//解码TV文件
	Task^ tvTask = gcnew Task(gcnew Action(tvdecode, &Decode::decodeaudio), CancelSource->Token);
	try {
		tvTask->Start();
	}
	catch (InvalidOperationException^ e) {
		tvdecode->clearfftdata();
		return -6;
	}
	if (Setting->paralleldecode)decodetasks->Add(tvTask);
	else {
		try {
			tvTask->Wait(CancelSource->Token);
		}
		catch (OperationCanceledException^ e) {
			tvdecode->clearfftdata();
			fftw_destroy_plan(plan);
			return -6;
		}
	}

	do {
		Application::DoEvents();
	} while (tvdecode->getsamprate() == 0);
	Decode^ bddecode = gcnew Decode(bdtext, Setting->FFTnum, Setting->outputpcm, Setting->minfinddb, tvdecode->getsamprate(), 2,
		decodetasks, CancelSource->Token, plan, gcnew ProgressCallback(this, &MyForm::progsingle));//解码BD文件
	Task^ bdTask = gcnew Task(gcnew Action(bddecode, &Decode::decodeaudio), CancelSource->Token);
	try {
		bdTask->Start();
	}
	catch (InvalidOperationException^ e) {
		tvdecode->clearfftdata();
		bddecode->clearfftdata();
		return -6;
	}
	decodetasks->Add(bdTask);
	
	try {
		Task::WaitAll(decodetasks->ToArray(), CancelSource->Token);
	}
	catch (OperationCanceledException^ e) {
		tvdecode->clearfftdata();
		bddecode->clearfftdata();
		fftw_destroy_plan(plan);
		return -6;
	}
	fftw_destroy_plan(plan);
	//输出解码时间
	long end = clock();
	double spend = double(end - start) / (double)CLOCKS_PER_SEC;

	Result->Text += "TV文件：  " + tvtext->Substring(tvtext->LastIndexOf("\\") + 1) + "\r\n" + tvdecode->getfeedback();
	Result->Text += "\r\nBD文件：  " + bdtext->Substring(bdtext->LastIndexOf("\\") + 1) + "\r\n" + bddecode->getfeedback() +
		"\r\n解码时间：" + spend.ToString() + "秒";
	if (tvdecode->getreturn() < 0 || bddecode->getreturn() < 0) return -4;

	int re = 0;
	if (Setting->matchass) {
		re = writeass(tvdecode, bddecode, asstext);
	}
	else {
		progsingle(3, 0);
	}
	drawpre(tvdecode, bddecode, re);
	if (re < 0) {
		tvdecode->clearfftdata();
		bddecode->clearfftdata();
		if (re == -2)return -6;
		else return -5;
	}
	progsingle(0, 0);
	return 0;
}

int BDMatch::MyForm::writeass(Decode^ tvdecode, Decode^ bddecode, String^ asstext)
{
	using namespace System::IO;
	using namespace System::Text;
	using namespace System::Text::RegularExpressions;
	using namespace System::Threading::Tasks;
	using namespace System::Collections::Concurrent;
	using namespace System::Collections::Generic;

	long startclock = clock();
	int tvmilisec = tvdecode->getmilisecnum();
	int bdmilisec = bddecode->getmilisecnum();
	int tvfftnum = tvdecode->getfftsampnum();
	int bdfftnum = bddecode->getfftsampnum();
	std::vector<std::vector<node*>>* tvfftdata = tvdecode->getfftdata();
	std::vector<std::vector<node*>>* bdfftdata = bddecode->getfftdata();
	int tvch = tvdecode->getchannels();
	int bdch = bddecode->getchannels();
	String^ tvass;
	String^ head = "";
	String^ content = "";
	tvass = File::ReadAllText(asstext);
	int eventpos = tvass->IndexOf("\r\n[Events]\r\n");
	if (eventpos == -1) {
		Result->Text += "\r\n输入字幕文件无效！";
		return -1;
	}
	eventpos += 2;
	head = tvass->Substring(0, eventpos);
	content = tvass->Substring(eventpos, tvass->Length - eventpos);
	tvass = "";
	Regex^ fileregex1 = gcnew Regex("Audio File: .*?\\r\\n");
	Regex^ fileregex2 = gcnew Regex("Video File: .*?\\r\\n");
	head = fileregex1->Replace(head, "Audio File: " + bddecode->getfilename() + "\r\n");
	head = fileregex2->Replace(head, "Video File: " + bddecode->getfilename() + "\r\n");
	//: 0,0:22:38.77,0:22:43.35
	Regex^ alltimeregex = gcnew Regex("\\r\\n[a-zA-Z]+: [0-9],[0-9]:[0-9]{2}:[0-9]{2}\\.[0-9]{2},[0-9]:[0-9]{2}:[0-9]{2}\\.[0-9]{2},");
	Regex^ headregex = gcnew Regex("\\r\\n[a-zA-Z]+: [0-9],");
	Regex^ timeregex = gcnew Regex("[0-9]:[0-9]{2}:[0-9]{2}\\.[0-9]{2}");
	MatchCollection^ alltimematch = alltimeregex->Matches(content);
	array<timec^>^ timelist = gcnew array<timec^>(alltimematch->Count);//储存时间
	std::vector<int>tvtime(alltimematch->Count), bdtime(alltimematch->Count);
	int rightshift = static_cast<int>(log2(Setting->FFTnum));
	//计算每行时间，屏蔽不必要的行
	for (int i = 0; i < alltimematch->Count; i++) {
		String^ match = alltimematch[i]->ToString();
		String^ timehead = headregex->Match(match)->Value;
		bool iscom = match->Contains("Comment");
		MatchCollection^ timematch = timeregex->Matches(match);
		String^ time = timematch[0]->Value;
		int start = int::Parse(time[0].ToString()) * 360000 + int::Parse(time->Substring(2, 2)) * 6000 +
			int::Parse(time->Substring(5, 2)) * 100 + int::Parse(time->Substring(8, 2));
		time = timematch[1]->Value;
		int end = int::Parse(time[0].ToString()) * 360000 + int::Parse(time->Substring(2, 2)) * 6000 +
			int::Parse(time->Substring(5, 2)) * 100 + int::Parse(time->Substring(8, 2));
		start = static_cast<int>(start / double(tvmilisec)* tvfftnum);
		end = static_cast<int>(end / double(tvmilisec)* tvfftnum);
		timelist[i] = gcnew timec(start, end, iscom, timehead);
		if (iscom) {
			tvtime[i] = -1;
			bdtime[i] = -1;
			Result->Text += "\r\n信息：第" + (i + 1).ToString() + "行为注释，将不作处理。";
			continue;
		}
		if (end == start) {
			tvtime[i] = -1;
			bdtime[i] = -1;
			Result->Text += "\r\n信息：第" + (i + 1).ToString() + "行时长为零，将不作处理。";
			continue;
		}
		if (end - start > Setting->maxlength * 100 / double(tvmilisec)* tvfftnum) {
			tvtime[i] = -1;
			bdtime[i] = -1;
			Result->Text += "\r\n警告：第" + (i + 1).ToString() + "行时长过长，将不作处理。";
			continue;
		}
		if (end >= tvfftnum) {
			tvtime[i] = -1;
			bdtime[i] = -1;
			Result->Text += "\r\n警告：第" + (i + 1).ToString() + "行超过音频长度，将不作处理。";
			continue;
		}
		int maxdb = -128;
		for (int j = start; j <= end; j++) {
			if ((*tvfftdata)[0][j]->maxv() > maxdb) {
				maxdb = (*tvfftdata)[0][j]->maxv();
			}
		}
		if (maxdb <= -128) {
			tvtime[i] = -1;
			bdtime[i] = -1;
			Result->Text += "\r\n警告：第" + (i + 1).ToString() + "行声音过小，将不作处理。";
			continue;
		}
		bool existed = false;
		int sameline;
		for (sameline = 0; sameline < i; sameline++) {
			if (start == tvtime[sameline] && end == timelist[sameline]->end()) {
				existed = true;
				break;
			}
		}
		if (existed) {
			tvtime[i] = -sameline - 2;
			bdtime[i] = -sameline - 2;
			continue;
		}
		tvtime[i] = start;
		bdtime[i] = 0;
	}
	//搜索匹配
	double mstofft = tvfftnum / static_cast<double>(tvmilisec);
	int ch = min(tvch, bdch);
	ch = min(ch, 2);
	int find0 = static_cast<int>(Setting->findfield * 100 * mstofft);
	int interval = static_cast<int>(mstofft);
	if (interval < 1) {
		interval = 1;
	}
	Int64 *diftime = new Int64[3];
	double aveindex = 0, maxindex = 0; int maxdelta = 0, maxline = 0;//调试用
	int offset = 0; int fivesec = 0; int lastlinetime = 0;
	if (Setting->fastmatch) {
		fivesec = static_cast<int>(500 * mstofft);
		Result->Text += "\r\n信息：使用快速匹配。";
	}
	for (int i = 0; i < alltimematch->Count; i++) {
		if (tvtime[i] >= 0) {
			if (Setting->fastmatch && offset && lastlinetime > fivesec && tvtime[i - 1] > 0 && labs(tvtime[i] - lastlinetime) < fivesec) {
				bdtime[i] = tvtime[i] + offset;
				lastlinetime = tvtime[i];
				continue;
			}
			int findstart = static_cast<int>(tvtime[i] - find0);
			int findend = static_cast<int>(tvtime[i] + find0);
			int duration = timelist[i]->end() - timelist[i]->start();
			findstart = max(0, findstart);
			findend = static_cast<int>(min(bdfftnum - duration - 1, findend));
			int findnum = (findend - findstart) / interval;
			//初筛
			int tvmax[tvmaxnum] , tvmaxtime[tvmaxnum];
			for (auto& j : tvmax) j = -128 * Setting->FFTnum / 2;
			for (auto& j : tvmaxtime) j = 0;
			for (int j = 0; j <= duration; j++) {
				if ((*tvfftdata)[0][j + tvtime[i]]->sum() > tvmax[0] || j == 0) {
					for (int k = tvmaxnum - 1; k > 0; k--) {
						tvmax[k] = tvmax[k - 1];
						tvmaxtime[k] = tvmaxtime[k - 1];
					}
					tvmax[0] = (*tvfftdata)[0][j + tvtime[i]]->sum();
					tvmaxtime[0] = j;
				}
			}
			bdsearch bdse(findnum);
			for (int j = 0; j <= findnum; j++) {
				int bdtimein = findstart + j * interval;
				int delta = 0;
				for (int k = 0; k < tvmaxnum; k++) {
					delta += labs((*bdfftdata)[0][bdtimein + tvmaxtime[k]]->sum() - tvmax[k]);
				}
				delta = delta >> rightshift;
				if (delta < 250)bdse.push(bdtimein, delta);
			}
			bdse.sort();
			//精确匹配
			diftime[0] = 0;
			diftime[1] = 9223372036854775807;
			int minchecknumcal = Setting->minchecknum;
			if (duration <= 75 * interval)minchecknumcal = findnum;
			else if (Setting->fastmatch)minchecknumcal = Setting->minchecknum / 2 * 3;
			diftime[2] = minchecknumcal;
			List<Task^>^ tasks = gcnew List<Task^>();
			for (int j = 0; j < bdse.size(); j++) {
				Var^ calvar = gcnew Var(tvfftdata, bdfftdata, tvtime[i], bdse.read(j),
					duration, ch, minchecknumcal, interval, ISAMode, diftime);
				Task^ varTask = gcnew Task(gcnew Action(calvar, &Var::caldiff), CancelSource->Token);
				if (varTask->Status != System::Threading::Tasks::TaskStatus::Canceled)varTask->Start();
				else {
					tvdecode->clearfftdata();
					bddecode->clearfftdata();
					Result->Text += "\r\n\r\n用户中止操作。";
					return -2;
				}
				tasks->Add(varTask);
			}
			try {
				Task::WaitAll(tasks->ToArray(), CancelSource->Token);
			}
			catch (OperationCanceledException^ e) {
				tvdecode->clearfftdata();
				bddecode->clearfftdata();
				Result->Text += "\r\n\r\n用户中止操作。";
				return -2;
			}
			//调试用->
			if (debugmode) {
				int delta1 = bdse.find(static_cast<int>(diftime[0]), 1);
				if (delta1 > maxdelta)maxdelta = delta1;
				//Result->Text += "\r\n" + delta1.ToString();
				double foundindex = bdse.find(static_cast<int>(diftime[0]), 0) / (double)findnum;
				aveindex = aveindex + foundindex;
				if (foundindex > maxindex&&duration > 75 * interval) {
					maxindex = foundindex;
					maxline = i + 1;
				}
			}
			//
			bdtime[i] = static_cast<int>(diftime[0]);
			if (Setting->fastmatch) {
				offset = bdtime[i] - tvtime[i];
				lastlinetime = tvtime[i];
			}
		}
		progsingle(3, (i + 1) / static_cast<double>(alltimematch->Count));
	}
	delete[] diftime;
	//调试用->
	if (debugmode) {
		aveindex /= alltimematch->Count / 100.0;
		maxindex *= 100;
		Result->Text += "\r\nAverage Found Index = " + aveindex.ToString() + "%    " + "Max Found Index= " + maxindex.ToString() +
			"%\r\nMax Found Line= " + maxline.ToString() + "    Max Delta= " + maxdelta.ToString();
	}
	//
	//绘图相关
	if (Setting->draw) {
		tvdraw.timelist = gcnew array<int, 2>(alltimematch->Count, 2);
		bddraw.timelist = gcnew array<int, 2>(alltimematch->Count, 2);
		tvdraw.linenum = alltimematch->Count;
		bddraw.linenum = alltimematch->Count;
		LineSel->Maximum = alltimematch->Count;
	}
	//写字幕
	for (int i = 0; i < alltimematch->Count; i++) {
		if (Setting->draw) {
			tvdraw.timelist[i, 0] = timelist[i]->start();
			tvdraw.timelist[i, 1] = timelist[i]->end();
		}
		if (tvtime[i] >= 0) {
			int duration = static_cast<int>(timelist[i]->end() - timelist[i]->start());
			timelist[i]->start(bdtime[i]);
			timelist[i]->end(bdtime[i] + duration);
			if (i < alltimematch->Count - 1 && timelist[i]->end() > bdtime[i + 1] && (timelist[i]->end() - bdtime[i + 1]) <= interval) {
				timelist[i]->end(bdtime[i + 1]);
				Result->Text += "\r\n信息：第" + (i + 1).ToString() + "行和第" + (i + 2).ToString() + "行发生微小重叠，已自动修正。";
			}
			if (Setting->draw) {
				bddraw.timelist[i, 0] = timelist[i]->start();
				bddraw.timelist[i, 1] = timelist[i]->end();
			}
		}
		else if (Setting->draw) {
			if (tvtime[i] == -1) {
				bddraw.timelist[i, 0] = -1;
				bddraw.timelist[i, 1] = -1;
				tvdraw.timelist[i, 0] = -1;
				tvdraw.timelist[i, 1] = -1;
			}
			else {
				bddraw.timelist[i, 0] = timelist[-tvtime[i] - 2]->start();
				bddraw.timelist[i, 1] = timelist[-tvtime[i] - 2]->end();
			}
		}
		int start = static_cast<int>(timelist[i]->start() / double(bdfftnum) * bdmilisec);
		int end = static_cast<int>(timelist[i]->end() / double(bdfftnum) * bdmilisec);
		String^ starttime = mstotime(start);
		String^ endtime = mstotime(end);
		String^ replacetext = timelist[i]->head() + starttime + "," + endtime + ",";
		content = content->Replace(alltimematch[i]->Value, replacetext);
	}
	String^ outfile = asstext->Substring(0, asstext->LastIndexOf(".")) + ".matched.ass";
	if (File::Exists(outfile))
	{
		File::Delete(outfile);
	}
	// Create the file.
	FileStream^ fs = File::Create(outfile);
	try
	{
		array<Byte>^ info = (gcnew UTF8Encoding(true))->GetBytes(head + content);
		fs->Write(info, 0, info->Length);
	}
	finally
	{
		if (fs)
			delete (IDisposable^)fs;
	}
	tvfftdata = nullptr;
	bdfftdata = nullptr;
	long endclock = clock();
	double spend = double(endclock - startclock) / (double)CLOCKS_PER_SEC;
	Result->Text += "\r\n匹配时间：" + spend.ToString() + "秒";
	return 0;
}


int BDMatch::MyForm::drawpre()
{
	if (tvdraw.num > 0) {
		for (int i = 0; i < tvdraw.num; i++) {
			if ((*tvdraw.data)[0][i] != nullptr)delete (*tvdraw.data)[0][i];
			if ((*tvdraw.data)[1][i] != nullptr)delete (*tvdraw.data)[1][i];
		}
		tvdraw.num = 0;
	}
	if (bddraw.num > 0) {
		for (int i = 0; i < bddraw.num; i++) {
			if ((*bddraw.data)[0][i] != nullptr)delete (*bddraw.data)[0][i];
			if ((*bddraw.data)[1][i] != nullptr)delete (*bddraw.data)[1][i];
		}
		bddraw.num = 0;
	}
	delete tvdraw.data;
	delete bddraw.data;
	tvdraw.data = nullptr;
	bddraw.data = nullptr;
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
	tvdraw.timelist = nullptr;
	bddraw.timelist = nullptr;
	ChartTime->Text = "";
	if (!Setting->draw)setrows();
	return 0;
}
int BDMatch::MyForm::drawpre(Decode ^ tvdecode, Decode ^ bddecode,int &re)
{
	if (Setting->draw) {
		tvdraw.data = tvdecode->getfftdata();
		bddraw.data = bddecode->getfftdata();
		tvdraw.num = tvdecode->getfftsampnum();
		bddraw.num = bddecode->getfftsampnum();
		tvdraw.ch = tvdecode->getchannels();
		bddraw.ch = bddecode->getchannels();
		tvdraw.milisec = tvdecode->getmilisecnum();
		bddraw.milisec = bddecode->getmilisecnum();
		ViewSel->SelectedIndex = 0;
		ChSelect->SelectedIndex = 0;
		ChSelect->Enabled = true;
		TimeRoll->Maximum = max(tvdraw.milisec, bddraw.milisec);
		TimeRoll->Value = 0;
		TimeRoll->Enabled = true;
		if (!re && Setting->matchass)ViewSel->Enabled = true;
		setrows();
		drawchart();
	}
	else
	{
		if (!re) {
			tvdecode->clearfftdata();
			bddecode->clearfftdata();
		}
		tvdraw.data = nullptr;
		bddraw.data = nullptr;
		tvdraw.num = 0;
		bddraw.num = 0;
	}
	return 0;
}
int BDMatch::MyForm::drawchart()
{
	int maxsampnum = max(tvdraw.num, bddraw.num);
	int milisec = max(tvdraw.milisec, bddraw.milisec);
	double mstofft = maxsampnum / static_cast<double>(milisec);
	int offset = 0;
	if (milisec < 10000) offset = 150;
	else if (milisec < 100000)offset = 250;
	else offset = 350;
	TimeRoll->TickFrequency = offset;
	TimeRoll->LargeChange = offset;
	TimeRoll->SmallChange = offset / 4;
	int tvstart = 0, tvend = 0, bdstart = 0;
	if (ViewSel->SelectedIndex == 0) {
		tvstart = static_cast<int>((TimeRoll->Value - offset)* mstofft);
		tvend = static_cast<int>((TimeRoll->Value + offset)* mstofft);
		bdstart = tvstart;
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
		if (tvdraw.timelist[static_cast<int>(LineSel->Value) - 1, 0] == -1)ChartTime->Text = "未匹配！";
		else ChartTime->Text = mstotime(static_cast<int>(tvdraw.timelist[static_cast<int>(LineSel->Value) - 1, 0] / double(tvdraw.num)*tvdraw.milisec))
			+ "\n" + mstotime(static_cast<int>(bddraw.timelist[static_cast<int>(LineSel->Value) - 1, 0] / double(bddraw.num)*bddraw.milisec));
	}
	int duration = tvend - tvstart + 1;
	Bitmap^ spectrum1 = gcnew Bitmap(duration, Setting->FFTnum);
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
			if (y >= Setting->FFTnum / 2) {
				if (0 <= x + bdstart && x + bdstart < bddraw.num)color = (*bddraw.data)[ChSelect->SelectedIndex][x + bdstart]->read0(Setting->FFTnum - y);
			}
			else if (0 <= x + tvstart && x + tvstart < tvdraw.num)
				color = (*tvdraw.data)[ChSelect->SelectedIndex][x + tvstart]->read0(Setting->FFTnum / 2 - y);
			color += 128;
			Color newColor = Color::FromArgb(color / 4, color, color);
			if (y >= Setting->FFTnum / 2) {
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
			for (int i = 0; i < 10; i++) {
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
		for (int i = 0; i < 10; i++) {
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
	//asstext = "E:\\Movie\\[VCB-Studio] Haikyuu!! 3rd Season [Ma10p_1080p]\\[VCB-Studio] Haikyuu!! 3rd Season [*][Ma10p_1080p][x265_flac_aac].ass";
	//tvtext = "E:\\Movie\\[VCB-Studio] Haikyuu!! 3rd Season [Ma10p_1080p]\\[JYFanSub][Haikyuu!! S3][*][720P][GB].mp4";
	//bdtext = "E:\\Movie\\[VCB-Studio] Haikyuu!! 3rd Season [Ma10p_1080p]\\[VCB-Studio] Haikyuu!! 3rd Season [*][Ma10p_1080p][x265_flac_aac].mkv";
	String ^ asstext = ASStext->Text;
	String ^ tvtext = TVtext->Text;
	String ^ bdtext = BDtext->Text;
	matchcount = finishedmatch = 0;
	if (asstext == "" || tvtext == "" || bdtext == "") {
		matchcontrol(true);
		MessageBox::Show(this, "文件名为空！", "BDMatch", MessageBoxButtons::OK);
		return -1;
	}
	if (tvtext == bdtext) {
		matchcontrol(true);
		MessageBox::Show(this, "BD和TV文件相同！", "BDMatch", MessageBoxButtons::OK);
		return -1;
	}
	int re = 0;
	if (!asstext->Contains("*")) {
		if (!tvtext->Contains("*") && !bdtext->Contains("*")) {
			if (!File::Exists(asstext)) {
				matchcontrol(true);
				MessageBox::Show(this, "ASS文件不存在！", "BDMatch", MessageBoxButtons::OK);
				return -1;
			}
			if (!File::Exists(tvtext)) {
				matchcontrol(true);
				MessageBox::Show(this, "TV文件不存在！", "BDMatch", MessageBoxButtons::OK);
				return -1;
			}
			if (!File::Exists(bdtext)) {
				matchcontrol(true);
				MessageBox::Show(this, "BD文件不存在！", "BDMatch", MessageBoxButtons::OK);
				return -1;
			}
			matchcount = 1;
			long start = clock();//开始计时
			Result->Text += "\r\nASS文件：  " + asstext->Substring(asstext->LastIndexOf("\\") + 1) + "\r\n";
			re = match(ASStext->Text, TVtext->Text, BDtext->Text);
			if (re < 0) {
				if (re == -6)	Result->Text += "\r\n用户中止操作。";
				matchcontrol(true);
				return re;
			}
			long end = clock();
			double spend = double(end - start) / (double)CLOCKS_PER_SEC;
			Result->Text += "\r\n总时间：" + spend.ToString() + "秒";
		}
		else {
			matchcontrol(true);
			MessageBox::Show(this, "文件名格式错误！", "BDMatch", MessageBoxButtons::OK);
			return -1;
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
				matchcontrol(true);
				MessageBox::Show(this, "ASS路径不存在！", "BDMatch", MessageBoxButtons::OK);
				return -2;
			}
			Regex^ assfilekey = gcnew Regex(returnregt(asstext));
			for (int i = 0; i < files->Length; i++)
				if (assfilekey->IsMatch(files[i])) assfiles->Add(files[i]);
			path = tvtext->Substring(0, tvtext->LastIndexOf("\\"));
			if (Directory::Exists(path))files = Directory::GetFiles(path);
			else {
				matchcontrol(true);
				MessageBox::Show(this, "TV路径不存在！", "BDMatch", MessageBoxButtons::OK);
				return -2;
			}
			Regex^ tvfilekey = gcnew Regex(returnregt(tvtext));
			for (int i = 0; i < files->Length; i++)
				if (tvfilekey->IsMatch(files[i])) tvfiles->Add(files[i]);
			path = bdtext->Substring(0, bdtext->LastIndexOf("\\"));
			if (Directory::Exists(path))files = Directory::GetFiles(path);
			else {
				matchcontrol(true);
				MessageBox::Show(this, "BD路径不存在！", "BDMatch", MessageBoxButtons::OK);
				return -2;
			}
			Regex^ bdfilekey = gcnew Regex(returnregt(bdtext));
			for (int i = 0; i < files->Length; i++)
				if (bdfilekey->IsMatch(files[i])) bdfiles->Add(files[i]);
			bool drawstore = Setting->draw;
			if (Setting->draw == true) {
				Result->Text += "批量处理将不作声谱图。\r\n";
				Setting->draw = false;
			}
			matchcount = assfiles->Count;
			long start = clock();//开始计时
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
						Result->Text += "\r\n用户中止操作。";
						matchcontrol(true);
						return -6;
					}
					Result->Text += "\r\n----------------------------------------------------------------------------------------------";
				}
				else {
					Result->Text += "\r\n未找到对应的TV或BD文件！";
					Result->Text += "\r\n----------------------------------------------------------------------------------------------";
				}
				finishedmatch = i + 1;
			}
			Setting->draw = drawstore;
			//结束计时
			long end = clock();
			double spend = double(end - start) / (double)CLOCKS_PER_SEC;
			Result->Text += "\r\n总时间：" + spend.ToString() + "秒";
		}
		else {
			matchcontrol(true);
			MessageBox::Show(this, "文件名格式错误！", "BDMatch", MessageBoxButtons::OK);
			return -1;
		}
	}
	if (re >= 0) {
		adddropdown(ASStext, ASStext->Text);
		adddropdown(TVtext, TVtext->Text);
		adddropdown(BDtext, BDtext->Text);
	}
	matchcontrol(true);
	return 0;
}
int BDMatch::MyForm::searchISA()
{
	Result->Text += "CPU：" + marshal_as<String^>(InstructionSet::Brand());
	ISAMode = 0;
	if (InstructionSet::AVX2() && InstructionSet::AVX()) {
		ISAMode = 2;
		Result->Text += "\r\n使用AVX、AVX2指令集加速匹配。";
	}
	else if (InstructionSet::SSE41() && InstructionSet::SSE2() && InstructionSet::SSSE3()) {
		ISAMode = 1;
		Result->Text += "\r\n使用SSE2、SSSE3、SSE4.1指令集加速匹配。";
	}
	Result->Text += "\r\n----------------------------------------------------------------------------------------------";
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
	static double findfieldpartion = Setting->matchass ? Setting->findfield * 2 : 0;
	switch (type) {
	case 1:
		progval[0] = val;
		break;
	case 2:
		progval[1] = val;
		progval[0] = 1;
		break;
	case 3:
		progval[2] = val;
		progval[1] = 1;
		break;
	default:
		progval[0] = val;
		progval[1] = val;
		progval[2] = val;
		findfieldpartion = Setting->matchass ? Setting->findfield * 2 : 0;
		return System::Void();
		break;
	}
	SingleProgress->Value = static_cast<int>(round(SingleProgress->Maximum * ((progval[0] + progval[1]) * 10 + progval[2] * findfieldpartion)
		/ (20 + findfieldpartion)));
	progtotal();
	return System::Void();
}
void BDMatch::MyForm::progtotal()
{
	double val = TotalProgress->Maximum *(finishedmatch
		+ SingleProgress->Value / static_cast<double>(SingleProgress->Maximum)) / static_cast<double>(matchcount);
	TotalProgress->Value = static_cast<int>(val);
	return System::Void();
}

System::Void BDMatch::MyForm::MyForm_Load(System::Object ^ sender, System::EventArgs ^ e)
{
	SingleProgress->CheckForIllegalCrossThreadCalls = false;
	TotalProgress->CheckForIllegalCrossThreadCalls = false;
	About->Text = "v" + appversion;
	loadsettings("settings.ini", Setting);
	return System::Void();
}

System::Void BDMatch::MyForm::MyForm_FormClosing(System::Object ^ sender, System::Windows::Forms::FormClosingEventArgs ^ e)
{
	savesettings("settings.ini", Setting);
	return System::Void();
}

System::Void BDMatch::MyForm::TVfind_Click(System::Object ^ sender, System::EventArgs ^ e)
{
	using namespace System::IO;
	Stream^ myStream;
	Filebrowse->Title = "获取文件名";
	Filebrowse->FileName = "";
	Filebrowse->Filter = "Video Files|*.mp4;*.mov;*.mkv;*.avi;*.wmv;*.mpeg;*.mpg;*.qt;*.rmvb;*.flv|All Files|*.*";
	Filebrowse->FilterIndex = 0;
	Filebrowse->RestoreDirectory = true;
	if (Filebrowse->ShowDialog() == System::Windows::Forms::DialogResult::OK)
	{
		if ((myStream = Filebrowse->OpenFile()) != nullptr)
		{
			using namespace System::Text::RegularExpressions;
			TVtext->Text = Filebrowse->FileName;
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
		TVtext->Text = files[0];
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
	Filebrowse->Title = "获取文件名";
	Filebrowse->FileName = "";
	Filebrowse->Filter = "Video Files|*.mp4;*.mov;*.mkv;*.avi;*.wmv;*.mpeg;*.mpg;*.qt;*.rmvb;*.flv|All Files|*.*";
	Filebrowse->FilterIndex = 0;
	Filebrowse->RestoreDirectory = true;
	if (Filebrowse->ShowDialog() == System::Windows::Forms::DialogResult::OK)
	{
		if ((myStream = Filebrowse->OpenFile()) != nullptr)
		{
			using namespace System::Text::RegularExpressions;
			BDtext->Text = Filebrowse->FileName;
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
		BDtext->Text = files[0];
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
	Filebrowse->Title = "获取文件名";
	Filebrowse->FileName = "";
	Filebrowse->Filter = "ASS Files|*.ass|All Files|*.*";
	Filebrowse->FilterIndex = 0;
	Filebrowse->RestoreDirectory = true;
	if (Filebrowse->ShowDialog() == System::Windows::Forms::DialogResult::OK)
	{
		if ((myStream = Filebrowse->OpenFile()) != nullptr)
		{
			using namespace System::Text::RegularExpressions;
			ASStext->Text = Filebrowse->FileName;
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
		ASStext->Text = files[0];
	}
	else
		if (e->Data->GetDataPresent(DataFormats::StringFormat)) {
			String^str = (String^)e->Data->GetData(DataFormats::StringFormat);
			ASStext->Text = str;
		}
	return System::Void();
}


System::Void BDMatch::MyForm::Match_Click(System::Object ^ sender, System::EventArgs ^ e)
{
	if (Match->Text == "匹配") {
		if (ASStext->Text == "debug mode") {
			debugmode = !debugmode;
			Result->Text = debugmode ? "调试模式打开。" : "调试模式关闭";
			return;
		}
		matchcontrol(false);
		CancelSource = gcnew System::Threading::CancellationTokenSource();
		Task<int>^matchtask = gcnew Task<int>(gcnew Func<int>(this, &MyForm::matchinput),
			CancelSource->Token, TaskCreationOptions::LongRunning);
		matchtask->Start();
	}
	else {
		CancelSource->Cancel();
		matchcontrol(true);
	}
	return System::Void();
}

System::Void BDMatch::MyForm::About_Click(System::Object ^ sender, System::EventArgs ^ e)
{
	MessageBox::Show(this, "BDMatch\nVersion " + appversion + "\nBy Thomasys, 2018\n\nReference:\nFFmpeg3.4.1\nFFTW3.3.7\n" +
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


