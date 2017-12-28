#pragma once
#include <sstream>
#include <string> 
#include <msclr\marshal_cppstd.h>
#include <stdlib.h>


namespace BDMatch {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace msclr::interop;

	/// <summary>
	/// Settings 摘要
	/// </summary>
	public delegate void IntCallback(int num);
	public delegate void BoolCallback(bool yes);
	public delegate void NullCallback();

	public ref class Settings : public System::Windows::Forms::Form
	{
	public:
		Settings(IntCallback ^ setFFTin, BoolCallback ^ setoutpcmin, IntCallback ^ setfindin, IntCallback ^ setmindbin,
			IntCallback ^ setlengthin, IntCallback ^ setminroundnum0, BoolCallback^ setdrawin, BoolCallback^ setmatchin,
			NullCallback^ nullbackin,
			int FFTnum, bool outpcmin, int findfield, int mindb, int maxlength, int minroundnum, bool draw, bool matchass);

	protected:
		/// <summary>
		/// 清理所有正在使用的资源。
		/// </summary>
		~Settings()
		{
			if (components)
			{
				delete components;
			}
		}
	private: IntCallback ^ setFFT = nullptr;
	private: BoolCallback ^ setoutpcm = nullptr;
	private: IntCallback ^ setfind = nullptr;
	private: IntCallback ^ setmindb = nullptr;
	private: IntCallback ^ setmaxlength = nullptr;
	private: IntCallback ^ setminroundnum = nullptr;
	private: BoolCallback ^ setdraw = nullptr;
	private: BoolCallback ^ setmatchass = nullptr;
	private: NullCallback ^ nullback = nullptr;

	private: System::Windows::Forms::CheckBox^  OutPCM;
	private: System::Windows::Forms::NumericUpDown^  FindSec;

	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::NumericUpDown^  MindB;

	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::NumericUpDown^  MaxLength;


	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::ComboBox^  FFTnumList;
	private: System::Windows::Forms::CheckBox^  Draw;
	private: System::Windows::Forms::CheckBox^  MatchAss;
	private: System::Windows::Forms::NumericUpDown^  MinRoundNum;

	private: System::Windows::Forms::Label^  label5;












	public:


	protected:

	private:
		/// <summary>
		/// 必需的设计器变量。
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// 设计器支持所需的方法 - 不要修改
		/// 使用代码编辑器修改此方法的内容。
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(Settings::typeid));
			this->OutPCM = (gcnew System::Windows::Forms::CheckBox());
			this->FindSec = (gcnew System::Windows::Forms::NumericUpDown());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->MindB = (gcnew System::Windows::Forms::NumericUpDown());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->MaxLength = (gcnew System::Windows::Forms::NumericUpDown());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->FFTnumList = (gcnew System::Windows::Forms::ComboBox());
			this->Draw = (gcnew System::Windows::Forms::CheckBox());
			this->MatchAss = (gcnew System::Windows::Forms::CheckBox());
			this->MinRoundNum = (gcnew System::Windows::Forms::NumericUpDown());
			this->label5 = (gcnew System::Windows::Forms::Label());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->FindSec))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->MindB))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->MaxLength))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->MinRoundNum))->BeginInit();
			this->SuspendLayout();
			// 
			// OutPCM
			// 
			this->OutPCM->AutoSize = true;
			this->OutPCM->Font = (gcnew System::Drawing::Font(L"微软雅黑", 13.8F));
			this->OutPCM->Location = System::Drawing::Point(69, 288);
			this->OutPCM->Name = L"OutPCM";
			this->OutPCM->Size = System::Drawing::Size(228, 35);
			this->OutPCM->TabIndex = 3;
			this->OutPCM->Text = L"输出解码后的音频";
			this->OutPCM->UseVisualStyleBackColor = true;
			this->OutPCM->CheckedChanged += gcnew System::EventHandler(this, &Settings::OutPCM_CheckedChanged);
			// 
			// FindSec
			// 
			this->FindSec->Font = (gcnew System::Drawing::Font(L"微软雅黑", 11.8F));
			this->FindSec->Location = System::Drawing::Point(253, 110);
			this->FindSec->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 100000, 0, 0, 0 });
			this->FindSec->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 2, 0, 0, 0 });
			this->FindSec->Name = L"FindSec";
			this->FindSec->Size = System::Drawing::Size(92, 33);
			this->FindSec->TabIndex = 4;
			this->FindSec->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 2, 0, 0, 0 });
			this->FindSec->ValueChanged += gcnew System::EventHandler(this, &Settings::FindSec_ValueChanged);
			// 
			// label2
			// 
			this->label2->BackColor = System::Drawing::Color::Transparent;
			this->label2->Font = (gcnew System::Drawing::Font(L"微软雅黑", 13.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(134)));
			this->label2->Location = System::Drawing::Point(63, 102);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(397, 44);
			this->label2->TabIndex = 5;
			this->label2->Text = L"查找范围：前后                 秒";
			this->label2->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// MindB
			// 
			this->MindB->Font = (gcnew System::Drawing::Font(L"微软雅黑", 11.8F));
			this->MindB->Location = System::Drawing::Point(253, 154);
			this->MindB->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 5, 0, 0, 0 });
			this->MindB->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 80, 0, 0, System::Int32::MinValue });
			this->MindB->Name = L"MindB";
			this->MindB->Size = System::Drawing::Size(92, 33);
			this->MindB->TabIndex = 6;
			this->MindB->ValueChanged += gcnew System::EventHandler(this, &Settings::MindB_ValueChanged);
			// 
			// label1
			// 
			this->label1->BackColor = System::Drawing::Color::Transparent;
			this->label1->Font = (gcnew System::Drawing::Font(L"微软雅黑", 13.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(134)));
			this->label1->Location = System::Drawing::Point(63, 146);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(397, 44);
			this->label1->TabIndex = 7;
			this->label1->Text = L"最小匹配响度：                 dB";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// MaxLength
			// 
			this->MaxLength->Font = (gcnew System::Drawing::Font(L"微软雅黑", 11.8F));
			this->MaxLength->Location = System::Drawing::Point(253, 198);
			this->MaxLength->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1000, 0, 0, 0 });
			this->MaxLength->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 20, 0, 0, 0 });
			this->MaxLength->Name = L"MaxLength";
			this->MaxLength->Size = System::Drawing::Size(92, 33);
			this->MaxLength->TabIndex = 8;
			this->MaxLength->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 30, 0, 0, 0 });
			this->MaxLength->ValueChanged += gcnew System::EventHandler(this, &Settings::MaxLength_ValueChanged);
			// 
			// label3
			// 
			this->label3->BackColor = System::Drawing::Color::Transparent;
			this->label3->Font = (gcnew System::Drawing::Font(L"微软雅黑", 13.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(134)));
			this->label3->Location = System::Drawing::Point(63, 190);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(397, 44);
			this->label3->TabIndex = 9;
			this->label3->Text = L"最大查找句长：                 秒";
			this->label3->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label4
			// 
			this->label4->BackColor = System::Drawing::Color::Transparent;
			this->label4->Font = (gcnew System::Drawing::Font(L"微软雅黑", 13.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(134)));
			this->label4->Location = System::Drawing::Point(63, 52);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(397, 44);
			this->label4->TabIndex = 10;
			this->label4->Text = L"FFT个数：";
			this->label4->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// FFTnumList
			// 
			this->FFTnumList->Font = (gcnew System::Drawing::Font(L"微软雅黑", 11.8F));
			this->FFTnumList->FormattingEnabled = true;
			this->FFTnumList->Items->AddRange(gcnew cli::array< System::Object^  >(9) {
				L"64", L"128", L"256", L"512", L"1024", L"2048",
					L"4096", L"8192", L"16384"
			});
			this->FFTnumList->Location = System::Drawing::Point(186, 59);
			this->FFTnumList->Name = L"FFTnumList";
			this->FFTnumList->Size = System::Drawing::Size(197, 35);
			this->FFTnumList->TabIndex = 11;
			this->FFTnumList->SelectedIndexChanged += gcnew System::EventHandler(this, &Settings::FFTnumList_SelectedIndexChanged);
			// 
			// Draw
			// 
			this->Draw->AutoSize = true;
			this->Draw->Font = (gcnew System::Drawing::Font(L"微软雅黑", 13.8F));
			this->Draw->Location = System::Drawing::Point(69, 327);
			this->Draw->Name = L"Draw";
			this->Draw->Size = System::Drawing::Size(132, 35);
			this->Draw->TabIndex = 12;
			this->Draw->Text = L"作声谱图";
			this->Draw->UseVisualStyleBackColor = true;
			this->Draw->CheckedChanged += gcnew System::EventHandler(this, &Settings::Draw_CheckedChanged);
			// 
			// MatchAss
			// 
			this->MatchAss->AutoSize = true;
			this->MatchAss->Font = (gcnew System::Drawing::Font(L"微软雅黑", 13.8F));
			this->MatchAss->Location = System::Drawing::Point(69, 367);
			this->MatchAss->Name = L"MatchAss";
			this->MatchAss->Size = System::Drawing::Size(204, 35);
			this->MatchAss->TabIndex = 13;
			this->MatchAss->Text = L"对字幕进行匹配";
			this->MatchAss->UseVisualStyleBackColor = true;
			this->MatchAss->CheckedChanged += gcnew System::EventHandler(this, &Settings::MatchAss_CheckedChanged);
			// 
			// MinRoundNum
			// 
			this->MinRoundNum->Font = (gcnew System::Drawing::Font(L"微软雅黑", 11.8F));
			this->MinRoundNum->Location = System::Drawing::Point(253, 242);
			this->MinRoundNum->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 100000, 0, 0, 0 });
			this->MinRoundNum->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 10, 0, 0, 0 });
			this->MinRoundNum->Name = L"MinRoundNum";
			this->MinRoundNum->Size = System::Drawing::Size(92, 33);
			this->MinRoundNum->TabIndex = 14;
			this->MinRoundNum->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 30, 0, 0, 0 });
			this->MinRoundNum->ValueChanged += gcnew System::EventHandler(this, &Settings::MinRoundNum_ValueChanged);
			// 
			// label5
			// 
			this->label5->BackColor = System::Drawing::Color::Transparent;
			this->label5->Font = (gcnew System::Drawing::Font(L"微软雅黑", 13.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(134)));
			this->label5->Location = System::Drawing::Point(63, 234);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(397, 44);
			this->label5->TabIndex = 15;
			this->label5->Text = L"选点确认次数：";
			this->label5->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// Settings
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 15);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(481, 442);
			this->Controls->Add(this->MinRoundNum);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->MatchAss);
			this->Controls->Add(this->Draw);
			this->Controls->Add(this->FFTnumList);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->MaxLength);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->MindB);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->FindSec);
			this->Controls->Add(this->OutPCM);
			this->Controls->Add(this->label2);
			this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"Settings";
			this->Text = L"Settings";
			this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &Settings::Settings_FormClosed);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->FindSec))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->MindB))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->MaxLength))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->MinRoundNum))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

	private: System::Void OutPCM_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void FindSec_ValueChanged(System::Object^  sender, System::EventArgs^  e);

	private: System::Void FFTnumList_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void MindB_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void MaxLength_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void MinRoundNum_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void Draw_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void MatchAss_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void Settings_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e);
};
}
