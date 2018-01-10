#include "Settings.h"
using namespace DataStruct;

BDMatch::Settings::Settings(SettingIntCallback^ setintbackin, SettingBoolCallback^ setboolbackin,
	NullCallback^ nullbackin, SettingVals ^ settingin)
{
	InitializeComponent();
	//
	//TODO:  在此处添加构造函数代码
	//
	setting = settingin;
	setintback = setintbackin;
	setboolback = setboolbackin;
	nullback = nullbackin;
	for (int i = 0; i < FFTnumList->Items->Count; i++) {
		if (int::Parse(FFTnumList->GetItemText(FFTnumList->Items[i])) == setting->FFTnum) {
			FFTnumList->SelectedIndex = i;
		}
	}
	OutPCM->Checked = setting->outputpcm;
	FindSec->Value = setting->findfield;
	MindB->Value = setting->minfinddb;
	MaxLengthSet->Value = setting->maxlength;
	DrawSet->Checked = setting->draw;
	MatchAssSet->Checked = setting->matchass;
	MinCheckNumSet->Value = setting->minchecknum;
	ParaDecode->Checked = setting->paralleldecode;
	FastMatchSet->Checked = setting->fastmatch;
}

System::Void BDMatch::Settings::Settings_FormClosed(System::Object ^ sender, System::Windows::Forms::FormClosedEventArgs ^ e)
{
	nullback();
	return System::Void();
}


System::Void BDMatch::Settings::OutPCM_CheckedChanged(System::Object ^ sender, System::EventArgs ^ e)
{
	if (this->Visible)setboolback->Invoke(OutputPCM, OutPCM->Checked);
	return System::Void();
}

System::Void BDMatch::Settings::FindSec_ValueChanged(System::Object ^ sender, System::EventArgs ^ e)
{
	if (this->Visible) {
		int findfield = static_cast<int>(FindSec->Value);
		if (findfield > 180) {
			MessageBox::Show(this, "查找范围过大会使匹配速度缓慢，请谨慎选择！", "BDMatch", MessageBoxButtons::OK);
		}
		setintback->Invoke(FindField, findfield);
	}
	return System::Void();
}

System::Void BDMatch::Settings::FFTnumList_SelectedIndexChanged(System::Object ^ sender, System::EventArgs ^ e)
{
	if (this->Visible) {
		int FFTnum = int::Parse(FFTnumList->GetItemText(FFTnumList->SelectedItem));
		if (FFTnum > 2048) {
			MessageBox::Show(this, "FFT个数过大会使匹配准确度下降，请谨慎选择！", "BDMatch", MessageBoxButtons::OK);
		}
		setintback->Invoke(FFTNum, FFTnum);
	}
	return System::Void();
}

System::Void BDMatch::Settings::MindB_ValueChanged(System::Object ^ sender, System::EventArgs ^ e)
{
	if (this->Visible) {
		int mindb = static_cast<int>(MindB->Value);
		if (mindb > 5) {
			MessageBox::Show(this, "最小响度过大会使很多时间轴不会进行匹配，请谨慎选择！", "BDMatch", MessageBoxButtons::OK);
		}
		setintback->Invoke(MinFindDb, mindb);
	}
	return System::Void();
}

System::Void BDMatch::Settings::MaxLengthSet_ValueChanged(System::Object ^ sender, System::EventArgs ^ e)
{
	if (this->Visible) {
		int maxlength = static_cast<int>(MaxLengthSet->Value);
		if (maxlength > 30) {
			MessageBox::Show(this, "最大查找句长过长会使匹配速度缓慢，请谨慎选择！", "BDMatch", MessageBoxButtons::OK);
		}
		setintback->Invoke(MaxLength, maxlength);
	}
	return System::Void();
}

System::Void BDMatch::Settings::MinCheckNumSet_ValueChanged(System::Object ^ sender, System::EventArgs ^ e)
{
	if (this->Visible) {
		setintback->Invoke(MinCheckNum, static_cast<int>(MinCheckNumSet->Value));
	}
	return System::Void();
}

System::Void BDMatch::Settings::DrawSet_CheckedChanged(System::Object ^ sender, System::EventArgs ^ e)
{
	if (this->Visible) {
		setboolback->Invoke(Draw, DrawSet->Checked);
	}
	return System::Void();
}

System::Void BDMatch::Settings::MatchAssSet_CheckedChanged(System::Object ^ sender, System::EventArgs ^ e)
{
	if (this->Visible) {
		setboolback->Invoke(MatchAss, MatchAssSet->Checked);
	}
	return System::Void();
}

System::Void BDMatch::Settings::ParaDecode_CheckedChanged(System::Object ^ sender, System::EventArgs ^ e)
{
	if (this->Visible) {
		setboolback->Invoke(ParallelDecode, ParaDecode->Checked);
	}
	return System::Void();
}

System::Void BDMatch::Settings::FastMatchSet_CheckedChanged(System::Object ^ sender, System::EventArgs ^ e)
{
	if (this->Visible) {
		setboolback->Invoke(FastMatch, FastMatchSet->Checked);
	}
	return System::Void();
}



