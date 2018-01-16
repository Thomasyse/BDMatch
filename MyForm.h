#pragma once
#include "datastruct.h"
#include "Settings.h"
#include "DecodePara.h"

namespace BDMatch {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace DataStruct;

	/// <summary>
	/// MyForm ժҪ
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
		{
			InitializeComponent();
			//
			//TODO:  �ڴ˴���ӹ��캯������
			//
		}

	protected:
		/// <summary>
		/// ������������ʹ�õ���Դ��
		/// </summary>
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: ref struct drawpara {
		int num = 0;
		int ch = 0;
		int milisec = 0;
		int linenum = 0;
		std::vector<std::vector<node*>>* data = nullptr;
		array<int, 2>^ timelist = nullptr;
	};
	private: 
		SettingVals ^ Setting = gcnew SettingVals;
		drawpara tvdraw, bddraw;
		Settings ^ setform = nullptr;
		unsigned int matchcount = 0, finishedmatch = 0;
		System::Threading::CancellationTokenSource^ CancelSource;

	private: System::Windows::Forms::Button^ Match;
	private: System::Windows::Forms::Button^ TVfind;
	private: System::Windows::Forms::Button^ BDfind;
	private: System::Windows::Forms::Button^ ASSfind;
	private: System::Windows::Forms::ComboBox^ TVtext;
	private: System::Windows::Forms::ComboBox^ BDtext;
	private: System::Windows::Forms::ComboBox^ ASStext;
	private: System::Windows::Forms::TextBox^ Result;
	private: System::Windows::Forms::ProgressBar^  TotalProgress;

	private: System::Windows::Forms::ProgressBar^  SingleProgress;





	private: System::Windows::Forms::Label^ label1;
	private: System::Windows::Forms::Label^ label2;
	private: System::Windows::Forms::Label^ label3;


	private: System::Windows::Forms::Label^ About;
	private: System::Windows::Forms::Label^ settings;


	private: System::Windows::Forms::FolderBrowserDialog^ Folderbrowse;
	private: System::Windows::Forms::OpenFileDialog^ Filebrowse;
	private: System::Windows::Forms::TableLayoutPanel^  AllTablePanel;



	private: System::Windows::Forms::SplitContainer^  splitContainer1;
	private: System::Windows::Forms::TrackBar^  TimeRoll;
	private: System::Windows::Forms::PictureBox^  Spectrum;
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel2;
	private: System::Windows::Forms::Label^  ChartTime;
	private: System::Windows::Forms::ComboBox^  ChSelect;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::ComboBox^  ViewSel;
	private: System::Windows::Forms::NumericUpDown^  LineSel;





	private:
		/// <summary>
		/// ����������������
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// �����֧������ķ��� - ��Ҫ�޸�
		/// ʹ�ô���༭���޸Ĵ˷��������ݡ�
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(MyForm::typeid));
			this->AllTablePanel = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->TotalProgress = (gcnew System::Windows::Forms::ProgressBar());
			this->settings = (gcnew System::Windows::Forms::Label());
			this->ASStext = (gcnew System::Windows::Forms::ComboBox());
			this->BDfind = (gcnew System::Windows::Forms::Button());
			this->TVfind = (gcnew System::Windows::Forms::Button());
			this->About = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->ASSfind = (gcnew System::Windows::Forms::Button());
			this->Match = (gcnew System::Windows::Forms::Button());
			this->TVtext = (gcnew System::Windows::Forms::ComboBox());
			this->BDtext = (gcnew System::Windows::Forms::ComboBox());
			this->Result = (gcnew System::Windows::Forms::TextBox());
			this->SingleProgress = (gcnew System::Windows::Forms::ProgressBar());
			this->splitContainer1 = (gcnew System::Windows::Forms::SplitContainer());
			this->TimeRoll = (gcnew System::Windows::Forms::TrackBar());
			this->Spectrum = (gcnew System::Windows::Forms::PictureBox());
			this->tableLayoutPanel2 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->ViewSel = (gcnew System::Windows::Forms::ComboBox());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->ChartTime = (gcnew System::Windows::Forms::Label());
			this->ChSelect = (gcnew System::Windows::Forms::ComboBox());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->LineSel = (gcnew System::Windows::Forms::NumericUpDown());
			this->Folderbrowse = (gcnew System::Windows::Forms::FolderBrowserDialog());
			this->Filebrowse = (gcnew System::Windows::Forms::OpenFileDialog());
			this->AllTablePanel->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->splitContainer1))->BeginInit();
			this->splitContainer1->Panel1->SuspendLayout();
			this->splitContainer1->Panel2->SuspendLayout();
			this->splitContainer1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->TimeRoll))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->Spectrum))->BeginInit();
			this->tableLayoutPanel2->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->LineSel))->BeginInit();
			this->SuspendLayout();
			// 
			// AllTablePanel
			// 
			this->AllTablePanel->ColumnCount = 4;
			this->AllTablePanel->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Absolute,
				100)));
			this->AllTablePanel->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Absolute,
				35)));
			this->AllTablePanel->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				100)));
			this->AllTablePanel->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Absolute,
				75)));
			this->AllTablePanel->Controls->Add(this->TotalProgress, 0, 6);
			this->AllTablePanel->Controls->Add(this->settings, 0, 4);
			this->AllTablePanel->Controls->Add(this->ASStext, 1, 1);
			this->AllTablePanel->Controls->Add(this->BDfind, 3, 3);
			this->AllTablePanel->Controls->Add(this->TVfind, 3, 2);
			this->AllTablePanel->Controls->Add(this->About, 3, 4);
			this->AllTablePanel->Controls->Add(this->label3, 0, 3);
			this->AllTablePanel->Controls->Add(this->label2, 0, 2);
			this->AllTablePanel->Controls->Add(this->label1, 0, 1);
			this->AllTablePanel->Controls->Add(this->ASSfind, 3, 1);
			this->AllTablePanel->Controls->Add(this->Match, 2, 4);
			this->AllTablePanel->Controls->Add(this->TVtext, 1, 2);
			this->AllTablePanel->Controls->Add(this->BDtext, 1, 3);
			this->AllTablePanel->Controls->Add(this->Result, 0, 7);
			this->AllTablePanel->Controls->Add(this->SingleProgress, 0, 5);
			this->AllTablePanel->Controls->Add(this->splitContainer1, 2, 8);
			this->AllTablePanel->Controls->Add(this->tableLayoutPanel2, 0, 8);
			this->AllTablePanel->Dock = System::Windows::Forms::DockStyle::Fill;
			this->AllTablePanel->Location = System::Drawing::Point(0, 0);
			this->AllTablePanel->Name = L"AllTablePanel";
			this->AllTablePanel->RowCount = 9;
			this->AllTablePanel->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 20)));
			this->AllTablePanel->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 44)));
			this->AllTablePanel->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 54)));
			this->AllTablePanel->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 54)));
			this->AllTablePanel->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 64)));
			this->AllTablePanel->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 40)));
			this->AllTablePanel->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 40)));
			this->AllTablePanel->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 40)));
			this->AllTablePanel->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 60)));
			this->AllTablePanel->Size = System::Drawing::Size(1081, 803);
			this->AllTablePanel->TabIndex = 4;
			// 
			// TotalProgress
			// 
			this->AllTablePanel->SetColumnSpan(this->TotalProgress, 4);
			this->TotalProgress->Dock = System::Windows::Forms::DockStyle::Fill;
			this->TotalProgress->Location = System::Drawing::Point(5, 279);
			this->TotalProgress->Margin = System::Windows::Forms::Padding(5, 3, 5, 3);
			this->TotalProgress->Maximum = 200;
			this->TotalProgress->Name = L"TotalProgress";
			this->TotalProgress->Size = System::Drawing::Size(1071, 34);
			this->TotalProgress->Step = 1;
			this->TotalProgress->Style = System::Windows::Forms::ProgressBarStyle::Continuous;
			this->TotalProgress->TabIndex = 18;
			// 
			// settings
			// 
			this->settings->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->settings->AutoSize = true;
			this->settings->BackColor = System::Drawing::Color::Transparent;
			this->settings->Font = (gcnew System::Drawing::Font(L"΢���ź�", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(134)));
			this->settings->Location = System::Drawing::Point(3, 209);
			this->settings->Margin = System::Windows::Forms::Padding(3, 7, 5, 7);
			this->settings->Name = L"settings";
			this->settings->Size = System::Drawing::Size(39, 20);
			this->settings->TabIndex = 16;
			this->settings->Text = L"����";
			this->settings->TextAlign = System::Drawing::ContentAlignment::BottomLeft;
			this->settings->Click += gcnew System::EventHandler(this, &MyForm::settings_Click);
			this->settings->MouseEnter += gcnew System::EventHandler(this, &MyForm::settings_MouseEnter);
			this->settings->MouseLeave += gcnew System::EventHandler(this, &MyForm::settings_MouseLeave);
			// 
			// ASStext
			// 
			this->ASStext->AllowDrop = true;
			this->AllTablePanel->SetColumnSpan(this->ASStext, 2);
			this->ASStext->Dock = System::Windows::Forms::DockStyle::Fill;
			this->ASStext->Font = (gcnew System::Drawing::Font(L"΢���ź�", 10.8F));
			this->ASStext->FormattingEnabled = true;
			this->ASStext->Location = System::Drawing::Point(103, 25);
			this->ASStext->Margin = System::Windows::Forms::Padding(3, 5, 0, 0);
			this->ASStext->MaxDropDownItems = 10;
			this->ASStext->Name = L"ASStext";
			this->ASStext->Size = System::Drawing::Size(903, 32);
			this->ASStext->TabIndex = 13;
			this->ASStext->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &MyForm::ASStext_DragDrop);
			this->ASStext->DragEnter += gcnew System::Windows::Forms::DragEventHandler(this, &MyForm::ASStext_DragEnter);
			// 
			// BDfind
			// 
			this->BDfind->Dock = System::Windows::Forms::DockStyle::Left;
			this->BDfind->Font = (gcnew System::Drawing::Font(L"΢���ź�", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(134)));
			this->BDfind->Location = System::Drawing::Point(1006, 133);
			this->BDfind->Margin = System::Windows::Forms::Padding(0, 15, 18, 1);
			this->BDfind->Name = L"BDfind";
			this->BDfind->Size = System::Drawing::Size(38, 38);
			this->BDfind->TabIndex = 11;
			this->BDfind->Text = L"��";
			this->BDfind->UseVisualStyleBackColor = true;
			this->BDfind->Click += gcnew System::EventHandler(this, &MyForm::BDfind_Click);
			// 
			// TVfind
			// 
			this->TVfind->Dock = System::Windows::Forms::DockStyle::Left;
			this->TVfind->Font = (gcnew System::Drawing::Font(L"΢���ź�", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(134)));
			this->TVfind->Location = System::Drawing::Point(1006, 79);
			this->TVfind->Margin = System::Windows::Forms::Padding(0, 15, 18, 1);
			this->TVfind->Name = L"TVfind";
			this->TVfind->Size = System::Drawing::Size(38, 38);
			this->TVfind->TabIndex = 10;
			this->TVfind->Text = L"��";
			this->TVfind->UseVisualStyleBackColor = true;
			this->TVfind->Click += gcnew System::EventHandler(this, &MyForm::TVfind_Click);
			// 
			// About
			// 
			this->About->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->About->AutoSize = true;
			this->About->BackColor = System::Drawing::Color::Transparent;
			this->About->Font = (gcnew System::Drawing::Font(L"΢���ź�", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(134)));
			this->About->Location = System::Drawing::Point(1024, 209);
			this->About->Margin = System::Windows::Forms::Padding(3, 7, 5, 7);
			this->About->Name = L"About";
			this->About->Size = System::Drawing::Size(52, 20);
			this->About->TabIndex = 7;
			this->About->Text = L"v0.0.0";
			this->About->TextAlign = System::Drawing::ContentAlignment::BottomRight;
			this->About->Click += gcnew System::EventHandler(this, &MyForm::About_Click);
			this->About->MouseEnter += gcnew System::EventHandler(this, &MyForm::About_MouseEnter);
			this->About->MouseLeave += gcnew System::EventHandler(this, &MyForm::About_MouseLeave);
			// 
			// label3
			// 
			this->label3->BackColor = System::Drawing::Color::Transparent;
			this->label3->Dock = System::Windows::Forms::DockStyle::Fill;
			this->label3->Font = (gcnew System::Drawing::Font(L"΢���ź�", 13));
			this->label3->Location = System::Drawing::Point(8, 125);
			this->label3->Margin = System::Windows::Forms::Padding(8, 7, 0, 0);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(92, 47);
			this->label3->TabIndex = 5;
			this->label3->Text = L"BD��";
			this->label3->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// label2
			// 
			this->label2->BackColor = System::Drawing::Color::Transparent;
			this->label2->Dock = System::Windows::Forms::DockStyle::Fill;
			this->label2->Font = (gcnew System::Drawing::Font(L"΢���ź�", 13));
			this->label2->Location = System::Drawing::Point(8, 71);
			this->label2->Margin = System::Windows::Forms::Padding(8, 7, 0, 0);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(92, 47);
			this->label2->TabIndex = 3;
			this->label2->Text = L"TV��";
			this->label2->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// label1
			// 
			this->label1->BackColor = System::Drawing::Color::Transparent;
			this->label1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->label1->Font = (gcnew System::Drawing::Font(L"΢���ź�", 13.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(134)));
			this->label1->Location = System::Drawing::Point(3, 20);
			this->label1->Margin = System::Windows::Forms::Padding(3, 0, 0, 0);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(97, 44);
			this->label1->TabIndex = 0;
			this->label1->Text = L"ASS��";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// ASSfind
			// 
			this->ASSfind->Dock = System::Windows::Forms::DockStyle::Left;
			this->ASSfind->Font = (gcnew System::Drawing::Font(L"΢���ź�", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(134)));
			this->ASSfind->Location = System::Drawing::Point(1006, 24);
			this->ASSfind->Margin = System::Windows::Forms::Padding(0, 4, 18, 2);
			this->ASSfind->Name = L"ASSfind";
			this->ASSfind->Size = System::Drawing::Size(38, 38);
			this->ASSfind->TabIndex = 2;
			this->ASSfind->Text = L"��";
			this->ASSfind->UseVisualStyleBackColor = true;
			this->ASSfind->Click += gcnew System::EventHandler(this, &MyForm::ASSfind_Click);
			// 
			// Match
			// 
			this->Match->Anchor = System::Windows::Forms::AnchorStyles::Top;
			this->Match->Font = (gcnew System::Drawing::Font(L"΢���ź�", 13.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(134)));
			this->Match->Location = System::Drawing::Point(493, 182);
			this->Match->Margin = System::Windows::Forms::Padding(3, 10, 3, 3);
			this->Match->MinimumSize = System::Drawing::Size(82, 43);
			this->Match->Name = L"Match";
			this->Match->Size = System::Drawing::Size(155, 45);
			this->Match->TabIndex = 8;
			this->Match->Text = L"ƥ��";
			this->Match->UseVisualStyleBackColor = true;
			this->Match->Click += gcnew System::EventHandler(this, &MyForm::Match_Click);
			// 
			// TVtext
			// 
			this->TVtext->AllowDrop = true;
			this->AllTablePanel->SetColumnSpan(this->TVtext, 2);
			this->TVtext->Dock = System::Windows::Forms::DockStyle::Fill;
			this->TVtext->Font = (gcnew System::Drawing::Font(L"΢���ź�", 10.8F));
			this->TVtext->FormattingEnabled = true;
			this->TVtext->Location = System::Drawing::Point(103, 80);
			this->TVtext->Margin = System::Windows::Forms::Padding(3, 16, 0, 0);
			this->TVtext->MaxDropDownItems = 10;
			this->TVtext->Name = L"TVtext";
			this->TVtext->Size = System::Drawing::Size(903, 32);
			this->TVtext->TabIndex = 4;
			this->TVtext->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &MyForm::TVtext_DragDrop);
			this->TVtext->DragEnter += gcnew System::Windows::Forms::DragEventHandler(this, &MyForm::TVtext_DragEnter);
			// 
			// BDtext
			// 
			this->BDtext->AllowDrop = true;
			this->AllTablePanel->SetColumnSpan(this->BDtext, 2);
			this->BDtext->Dock = System::Windows::Forms::DockStyle::Fill;
			this->BDtext->Font = (gcnew System::Drawing::Font(L"΢���ź�", 10.8F));
			this->BDtext->FormattingEnabled = true;
			this->BDtext->Location = System::Drawing::Point(103, 134);
			this->BDtext->Margin = System::Windows::Forms::Padding(3, 16, 0, 0);
			this->BDtext->MaxDropDownItems = 10;
			this->BDtext->Name = L"BDtext";
			this->BDtext->Size = System::Drawing::Size(903, 32);
			this->BDtext->TabIndex = 9;
			this->BDtext->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &MyForm::BDtext_DragDrop);
			this->BDtext->DragEnter += gcnew System::Windows::Forms::DragEventHandler(this, &MyForm::BDtext_DragEnter);
			// 
			// Result
			// 
			this->Result->BackColor = System::Drawing::Color::White;
			this->AllTablePanel->SetColumnSpan(this->Result, 4);
			this->Result->Dock = System::Windows::Forms::DockStyle::Fill;
			this->Result->Font = (gcnew System::Drawing::Font(L"΢���ź�", 10.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(134)));
			this->Result->Location = System::Drawing::Point(5, 321);
			this->Result->Margin = System::Windows::Forms::Padding(5);
			this->Result->Multiline = true;
			this->Result->Name = L"Result";
			this->Result->ReadOnly = true;
			this->Result->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->Result->Size = System::Drawing::Size(1071, 184);
			this->Result->TabIndex = 14;
			// 
			// SingleProgress
			// 
			this->AllTablePanel->SetColumnSpan(this->SingleProgress, 4);
			this->SingleProgress->Dock = System::Windows::Forms::DockStyle::Fill;
			this->SingleProgress->Location = System::Drawing::Point(5, 239);
			this->SingleProgress->Margin = System::Windows::Forms::Padding(5, 3, 5, 3);
			this->SingleProgress->Maximum = 200;
			this->SingleProgress->Name = L"SingleProgress";
			this->SingleProgress->Size = System::Drawing::Size(1071, 34);
			this->SingleProgress->Step = 1;
			this->SingleProgress->Style = System::Windows::Forms::ProgressBarStyle::Continuous;
			this->SingleProgress->TabIndex = 15;
			// 
			// splitContainer1
			// 
			this->AllTablePanel->SetColumnSpan(this->splitContainer1, 2);
			this->splitContainer1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->splitContainer1->FixedPanel = System::Windows::Forms::FixedPanel::Panel1;
			this->splitContainer1->Location = System::Drawing::Point(138, 513);
			this->splitContainer1->Margin = System::Windows::Forms::Padding(3, 3, 5, 3);
			this->splitContainer1->Name = L"splitContainer1";
			this->splitContainer1->Orientation = System::Windows::Forms::Orientation::Horizontal;
			// 
			// splitContainer1.Panel1
			// 
			this->splitContainer1->Panel1->Controls->Add(this->TimeRoll);
			// 
			// splitContainer1.Panel2
			// 
			this->splitContainer1->Panel2->Controls->Add(this->Spectrum);
			this->splitContainer1->Size = System::Drawing::Size(938, 287);
			this->splitContainer1->SplitterDistance = 30;
			this->splitContainer1->TabIndex = 21;
			// 
			// TimeRoll
			// 
			this->TimeRoll->Dock = System::Windows::Forms::DockStyle::Fill;
			this->TimeRoll->Enabled = false;
			this->TimeRoll->LargeChange = 100;
			this->TimeRoll->Location = System::Drawing::Point(0, 0);
			this->TimeRoll->Maximum = 200;
			this->TimeRoll->Name = L"TimeRoll";
			this->TimeRoll->Size = System::Drawing::Size(938, 30);
			this->TimeRoll->TabIndex = 0;
			this->TimeRoll->TickFrequency = 100;
			this->TimeRoll->Scroll += gcnew System::EventHandler(this, &MyForm::TimeRoll_Scroll);
			// 
			// Spectrum
			// 
			this->Spectrum->Dock = System::Windows::Forms::DockStyle::Fill;
			this->Spectrum->Location = System::Drawing::Point(0, 0);
			this->Spectrum->Name = L"Spectrum";
			this->Spectrum->Size = System::Drawing::Size(938, 253);
			this->Spectrum->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->Spectrum->TabIndex = 0;
			this->Spectrum->TabStop = false;
			// 
			// tableLayoutPanel2
			// 
			this->tableLayoutPanel2->ColumnCount = 1;
			this->AllTablePanel->SetColumnSpan(this->tableLayoutPanel2, 2);
			this->tableLayoutPanel2->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				100)));
			this->tableLayoutPanel2->Controls->Add(this->ViewSel, 0, 3);
			this->tableLayoutPanel2->Controls->Add(this->label4, 0, 2);
			this->tableLayoutPanel2->Controls->Add(this->ChartTime, 0, 5);
			this->tableLayoutPanel2->Controls->Add(this->ChSelect, 0, 1);
			this->tableLayoutPanel2->Controls->Add(this->label5, 0, 0);
			this->tableLayoutPanel2->Controls->Add(this->LineSel, 0, 4);
			this->tableLayoutPanel2->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tableLayoutPanel2->Location = System::Drawing::Point(5, 513);
			this->tableLayoutPanel2->Margin = System::Windows::Forms::Padding(5, 3, 3, 3);
			this->tableLayoutPanel2->Name = L"tableLayoutPanel2";
			this->tableLayoutPanel2->RowCount = 6;
			this->tableLayoutPanel2->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 45)));
			this->tableLayoutPanel2->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 45)));
			this->tableLayoutPanel2->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 45)));
			this->tableLayoutPanel2->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 45)));
			this->tableLayoutPanel2->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 45)));
			this->tableLayoutPanel2->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 45)));
			this->tableLayoutPanel2->Size = System::Drawing::Size(127, 287);
			this->tableLayoutPanel2->TabIndex = 22;
			// 
			// ViewSel
			// 
			this->ViewSel->Dock = System::Windows::Forms::DockStyle::Fill;
			this->ViewSel->Enabled = false;
			this->ViewSel->Font = (gcnew System::Drawing::Font(L"΢���ź�", 11.8F));
			this->ViewSel->FormattingEnabled = true;
			this->ViewSel->Items->AddRange(gcnew cli::array< System::Object^  >(2) { L"��ʱ��", L"����" });
			this->ViewSel->Location = System::Drawing::Point(3, 138);
			this->ViewSel->Name = L"ViewSel";
			this->ViewSel->Size = System::Drawing::Size(121, 35);
			this->ViewSel->TabIndex = 7;
			this->ViewSel->SelectedIndexChanged += gcnew System::EventHandler(this, &MyForm::ViewSel_SelectedIndexChanged);
			// 
			// label4
			// 
			this->label4->BackColor = System::Drawing::Color::Transparent;
			this->label4->Dock = System::Windows::Forms::DockStyle::Fill;
			this->label4->Font = (gcnew System::Drawing::Font(L"΢���ź�", 13.8F));
			this->label4->Location = System::Drawing::Point(3, 90);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(121, 45);
			this->label4->TabIndex = 6;
			this->label4->Text = L"�鿴��";
			this->label4->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// ChartTime
			// 
			this->ChartTime->BackColor = System::Drawing::Color::Transparent;
			this->ChartTime->Dock = System::Windows::Forms::DockStyle::Fill;
			this->ChartTime->Font = (gcnew System::Drawing::Font(L"΢���ź�", 11.8F));
			this->ChartTime->Location = System::Drawing::Point(3, 225);
			this->ChartTime->Name = L"ChartTime";
			this->ChartTime->Size = System::Drawing::Size(121, 62);
			this->ChartTime->TabIndex = 5;
			this->ChartTime->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// ChSelect
			// 
			this->ChSelect->Dock = System::Windows::Forms::DockStyle::Fill;
			this->ChSelect->Enabled = false;
			this->ChSelect->Font = (gcnew System::Drawing::Font(L"΢���ź�", 11.8F));
			this->ChSelect->FormattingEnabled = true;
			this->ChSelect->Items->AddRange(gcnew cli::array< System::Object^  >(2) { L"��", L"��" });
			this->ChSelect->Location = System::Drawing::Point(3, 48);
			this->ChSelect->Name = L"ChSelect";
			this->ChSelect->Size = System::Drawing::Size(121, 35);
			this->ChSelect->TabIndex = 4;
			this->ChSelect->SelectedIndexChanged += gcnew System::EventHandler(this, &MyForm::ChSelect_SelectedIndexChanged);
			// 
			// label5
			// 
			this->label5->BackColor = System::Drawing::Color::Transparent;
			this->label5->Dock = System::Windows::Forms::DockStyle::Fill;
			this->label5->Font = (gcnew System::Drawing::Font(L"΢���ź�", 13.8F));
			this->label5->Location = System::Drawing::Point(3, 0);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(121, 45);
			this->label5->TabIndex = 2;
			this->label5->Text = L"������";
			this->label5->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// LineSel
			// 
			this->LineSel->Enabled = false;
			this->LineSel->Font = (gcnew System::Drawing::Font(L"΢���ź�", 13.8F));
			this->LineSel->Location = System::Drawing::Point(3, 183);
			this->LineSel->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
			this->LineSel->Name = L"LineSel";
			this->LineSel->Size = System::Drawing::Size(120, 38);
			this->LineSel->TabIndex = 8;
			this->LineSel->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
			this->LineSel->ValueChanged += gcnew System::EventHandler(this, &MyForm::LineSel_ValueChanged);
			// 
			// Filebrowse
			// 
			this->Filebrowse->FileName = L"openFileDialog1";
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 15);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1081, 803);
			this->Controls->Add(this->AllTablePanel);
			this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
			this->Name = L"MyForm";
			this->Text = L"BDMatch";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &MyForm::MyForm_FormClosing);
			this->Load += gcnew System::EventHandler(this, &MyForm::MyForm_Load);
			this->AllTablePanel->ResumeLayout(false);
			this->AllTablePanel->PerformLayout();
			this->splitContainer1->Panel1->ResumeLayout(false);
			this->splitContainer1->Panel1->PerformLayout();
			this->splitContainer1->Panel2->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->splitContainer1))->EndInit();
			this->splitContainer1->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->TimeRoll))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->Spectrum))->EndInit();
			this->tableLayoutPanel2->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->LineSel))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion

	public:
		void SetVals(SettingType type, int val);
		void nullsetform();
		void progsingle(int type, double val);
		void progtotal();

	private: 
		int match(String^ ASSText, String^ TVText, String^ BDText);
		int writeass(Decode^ tvdecode, Decode^ bddecode, String^ ASSText);
		int drawpre();
		int drawpre(Decode^ tvdecode, Decode^ bddecode, int &re);
		int drawchart();
		String ^ mstotime(int ms);
		int setrows();
		int adddropdown(ComboBox^ combo, String^ text);
		int loadsettings(String^ path, SettingVals^ settingvals);
		int savesettings(String^ path, SettingVals^ settingvals);
		int matchinput();
		String ^ returnregt(String ^ search);
		int matchcontrol(bool val);

	private: System::Void MyForm_Load(System::Object^  sender, System::EventArgs^  e);
	private: System::Void MyForm_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e);

	private: System::Void TVfind_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void TVtext_DragEnter(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e);
	private: System::Void TVtext_DragDrop(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e);
	private: System::Void BDfind_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void BDtext_DragEnter(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e);
	private: System::Void BDtext_DragDrop(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e);
	private: System::Void ASSfind_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void ASStext_DragEnter(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e);
	private: System::Void ASStext_DragDrop(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e);

	private: System::Void Match_Click(System::Object^  sender, System::EventArgs^  e);

	private: System::Void About_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void About_MouseEnter(System::Object^  sender, System::EventArgs^  e);
	private: System::Void About_MouseLeave(System::Object^  sender, System::EventArgs^  e);
	private: System::Void settings_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void settings_MouseEnter(System::Object^  sender, System::EventArgs^  e);
	private: System::Void settings_MouseLeave(System::Object^  sender, System::EventArgs^  e);

	private: System::Void ChSelect_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void ViewSel_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void TimeRoll_Scroll(System::Object^  sender, System::EventArgs^  e);
	private: System::Void LineSel_ValueChanged(System::Object^  sender, System::EventArgs^  e);

};
}