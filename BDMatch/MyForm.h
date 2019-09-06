#pragma once
#include "headers/datastruct1.h"
#include "Settings.h"
#pragma unmanaged
#include "headers/user interface.h"
#include <atomic>
#pragma managed

namespace BDMatch {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace DataStruct1;

	/// <summary>
	/// MyForm 摘要
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
		{
			InitializeComponent();
			//
			//TODO:  在此处添加构造函数代码
			//
		}
	protected:
		/// <summary>
		/// 清理所有正在使用的资源。
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
		int line_num = 0;
		int fft_num = 0;
		double ttf = 1.0;//Time to Frequency
		char** spec = nullptr;
		array<int, 2>^ time_list = nullptr;
	};
	private:
		SettingVals ^ setting = gcnew SettingVals;
		drawpara tv_draw, bd_draw;
		bool draw_store;
		Settings ^ set_form = nullptr;
		unsigned int match_num = 0, fin_match_num = 0, matches_num = 0, fin_matches_num = 0;
		String^ output_path = "";
		bool debug_mode = false;
		System::Threading::CancellationTokenSource^ cancel_source;
		int ISA_mode = 0;
		TaskBar *taskbar;
		std::atomic_flag *keep_processing = new std::atomic_flag;

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
	private: System::Windows::Forms::Label^  ASSLabel;
	private: System::Windows::Forms::Label^  TVLabel;
	private: System::Windows::Forms::Label^  BDLabel;





	private: System::Windows::Forms::Label^ About;
	private: System::Windows::Forms::Label^ settings;

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
	private: System::Windows::Forms::Panel^  TextEditorPanel;



	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel1;
	private: System::Windows::Forms::Label^  EditorLabel;

	private: System::Windows::Forms::Button^  CompleteEdit;

	private: System::Windows::Forms::TextBox^  TextEditor;



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
			System::ComponentModel::ComponentResourceManager^ resources = (gcnew System::ComponentModel::ComponentResourceManager(MyForm::typeid));
			this->AllTablePanel = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->TotalProgress = (gcnew System::Windows::Forms::ProgressBar());
			this->settings = (gcnew System::Windows::Forms::Label());
			this->ASStext = (gcnew System::Windows::Forms::ComboBox());
			this->BDfind = (gcnew System::Windows::Forms::Button());
			this->TVfind = (gcnew System::Windows::Forms::Button());
			this->About = (gcnew System::Windows::Forms::Label());
			this->BDLabel = (gcnew System::Windows::Forms::Label());
			this->TVLabel = (gcnew System::Windows::Forms::Label());
			this->ASSLabel = (gcnew System::Windows::Forms::Label());
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
			this->Filebrowse = (gcnew System::Windows::Forms::OpenFileDialog());
			this->TextEditorPanel = (gcnew System::Windows::Forms::Panel());
			this->tableLayoutPanel1 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->EditorLabel = (gcnew System::Windows::Forms::Label());
			this->CompleteEdit = (gcnew System::Windows::Forms::Button());
			this->TextEditor = (gcnew System::Windows::Forms::TextBox());
			this->AllTablePanel->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->splitContainer1))->BeginInit();
			this->splitContainer1->Panel1->SuspendLayout();
			this->splitContainer1->Panel2->SuspendLayout();
			this->splitContainer1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->TimeRoll))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->Spectrum))->BeginInit();
			this->tableLayoutPanel2->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->LineSel))->BeginInit();
			this->TextEditorPanel->SuspendLayout();
			this->tableLayoutPanel1->SuspendLayout();
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
			this->AllTablePanel->Controls->Add(this->BDLabel, 0, 3);
			this->AllTablePanel->Controls->Add(this->TVLabel, 0, 2);
			this->AllTablePanel->Controls->Add(this->ASSLabel, 0, 1);
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
			this->settings->Font = (gcnew System::Drawing::Font(L"微软雅黑", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(134)));
			this->settings->Location = System::Drawing::Point(3, 209);
			this->settings->Margin = System::Windows::Forms::Padding(3, 7, 5, 7);
			this->settings->Name = L"settings";
			this->settings->Size = System::Drawing::Size(39, 20);
			this->settings->TabIndex = 16;
			this->settings->Text = L"设置";
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
			this->ASStext->Font = (gcnew System::Drawing::Font(L"微软雅黑", 10.8F));
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
			this->BDfind->Font = (gcnew System::Drawing::Font(L"微软雅黑", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(134)));
			this->BDfind->Location = System::Drawing::Point(1006, 133);
			this->BDfind->Margin = System::Windows::Forms::Padding(0, 15, 18, 1);
			this->BDfind->Name = L"BDfind";
			this->BDfind->Size = System::Drawing::Size(38, 38);
			this->BDfind->TabIndex = 11;
			this->BDfind->Text = L"…";
			this->BDfind->UseVisualStyleBackColor = true;
			this->BDfind->Click += gcnew System::EventHandler(this, &MyForm::BDfind_Click);
			// 
			// TVfind
			// 
			this->TVfind->Dock = System::Windows::Forms::DockStyle::Left;
			this->TVfind->Font = (gcnew System::Drawing::Font(L"微软雅黑", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(134)));
			this->TVfind->Location = System::Drawing::Point(1006, 79);
			this->TVfind->Margin = System::Windows::Forms::Padding(0, 15, 18, 1);
			this->TVfind->Name = L"TVfind";
			this->TVfind->Size = System::Drawing::Size(38, 38);
			this->TVfind->TabIndex = 10;
			this->TVfind->Text = L"…";
			this->TVfind->UseVisualStyleBackColor = true;
			this->TVfind->Click += gcnew System::EventHandler(this, &MyForm::TVfind_Click);
			// 
			// About
			// 
			this->About->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->About->AutoSize = true;
			this->About->BackColor = System::Drawing::Color::Transparent;
			this->About->Font = (gcnew System::Drawing::Font(L"微软雅黑", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
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
			// BDLabel
			// 
			this->BDLabel->BackColor = System::Drawing::Color::Transparent;
			this->BDLabel->Dock = System::Windows::Forms::DockStyle::Fill;
			this->BDLabel->Font = (gcnew System::Drawing::Font(L"微软雅黑", 13));
			this->BDLabel->Location = System::Drawing::Point(8, 125);
			this->BDLabel->Margin = System::Windows::Forms::Padding(8, 7, 0, 0);
			this->BDLabel->Name = L"BDLabel";
			this->BDLabel->Size = System::Drawing::Size(92, 47);
			this->BDLabel->TabIndex = 5;
			this->BDLabel->Text = L"BD：";
			this->BDLabel->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			this->BDLabel->MouseDoubleClick += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::BDLabel_MouseDoubleClick);
			// 
			// TVLabel
			// 
			this->TVLabel->BackColor = System::Drawing::Color::Transparent;
			this->TVLabel->Dock = System::Windows::Forms::DockStyle::Fill;
			this->TVLabel->Font = (gcnew System::Drawing::Font(L"微软雅黑", 13));
			this->TVLabel->Location = System::Drawing::Point(8, 71);
			this->TVLabel->Margin = System::Windows::Forms::Padding(8, 7, 0, 0);
			this->TVLabel->Name = L"TVLabel";
			this->TVLabel->Size = System::Drawing::Size(92, 47);
			this->TVLabel->TabIndex = 3;
			this->TVLabel->Text = L"TV：";
			this->TVLabel->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			this->TVLabel->MouseDoubleClick += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::TVLabel_MouseDoubleClick);
			// 
			// ASSLabel
			// 
			this->ASSLabel->BackColor = System::Drawing::Color::Transparent;
			this->ASSLabel->Dock = System::Windows::Forms::DockStyle::Fill;
			this->ASSLabel->Font = (gcnew System::Drawing::Font(L"微软雅黑", 13.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(134)));
			this->ASSLabel->Location = System::Drawing::Point(3, 20);
			this->ASSLabel->Margin = System::Windows::Forms::Padding(3, 0, 0, 0);
			this->ASSLabel->Name = L"ASSLabel";
			this->ASSLabel->Size = System::Drawing::Size(97, 44);
			this->ASSLabel->TabIndex = 0;
			this->ASSLabel->Text = L"ASS：";
			this->ASSLabel->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			this->ASSLabel->MouseDoubleClick += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::ASSLabel_MouseDoubleClick);
			// 
			// ASSfind
			// 
			this->ASSfind->Dock = System::Windows::Forms::DockStyle::Left;
			this->ASSfind->Font = (gcnew System::Drawing::Font(L"微软雅黑", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(134)));
			this->ASSfind->Location = System::Drawing::Point(1006, 24);
			this->ASSfind->Margin = System::Windows::Forms::Padding(0, 4, 18, 2);
			this->ASSfind->Name = L"ASSfind";
			this->ASSfind->Size = System::Drawing::Size(38, 38);
			this->ASSfind->TabIndex = 2;
			this->ASSfind->Text = L"…";
			this->ASSfind->UseVisualStyleBackColor = true;
			this->ASSfind->Click += gcnew System::EventHandler(this, &MyForm::ASSfind_Click);
			// 
			// Match
			// 
			this->Match->Anchor = System::Windows::Forms::AnchorStyles::Top;
			this->Match->Font = (gcnew System::Drawing::Font(L"微软雅黑", 13.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(134)));
			this->Match->Location = System::Drawing::Point(493, 182);
			this->Match->Margin = System::Windows::Forms::Padding(3, 10, 3, 3);
			this->Match->MinimumSize = System::Drawing::Size(82, 43);
			this->Match->Name = L"Match";
			this->Match->Size = System::Drawing::Size(155, 45);
			this->Match->TabIndex = 8;
			this->Match->Text = L"匹配";
			this->Match->UseVisualStyleBackColor = true;
			this->Match->Click += gcnew System::EventHandler(this, &MyForm::Match_Click);
			// 
			// TVtext
			// 
			this->TVtext->AllowDrop = true;
			this->AllTablePanel->SetColumnSpan(this->TVtext, 2);
			this->TVtext->Dock = System::Windows::Forms::DockStyle::Fill;
			this->TVtext->Font = (gcnew System::Drawing::Font(L"微软雅黑", 10.8F));
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
			this->BDtext->Font = (gcnew System::Drawing::Font(L"微软雅黑", 10.8F));
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
			this->Result->Font = (gcnew System::Drawing::Font(L"微软雅黑", 10.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(134)));
			this->Result->Location = System::Drawing::Point(5, 321);
			this->Result->Margin = System::Windows::Forms::Padding(5);
			this->Result->Multiline = true;
			this->Result->Name = L"Result";
			this->Result->ReadOnly = true;
			this->Result->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->Result->Size = System::Drawing::Size(1071, 184);
			this->Result->TabIndex = 14;
			this->Result->TextChanged += gcnew System::EventHandler(this, &MyForm::Result_TextChanged);
			// 
			// SingleProgress
			// 
			this->SingleProgress->BackColor = System::Drawing::SystemColors::Control;
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
			this->ViewSel->Font = (gcnew System::Drawing::Font(L"微软雅黑", 11.8F));
			this->ViewSel->FormattingEnabled = true;
			this->ViewSel->Items->AddRange(gcnew cli::array< System::Object^  >(2) { L"按时间", L"按行" });
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
			this->label4->Font = (gcnew System::Drawing::Font(L"微软雅黑", 13.8F));
			this->label4->Location = System::Drawing::Point(3, 90);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(121, 45);
			this->label4->TabIndex = 6;
			this->label4->Text = L"查看：";
			this->label4->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// ChartTime
			// 
			this->ChartTime->BackColor = System::Drawing::Color::Transparent;
			this->ChartTime->Dock = System::Windows::Forms::DockStyle::Fill;
			this->ChartTime->Font = (gcnew System::Drawing::Font(L"微软雅黑", 11.8F));
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
			this->ChSelect->Font = (gcnew System::Drawing::Font(L"微软雅黑", 11.8F));
			this->ChSelect->FormattingEnabled = true;
			this->ChSelect->Items->AddRange(gcnew cli::array< System::Object^  >(2) { L"左", L"右" });
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
			this->label5->Font = (gcnew System::Drawing::Font(L"微软雅黑", 13.8F));
			this->label5->Location = System::Drawing::Point(3, 0);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(121, 45);
			this->label5->TabIndex = 2;
			this->label5->Text = L"声道：";
			this->label5->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// LineSel
			// 
			this->LineSel->Enabled = false;
			this->LineSel->Font = (gcnew System::Drawing::Font(L"微软雅黑", 13.8F));
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
			this->Filebrowse->Multiselect = true;
			// 
			// TextEditorPanel
			// 
			this->TextEditorPanel->Controls->Add(this->tableLayoutPanel1);
			this->TextEditorPanel->Location = System::Drawing::Point(178, 0);
			this->TextEditorPanel->Name = L"TextEditorPanel";
			this->TextEditorPanel->Size = System::Drawing::Size(138, 426);
			this->TextEditorPanel->TabIndex = 5;
			// 
			// tableLayoutPanel1
			// 
			this->tableLayoutPanel1->ColumnCount = 3;
			this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				50)));
			this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Absolute,
				150)));
			this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				50)));
			this->tableLayoutPanel1->Controls->Add(this->EditorLabel, 0, 0);
			this->tableLayoutPanel1->Controls->Add(this->CompleteEdit, 1, 2);
			this->tableLayoutPanel1->Controls->Add(this->TextEditor, 0, 1);
			this->tableLayoutPanel1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tableLayoutPanel1->Location = System::Drawing::Point(0, 0);
			this->tableLayoutPanel1->Name = L"tableLayoutPanel1";
			this->tableLayoutPanel1->RowCount = 3;
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 50)));
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 100)));
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 50)));
			this->tableLayoutPanel1->Size = System::Drawing::Size(138, 426);
			this->tableLayoutPanel1->TabIndex = 0;
			// 
			// EditorLabel
			// 
			this->EditorLabel->BackColor = System::Drawing::Color::Transparent;
			this->tableLayoutPanel1->SetColumnSpan(this->EditorLabel, 3);
			this->EditorLabel->Dock = System::Windows::Forms::DockStyle::Fill;
			this->EditorLabel->Font = (gcnew System::Drawing::Font(L"微软雅黑", 13.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(134)));
			this->EditorLabel->Location = System::Drawing::Point(3, 0);
			this->EditorLabel->Margin = System::Windows::Forms::Padding(3, 0, 0, 0);
			this->EditorLabel->Name = L"EditorLabel";
			this->EditorLabel->Size = System::Drawing::Size(135, 50);
			this->EditorLabel->TabIndex = 1;
			this->EditorLabel->Text = L"输入编辑";
			this->EditorLabel->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// CompleteEdit
			// 
			this->CompleteEdit->Dock = System::Windows::Forms::DockStyle::Fill;
			this->CompleteEdit->Font = (gcnew System::Drawing::Font(L"微软雅黑", 13.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(134)));
			this->CompleteEdit->Location = System::Drawing::Point(-3, 379);
			this->CompleteEdit->Name = L"CompleteEdit";
			this->CompleteEdit->Size = System::Drawing::Size(144, 44);
			this->CompleteEdit->TabIndex = 2;
			this->CompleteEdit->Text = L"完成";
			this->CompleteEdit->UseVisualStyleBackColor = true;
			this->CompleteEdit->Click += gcnew System::EventHandler(this, &MyForm::CompleteEdit_Click);
			// 
			// TextEditor
			// 
			this->tableLayoutPanel1->SetColumnSpan(this->TextEditor, 3);
			this->TextEditor->Dock = System::Windows::Forms::DockStyle::Fill;
			this->TextEditor->Font = (gcnew System::Drawing::Font(L"微软雅黑", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(134)));
			this->TextEditor->Location = System::Drawing::Point(3, 53);
			this->TextEditor->Multiline = true;
			this->TextEditor->Name = L"TextEditor";
			this->TextEditor->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->TextEditor->Size = System::Drawing::Size(132, 320);
			this->TextEditor->TabIndex = 3;
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 15);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1081, 803);
			this->Controls->Add(this->TextEditorPanel);
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
			this->TextEditorPanel->ResumeLayout(false);
			this->tableLayoutPanel1->ResumeLayout(false);
			this->tableLayoutPanel1->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion

	public:
		void set_vals(SettingType type, int val);
		void null_set_form();
		void prog_single(int type, double val);
		void prog_total();
		void feedback(const char* input, const long long len);

	private:
		int match(String^ ASSText, String^ TVText, String^ BDText);
		int draw_pre(); 
		int BDMatch::MyForm::write_ass(const char* ass_path, const char* output_path);
		int draw_pre(const int &re);
		int draw_chart();
		String ^ ms2time(int ms);
		int set_rows();
		int add_dropdown(ComboBox^ combo, String^ text);
		int load_settings(String^ path, SettingVals^ settingvals);
		int save_settings(String^ path, SettingVals^ settingvals);
		int match_input();
		int search_ISA();

		String ^ return_regt(String ^ search);
		int match_control(bool val);

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

	private: System::Void Result_TextChanged(System::Object^  sender, System::EventArgs^  e);

	private: System::Void CompleteEdit_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void ASSLabel_MouseDoubleClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	private: System::Void TVLabel_MouseDoubleClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	private: System::Void BDLabel_MouseDoubleClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	};
}