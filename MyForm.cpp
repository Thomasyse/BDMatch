#include "MyForm.h"

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


int BDMatch::MyForm::match()
{
	using namespace System::IO;
	using namespace System::Threading;
	using namespace System::Threading::Tasks;
	using namespace System::Collections::Concurrent;
	using namespace System::Collections::Generic;
	using namespace Node;

	using namespace System::Text;

	if (tvdraw.num > 0) {
		for (int i = 0; i < tvdraw.num; i++) {
			if (tvdraw.data[0, i] != nullptr)tvdraw.data[0, i]->release();
			if (tvdraw.data[1, i] != nullptr)tvdraw.data[1, i]->release();
		}
		tvdraw.num = 0;
	}
	if (bddraw.num > 0) {
		for (int i = 0; i < bddraw.num; i++) {
			if (bddraw.data[0, i] != nullptr)bddraw.data[0, i]->release();
			if (bddraw.data[1, i] != nullptr)bddraw.data[1, i]->release();
		}
		bddraw.num = 0;
	}
	FileSelect->Enabled = false;
	ChSelect->Enabled = false;
	TimeRoll->Enabled = false;
	FileSelect->SelectedIndex = 0;
	ChSelect->SelectedIndex = 0;
	tvdraw.ch = 0;
	bddraw.ch = 0;
	tvdraw.milisec = 0;
	bddraw.milisec = 0;
	ChartTime->Text = "";
	if (!draw)setrows();

	Result->Text = "";
	if (!File::Exists(ASStext->Text)) {
		MessageBox::Show(this, "ASS文件不存在！", "BDMatch", MessageBoxButtons::OK);
		return -1;
	}
	if (!File::Exists(TVtext->Text)) {
		MessageBox::Show(this, "TV文件不存在！", "BDMatch", MessageBoxButtons::OK);
		return -1;
	}
	if (!File::Exists(BDtext->Text)) {
		MessageBox::Show(this, "BD文件不存在！", "BDMatch", MessageBoxButtons::OK);
		return -1;
	}
	if (TVtext->Text == BDtext->Text) {
		MessageBox::Show(this, "BD和TV文件相同！", "BDMatch", MessageBoxButtons::OK);
		return -1;
	}
	int tvch = 0, bdch = 0;
	int tvsampnum = 0, bdsampnum = 0;
	int tvmilisec = 0, bdmilisec = 0;
	array<node^, 2>^ tvfftdata;
	array<node^, 2>^ bdfftdata;
	Result->Text = "";
	tvprogressBar->Value = 0;
	bdprogressBar->Value = 0;

	List<Task^>^ tvtasks = gcnew List<Task^>();
	List<Task^>^ bdtasks = gcnew List<Task^>();
	long tvstart = clock();//开始计时
	Decode^ tvdecode = gcnew Decode(TVtext->Text, FFTnum, outputpcm, minfinddb, 0, tvtasks,
		gcnew ProgressCallback(this, &MyForm::progtv), gcnew ProgMaxCallback(this, &MyForm::progtvmax));//解码TV文件
	Task^ tvTask = gcnew Task(gcnew Action(tvdecode, &Decode::decodeaudio));
	tvTask->Start();
	tvTask->Wait();
	
	long bdstart = clock();//开始计时
	Decode^ bddecode = gcnew Decode(BDtext->Text, FFTnum, outputpcm, minfinddb, tvdecode->getsamprate(), bdtasks,
		gcnew ProgressCallback(this, &MyForm::progbd), gcnew ProgMaxCallback(this, &MyForm::progbdmax));//解码BD文件
	Task^ bdTask = gcnew Task(gcnew Action(bddecode, &Decode::decodeaudio));
	bdTask->Start();
	bdTask->Wait();

	Task::WaitAll(tvtasks->ToArray());
	//输出解码时间
	long tvend = clock();
	double tvspend = double(tvend - tvstart) / (double)CLOCKS_PER_SEC;
	Task::WaitAll(bdtasks->ToArray());
	//输出解码时间
	long bdend = clock();
	double bdspend = double(bdend - bdstart) / (double)CLOCKS_PER_SEC;
	tvspend = tvspend - bdspend;

	Result->Text += "TV文件：\r\n" + tvdecode->getfeedback() + "\r\n解码时间：" + tvspend.ToString() + "秒";
	Result->Text += "\r\nBD文件：\r\n" + bddecode->getfeedback()+ "\r\n解码时间：" + bdspend.ToString() + "秒";
	if (tvdecode->getreturn() < 0 || bddecode->getreturn() < 0) return-1;
	tvfftdata = tvdecode->getfftdata();
	bdfftdata = bddecode->getfftdata();
	tvsampnum = tvdecode->getfftsampnum();
	bdsampnum = bddecode->getfftsampnum();
	tvmilisec = tvdecode->getmilisecnum();
	bdmilisec = bddecode->getmilisecnum();

	tvprogressBar->Value = tvprogressBar->Maximum;
	bdprogressBar->Value = bdprogressBar->Maximum;
	
	if (matchass) {
		writeass(tvdecode, bddecode);
	}

	if (draw) {
		tvdraw.data = tvfftdata;
		bddraw.data = bdfftdata;
		tvdraw.num = tvsampnum;
		bddraw.num = bdsampnum;
		tvdraw.ch = tvdecode->getchannels();
		bddraw.ch = bddecode->getchannels();
		tvdraw.milisec = tvmilisec;
		bddraw.milisec = bdmilisec;
		FileSelect->SelectedIndex = 0;
		FileSelect->Enabled = true;
		ChSelect->SelectedIndex = 0;
		ChSelect->Enabled = true;
		TimeRoll->Maximum = tvdraw.milisec;
		TimeRoll->Value = 0;
		TimeRoll->Enabled = true;
		setrows();
		drawchart();
	}
	else
	{
		for (int i = 0; i < tvsampnum; i++) {
			if (tvfftdata[0, i] != nullptr)tvfftdata[0, i]->release();
			if (tvfftdata[1, i] != nullptr)tvfftdata[1, i]->release();
		}
		for (int i = 0; i < bdsampnum; i++) {
			if (bdfftdata[0, i] != nullptr)bdfftdata[0, i]->release();
			if (bdfftdata[1, i] != nullptr)bdfftdata[1, i]->release();
		}
	}

	tvprogressBar->Value = tvprogressBar->Maximum;
	bdprogressBar->Value = bdprogressBar->Maximum;

	return 0;
}

int BDMatch::MyForm::writeass(Decode^ tvdecode, Decode^ bddecode)
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
	array<node^, 2>^ tvfftdata = tvdecode->getfftdata();
	array<node^, 2>^ bdfftdata = bddecode->getfftdata();
	int tvch = tvdecode->getchannels();
	int bdch = bddecode->getchannels();
	
	tvprogressBar->Value = 0;
	bdprogressBar->Value = 0;
	String^ tvass;
	String^ head = "";
	String^ content = "";
	UTF8Encoding^ temp = gcnew UTF8Encoding(true);
	tvass = File::ReadAllText(ASStext->Text);
	int eventpos = tvass->IndexOf("\r\n[Events]\r\n");
	if (eventpos == -1) {
		Result->Text += "\r\n输入字幕文件无效！";
		return -1;
	}
	eventpos += 2;
	head = tvass->Substring(0, eventpos);
	content = tvass->Substring(eventpos, tvass->Length - eventpos);
	tvass = "";
	//: 0,0:22:38.77,0:22:43.35
	Regex^ alltimeregex = gcnew Regex("\\r\\n[a-zA-Z]+: [0-9],[0-9]:[0-9]{2}:[0-9]{2}\\.[0-9]{2},[0-9]:[0-9]{2}:[0-9]{2}\\.[0-9]{2},");
	Regex^ headregex = gcnew Regex("\\r\\n[a-zA-Z]+: [0-9],");
	Regex^ timeregex = gcnew Regex("[0-9]:[0-9]{2}:[0-9]{2}\\.[0-9]{2}");
	MatchCollection^ alltimematch = alltimeregex->Matches(content);
	tvprogressBar->Maximum = alltimematch->Count;
	bdprogressBar->Maximum = alltimematch->Count;
	array<timec^>^ timelist = gcnew array<timec^>(alltimematch->Count);//储存时间
	std::vector<int>tvtime, bdtime;
	int rightshift = log2(FFTnum);
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
			tvtime.push_back(-1);
			bdtime.push_back(-1);
			Result->Text += "\r\n信息：第" + (i + 1).ToString() + "行为注释，将不作处理。";
			continue;
		}
		if (end == start) {
			tvtime.push_back(-1);
			bdtime.push_back(-1);
			Result->Text += "\r\n信息：第" + (i + 1).ToString() + "行时长为零，将不作处理。";
			continue;
		}
		if (end - start > maxlength * 100 / double(tvmilisec)* tvfftnum) {
			tvtime.push_back(-1);
			bdtime.push_back(-1);
			Result->Text += "\r\n警告：第" + (i + 1).ToString() + "行时长过长，将不作处理。";
			continue;
		}
		if (end >= tvfftnum) {
			tvtime.push_back(-1);
			bdtime.push_back(-1);
			Result->Text += "\r\n警告：第" + (i + 1).ToString() + "行超过音频长度，将不作处理。";
			continue;
		}
		int maxdb = -128;
		for (int j = start; j <= end; j++) {
			if (tvfftdata[0, j]->maxv() > maxdb) {
				maxdb = tvfftdata[0, j]->maxv();
			}
		}
		if (maxdb <= -128) {
			tvtime.push_back(-1);
			bdtime.push_back(-1);
			Result->Text += "\r\n警告：第" + (i + 1).ToString() + "行声音过小，将不作处理。";
			continue;
		}
		bool existed = false;
		for (int j = 0; j < i; j++) {
			if (start == tvtime[j] && end == timelist[j]->end()) {
				existed = true;
				break;
			}
		}
		if (existed) {
			tvtime.push_back(-1);
			bdtime.push_back(-1);
			continue;
		}
		tvtime.push_back(start);
		bdtime.push_back(0);
	}
	int ch = min(tvch, bdch);
	ch = min(ch, 2);
	int find0 = findfield * 100;
	int interval = tvfftnum / tvmilisec;
	if (interval < 1) {
		find0 = static_cast<int>(find0 / double(tvmilisec)*tvfftnum);
		interval = 1;
	}
	array<Int64> ^diftime = gcnew array<Int64>(2);
	int maxdelta = 0, maxrow = 0;
	for (int i = 0; i < alltimematch->Count; i++) {
		if (tvtime[i] >= 0) {
			int findstart = static_cast<int>(tvtime[i] - find0 * interval);
			int findend = static_cast<int>(tvtime[i] + find0 * interval);
			int duration = timelist[i]->end() - timelist[i]->start();
			findstart = max(0, findstart);
			findend = static_cast<int>(min(bdfftnum - duration, findend));
			int findnum = (findend - findstart) / interval;
			int tvmax = 0, tvmaxtime = 0;
			for (int j = 0; j <= duration; j++) {
				if (tvfftdata[0, j + tvtime[i]]->sum() > tvmax || j == 0) {
					tvmax = tvfftdata[0, j + tvtime[i]]->sum();
					tvmaxtime = j;
				}
			}
			std::vector<int>bdsearch;
			int smallnum = 0;
			int midnum = 0;
			for (int j = findnum; j >=0 ; j--) {
				int bdin = findstart + j * interval;
				int delta = abs(bdfftdata[0, bdin + tvmaxtime]->sum() - tvmax);
				delta >>= rightshift;
				if (delta < 5) {
					bdsearch.insert(bdsearch.begin(), bdin);
					smallnum++;
					midnum++;
				}
				else if (delta < 15) {
					bdsearch.insert(bdsearch.begin() + smallnum, bdin);
					midnum++;
				}
				else if (delta < 27)bdsearch.insert(bdsearch.begin() + midnum, bdin);
				else if (delta < 41)bdsearch.push_back(bdin);
			}
			diftime[0] = 0;
			diftime[1] = 9223372036854775807;
			List<Task^>^ tasks = gcnew List<Task^>();
			for (int j = 0; j < bdsearch.size(); j++) {
				Var^ calvar = gcnew Var(tvfftdata, bdfftdata, tvdecode->getsamprate(), tvtime[i], bdsearch[j],
					duration, ch, diftime);
				Task^ varTask = gcnew Task(gcnew Action(calvar, &Var::caldiff));
				varTask->Start();
				tasks->Add(varTask);
			}
			Task::WaitAll(tasks->ToArray());
			int delta1 = abs(bdfftdata[0, diftime[0] + tvmaxtime]->sum() - tvmax) / FFTnum;//调试用
			if (delta1 > maxdelta) {
				maxdelta = delta1;
				maxrow = i + 1;
			}
			bdtime[i] = static_cast<int>(diftime[0]);
		}
		tvprogressBar->PerformStep();
		bdprogressBar->PerformStep();
	}
	delete diftime;
	Result->Text += "\r\nmaxdelta=" + maxdelta.ToString() + "    maxrow=" + maxrow.ToString();
	for (int i = 0; i < alltimematch->Count; i++) {
		if (tvtime[i] >= 0) {
			int duration = static_cast<int>(timelist[i]->end() - timelist[i]->start());
			timelist[i]->start(bdtime[i]);
			timelist[i]->end(bdtime[i] + duration);
			if (i < alltimematch->Count - 1 && timelist[i]->end() > bdtime[i+1] && (timelist[i]->end() - bdtime[i+1]) <= interval) {
				timelist[i]->end(bdtime[i + 1]);
				Result->Text += "\r\n信息：第" + (i + 1).ToString() + "行和第" + (i + 2).ToString() + "行发生微小重叠，已自动修正。";
			}
		}
		int start = static_cast<int>(timelist[i]->start() / double(bdfftnum) * bdmilisec);
		int end = static_cast<int>(timelist[i]->end() / double(bdfftnum) * bdmilisec);
		String^ starttime = mstotime(start);
		String^ endtime = mstotime(end);
		String^ replacetext = timelist[i]->head() + starttime + "," + endtime + ",";
		content = content->Replace(alltimematch[i]->Value, replacetext);
	}
	String^ outfile = ASStext->Text->Substring(0, ASStext->Text->LastIndexOf(".")) + ".matched.ass";
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
	long endclock = clock();
	double spend = double(endclock - startclock) / (double)CLOCKS_PER_SEC;
	Result->Text += "\r\n匹配时间：" + spend.ToString() + "秒";
	return 0;
}

int BDMatch::MyForm::drawchart()
{
	using namespace System::Threading::Tasks;
	int milisec = FileSelect->SelectedIndex == 0 ? tvdraw.milisec : bddraw.milisec;
	int offset = 0;
	if (milisec < 10000) offset = 100;
	else if (milisec < 100000)offset = 250;
	else offset = 500;
	TimeRoll->TickFrequency = offset;
	int sampnum = FileSelect->SelectedIndex == 0 ? tvdraw.num : bddraw.num;
	ChartTime->Text = mstotime(TimeRoll->Value);
	int start = max(0, TimeRoll->Value - offset);
	start = int(start / float(milisec) *sampnum);
	int end = min(milisec, TimeRoll->Value + offset);
	end = int(end / float(milisec) * sampnum);
	end = min(end, sampnum - 1);
	int duration = end - start + 1;
	Bitmap^ spectrum1 = gcnew Bitmap(duration, FFTnum / 2);
	int x;
	int y;

	// Loop through the images pixels to reset color.
	for (x = 0; x < spectrum1->Width; x++)
	{
		for (y = 0; y < spectrum1->Height; y++)
		{
			int color = 0;
			if (FileSelect->SelectedIndex == 0)color = tvdraw.data[ChSelect->SelectedIndex, x + start]->read0(y);
			if (FileSelect->SelectedIndex == 1)color = bddraw.data[ChSelect->SelectedIndex, x + start]->read0(y);
			color += 128;
			color = max(0, color);
			color = min(255, color);
			Color newColor = Color::FromArgb(0, color, color);
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
	if (draw) {
		AllTablePanel->RowStyles[7]->Height = 0.4;
		AllTablePanel->RowStyles[8]->Height = 0.6;
	}
	else
	{
		AllTablePanel->RowStyles[7]->Height = 1;
		AllTablePanel->RowStyles[8]->Height = 0;
	}
	return 0;
}

void BDMatch::MyForm::setFFTnum(int num)
{
	FFTnum = num;
	return System::Void();
}
void BDMatch::MyForm::setoutpcm(bool yes)
{
	outputpcm = yes;
	return System::Void();
}
void BDMatch::MyForm::setfindfield(int num)
{
	findfield = num;
	return System::Void();
}
void BDMatch::MyForm::setmindb(int num)
{
	minfinddb = num;
	return System::Void();
}
void BDMatch::MyForm::setmaxlength(int num)
{
	maxlength = num;
	return System::Void();
}
void BDMatch::MyForm::setdraw(bool yes) {
	draw = yes;
	return System::Void();
}
void BDMatch::MyForm::setmatchass(bool yes)
{
	matchass = yes;
	return System::Void();
}
void BDMatch::MyForm::nullsetform() {
	setform = nullptr;
}

void BDMatch::MyForm::progtv()
{
	tvprogressBar->PerformStep();
	return System::Void();
}
void BDMatch::MyForm::progbd()
{
	bdprogressBar->PerformStep();
	return System::Void();
}
void BDMatch::MyForm::progtvmax(int max)
{
	tvprogressBar->Maximum = max;
	return System::Void();
}
void BDMatch::MyForm::progbdmax(int max)
{
	bdprogressBar->Maximum = max;
	return System::Void();
}

System::Void BDMatch::MyForm::MyForm_Load(System::Object ^ sender, System::EventArgs ^ e)
{
	tvprogressBar->CheckForIllegalCrossThreadCalls = false;
	bdprogressBar->CheckForIllegalCrossThreadCalls = false;
	setrows();
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
	match();
	return System::Void();
}

System::Void BDMatch::MyForm::About_Click(System::Object ^ sender, System::EventArgs ^ e)
{
	MessageBox::Show(this, "BDMatch\nVersion 0.5.0\nBy Thomasys, 2017\n\nReference:\nFFmpeg3.4.1\nFFTW3.3.7\n" +
		"Matteo Frigo and Steven G. Johnson, Proceedings of the IEEE 93 (2), 216–231 (2005). ", "关于", MessageBoxButtons::OK);
	return System::Void();
}

System::Void BDMatch::MyForm::settings_Click(System::Object ^ sender, System::EventArgs ^ e)
{
	if (!setform) {
		setform = gcnew Settings(gcnew IntCallback(this, &MyForm::setFFTnum), gcnew BoolCallback(this, &MyForm::setoutpcm),
			gcnew IntCallback(this, &MyForm::setfindfield), gcnew IntCallback(this, &MyForm::setmindb),
			gcnew IntCallback(this, &MyForm::setmaxlength), gcnew BoolCallback(this, &MyForm::setdraw),
			gcnew BoolCallback(this, &MyForm::setmatchass), gcnew NullCallback(this, &MyForm::nullsetform),
			FFTnum, outputpcm, findfield, minfinddb, maxlength, draw, matchass);
	}
	setform->Show();
	if (!setform->Focused)setform->Focus();
	return System::Void();
}

System::Void BDMatch::MyForm::FileSelect_SelectedIndexChanged(System::Object ^ sender, System::EventArgs ^ e)
{
	if (FileSelect->Enabled) {
		ChSelect->Enabled = false;
		TimeRoll->Enabled = false;
		if (FileSelect->SelectedIndex == 0) {
			if (tvdraw.ch < 2) {
				ChSelect->SelectedIndex = 0;
			}
			else ChSelect->Enabled = true;
			TimeRoll->Maximum = tvdraw.milisec;
		}
		else {
			if (bddraw.ch < 2) {
				ChSelect->SelectedIndex = 0;
			}
			else ChSelect->Enabled = true;
			TimeRoll->Maximum = bddraw.milisec;
		}
		TimeRoll->Value = 0;
		TimeRoll->Enabled = true;
		drawchart();
	}
	return System::Void();
}
System::Void BDMatch::MyForm::ChSelect_SelectedIndexChanged(System::Object ^ sender, System::EventArgs ^ e)
{
	if (ChSelect->Enabled)drawchart();
	return System::Void();
}
System::Void BDMatch::MyForm::TimeRoll_Scroll(System::Object ^ sender, System::EventArgs ^ e)
{
	if (TimeRoll->Enabled)drawchart();
	return System::Void();
}

