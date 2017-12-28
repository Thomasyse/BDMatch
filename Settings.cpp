#include "Settings.h"

BDMatch::Settings::Settings(IntCallback ^ setFFTin, BoolCallback ^ setoutpcmin, IntCallback ^ setfindin, IntCallback ^ setmindbin,
	IntCallback ^ setlengthin, IntCallback ^ setminroundnum0, BoolCallback^ setdrawin, BoolCallback^ setmatchin, NullCallback^ nullbackin,
	int FFTnum, bool outpcmin, int findfield, int mindb, int maxlength, int minroundnum, bool draw, bool matchass)
{
	InitializeComponent();
	//
	//TODO:  在此处添加构造函数代码
	//
	setFFT = setFFTin;
	setoutpcm = setoutpcmin;
	setfind = setfindin;
	setmindb = setmindbin;
	setmaxlength = setlengthin;
	setminroundnum = setminroundnum0;
	setdraw = setdrawin;
	setmatchass = setmatchin;
	nullback = nullbackin;
	for (int i = 0; i < FFTnumList->Items->Count; i++) {
		if (int::Parse(FFTnumList->GetItemText(FFTnumList->Items[i])) == FFTnum) {
			FFTnumList->SelectedIndex = i;
		}
	}
	OutPCM->Checked = outpcmin;
	FindSec->Value = findfield;
	MindB->Value = mindb;
	MaxLength->Value = maxlength;
	Draw->Checked = draw;
	MatchAss->Checked = matchass;
	MinRoundNum->Value = minroundnum;
}

System::Void BDMatch::Settings::OutPCM_CheckedChanged(System::Object ^ sender, System::EventArgs ^ e)
{
	setoutpcm->Invoke(OutPCM->Checked);
	return System::Void();
}

System::Void BDMatch::Settings::FindSec_ValueChanged(System::Object ^ sender, System::EventArgs ^ e)
{
	int findfield = static_cast<int>(FindSec->Value);
	if (findfield> 180) {
		MessageBox::Show(this, "查找范围过大会使匹配速度缓慢，请谨慎选择！", "BDMatch", MessageBoxButtons::OK);
	}
	setfind->Invoke(findfield);
	return System::Void();
}

System::Void BDMatch::Settings::FFTnumList_SelectedIndexChanged(System::Object ^ sender, System::EventArgs ^ e)
{
	int FFTnum = int::Parse(FFTnumList->GetItemText(FFTnumList->SelectedItem));
	if (FFTnum> 2048) {
		MessageBox::Show(this, "FFT个数过大会使匹配准确度下降，请谨慎选择！", "BDMatch", MessageBoxButtons::OK);
	}
	setFFT->Invoke(FFTnum);
	return System::Void();
}

System::Void BDMatch::Settings::MindB_ValueChanged(System::Object ^ sender, System::EventArgs ^ e)
{
	int mindb = static_cast<int>(MindB->Value);
	if (mindb > 5) {
		MessageBox::Show(this, "最小匹配响度过大会使很多行字幕不会进行匹配，请谨慎选择！", "BDMatch", MessageBoxButtons::OK);
	}
	setmindb->Invoke(mindb);
	return System::Void();
}

System::Void BDMatch::Settings::MaxLength_ValueChanged(System::Object ^ sender, System::EventArgs ^ e)
{
	int maxlength = static_cast<int>(MaxLength->Value);
	if (maxlength > 30) {
		MessageBox::Show(this, "最大查找句长过长会使匹配速度缓慢，请谨慎选择！", "BDMatch", MessageBoxButtons::OK);
	}
	setmaxlength->Invoke(maxlength);
	return System::Void();
}

System::Void BDMatch::Settings::MinRoundNum_ValueChanged(System::Object ^ sender, System::EventArgs ^ e)
{
	setminroundnum->Invoke(static_cast<int>(MinRoundNum->Value));
	return System::Void();
}

System::Void BDMatch::Settings::Draw_CheckedChanged(System::Object ^ sender, System::EventArgs ^ e)
{
	setdraw->Invoke(Draw->Checked);
	return System::Void();
}

System::Void BDMatch::Settings::MatchAss_CheckedChanged(System::Object ^ sender, System::EventArgs ^ e)
{
	setmatchass->Invoke(MatchAss->Checked);
	return System::Void();
}

System::Void BDMatch::Settings::Settings_FormClosed(System::Object ^ sender, System::Windows::Forms::FormClosedEventArgs ^ e)
{
	nullback();
	return System::Void();
}

