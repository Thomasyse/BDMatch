#pragma once
#include "datastruct1.h"

namespace BDMatch {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace DataStruct1;

	/// <summary>
	/// Settings 摘要
	/// </summary>
	public delegate void SettingCallback(SettingType type, int val);
	public delegate void NullCallback();
	
	public ref class Settings : public System::Windows::Forms::Form
	{
	public:
		Settings(SettingCallback^ setbackin, NullCallback^ nullbackin, SettingVals ^ settingin);

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
	private: SettingVals ^ setting = nullptr;
	private: SettingCallback ^ setback = nullptr;
	private: NullCallback ^ nullback = nullptr;
	private: System::Windows::Forms::CheckBox^  OutPCM;



	private: System::Windows::Forms::NumericUpDown^  FindSec;

	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::NumericUpDown^  MindB;

	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::NumericUpDown^  MaxLengthSet;



	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::ComboBox^  FFTnumList;
	private: System::Windows::Forms::CheckBox^  DrawSet;
	private: System::Windows::Forms::CheckBox^  MatchAssSet;


	private: System::Windows::Forms::NumericUpDown^  MinCheckNumSet;


	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::CheckBox^  ParaDecode;
	private: System::Windows::Forms::TabControl^  tabControl1;
	private: System::Windows::Forms::TabPage^  DecodeTabPage;
	private: System::Windows::Forms::TabPage^  MatchTabPage;
	private: System::Windows::Forms::TabPage^  SpectrumTabPage;




	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::Label^  label7;
	private: System::Windows::Forms::Label^  label8;
	private: System::Windows::Forms::Label^  label9;
	private: System::Windows::Forms::Label^  label10;
	private: System::Windows::Forms::Label^  label11;
	private: System::Windows::Forms::Label^  label12;
	private: System::Windows::Forms::Label^  label13;
	private: System::Windows::Forms::Label^  label14;
	private: System::Windows::Forms::TableLayoutPanel^  DecodeLayoutPanel;

	private: System::Windows::Forms::Label^  label15;
	private: System::Windows::Forms::TableLayoutPanel^  MatchLayoutPanel;


	private: System::Windows::Forms::Label^  label16;
	private: System::Windows::Forms::Label^  label17;
	private: System::Windows::Forms::TableLayoutPanel^  SpectrumLayoutPanel;
	private: System::Windows::Forms::CheckBox^  FastMatchSet;
	private: System::Windows::Forms::Label^  label18;
	private: System::Windows::Forms::Label^  label19;
	private: System::Windows::Forms::NumericUpDown^  AssOffsetSet;



	private: System::Windows::Forms::Label^  label20;
	private: System::Windows::Forms::Label^  label21;
	private: System::Windows::Forms::CheckBox^  VolMatchBox;


	private: System::Windows::Forms::Label^  label22;






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
			this->MaxLengthSet = (gcnew System::Windows::Forms::NumericUpDown());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->FFTnumList = (gcnew System::Windows::Forms::ComboBox());
			this->DrawSet = (gcnew System::Windows::Forms::CheckBox());
			this->MatchAssSet = (gcnew System::Windows::Forms::CheckBox());
			this->MinCheckNumSet = (gcnew System::Windows::Forms::NumericUpDown());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->ParaDecode = (gcnew System::Windows::Forms::CheckBox());
			this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
			this->MatchTabPage = (gcnew System::Windows::Forms::TabPage());
			this->MatchLayoutPanel = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->label21 = (gcnew System::Windows::Forms::Label());
			this->label20 = (gcnew System::Windows::Forms::Label());
			this->AssOffsetSet = (gcnew System::Windows::Forms::NumericUpDown());
			this->label19 = (gcnew System::Windows::Forms::Label());
			this->label18 = (gcnew System::Windows::Forms::Label());
			this->FastMatchSet = (gcnew System::Windows::Forms::CheckBox());
			this->label17 = (gcnew System::Windows::Forms::Label());
			this->label14 = (gcnew System::Windows::Forms::Label());
			this->label16 = (gcnew System::Windows::Forms::Label());
			this->label13 = (gcnew System::Windows::Forms::Label());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->label11 = (gcnew System::Windows::Forms::Label());
			this->DecodeTabPage = (gcnew System::Windows::Forms::TabPage());
			this->DecodeLayoutPanel = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->VolMatchBox = (gcnew System::Windows::Forms::CheckBox());
			this->label22 = (gcnew System::Windows::Forms::Label());
			this->label15 = (gcnew System::Windows::Forms::Label());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->SpectrumTabPage = (gcnew System::Windows::Forms::TabPage());
			this->SpectrumLayoutPanel = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->label12 = (gcnew System::Windows::Forms::Label());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->FindSec))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->MindB))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->MaxLengthSet))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->MinCheckNumSet))->BeginInit();
			this->tabControl1->SuspendLayout();
			this->MatchTabPage->SuspendLayout();
			this->MatchLayoutPanel->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->AssOffsetSet))->BeginInit();
			this->DecodeTabPage->SuspendLayout();
			this->DecodeLayoutPanel->SuspendLayout();
			this->SpectrumTabPage->SuspendLayout();
			this->SpectrumLayoutPanel->SuspendLayout();
			this->SuspendLayout();
			// 
			// OutPCM
			// 
			this->OutPCM->AutoSize = true;
			this->DecodeLayoutPanel->SetColumnSpan(this->OutPCM, 3);
			this->OutPCM->Dock = System::Windows::Forms::DockStyle::Left;
			this->OutPCM->Font = (gcnew System::Drawing::Font(L"微软雅黑", 13.8F));
			this->OutPCM->Location = System::Drawing::Point(13, 231);
			this->OutPCM->Margin = System::Windows::Forms::Padding(7, 3, 3, 3);
			this->OutPCM->Name = L"OutPCM";
			this->OutPCM->Size = System::Drawing::Size(228, 35);
			this->OutPCM->TabIndex = 3;
			this->OutPCM->Text = L"输出解码后的音频";
			this->OutPCM->UseVisualStyleBackColor = true;
			this->OutPCM->CheckedChanged += gcnew System::EventHandler(this, &Settings::OutPCM_CheckedChanged);
			// 
			// FindSec
			// 
			this->FindSec->Dock = System::Windows::Forms::DockStyle::Left;
			this->FindSec->Font = (gcnew System::Drawing::Font(L"微软雅黑", 11.8F));
			this->FindSec->Location = System::Drawing::Point(213, 94);
			this->FindSec->Margin = System::Windows::Forms::Padding(3, 7, 3, 3);
			this->FindSec->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 100000, 0, 0, 0 });
			this->FindSec->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
			this->FindSec->Name = L"FindSec";
			this->FindSec->Size = System::Drawing::Size(92, 33);
			this->FindSec->TabIndex = 4;
			this->FindSec->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 2, 0, 0, 0 });
			this->FindSec->ValueChanged += gcnew System::EventHandler(this, &Settings::FindSec_ValueChanged);
			// 
			// label2
			// 
			this->label2->BackColor = System::Drawing::Color::Transparent;
			this->label2->Dock = System::Windows::Forms::DockStyle::Fill;
			this->label2->Font = (gcnew System::Drawing::Font(L"微软雅黑", 13.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(134)));
			this->label2->Location = System::Drawing::Point(9, 87);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(198, 44);
			this->label2->TabIndex = 5;
			this->label2->Text = L"查找范围：前后";
			this->label2->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// MindB
			// 
			this->MindB->Dock = System::Windows::Forms::DockStyle::Left;
			this->MindB->Font = (gcnew System::Drawing::Font(L"微软雅黑", 11.8F));
			this->MindB->Location = System::Drawing::Point(153, 149);
			this->MindB->Margin = System::Windows::Forms::Padding(3, 7, 3, 3);
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
			this->label1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->label1->Font = (gcnew System::Drawing::Font(L"微软雅黑", 13.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(134)));
			this->label1->Location = System::Drawing::Point(9, 142);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(138, 44);
			this->label1->TabIndex = 7;
			this->label1->Text = L"最小响度：";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// MaxLengthSet
			// 
			this->MaxLengthSet->Dock = System::Windows::Forms::DockStyle::Left;
			this->MaxLengthSet->Font = (gcnew System::Drawing::Font(L"微软雅黑", 11.8F));
			this->MaxLengthSet->Location = System::Drawing::Point(213, 205);
			this->MaxLengthSet->Margin = System::Windows::Forms::Padding(3, 7, 3, 3);
			this->MaxLengthSet->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1000, 0, 0, 0 });
			this->MaxLengthSet->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 20, 0, 0, 0 });
			this->MaxLengthSet->Name = L"MaxLengthSet";
			this->MaxLengthSet->Size = System::Drawing::Size(92, 33);
			this->MaxLengthSet->TabIndex = 8;
			this->MaxLengthSet->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 30, 0, 0, 0 });
			this->MaxLengthSet->ValueChanged += gcnew System::EventHandler(this, &Settings::MaxLengthSet_ValueChanged);
			// 
			// label3
			// 
			this->label3->BackColor = System::Drawing::Color::Transparent;
			this->label3->Dock = System::Windows::Forms::DockStyle::Fill;
			this->label3->Font = (gcnew System::Drawing::Font(L"微软雅黑", 13.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(134)));
			this->label3->Location = System::Drawing::Point(9, 198);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(198, 44);
			this->label3->TabIndex = 9;
			this->label3->Text = L"最大查找句长：";
			this->label3->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label4
			// 
			this->label4->BackColor = System::Drawing::Color::Transparent;
			this->label4->Dock = System::Windows::Forms::DockStyle::Fill;
			this->label4->Font = (gcnew System::Drawing::Font(L"微软雅黑", 13.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(134)));
			this->label4->Location = System::Drawing::Point(9, 6);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(138, 44);
			this->label4->TabIndex = 10;
			this->label4->Text = L"FFT个数：";
			this->label4->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// FFTnumList
			// 
			this->DecodeLayoutPanel->SetColumnSpan(this->FFTnumList, 2);
			this->FFTnumList->Dock = System::Windows::Forms::DockStyle::Left;
			this->FFTnumList->Font = (gcnew System::Drawing::Font(L"微软雅黑", 11.8F));
			this->FFTnumList->FormattingEnabled = true;
			this->FFTnumList->Items->AddRange(gcnew cli::array< System::Object^  >(9) {
				L"64", L"128", L"256", L"512", L"1024", L"2048",
					L"4096", L"8192", L"16384"
			});
			this->FFTnumList->Location = System::Drawing::Point(153, 10);
			this->FFTnumList->Margin = System::Windows::Forms::Padding(3, 4, 3, 3);
			this->FFTnumList->Name = L"FFTnumList";
			this->FFTnumList->Size = System::Drawing::Size(197, 35);
			this->FFTnumList->TabIndex = 11;
			this->FFTnumList->SelectedIndexChanged += gcnew System::EventHandler(this, &Settings::FFTnumList_SelectedIndexChanged);
			// 
			// DrawSet
			// 
			this->DrawSet->AutoSize = true;
			this->SpectrumLayoutPanel->SetColumnSpan(this->DrawSet, 2);
			this->DrawSet->Dock = System::Windows::Forms::DockStyle::Left;
			this->DrawSet->Font = (gcnew System::Drawing::Font(L"微软雅黑", 13.8F));
			this->DrawSet->Location = System::Drawing::Point(16, 9);
			this->DrawSet->Margin = System::Windows::Forms::Padding(10, 3, 3, 3);
			this->DrawSet->Name = L"DrawSet";
			this->DrawSet->Size = System::Drawing::Size(132, 35);
			this->DrawSet->TabIndex = 12;
			this->DrawSet->Text = L"作声谱图";
			this->DrawSet->UseVisualStyleBackColor = true;
			this->DrawSet->CheckedChanged += gcnew System::EventHandler(this, &Settings::DrawSet_CheckedChanged);
			// 
			// MatchAssSet
			// 
			this->MatchAssSet->AutoSize = true;
			this->MatchLayoutPanel->SetColumnSpan(this->MatchAssSet, 3);
			this->MatchAssSet->Dock = System::Windows::Forms::DockStyle::Left;
			this->MatchAssSet->Font = (gcnew System::Drawing::Font(L"微软雅黑", 13.8F));
			this->MatchAssSet->Location = System::Drawing::Point(13, 9);
			this->MatchAssSet->Margin = System::Windows::Forms::Padding(7, 3, 3, 3);
			this->MatchAssSet->Name = L"MatchAssSet";
			this->MatchAssSet->Size = System::Drawing::Size(204, 35);
			this->MatchAssSet->TabIndex = 13;
			this->MatchAssSet->Text = L"对字幕进行匹配";
			this->MatchAssSet->UseVisualStyleBackColor = true;
			this->MatchAssSet->CheckedChanged += gcnew System::EventHandler(this, &Settings::MatchAssSet_CheckedChanged);
			// 
			// MinCheckNumSet
			// 
			this->MinCheckNumSet->Dock = System::Windows::Forms::DockStyle::Left;
			this->MinCheckNumSet->Font = (gcnew System::Drawing::Font(L"微软雅黑", 11.8F));
			this->MinCheckNumSet->Location = System::Drawing::Point(213, 291);
			this->MinCheckNumSet->Margin = System::Windows::Forms::Padding(3, 7, 3, 3);
			this->MinCheckNumSet->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1000000, 0, 0, 0 });
			this->MinCheckNumSet->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 10, 0, 0, 0 });
			this->MinCheckNumSet->Name = L"MinCheckNumSet";
			this->MinCheckNumSet->Size = System::Drawing::Size(92, 33);
			this->MinCheckNumSet->TabIndex = 14;
			this->MinCheckNumSet->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 30, 0, 0, 0 });
			this->MinCheckNumSet->ValueChanged += gcnew System::EventHandler(this, &Settings::MinCheckNumSet_ValueChanged);
			// 
			// label5
			// 
			this->label5->BackColor = System::Drawing::Color::Transparent;
			this->label5->Dock = System::Windows::Forms::DockStyle::Fill;
			this->label5->Font = (gcnew System::Drawing::Font(L"微软雅黑", 13.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(134)));
			this->label5->Location = System::Drawing::Point(9, 284);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(198, 44);
			this->label5->TabIndex = 15;
			this->label5->Text = L"选点确认次数：";
			this->label5->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// ParaDecode
			// 
			this->ParaDecode->AutoSize = true;
			this->DecodeLayoutPanel->SetColumnSpan(this->ParaDecode, 3);
			this->ParaDecode->Dock = System::Windows::Forms::DockStyle::Left;
			this->ParaDecode->Font = (gcnew System::Drawing::Font(L"微软雅黑", 13.8F));
			this->ParaDecode->Location = System::Drawing::Point(13, 337);
			this->ParaDecode->Margin = System::Windows::Forms::Padding(7, 3, 3, 3);
			this->ParaDecode->Name = L"ParaDecode";
			this->ParaDecode->Size = System::Drawing::Size(132, 35);
			this->ParaDecode->TabIndex = 16;
			this->ParaDecode->Text = L"并行解码";
			this->ParaDecode->UseVisualStyleBackColor = true;
			this->ParaDecode->CheckedChanged += gcnew System::EventHandler(this, &Settings::ParaDecode_CheckedChanged);
			// 
			// tabControl1
			// 
			this->tabControl1->Controls->Add(this->MatchTabPage);
			this->tabControl1->Controls->Add(this->DecodeTabPage);
			this->tabControl1->Controls->Add(this->SpectrumTabPage);
			this->tabControl1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tabControl1->Font = (gcnew System::Drawing::Font(L"微软雅黑", 13));
			this->tabControl1->Location = System::Drawing::Point(8, 5);
			this->tabControl1->Name = L"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(872, 562);
			this->tabControl1->TabIndex = 17;
			// 
			// MatchTabPage
			// 
			this->MatchTabPage->Controls->Add(this->MatchLayoutPanel);
			this->MatchTabPage->Font = (gcnew System::Drawing::Font(L"微软雅黑", 12));
			this->MatchTabPage->Location = System::Drawing::Point(4, 39);
			this->MatchTabPage->Name = L"MatchTabPage";
			this->MatchTabPage->Size = System::Drawing::Size(864, 519);
			this->MatchTabPage->TabIndex = 1;
			this->MatchTabPage->Text = L"匹配设置";
			this->MatchTabPage->UseVisualStyleBackColor = true;
			// 
			// MatchLayoutPanel
			// 
			this->MatchLayoutPanel->AutoScroll = true;
			this->MatchLayoutPanel->AutoSize = true;
			this->MatchLayoutPanel->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
			this->MatchLayoutPanel->ColumnCount = 3;
			this->MatchLayoutPanel->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Absolute,
				204)));
			this->MatchLayoutPanel->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Absolute,
				99)));
			this->MatchLayoutPanel->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				100)));
			this->MatchLayoutPanel->Controls->Add(this->label21, 0, 9);
			this->MatchLayoutPanel->Controls->Add(this->label20, 2, 8);
			this->MatchLayoutPanel->Controls->Add(this->AssOffsetSet, 1, 8);
			this->MatchLayoutPanel->Controls->Add(this->label19, 0, 8);
			this->MatchLayoutPanel->Controls->Add(this->label18, 0, 11);
			this->MatchLayoutPanel->Controls->Add(this->FastMatchSet, 0, 10);
			this->MatchLayoutPanel->Controls->Add(this->label17, 2, 4);
			this->MatchLayoutPanel->Controls->Add(this->label14, 0, 7);
			this->MatchLayoutPanel->Controls->Add(this->label16, 2, 2);
			this->MatchLayoutPanel->Controls->Add(this->MinCheckNumSet, 1, 6);
			this->MatchLayoutPanel->Controls->Add(this->label13, 0, 5);
			this->MatchLayoutPanel->Controls->Add(this->label5, 0, 6);
			this->MatchLayoutPanel->Controls->Add(this->MatchAssSet, 0, 0);
			this->MatchLayoutPanel->Controls->Add(this->label10, 0, 1);
			this->MatchLayoutPanel->Controls->Add(this->label11, 0, 3);
			this->MatchLayoutPanel->Controls->Add(this->label2, 0, 2);
			this->MatchLayoutPanel->Controls->Add(this->MaxLengthSet, 1, 4);
			this->MatchLayoutPanel->Controls->Add(this->FindSec, 1, 2);
			this->MatchLayoutPanel->Controls->Add(this->label3, 0, 4);
			this->MatchLayoutPanel->Dock = System::Windows::Forms::DockStyle::Fill;
			this->MatchLayoutPanel->GrowStyle = System::Windows::Forms::TableLayoutPanelGrowStyle::FixedSize;
			this->MatchLayoutPanel->Location = System::Drawing::Point(0, 0);
			this->MatchLayoutPanel->Name = L"MatchLayoutPanel";
			this->MatchLayoutPanel->Padding = System::Windows::Forms::Padding(6, 6, 30, 6);
			this->MatchLayoutPanel->RowCount = 12;
			this->MatchLayoutPanel->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
			this->MatchLayoutPanel->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
			this->MatchLayoutPanel->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
			this->MatchLayoutPanel->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
			this->MatchLayoutPanel->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
			this->MatchLayoutPanel->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
			this->MatchLayoutPanel->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
			this->MatchLayoutPanel->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
			this->MatchLayoutPanel->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
			this->MatchLayoutPanel->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
			this->MatchLayoutPanel->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
			this->MatchLayoutPanel->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
			this->MatchLayoutPanel->Size = System::Drawing::Size(864, 519);
			this->MatchLayoutPanel->TabIndex = 25;
			// 
			// label21
			// 
			this->label21->AutoEllipsis = true;
			this->label21->AutoSize = true;
			this->MatchLayoutPanel->SetColumnSpan(this->label21, 3);
			this->label21->Dock = System::Windows::Forms::DockStyle::Fill;
			this->label21->Font = (gcnew System::Drawing::Font(L"微软雅黑", 11));
			this->label21->Location = System::Drawing::Point(51, 446);
			this->label21->Margin = System::Windows::Forms::Padding(45, 7, 3, 10);
			this->label21->Name = L"label21";
			this->label21->Size = System::Drawing::Size(759, 25);
			this->label21->TabIndex = 30;
			this->label21->Text = L"将ASS文件中的时间轴进行一定时间偏置之后再进行匹配。";
			// 
			// label20
			// 
			this->label20->BackColor = System::Drawing::Color::Transparent;
			this->label20->Dock = System::Windows::Forms::DockStyle::Fill;
			this->label20->Font = (gcnew System::Drawing::Font(L"微软雅黑", 13.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(134)));
			this->label20->Location = System::Drawing::Point(312, 395);
			this->label20->Name = L"label20";
			this->label20->Size = System::Drawing::Size(498, 44);
			this->label20->TabIndex = 29;
			this->label20->Text = L"厘秒";
			this->label20->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// AssOffsetSet
			// 
			this->AssOffsetSet->Dock = System::Windows::Forms::DockStyle::Left;
			this->AssOffsetSet->Font = (gcnew System::Drawing::Font(L"微软雅黑", 11.8F));
			this->AssOffsetSet->Location = System::Drawing::Point(213, 402);
			this->AssOffsetSet->Margin = System::Windows::Forms::Padding(3, 7, 3, 3);
			this->AssOffsetSet->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 10000000, 0, 0, 0 });
			this->AssOffsetSet->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 10000000, 0, 0, System::Int32::MinValue });
			this->AssOffsetSet->Name = L"AssOffsetSet";
			this->AssOffsetSet->Size = System::Drawing::Size(92, 33);
			this->AssOffsetSet->TabIndex = 28;
			this->AssOffsetSet->ValueChanged += gcnew System::EventHandler(this, &Settings::AssOffsetSet_ValueChanged);
			// 
			// label19
			// 
			this->label19->BackColor = System::Drawing::Color::Transparent;
			this->label19->Dock = System::Windows::Forms::DockStyle::Fill;
			this->label19->Font = (gcnew System::Drawing::Font(L"微软雅黑", 13.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(134)));
			this->label19->Location = System::Drawing::Point(9, 395);
			this->label19->Name = L"label19";
			this->label19->Size = System::Drawing::Size(198, 44);
			this->label19->TabIndex = 27;
			this->label19->Text = L"ASS偏置：延后";
			this->label19->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label18
			// 
			this->label18->AutoEllipsis = true;
			this->label18->AutoSize = true;
			this->MatchLayoutPanel->SetColumnSpan(this->label18, 3);
			this->label18->Dock = System::Windows::Forms::DockStyle::Fill;
			this->label18->Font = (gcnew System::Drawing::Font(L"微软雅黑", 11));
			this->label18->Location = System::Drawing::Point(51, 529);
			this->label18->Margin = System::Windows::Forms::Padding(45, 7, 3, 10);
			this->label18->Name = L"label18";
			this->label18->Size = System::Drawing::Size(759, 50);
			this->label18->TabIndex = 26;
			this->label18->Text = L"只匹配少许时间轴，其余时间轴根据匹配结果直接进行调整。由于BD文件可能会做出一些修改，匹配结果可能会不准确。";
			// 
			// FastMatchSet
			// 
			this->FastMatchSet->AutoSize = true;
			this->MatchLayoutPanel->SetColumnSpan(this->FastMatchSet, 3);
			this->FastMatchSet->Dock = System::Windows::Forms::DockStyle::Left;
			this->FastMatchSet->Font = (gcnew System::Drawing::Font(L"微软雅黑", 13.8F));
			this->FastMatchSet->Location = System::Drawing::Point(13, 484);
			this->FastMatchSet->Margin = System::Windows::Forms::Padding(7, 3, 3, 3);
			this->FastMatchSet->Name = L"FastMatchSet";
			this->FastMatchSet->Size = System::Drawing::Size(132, 35);
			this->FastMatchSet->TabIndex = 25;
			this->FastMatchSet->Text = L"快速匹配";
			this->FastMatchSet->UseVisualStyleBackColor = true;
			this->FastMatchSet->CheckedChanged += gcnew System::EventHandler(this, &Settings::FastMatchSet_CheckedChanged);
			// 
			// label17
			// 
			this->label17->BackColor = System::Drawing::Color::Transparent;
			this->label17->Dock = System::Windows::Forms::DockStyle::Fill;
			this->label17->Font = (gcnew System::Drawing::Font(L"微软雅黑", 13.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(134)));
			this->label17->Location = System::Drawing::Point(312, 198);
			this->label17->Name = L"label17";
			this->label17->Size = System::Drawing::Size(498, 44);
			this->label17->TabIndex = 23;
			this->label17->Text = L"秒";
			this->label17->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label14
			// 
			this->label14->AutoEllipsis = true;
			this->label14->AutoSize = true;
			this->MatchLayoutPanel->SetColumnSpan(this->label14, 3);
			this->label14->Dock = System::Windows::Forms::DockStyle::Fill;
			this->label14->Font = (gcnew System::Drawing::Font(L"微软雅黑", 11));
			this->label14->Location = System::Drawing::Point(51, 335);
			this->label14->Margin = System::Windows::Forms::Padding(45, 7, 3, 10);
			this->label14->Name = L"label14";
			this->label14->Size = System::Drawing::Size(759, 50);
			this->label14->TabIndex = 24;
			this->label14->Text = L"对搜索到的暂优选点进行确认，确认足够次数后即会取作最佳选点。过小的确认次数会导致匹配不精确，过大的确认次数会加长匹配时间。";
			// 
			// label16
			// 
			this->label16->BackColor = System::Drawing::Color::Transparent;
			this->label16->Dock = System::Windows::Forms::DockStyle::Fill;
			this->label16->Font = (gcnew System::Drawing::Font(L"微软雅黑", 13.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(134)));
			this->label16->Location = System::Drawing::Point(312, 87);
			this->label16->Name = L"label16";
			this->label16->Size = System::Drawing::Size(498, 44);
			this->label16->TabIndex = 22;
			this->label16->Text = L"秒";
			this->label16->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label13
			// 
			this->label13->AutoEllipsis = true;
			this->label13->AutoSize = true;
			this->MatchLayoutPanel->SetColumnSpan(this->label13, 3);
			this->label13->Dock = System::Windows::Forms::DockStyle::Fill;
			this->label13->Font = (gcnew System::Drawing::Font(L"微软雅黑", 11));
			this->label13->Location = System::Drawing::Point(51, 249);
			this->label13->Margin = System::Windows::Forms::Padding(45, 7, 3, 10);
			this->label13->Name = L"label13";
			this->label13->Size = System::Drawing::Size(759, 25);
			this->label13->TabIndex = 23;
			this->label13->Text = L"超过最大查找句长的时间轴将不会进行匹配，以防止过长的空行影响匹配速度。";
			// 
			// label10
			// 
			this->label10->AutoEllipsis = true;
			this->label10->AutoSize = true;
			this->MatchLayoutPanel->SetColumnSpan(this->label10, 3);
			this->label10->Dock = System::Windows::Forms::DockStyle::Fill;
			this->label10->Font = (gcnew System::Drawing::Font(L"微软雅黑", 11));
			this->label10->Location = System::Drawing::Point(51, 52);
			this->label10->Margin = System::Windows::Forms::Padding(45, 5, 3, 10);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(759, 25);
			this->label10->TabIndex = 21;
			this->label10->Text = L"取消勾选则会只进行音频解码，不会进行匹配。";
			// 
			// label11
			// 
			this->label11->AutoEllipsis = true;
			this->label11->AutoSize = true;
			this->MatchLayoutPanel->SetColumnSpan(this->label11, 3);
			this->label11->Dock = System::Windows::Forms::DockStyle::Fill;
			this->label11->Font = (gcnew System::Drawing::Font(L"微软雅黑", 11));
			this->label11->Location = System::Drawing::Point(51, 138);
			this->label11->Margin = System::Windows::Forms::Padding(45, 7, 3, 10);
			this->label11->Name = L"label11";
			this->label11->Size = System::Drawing::Size(759, 50);
			this->label11->TabIndex = 22;
			this->label11->Text = L"决定在ASS时间轴多大时间范围内查找最佳匹配时间。查找范围增加会加长匹配时间，过小的查找范围会导致无法匹配到正确的时间。";
			// 
			// DecodeTabPage
			// 
			this->DecodeTabPage->Controls->Add(this->DecodeLayoutPanel);
			this->DecodeTabPage->Font = (gcnew System::Drawing::Font(L"微软雅黑", 12));
			this->DecodeTabPage->Location = System::Drawing::Point(4, 39);
			this->DecodeTabPage->Name = L"DecodeTabPage";
			this->DecodeTabPage->Size = System::Drawing::Size(864, 519);
			this->DecodeTabPage->TabIndex = 0;
			this->DecodeTabPage->Text = L"解码设置";
			this->DecodeTabPage->UseVisualStyleBackColor = true;
			// 
			// DecodeLayoutPanel
			// 
			this->DecodeLayoutPanel->AutoScroll = true;
			this->DecodeLayoutPanel->AutoSize = true;
			this->DecodeLayoutPanel->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
			this->DecodeLayoutPanel->ColumnCount = 3;
			this->DecodeLayoutPanel->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Absolute,
				144)));
			this->DecodeLayoutPanel->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Absolute,
				99)));
			this->DecodeLayoutPanel->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				100)));
			this->DecodeLayoutPanel->Controls->Add(this->VolMatchBox, 0, 8);
			this->DecodeLayoutPanel->Controls->Add(this->label22, 0, 9);
			this->DecodeLayoutPanel->Controls->Add(this->label15, 2, 2);
			this->DecodeLayoutPanel->Controls->Add(this->label8, 0, 5);
			this->DecodeLayoutPanel->Controls->Add(this->label7, 0, 3);
			this->DecodeLayoutPanel->Controls->Add(this->label6, 0, 1);
			this->DecodeLayoutPanel->Controls->Add(this->OutPCM, 0, 4);
			this->DecodeLayoutPanel->Controls->Add(this->ParaDecode, 0, 6);
			this->DecodeLayoutPanel->Controls->Add(this->label1, 0, 2);
			this->DecodeLayoutPanel->Controls->Add(this->MindB, 1, 2);
			this->DecodeLayoutPanel->Controls->Add(this->label9, 0, 7);
			this->DecodeLayoutPanel->Controls->Add(this->label4, 0, 0);
			this->DecodeLayoutPanel->Controls->Add(this->FFTnumList, 1, 0);
			this->DecodeLayoutPanel->Dock = System::Windows::Forms::DockStyle::Fill;
			this->DecodeLayoutPanel->GrowStyle = System::Windows::Forms::TableLayoutPanelGrowStyle::FixedSize;
			this->DecodeLayoutPanel->Location = System::Drawing::Point(0, 0);
			this->DecodeLayoutPanel->Name = L"DecodeLayoutPanel";
			this->DecodeLayoutPanel->Padding = System::Windows::Forms::Padding(6, 6, 30, 6);
			this->DecodeLayoutPanel->RowCount = 10;
			this->DecodeLayoutPanel->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
			this->DecodeLayoutPanel->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
			this->DecodeLayoutPanel->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
			this->DecodeLayoutPanel->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
			this->DecodeLayoutPanel->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
			this->DecodeLayoutPanel->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
			this->DecodeLayoutPanel->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
			this->DecodeLayoutPanel->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
			this->DecodeLayoutPanel->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
			this->DecodeLayoutPanel->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
			this->DecodeLayoutPanel->Size = System::Drawing::Size(864, 519);
			this->DecodeLayoutPanel->TabIndex = 22;
			// 
			// VolMatchBox
			// 
			this->VolMatchBox->AutoSize = true;
			this->DecodeLayoutPanel->SetColumnSpan(this->VolMatchBox, 3);
			this->VolMatchBox->Dock = System::Windows::Forms::DockStyle::Left;
			this->VolMatchBox->Font = (gcnew System::Drawing::Font(L"微软雅黑", 13.8F));
			this->VolMatchBox->Location = System::Drawing::Point(13, 443);
			this->VolMatchBox->Margin = System::Windows::Forms::Padding(7, 3, 3, 3);
			this->VolMatchBox->Name = L"VolMatchBox";
			this->VolMatchBox->Size = System::Drawing::Size(132, 35);
			this->VolMatchBox->TabIndex = 22;
			this->VolMatchBox->Text = L"响度补偿";
			this->VolMatchBox->UseVisualStyleBackColor = true;
			this->VolMatchBox->CheckedChanged += gcnew System::EventHandler(this, &Settings::VolMatchBox_CheckedChanged);
			// 
			// label22
			// 
			this->label22->AutoEllipsis = true;
			this->label22->AutoSize = true;
			this->DecodeLayoutPanel->SetColumnSpan(this->label22, 3);
			this->label22->Dock = System::Windows::Forms::DockStyle::Fill;
			this->label22->Font = (gcnew System::Drawing::Font(L"微软雅黑", 11));
			this->label22->Location = System::Drawing::Point(51, 486);
			this->label22->Margin = System::Windows::Forms::Padding(45, 5, 3, 10);
			this->label22->Name = L"label22";
			this->label22->Size = System::Drawing::Size(780, 25);
			this->label22->TabIndex = 23;
			this->label22->Text = L"对BD音频响度进行补偿，使BD音频的响度与TV音频一致，此时会对BD音频进行两次解码。";
			// 
			// label15
			// 
			this->label15->BackColor = System::Drawing::Color::Transparent;
			this->label15->Dock = System::Windows::Forms::DockStyle::Fill;
			this->label15->Font = (gcnew System::Drawing::Font(L"微软雅黑", 13.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(134)));
			this->label15->Location = System::Drawing::Point(252, 142);
			this->label15->Name = L"label15";
			this->label15->Size = System::Drawing::Size(579, 44);
			this->label15->TabIndex = 21;
			this->label15->Text = L"dB";
			this->label15->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label8
			// 
			this->label8->AutoEllipsis = true;
			this->label8->AutoSize = true;
			this->DecodeLayoutPanel->SetColumnSpan(this->label8, 3);
			this->label8->Dock = System::Windows::Forms::DockStyle::Fill;
			this->label8->Font = (gcnew System::Drawing::Font(L"微软雅黑", 11));
			this->label8->Location = System::Drawing::Point(51, 274);
			this->label8->Margin = System::Windows::Forms::Padding(45, 5, 3, 10);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(780, 50);
			this->label8->TabIndex = 19;
			this->label8->Text = L"生成WAV格式文件，用以检查解码是否正常工作。重采样生成的音频会有噪声，但不会影响匹配。";
			// 
			// label7
			// 
			this->label7->AutoEllipsis = true;
			this->label7->AutoSize = true;
			this->DecodeLayoutPanel->SetColumnSpan(this->label7, 3);
			this->label7->Dock = System::Windows::Forms::DockStyle::Fill;
			this->label7->Font = (gcnew System::Drawing::Font(L"微软雅黑", 11));
			this->label7->Location = System::Drawing::Point(51, 193);
			this->label7->Margin = System::Windows::Forms::Padding(45, 7, 3, 10);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(780, 25);
			this->label7->TabIndex = 18;
			this->label7->Text = L"最小响度以下的噪声会被过滤，以保证解码生成的声谱不被噪声干扰。";
			// 
			// label6
			// 
			this->label6->AutoEllipsis = true;
			this->label6->AutoSize = true;
			this->DecodeLayoutPanel->SetColumnSpan(this->label6, 3);
			this->label6->Dock = System::Windows::Forms::DockStyle::Fill;
			this->label6->Font = (gcnew System::Drawing::Font(L"微软雅黑", 11));
			this->label6->Location = System::Drawing::Point(51, 57);
			this->label6->Margin = System::Windows::Forms::Padding(45, 7, 3, 10);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(780, 75);
			this->label6->TabIndex = 17;
			this->label6->Text = L"进行一次快速傅里叶变换（Fast Fourier Transform）所输入的采样点个数。请设置为接近采样率百分之一的数值。FFT个数过小会导致解码和匹配速度缓慢"
				L"，FFT个数过大会导致匹配精度下降。";
			// 
			// label9
			// 
			this->label9->AutoEllipsis = true;
			this->label9->AutoSize = true;
			this->DecodeLayoutPanel->SetColumnSpan(this->label9, 3);
			this->label9->Dock = System::Windows::Forms::DockStyle::Fill;
			this->label9->Font = (gcnew System::Drawing::Font(L"微软雅黑", 11));
			this->label9->Location = System::Drawing::Point(51, 380);
			this->label9->Margin = System::Windows::Forms::Padding(45, 5, 3, 10);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(780, 50);
			this->label9->TabIndex = 20;
			this->label9->Text = L"同时进行TV和BD文件的解码，请在拥有高性能硬盘的设备上开启此选项，否则可能会导致在硬盘负载较大时解码速度急剧下降。";
			// 
			// SpectrumTabPage
			// 
			this->SpectrumTabPage->Controls->Add(this->SpectrumLayoutPanel);
			this->SpectrumTabPage->Font = (gcnew System::Drawing::Font(L"微软雅黑", 12));
			this->SpectrumTabPage->Location = System::Drawing::Point(4, 39);
			this->SpectrumTabPage->Name = L"SpectrumTabPage";
			this->SpectrumTabPage->Size = System::Drawing::Size(864, 519);
			this->SpectrumTabPage->TabIndex = 2;
			this->SpectrumTabPage->Text = L"声谱设置";
			this->SpectrumTabPage->UseVisualStyleBackColor = true;
			// 
			// SpectrumLayoutPanel
			// 
			this->SpectrumLayoutPanel->AutoScroll = true;
			this->SpectrumLayoutPanel->AutoSize = true;
			this->SpectrumLayoutPanel->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
			this->SpectrumLayoutPanel->ColumnCount = 2;
			this->SpectrumLayoutPanel->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Absolute,
				150)));
			this->SpectrumLayoutPanel->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				100)));
			this->SpectrumLayoutPanel->Controls->Add(this->DrawSet, 0, 0);
			this->SpectrumLayoutPanel->Controls->Add(this->label12, 0, 1);
			this->SpectrumLayoutPanel->Dock = System::Windows::Forms::DockStyle::Fill;
			this->SpectrumLayoutPanel->Location = System::Drawing::Point(0, 0);
			this->SpectrumLayoutPanel->Name = L"SpectrumLayoutPanel";
			this->SpectrumLayoutPanel->Padding = System::Windows::Forms::Padding(6, 6, 30, 6);
			this->SpectrumLayoutPanel->RowCount = 2;
			this->SpectrumLayoutPanel->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
			this->SpectrumLayoutPanel->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
			this->SpectrumLayoutPanel->Size = System::Drawing::Size(864, 519);
			this->SpectrumLayoutPanel->TabIndex = 23;
			// 
			// label12
			// 
			this->label12->AutoEllipsis = true;
			this->label12->AutoSize = true;
			this->SpectrumLayoutPanel->SetColumnSpan(this->label12, 2);
			this->label12->Dock = System::Windows::Forms::DockStyle::Fill;
			this->label12->Font = (gcnew System::Drawing::Font(L"微软雅黑", 11));
			this->label12->Location = System::Drawing::Point(51, 52);
			this->label12->Margin = System::Windows::Forms::Padding(45, 5, 3, 10);
			this->label12->Name = L"label12";
			this->label12->Size = System::Drawing::Size(780, 451);
			this->label12->TabIndex = 22;
			this->label12->Text = L"打开此选项可以查看解码得到的声谱图，并对原字幕时间轴和匹配生成的时间轴对应的声谱进行对比。";
			// 
			// Settings
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 15);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(885, 572);
			this->Controls->Add(this->tabControl1);
			this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"Settings";
			this->Padding = System::Windows::Forms::Padding(8, 5, 5, 5);
			this->Text = L"Settings";
			this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &Settings::Settings_FormClosed);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->FindSec))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->MindB))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->MaxLengthSet))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->MinCheckNumSet))->EndInit();
			this->tabControl1->ResumeLayout(false);
			this->MatchTabPage->ResumeLayout(false);
			this->MatchTabPage->PerformLayout();
			this->MatchLayoutPanel->ResumeLayout(false);
			this->MatchLayoutPanel->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->AssOffsetSet))->EndInit();
			this->DecodeTabPage->ResumeLayout(false);
			this->DecodeTabPage->PerformLayout();
			this->DecodeLayoutPanel->ResumeLayout(false);
			this->DecodeLayoutPanel->PerformLayout();
			this->SpectrumTabPage->ResumeLayout(false);
			this->SpectrumTabPage->PerformLayout();
			this->SpectrumLayoutPanel->ResumeLayout(false);
			this->SpectrumLayoutPanel->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion

	private: System::Void Settings_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e);

	private: System::Void OutPCM_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void FindSec_ValueChanged(System::Object^  sender, System::EventArgs^  e);

	private: System::Void FFTnumList_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void MindB_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void MaxLengthSet_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void MinCheckNumSet_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void DrawSet_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void MatchAssSet_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void ParaDecode_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void VolMatchBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void FastMatchSet_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void AssOffsetSet_ValueChanged(System::Object^  sender, System::EventArgs^  e);
};

}
