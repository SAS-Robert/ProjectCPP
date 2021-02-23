// Library config
#include <string>;
#include <thread>
#include <random>
#include <string>
#include "gui_header.h"
//#include <thread>
#pragma once

using namespace std;

// User Interface definition
namespace DesktopApp02 {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	// New
	using namespace System::Threading;
	// ----
	/// <summary>
	/// Summary for MyForm
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^ rampMinus;

	private: System::Windows::Forms::Label^ rampValue;
	public: System::Windows::Forms::Button^ rampPlus;

	private: System::Windows::Forms::Label^ rampLabel;
	private: System::Windows::Forms::Label^ curLabel;
	public: System::Windows::Forms::Button^ curPlus;

	public: System::Windows::Forms::Label^ curValue;
	public: System::Windows::Forms::Button^ curMinus;
	private: System::Windows::Forms::Label^ pLabel;

	private: System::Windows::Forms::Button^ repPlus;
	private: System::Windows::Forms::Label^ repValue;

	private: System::Windows::Forms::Button^ repMinus;
	private: System::Windows::Forms::Button^ enButton;
	private: System::Windows::Forms::DomainUpDown^ selectCH;
	private: System::Windows::Forms::Label^ mainTitle;

	private: System::Windows::Forms::Button^ startStim;
	private: System::Windows::Forms::Button^ quitStim;
	private: System::Windows::Forms::Label^ stimTitle;
	private: System::Windows::Forms::Label^ statusTitle;
	private: System::Windows::Forms::Label^ statusGrid;
	private: System::Windows::Forms::Label^ label2;
	private: System::Windows::Forms::Label^ stateLabel;
	private: System::Windows::Forms::Label^ stateMessage;
	private: System::Windows::Forms::Button^ thButton;
	private: System::Windows::Forms::Button^ startButton;
	private: System::Windows::Forms::Button^ autoButton;
	private: System::Windows::Forms::Button^ manButton;
	private: System::Windows::Forms::Button^ xButton;
	private: System::Windows::Forms::Button^ abortEx;

	private: System::Windows::Forms::Label^ calTitle;
	private: System::Windows::Forms::Button^ repeatButton;
	private: System::Windows::Forms::Button^ newButton;
	private: System::Windows::Forms::Label^ label1;
	
	// NEW
	System::ComponentModel::BackgroundWorker^ backgroundWorker1;
	System::Windows::Forms::MessageBox^ displayStuff; // New
	BackgroundWorker bw; // Background worker
	protected:

	protected:

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;


#pragma region Windows Form Designer generated code
		// New

		// new end
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->rampMinus = (gcnew System::Windows::Forms::Button());
			this->rampValue = (gcnew System::Windows::Forms::Label());
			this->rampPlus = (gcnew System::Windows::Forms::Button());
			this->rampLabel = (gcnew System::Windows::Forms::Label());
			this->curLabel = (gcnew System::Windows::Forms::Label());
			this->curPlus = (gcnew System::Windows::Forms::Button());
			this->curValue = (gcnew System::Windows::Forms::Label());
			this->curMinus = (gcnew System::Windows::Forms::Button());
			this->pLabel = (gcnew System::Windows::Forms::Label());
			this->repPlus = (gcnew System::Windows::Forms::Button());
			this->repValue = (gcnew System::Windows::Forms::Label());
			this->repMinus = (gcnew System::Windows::Forms::Button());
			this->enButton = (gcnew System::Windows::Forms::Button());
			this->selectCH = (gcnew System::Windows::Forms::DomainUpDown());
			this->mainTitle = (gcnew System::Windows::Forms::Label());
			this->startStim = (gcnew System::Windows::Forms::Button());
			this->quitStim = (gcnew System::Windows::Forms::Button());
			this->stimTitle = (gcnew System::Windows::Forms::Label());
			this->statusTitle = (gcnew System::Windows::Forms::Label());
			this->statusGrid = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->stateLabel = (gcnew System::Windows::Forms::Label());
			this->stateMessage = (gcnew System::Windows::Forms::Label());
			this->thButton = (gcnew System::Windows::Forms::Button());
			this->startButton = (gcnew System::Windows::Forms::Button());
			this->autoButton = (gcnew System::Windows::Forms::Button());
			this->manButton = (gcnew System::Windows::Forms::Button());
			this->xButton = (gcnew System::Windows::Forms::Button());
			this->abortEx = (gcnew System::Windows::Forms::Button());
			this->calTitle = (gcnew System::Windows::Forms::Label());
			this->repeatButton = (gcnew System::Windows::Forms::Button());
			this->newButton = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			// New
			this->backgroundWorker1 = (gcnew System::ComponentModel::BackgroundWorker());
			this->SuspendLayout();
			// 
			// rampMinus
			// 
			this->rampMinus->BackColor = System::Drawing::SystemColors::Highlight;
			this->rampMinus->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->rampMinus->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->rampMinus->ImageAlign = System::Drawing::ContentAlignment::BottomCenter;
			this->rampMinus->Location = System::Drawing::Point(375, 215);
			this->rampMinus->Name = L"rampMinus";
			this->rampMinus->Size = System::Drawing::Size(75, 34);
			this->rampMinus->TabIndex = 0;
			this->rampMinus->Text = L"-";
			this->rampMinus->UseVisualStyleBackColor = false;
			// 
			// rampValue
			// 
			this->rampValue->AutoSize = true;
			this->rampValue->Font = (gcnew System::Drawing::Font(L"Berlin Sans FB", 20.25F));
			this->rampValue->Location = System::Drawing::Point(456, 213);
			this->rampValue->Name = L"rampValue";
			this->rampValue->Size = System::Drawing::Size(83, 30);
			this->rampValue->TabIndex = 1;
			this->rampValue->Text = L"rvalue";
			// 
			// rampPlus
			// 
			this->rampPlus->BackColor = System::Drawing::SystemColors::Highlight;
			this->rampPlus->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->rampPlus->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->rampPlus->ImageAlign = System::Drawing::ContentAlignment::BottomCenter;
			this->rampPlus->Location = System::Drawing::Point(548, 215);
			this->rampPlus->Name = L"rampPlus";
			this->rampPlus->Size = System::Drawing::Size(75, 34);
			this->rampPlus->TabIndex = 2;
			this->rampPlus->Text = L"+";
			this->rampPlus->UseVisualStyleBackColor = false;
			// 
			// rampLabel
			// 
			this->rampLabel->AutoSize = true;
			this->rampLabel->Font = (gcnew System::Drawing::Font(L"Berlin Sans FB", 20.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->rampLabel->Location = System::Drawing::Point(456, 180);
			this->rampLabel->Name = L"rampLabel";
			this->rampLabel->Size = System::Drawing::Size(85, 30);
			this->rampLabel->TabIndex = 3;
			this->rampLabel->Text = L"RAMP";
			// 
			// curLabel
			// 
			this->curLabel->AutoSize = true;
			this->curLabel->Font = (gcnew System::Drawing::Font(L"Berlin Sans FB", 20.25F));
			this->curLabel->Location = System::Drawing::Point(441, 264);
			this->curLabel->Name = L"curLabel";
			this->curLabel->Size = System::Drawing::Size(127, 30);
			this->curLabel->TabIndex = 7;
			this->curLabel->Text = L"CURRENT";
			this->curLabel->Click += gcnew System::EventHandler(this, &MyForm::curLabel_Click);
			// 
			// curPlus
			// 
			this->curPlus->BackColor = System::Drawing::SystemColors::Highlight;
			this->curPlus->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->curPlus->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->curPlus->ImageAlign = System::Drawing::ContentAlignment::BottomCenter;
			this->curPlus->Location = System::Drawing::Point(548, 299);
			this->curPlus->Name = L"curPlus";
			this->curPlus->Size = System::Drawing::Size(75, 34);
			this->curPlus->TabIndex = 6;
			this->curPlus->Text = L"+";
			this->curPlus->UseVisualStyleBackColor = false;
			// 
			// curValue
			// 
			this->curValue->AutoSize = true;
			this->curValue->Font = (gcnew System::Drawing::Font(L"Berlin Sans FB", 20.25F));
			this->curValue->Location = System::Drawing::Point(456, 297);
			this->curValue->Name = L"curValue";
			this->curValue->Size = System::Drawing::Size(85, 30);
			this->curValue->TabIndex = 5;
			this->curValue->Text = L"cvalue";
			// 
			// curMinus
			// 
			this->curMinus->BackColor = System::Drawing::SystemColors::Highlight;
			this->curMinus->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->curMinus->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->curMinus->ImageAlign = System::Drawing::ContentAlignment::BottomCenter;
			this->curMinus->Location = System::Drawing::Point(375, 299);
			this->curMinus->Name = L"curMinus";
			this->curMinus->Size = System::Drawing::Size(75, 34);
			this->curMinus->TabIndex = 4;
			this->curMinus->Text = L"-";
			this->curMinus->UseVisualStyleBackColor = false;
			// 
			// pLabel
			// 
			this->pLabel->AutoSize = true;
			this->pLabel->Font = (gcnew System::Drawing::Font(L"Berlin Sans FB", 20.25F));
			this->pLabel->Location = System::Drawing::Point(53, 264);
			this->pLabel->Name = L"pLabel";
			this->pLabel->Size = System::Drawing::Size(170, 30);
			this->pLabel->TabIndex = 11;
			this->pLabel->Text = L"REPETITIONS";
			// 
			// repPlus
			// 
			this->repPlus->BackColor = System::Drawing::SystemColors::Highlight;
			this->repPlus->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->repPlus->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->repPlus->ImageAlign = System::Drawing::ContentAlignment::BottomCenter;
			this->repPlus->Location = System::Drawing::Point(184, 299);
			this->repPlus->Name = L"repPlus";
			this->repPlus->Size = System::Drawing::Size(75, 34);
			this->repPlus->TabIndex = 10;
			this->repPlus->Text = L"+";
			this->repPlus->UseVisualStyleBackColor = false;
			// 
			// repValue
			// 
			this->repValue->AutoSize = true;
			this->repValue->Font = (gcnew System::Drawing::Font(L"Berlin Sans FB", 20.25F));
			this->repValue->Location = System::Drawing::Point(92, 297);
			this->repValue->Name = L"repValue";
			this->repValue->Size = System::Drawing::Size(89, 30);
			this->repValue->TabIndex = 9;
			this->repValue->Text = L"pvalue";
			// 
			// repMinus
			// 
			this->repMinus->BackColor = System::Drawing::SystemColors::Highlight;
			this->repMinus->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->repMinus->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->repMinus->ImageAlign = System::Drawing::ContentAlignment::BottomCenter;
			this->repMinus->Location = System::Drawing::Point(11, 299);
			this->repMinus->Name = L"repMinus";
			this->repMinus->Size = System::Drawing::Size(75, 34);
			this->repMinus->TabIndex = 8;
			this->repMinus->Text = L"-";
			this->repMinus->UseVisualStyleBackColor = false;
			// 
			// enButton
			// 
			this->enButton->BackColor = System::Drawing::SystemColors::ActiveCaption;
			this->enButton->Font = (gcnew System::Drawing::Font(L"MS Reference Sans Serif", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->enButton->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->enButton->ImageAlign = System::Drawing::ContentAlignment::BottomCenter;
			this->enButton->Location = System::Drawing::Point(590, 124);
			this->enButton->Name = L"enButton";
			this->enButton->Size = System::Drawing::Size(109, 66);
			this->enButton->TabIndex = 12;
			this->enButton->Text = L"ENABLE CHANNEL";
			this->enButton->UseVisualStyleBackColor = false;
			// 
			// selectCH
			// 
			this->selectCH->Font = (gcnew System::Drawing::Font(L"MS Reference Sans Serif", 20.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->selectCH->Items->Add(L"Red channel");
			this->selectCH->Items->Add(L"Blue channel");
			this->selectCH->Items->Add(L"Black channel");
			this->selectCH->Items->Add(L"White channel");
			this->selectCH->Location = System::Drawing::Point(372, 135);
			this->selectCH->Name = L"selectCH";
			this->selectCH->Size = System::Drawing::Size(197, 40);
			this->selectCH->TabIndex = 15;
			this->selectCH->Text = L"Sel Channel";
			this->selectCH->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// mainTitle
			// 
			this->mainTitle->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->mainTitle->Font = (gcnew System::Drawing::Font(L"Trebuchet MS", 39.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->mainTitle->Location = System::Drawing::Point(-2, -1);
			this->mainTitle->Name = L"mainTitle";
			this->mainTitle->Size = System::Drawing::Size(710, 77);
			this->mainTitle->TabIndex = 16;
			this->mainTitle->Text = L"SENSE AND STIMULATING";
			this->mainTitle->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			this->mainTitle->Click += gcnew System::EventHandler(this, &MyForm::mainTitle_Click);
			// 
			// startStim
			// 
			this->startStim->BackColor = System::Drawing::SystemColors::GrayText;
			this->startStim->Cursor = System::Windows::Forms::Cursors::Default;
			this->startStim->Font = (gcnew System::Drawing::Font(L"MS Reference Sans Serif", 14.25F));
			this->startStim->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->startStim->ImageAlign = System::Drawing::ContentAlignment::BottomCenter;
			this->startStim->Location = System::Drawing::Point(387, 408);
			this->startStim->Name = L"startStim";
			this->startStim->Size = System::Drawing::Size(109, 66);
			this->startStim->TabIndex = 17;
			this->startStim->Text = L"START STIM";
			this->startStim->UseVisualStyleBackColor = false;
			// 
			// quitStim
			// 
			this->quitStim->BackColor = System::Drawing::SystemColors::GrayText;
			this->quitStim->Cursor = System::Windows::Forms::Cursors::Default;
			this->quitStim->Font = (gcnew System::Drawing::Font(L"MS Reference Sans Serif", 14.25F));
			this->quitStim->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->quitStim->ImageAlign = System::Drawing::ContentAlignment::BottomCenter;
			this->quitStim->Location = System::Drawing::Point(502, 408);
			this->quitStim->Name = L"quitStim";
			this->quitStim->Size = System::Drawing::Size(109, 66);
			this->quitStim->TabIndex = 18;
			this->quitStim->Text = L"QUIT STIM";
			this->quitStim->UseVisualStyleBackColor = false;
			// 
			// stimTitle
			// 
			this->stimTitle->AutoSize = true;
			this->stimTitle->Font = (gcnew System::Drawing::Font(L"Trebuchet MS", 27.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->stimTitle->Location = System::Drawing::Point(405, 77);
			this->stimTitle->Name = L"stimTitle";
			this->stimTitle->Size = System::Drawing::Size(248, 46);
			this->stimTitle->TabIndex = 19;
			this->stimTitle->Text = L"STIMULATION";
			this->stimTitle->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// statusTitle
			// 
			this->statusTitle->AutoSize = true;
			this->statusTitle->Font = (gcnew System::Drawing::Font(L"Trebuchet MS", 27.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->statusTitle->Location = System::Drawing::Point(96, 77);
			this->statusTitle->Name = L"statusTitle";
			this->statusTitle->Size = System::Drawing::Size(144, 46);
			this->statusTitle->TabIndex = 20;
			this->statusTitle->Text = L"STATUS";
			this->statusTitle->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// statusGrid
			// 
			this->statusGrid->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->statusGrid->Font = (gcnew System::Drawing::Font(L"Trebuchet MS", 27.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->statusGrid->Location = System::Drawing::Point(-2, 76);
			this->statusGrid->Name = L"statusGrid";
			this->statusGrid->Size = System::Drawing::Size(352, 408);
			this->statusGrid->TabIndex = 21;
			this->statusGrid->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			this->statusGrid->Click += gcnew System::EventHandler(this, &MyForm::label2_Click);
			//
			// backgroundWorker1
			//
			this->backgroundWorker1->WorkerSupportsCancellation = true;
			this->backgroundWorker1->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &MyForm::backgroundWorker1_DoWork);
			this->backgroundWorker1->RunWorkerCompleted += gcnew System::ComponentModel::RunWorkerCompletedEventHandler(this, &MyForm::backgroundWorker1_RunWorkerCompleted);
			//
			// 
			// label2
			// 
			this->label2->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->label2->Font = (gcnew System::Drawing::Font(L"Trebuchet MS", 27.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label2->Location = System::Drawing::Point(346, 76);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(362, 408);
			this->label2->TabIndex = 22;
			this->label2->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// stateLabel
			// 
			this->stateLabel->Font = (gcnew System::Drawing::Font(L"Berlin Sans FB", 20.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->stateLabel->Location = System::Drawing::Point(12, 135);
			this->stateLabel->Name = L"stateLabel";
			this->stateLabel->Size = System::Drawing::Size(300, 30);
			this->stateLabel->TabIndex = 23;
			this->stateLabel->Text = L"STATE";
			// 
			// stateMessage
			// 
			this->stateMessage->Font = (gcnew System::Drawing::Font(L"Berlin Sans FB", 20.25F));
			this->stateMessage->Location = System::Drawing::Point(12, 180);
			this->stateMessage->Name = L"stateMessage";
			this->stateMessage->Size = System::Drawing::Size(300, 69);
			this->stateMessage->TabIndex = 24;
			this->stateMessage->Text = L"pvalue";
			// 
			// thButton
			// 
			this->thButton->BackColor = System::Drawing::SystemColors::Highlight;
			this->thButton->Cursor = System::Windows::Forms::Cursors::Default;
			this->thButton->Font = (gcnew System::Drawing::Font(L"MS Reference Sans Serif", 14.25F));
			this->thButton->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->thButton->ImageAlign = System::Drawing::ContentAlignment::BottomCenter;
			this->thButton->Location = System::Drawing::Point(6, 402);
			this->thButton->Name = L"thButton";
			this->thButton->Size = System::Drawing::Size(109, 66);
			this->thButton->TabIndex = 25;
			this->thButton->Text = L"SET TH";
			this->thButton->UseVisualStyleBackColor = false;
			// 
			// startButton
			// 
			this->startButton->BackColor = System::Drawing::SystemColors::Highlight;
			this->startButton->Cursor = System::Windows::Forms::Cursors::Default;
			this->startButton->Font = (gcnew System::Drawing::Font(L"MS Reference Sans Serif", 14.25F));
			this->startButton->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->startButton->ImageAlign = System::Drawing::ContentAlignment::BottomCenter;
			this->startButton->Location = System::Drawing::Point(121, 402);
			this->startButton->Name = L"startButton";
			this->startButton->Size = System::Drawing::Size(109, 66);
			this->startButton->TabIndex = 26;
			this->startButton->Text = L"START TRAIN";
			this->startButton->UseVisualStyleBackColor = false;
			this->startButton->Click += gcnew System::EventHandler(this, &MyForm::button2_Click);
			// 
			// autoButton
			// 
			this->autoButton->BackColor = System::Drawing::SystemColors::GrayText;
			this->autoButton->Cursor = System::Windows::Forms::Cursors::Default;
			this->autoButton->Font = (gcnew System::Drawing::Font(L"MS Reference Sans Serif", 14.25F));
			this->autoButton->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->autoButton->ImageAlign = System::Drawing::ContentAlignment::BottomCenter;
			this->autoButton->Location = System::Drawing::Point(355, 487);
			this->autoButton->Name = L"autoButton";
			this->autoButton->Size = System::Drawing::Size(109, 66);
			this->autoButton->TabIndex = 27;
			this->autoButton->Text = L"AUTO";
			this->autoButton->UseVisualStyleBackColor = false;
			// 
			// manButton
			// 
			this->manButton->BackColor = System::Drawing::SystemColors::GrayText;
			this->manButton->Cursor = System::Windows::Forms::Cursors::Default;
			this->manButton->Font = (gcnew System::Drawing::Font(L"MS Reference Sans Serif", 14.25F));
			this->manButton->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->manButton->ImageAlign = System::Drawing::ContentAlignment::BottomCenter;
			this->manButton->Location = System::Drawing::Point(470, 487);
			this->manButton->Name = L"manButton";
			this->manButton->Size = System::Drawing::Size(109, 66);
			this->manButton->TabIndex = 28;
			this->manButton->Text = L"MANUAL";
			this->manButton->UseVisualStyleBackColor = false;
			// 
			// xButton
			// 
			this->xButton->BackColor = System::Drawing::SystemColors::InactiveCaptionText;
			this->xButton->Cursor = System::Windows::Forms::Cursors::Default;
			this->xButton->Font = (gcnew System::Drawing::Font(L"MS Reference Sans Serif", 14.25F));
			this->xButton->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->xButton->ImageAlign = System::Drawing::ContentAlignment::BottomCenter;
			this->xButton->Location = System::Drawing::Point(585, 487);
			this->xButton->Name = L"xButton";
			this->xButton->Size = System::Drawing::Size(109, 66);
			this->xButton->TabIndex = 29;
			this->xButton->Text = L"X";
			this->xButton->UseVisualStyleBackColor = false;
			// 
			// abortEx
			// 
			this->abortEx->BackColor = System::Drawing::SystemColors::Highlight;
			this->abortEx->Cursor = System::Windows::Forms::Cursors::Default;
			this->abortEx->Font = (gcnew System::Drawing::Font(L"MS Reference Sans Serif", 14.25F));
			this->abortEx->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->abortEx->ImageAlign = System::Drawing::ContentAlignment::BottomCenter;
			this->abortEx->Location = System::Drawing::Point(236, 402);
			this->abortEx->Name = L"abortEx";
			this->abortEx->Size = System::Drawing::Size(109, 66);
			this->abortEx->TabIndex = 30;
			this->abortEx->Text = L"END EXER";
			this->abortEx->UseVisualStyleBackColor = false;
			// 
			// calTitle
			// 
			this->calTitle->AutoSize = true;
			this->calTitle->Font = (gcnew System::Drawing::Font(L"Trebuchet MS", 27.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->calTitle->Location = System::Drawing::Point(56, 495);
			this->calTitle->Name = L"calTitle";
			this->calTitle->Size = System::Drawing::Size(244, 46);
			this->calTitle->TabIndex = 31;
			this->calTitle->Text = L"CALIBRATION";
			this->calTitle->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			this->calTitle->Click += gcnew System::EventHandler(this, &MyForm::label1_Click_2);
			// 
			// repeatButton
			// 
			this->repeatButton->BackColor = System::Drawing::SystemColors::GrayText;
			this->repeatButton->Cursor = System::Windows::Forms::Cursors::Default;
			this->repeatButton->Font = (gcnew System::Drawing::Font(L"MS Reference Sans Serif", 14.25F));
			this->repeatButton->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->repeatButton->ImageAlign = System::Drawing::ContentAlignment::BottomCenter;
			this->repeatButton->Location = System::Drawing::Point(355, 559);
			this->repeatButton->Name = L"repeatButton";
			this->repeatButton->Size = System::Drawing::Size(109, 66);
			this->repeatButton->TabIndex = 32;
			this->repeatButton->Text = L"SAME EXER";
			this->repeatButton->UseVisualStyleBackColor = false;
			// 
			// newButton
			// 
			this->newButton->BackColor = System::Drawing::SystemColors::GrayText;
			this->newButton->Cursor = System::Windows::Forms::Cursors::Default;
			this->newButton->Font = (gcnew System::Drawing::Font(L"MS Reference Sans Serif", 14.25F));
			this->newButton->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->newButton->ImageAlign = System::Drawing::ContentAlignment::BottomCenter;
			this->newButton->Location = System::Drawing::Point(470, 559);
			this->newButton->Name = L"newButton";
			this->newButton->Size = System::Drawing::Size(109, 66);
			this->newButton->TabIndex = 33;
			this->newButton->Text = L"OTHER EXER";
			this->newButton->UseVisualStyleBackColor = false;
			// 
			// label1
			// 
			this->label1->Font = (gcnew System::Drawing::Font(L"Trebuchet MS", 27.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(11, 571);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(289, 46);
			this->label1->TabIndex = 34;
			this->label1->Text = L"NEXT EXERCISE";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::ButtonHighlight;
			this->BackgroundImageLayout = System::Windows::Forms::ImageLayout::None;
			this->ClientSize = System::Drawing::Size(704, 630);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->newButton);
			this->Controls->Add(this->repeatButton);
			this->Controls->Add(this->calTitle);
			this->Controls->Add(this->abortEx);
			this->Controls->Add(this->xButton);
			this->Controls->Add(this->manButton);
			this->Controls->Add(this->autoButton);
			this->Controls->Add(this->startButton);
			this->Controls->Add(this->thButton);
			this->Controls->Add(this->stateMessage);
			this->Controls->Add(this->stateLabel);
			this->Controls->Add(this->statusTitle);
			this->Controls->Add(this->stimTitle);
			this->Controls->Add(this->quitStim);
			this->Controls->Add(this->startStim);
			this->Controls->Add(this->mainTitle);
			this->Controls->Add(this->selectCH);
			this->Controls->Add(this->enButton);
			this->Controls->Add(this->pLabel);
			this->Controls->Add(this->repPlus);
			this->Controls->Add(this->repValue);
			this->Controls->Add(this->repMinus);
			this->Controls->Add(this->curLabel);
			this->Controls->Add(this->curPlus);
			this->Controls->Add(this->curValue);
			this->Controls->Add(this->curMinus);
			this->Controls->Add(this->rampLabel);
			this->Controls->Add(this->rampPlus);
			this->Controls->Add(this->rampValue);
			this->Controls->Add(this->rampMinus);
			this->Controls->Add(this->statusGrid);
			this->Controls->Add(this->label2);
			this->Name = L"MyForm";
			this->Text = L"SAS";
			this->ResumeLayout(false);
			this->PerformLayout();

		}


#pragma endregion
	private: System::Void curLabel_Click(System::Object^ sender, System::EventArgs^ e) {
		GUI_GLOBAL.dummy = 1;
		// Close background task
		this->backgroundWorker1->CancelAsync();
	}
private: System::Void mainTitle_Click(System::Object^ sender, System::EventArgs^ e) {
	// Start background 
	this->backgroundWorker1->RunWorkerAsync(2000);
}
private: System::Void label2_Click(System::Object^ sender, System::EventArgs^ e) {
}
private: System::Void button2_Click(System::Object^ sender, System::EventArgs^ e) {
}
private: System::Void label1_Click_2(System::Object^ sender, System::EventArgs^ e) {
}

	   //void backgroundWorker1_DoWork(Object sender, EventArgs e)
private: System::Void backgroundWorker1_DoWork(System::Object^ sender, DoWorkEventArgs^ e)
	   {
		   // Do not access the form's BackgroundWorker reference directly.
		   // Instead, use the reference provided by the sender parameter.
		   //BackgroundWorker bw; // = sender();// as BackgroundWorker;

		   // Extract the argument.
		   int arg = (int)e->Argument;

		   // Start the time-consuming operation.
		   e->Result = TimeConsumingOperation(arg);

		   // If the operation was canceled by the user,
		   // set the DoWorkEventArgs.Cancel property to true.
		   if (this->backgroundWorker1->CancellationPending)
		   {
			   e->Cancel = true;
		   }
	   }

	   // This event handler demonstrates how to interpret
	   // the outcome of the asynchronous operation implemented
	   // in the DoWork event handler.
private: System::Void backgroundWorker1_RunWorkerCompleted(System::Object^ sender, RunWorkerCompletedEventArgs^ e)
	   {
		   if (e->Cancelled)
		   {
			   // The user canceled the operation.
			   //MessageBox->Show("Operation was canceled");
			   this->displayStuff->Show("Operation was canceled");
		   }
		   else if (e->Error)
		   {
			   // There was an error during the operation.
			   //string msg = "An error occurred: {0}" + e.Error.Message;
			   //MessageBox.Show(msg);
			   this->displayStuff->Show("An error occurred: {0}");
		   }
		   else
		   {
			   // The operation completed normally.
			   //string msg = String.Format("Result = {0}", e.Result);
			   //System::Windows::Forms::MessageBox::Show(msg);
			   this->displayStuff->Show("Result = {0}");
		   }
	   }

	   int TimeConsumingOperation(int sleepPeriod)
	   {
		   int result = 0;

		   //Random rand();// = new Random();

		   random_device rand;
		   mt19937 gen(rand());

		   while (!this->backgroundWorker1->CancellationPending)
		   {
			   bool exit = false;
			   switch (gen())
			   {
				   // Raise an exception.
			   case 0:
			   {
				   throw gcnew System::Exception("An error condition occurred.");
				   //throw new Exception("An error condition occurred.");
				   break;
			   }

			   // Sleep for the number of milliseconds
			   // specified by the sleepPeriod parameter.
			   case 1:
			   {
				   System::Threading::Thread::Sleep(sleepPeriod);
				   break;
			   }

			   // Exit and return normally.
			   case 2:
			   {
				   result = 23;
				   exit = true;
				   break;
			   }

			   default:
			   {
				   break;
			   }
			   }

			   if (exit)
			   {
				   break;
			   }
		   }

		   return result;
	   }
};
}
