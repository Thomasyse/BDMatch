#include "Settings.h"

BDMatch::Settings::Settings(IntCallback ^ setFFTin, BoolCallback ^ setoutpcmin, IntCallback ^ setfindin, IntCallback ^ setmindbin,
	IntCallback ^ setlengthin,	BoolCallback^ setdrawin, BoolCallback^ setmatchin, NullCallback^ nullbackin,
	int FFTnum, bool outpcmin, int findfield, int mindb, int varnum, bool draw, bool matchass)
{
	InitializeComponent();
	//
	//TODO:  �ڴ˴���ӹ��캯������
	//
	setFFT = setFFTin;
	setoutpcm = setoutpcmin;
	setfind = setfindin;
	setmindb = setmindbin;
	setmaxlength = setlengthin;
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
	MaxLength->Value = varnum;
	Draw->Checked = draw;
	MatchAss->Checked = matchass;
}

System::Void BDMatch::Settings::OutPCM_CheckedChanged(System::Object ^ sender, System::EventArgs ^ e)
{
	setoutpcm->Invoke(OutPCM->Checked);
	return System::Void();
}

System::Void BDMatch::Settings::FindSec_ValueChanged(System::Object ^ sender, System::EventArgs ^ e)
{
	int findfield = (int)FindSec->Value;
	if (findfield> 100) {
		MessageBox::Show(this, "���ҷ�Χ�����ʹƥ���ٶȻ����������ѡ��", "BDMatch", MessageBoxButtons::OK);
	}
	setfind->Invoke(findfield);
	return System::Void();
}

System::Void BDMatch::Settings::FFTnumList_SelectedIndexChanged(System::Object ^ sender, System::EventArgs ^ e)
{
	int FFTnum = int::Parse(FFTnumList->GetItemText(FFTnumList->SelectedItem));
	if (FFTnum> 4096) {
		MessageBox::Show(this, "FFT���������ʹƥ���ٶȻ�����׼ȷ�Ȳ�һ���������������ѡ��", "BDMatch", MessageBoxButtons::OK);
	}
	setFFT->Invoke(FFTnum);
	return System::Void();
}

System::Void BDMatch::Settings::MindB_ValueChanged(System::Object ^ sender, System::EventArgs ^ e)
{
	int mindb = (int)MindB->Value;
	if (mindb > 5) {
		MessageBox::Show(this, "��Сƥ����ȹ����ʹ�ܶ�����Ļ�������ƥ�䣬�����ѡ��", "BDMatch", MessageBoxButtons::OK);
	}
	setmindb->Invoke(mindb);
	return System::Void();
}

System::Void BDMatch::Settings::MaxLength_ValueChanged(System::Object ^ sender, System::EventArgs ^ e)
{
	int maxlength = (int)MaxLength->Value;
	if (maxlength > 30) {
		MessageBox::Show(this, "�����Ҿ䳤������ʹƥ���ٶȻ����������ѡ��", "BDMatch", MessageBoxButtons::OK);
	}
	setmaxlength->Invoke(maxlength);
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

