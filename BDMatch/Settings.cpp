#include "Settings.h"
using namespace DataStruct1;

BDMatch::Settings::Settings(SettingCallback^ setbackin, NullCallback^ nullbackin, SettingVals ^ settingin)
{
	InitializeComponent();
	//
	//TODO:  �ڴ˴���ӹ��캯������
	//
	setting = settingin;
	setback = setbackin;
	nullback = nullbackin;
	for (int i = 0; i < FFTnumList->Items->Count; i++) {
		if (int::Parse(FFTnumList->GetItemText(FFTnumList->Items[i])) == setting->fft_num) {
			FFTnumList->SelectedIndex = i;
		}
	}
	OutPCM->Checked = setting->output_pcm;
	FindSec->Value = setting->find_field;
	MindB->Value = setting->min_find_db;
	MaxLengthSet->Value = setting->max_length;
	AssOffsetSet->Value = setting->ass_offset;
	DrawSet->Checked = setting->draw;
	MatchAssSet->Checked = setting->match_ass;
	MinCheckNumSet->Value = setting->min_check_num;
	ParaDecode->Checked = setting->parallel_decode;
	FastMatchSet->Checked = setting->fast_match;
	VolMatchBox->Checked = setting->vol_match;
}

System::Void BDMatch::Settings::Settings_FormClosed(System::Object ^ sender, System::Windows::Forms::FormClosedEventArgs ^ e)
{
	nullback();
	return System::Void();
}


System::Void BDMatch::Settings::OutPCM_CheckedChanged(System::Object ^ sender, System::EventArgs ^ e)
{
	if (this->Visible)setback->Invoke(OutputPCM, static_cast<int>(OutPCM->Checked));
	return System::Void();
}

System::Void BDMatch::Settings::FindSec_ValueChanged(System::Object ^ sender, System::EventArgs ^ e)
{
	if (this->Visible) {
		int findfield = static_cast<int>(FindSec->Value);
		if (findfield > 600) {
			MessageBox::Show(this, "���ҷ�Χ�����ʹƥ���ٶȻ����������ѡ��", "BDMatch", MessageBoxButtons::OK);
		}
		setback->Invoke(FindField, findfield);
	}
	return System::Void();
}

System::Void BDMatch::Settings::FFTnumList_SelectedIndexChanged(System::Object ^ sender, System::EventArgs ^ e)
{
	if (this->Visible) {
		int FFTnum = int::Parse(FFTnumList->GetItemText(FFTnumList->SelectedItem));
		if (FFTnum > 2048) {
			MessageBox::Show(this, "FFT���������ʹƥ��׼ȷ���½��������ѡ��", "BDMatch", MessageBoxButtons::OK);
		}
		setback->Invoke(FFTNum, FFTnum);
	}
	return System::Void();
}

System::Void BDMatch::Settings::MindB_ValueChanged(System::Object ^ sender, System::EventArgs ^ e)
{
	if (this->Visible) {
		int mindb = static_cast<int>(MindB->Value);
		if (mindb > -5) {
			MessageBox::Show(this, "��С��ȹ����ʹ�ܶ�ʱ���᲻�����ƥ�䣬�����ѡ��", "BDMatch", MessageBoxButtons::OK);
		}
		setback->Invoke(MinFinddB, mindb);
	}
	return System::Void();
}

System::Void BDMatch::Settings::MaxLengthSet_ValueChanged(System::Object ^ sender, System::EventArgs ^ e)
{
	if (this->Visible) {
		int maxlength = static_cast<int>(MaxLengthSet->Value);
		if (maxlength > 30) {
			MessageBox::Show(this, "�����Ҿ䳤������ʹƥ���ٶȻ����������ѡ��", "BDMatch", MessageBoxButtons::OK);
		}
		setback->Invoke(MaxLength, maxlength);
	}
	return System::Void();
}

System::Void BDMatch::Settings::MinCheckNumSet_ValueChanged(System::Object ^ sender, System::EventArgs ^ e)
{
	if (this->Visible) {
		setback->Invoke(MinCheckNum, static_cast<int>(MinCheckNumSet->Value));
	}
	return System::Void();
}

System::Void BDMatch::Settings::DrawSet_CheckedChanged(System::Object ^ sender, System::EventArgs ^ e)
{
	if (this->Visible) {
		setback->Invoke(Draw, static_cast<int>(DrawSet->Checked));
	}
	return System::Void();
}

System::Void BDMatch::Settings::MatchAssSet_CheckedChanged(System::Object ^ sender, System::EventArgs ^ e)
{
	if (this->Visible) {
		setback->Invoke(MatchAss, static_cast<int>(MatchAssSet->Checked));
	}
	return System::Void();
}

System::Void BDMatch::Settings::ParaDecode_CheckedChanged(System::Object ^ sender, System::EventArgs ^ e)
{
	if (this->Visible) {
		setback->Invoke(ParallelDecode, static_cast<int>(ParaDecode->Checked));
	}
	return System::Void();
}

System::Void BDMatch::Settings::VolMatchBox_CheckedChanged(System::Object ^ sender, System::EventArgs ^ e)
{
	if (this->Visible) {
		setback->Invoke(VolMatch, static_cast<int>(VolMatchBox->Checked));
	}
	return System::Void();
}

System::Void BDMatch::Settings::FastMatchSet_CheckedChanged(System::Object ^ sender, System::EventArgs ^ e)
{
	if (this->Visible) {
		setback->Invoke(FastMatch, static_cast<int>(FastMatchSet->Checked));
	}
	return System::Void();
}

System::Void BDMatch::Settings::AssOffsetSet_ValueChanged(System::Object ^ sender, System::EventArgs ^ e)
{
	if (this->Visible) {
		setback->Invoke(AssOffset, static_cast<int>(AssOffsetSet->Value));
	}
	return System::Void();
}



