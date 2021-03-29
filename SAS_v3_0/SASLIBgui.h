#pragma once
#include <random>
#include <string>

#include "SASLIBshared.h"
#include "SASLIBbasic.hpp"
/* ---> Some notes on the background run:
* It requires a Backgroundworker type field
* Some Background fields need to be associated with methods and set up
* A backgroundworker starts with RunWorkerAsync and ends with CancelAsync
* A backgroundworker must run from beginning to end before being started again
* The method assigned to DoWorkEventHandler runs when RunWorkerAsync is called
* The method assigned to RunWorkerCompletedEventHandler will run when the CancelAsync is called
* The method assigned to ProgressChangedEventHandler is called indirectly from DoWorkEventHandler
* ---> In this project:
* There is only 1 background worker, which updates the UI
* It is launched only 1 at the begining (constructor)
* It ends among the main program
* As a ref class, it cannot be set as a global variable, so it communicates with the rest of SAS
* through a class-object, declared as global variable, where both (SAS thread and GUI) read and write
* */
namespace SASv30 {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for MyForm
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
		{
			InitializeComponent();
			InitializeSASGUI(); // This MUST run after the components init
			// Start Background
			dummy = 0;
			this->backgroundWorker1->RunWorkerAsync(2000);
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
	// Class fields
	public:
		int dummy, nextIndex, nextExercise;
		const int UPDATE_PERIOD_MS = 100;
		// SAS <-> GUI fields
		RehaMove3_Req_Type Move3_gui;
		User_Req_Type user_gui;
		state_Type state;
		bool bUser, bMove3, bSetupPressed;
		bool trainStart, stimA_active, stimM_active, gui_repeat, gui_new;
		//char intStr[32];

		// For isMoving testing
		double isVelocity;
		const double isDelta = 0.1;
		
		System::ComponentModel::BackgroundWorker^ backgroundWorker1;
		System::Windows::Forms::Label^ statusMsg;
		System::Windows::Forms::Label^ statusLabel;
		System::Windows::Forms::Label^ statusTitle;
		System::Windows::Forms::Label^ stimTitle;
		System::Windows::Forms::Label^ exerciseTitle;
		System::Windows::Forms::Button^ thButton;
		System::Windows::Forms::Label^ rampTitle;
		System::Windows::Forms::Label^ rampValue;
		System::Windows::Forms::Button^ rampPlus;
		System::Windows::Forms::Button^ rampMinus;
		System::Windows::Forms::Button^ curMinus;
		System::Windows::Forms::Button^ curPlus;
		System::Windows::Forms::Label^ curValue;
		System::Windows::Forms::Label^ curTitle;
		System::Windows::Forms::Button^ fqMinus;
		System::Windows::Forms::Button^ fqPlus;
		System::Windows::Forms::Label^ fqValue;
		System::Windows::Forms::Label^ fqTitle;
		System::Windows::Forms::Label^ statusDebug;
		System::Windows::Forms::Button^ stimButton;
		System::Windows::Forms::Label^ currentTitle;
		System::Windows::Forms::Label^ nextTitle;
		System::Windows::Forms::Label^ currentLabel;
		System::Windows::Forms::ComboBox^ methodBox;
		System::Windows::Forms::Label^ methodLabel;
		System::Windows::Forms::Button^ velMinus;
		System::Windows::Forms::Button^ velPlus;
		System::Windows::Forms::Label^ velValue;
		System::Windows::Forms::Label^ velTitle;
		System::Windows::Forms::Label^ exerciseLabel;
		System::Windows::Forms::ComboBox^ exerciseBox;
		System::Windows::Forms::Label^ currentExLabel;
		System::Windows::Forms::Label^ nextExTitle;
		System::Windows::Forms::Label^ currentExTitle;
	public: System::Windows::Forms::Label^ velCurrent;
	private: System::Windows::Forms::BindingSource^ bindingSource1;
	public:
	private: System::ComponentModel::IContainer^ components;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->backgroundWorker1 = (gcnew System::ComponentModel::BackgroundWorker());
			this->statusMsg = (gcnew System::Windows::Forms::Label());
			this->statusLabel = (gcnew System::Windows::Forms::Label());
			this->statusTitle = (gcnew System::Windows::Forms::Label());
			this->stimTitle = (gcnew System::Windows::Forms::Label());
			this->exerciseTitle = (gcnew System::Windows::Forms::Label());
			this->thButton = (gcnew System::Windows::Forms::Button());
			this->rampTitle = (gcnew System::Windows::Forms::Label());
			this->rampValue = (gcnew System::Windows::Forms::Label());
			this->rampPlus = (gcnew System::Windows::Forms::Button());
			this->rampMinus = (gcnew System::Windows::Forms::Button());
			this->curMinus = (gcnew System::Windows::Forms::Button());
			this->curPlus = (gcnew System::Windows::Forms::Button());
			this->curValue = (gcnew System::Windows::Forms::Label());
			this->curTitle = (gcnew System::Windows::Forms::Label());
			this->fqMinus = (gcnew System::Windows::Forms::Button());
			this->fqPlus = (gcnew System::Windows::Forms::Button());
			this->fqValue = (gcnew System::Windows::Forms::Label());
			this->fqTitle = (gcnew System::Windows::Forms::Label());
			this->statusDebug = (gcnew System::Windows::Forms::Label());
			this->stimButton = (gcnew System::Windows::Forms::Button());
			this->currentTitle = (gcnew System::Windows::Forms::Label());
			this->nextTitle = (gcnew System::Windows::Forms::Label());
			this->currentLabel = (gcnew System::Windows::Forms::Label());
			this->methodBox = (gcnew System::Windows::Forms::ComboBox());
			this->methodLabel = (gcnew System::Windows::Forms::Label());
			this->velMinus = (gcnew System::Windows::Forms::Button());
			this->velPlus = (gcnew System::Windows::Forms::Button());
			this->velValue = (gcnew System::Windows::Forms::Label());
			this->velTitle = (gcnew System::Windows::Forms::Label());
			this->exerciseLabel = (gcnew System::Windows::Forms::Label());
			this->exerciseBox = (gcnew System::Windows::Forms::ComboBox());
			this->currentExLabel = (gcnew System::Windows::Forms::Label());
			this->nextExTitle = (gcnew System::Windows::Forms::Label());
			this->currentExTitle = (gcnew System::Windows::Forms::Label());
			this->velCurrent = (gcnew System::Windows::Forms::Label());
			this->bindingSource1 = (gcnew System::Windows::Forms::BindingSource(this->components));
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->bindingSource1))->BeginInit();
			this->SuspendLayout();
			// 
			// backgroundWorker1
			// 
			this->backgroundWorker1->WorkerReportsProgress = true;
			this->backgroundWorker1->WorkerSupportsCancellation = true;
			this->backgroundWorker1->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &MyForm::launchBackground);
			this->backgroundWorker1->ProgressChanged += gcnew System::ComponentModel::ProgressChangedEventHandler(this, &MyForm::updateBackground);
			this->backgroundWorker1->RunWorkerCompleted += gcnew System::ComponentModel::RunWorkerCompletedEventHandler(this, &MyForm::endBackground);
			// 
			// statusMsg
			// 
			this->statusMsg->Location = System::Drawing::Point(249, 12);
			this->statusMsg->Name = L"statusMsg";
			this->statusMsg->Size = System::Drawing::Size(323, 86);
			this->statusMsg->TabIndex = 4;
			this->statusMsg->Text = L"status message here";
			// 
			// statusLabel
			// 
			this->statusLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->statusLabel->Location = System::Drawing::Point(12, 40);
			this->statusLabel->Name = L"statusLabel";
			this->statusLabel->Size = System::Drawing::Size(213, 51);
			this->statusLabel->TabIndex = 5;
			this->statusLabel->Text = L"status Label here";
			// 
			// statusTitle
			// 
			this->statusTitle->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->statusTitle->ForeColor = System::Drawing::SystemColors::Highlight;
			this->statusTitle->Location = System::Drawing::Point(7, 9);
			this->statusTitle->Name = L"statusTitle";
			this->statusTitle->Size = System::Drawing::Size(236, 31);
			this->statusTitle->TabIndex = 6;
			this->statusTitle->Text = L"PROGRAM STATUS:";
			// 
			// stimTitle
			// 
			this->stimTitle->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->stimTitle->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->stimTitle->ForeColor = System::Drawing::SystemColors::Highlight;
			this->stimTitle->Location = System::Drawing::Point(-1, 104);
			this->stimTitle->Name = L"stimTitle";
			this->stimTitle->Size = System::Drawing::Size(300, 461);
			this->stimTitle->TabIndex = 7;
			this->stimTitle->Text = L"STIMULATION";
			this->stimTitle->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// exerciseTitle
			// 
			this->exerciseTitle->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->exerciseTitle->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->exerciseTitle->ForeColor = System::Drawing::SystemColors::Highlight;
			this->exerciseTitle->Location = System::Drawing::Point(298, 104);
			this->exerciseTitle->Name = L"exerciseTitle";
			this->exerciseTitle->Size = System::Drawing::Size(300, 461);
			this->exerciseTitle->TabIndex = 8;
			this->exerciseTitle->Text = L"EXERCISE SETTINGS";
			this->exerciseTitle->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// thButton
			// 
			this->thButton->BackColor = System::Drawing::SystemColors::Highlight;
			this->thButton->FlatAppearance->BorderColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(192)),
				static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->thButton->FlatAppearance->BorderSize = 0;
			this->thButton->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->thButton->Font = (gcnew System::Drawing::Font(L"Microsoft JhengHei UI", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->thButton->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->thButton->Location = System::Drawing::Point(321, 332);
			this->thButton->Name = L"thButton";
			this->thButton->Size = System::Drawing::Size(251, 78);
			this->thButton->TabIndex = 10;
			this->thButton->Text = L"SET THRESHOLD";
			this->thButton->UseVisualStyleBackColor = false;
			this->thButton->Click += gcnew System::EventHandler(this, &MyForm::click_thButton);
			// 
			// rampTitle
			// 
			this->rampTitle->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->rampTitle->ForeColor = System::Drawing::SystemColors::ControlText;
			this->rampTitle->Location = System::Drawing::Point(68, 144);
			this->rampTitle->Name = L"rampTitle";
			this->rampTitle->Size = System::Drawing::Size(175, 31);
			this->rampTitle->TabIndex = 14;
			this->rampTitle->Text = L"RAMP";
			this->rampTitle->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// rampValue
			// 
			this->rampValue->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->rampValue->ForeColor = System::Drawing::SystemColors::Highlight;
			this->rampValue->Location = System::Drawing::Point(68, 188);
			this->rampValue->Name = L"rampValue";
			this->rampValue->Size = System::Drawing::Size(175, 31);
			this->rampValue->TabIndex = 15;
			this->rampValue->Text = L"RAMP";
			this->rampValue->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// rampPlus
			// 
			this->rampPlus->BackColor = System::Drawing::SystemColors::Highlight;
			this->rampPlus->FlatAppearance->BorderColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(192)),
				static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->rampPlus->FlatAppearance->BorderSize = 0;
			this->rampPlus->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->rampPlus->Font = (gcnew System::Drawing::Font(L"Microsoft JhengHei UI", 20.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->rampPlus->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->rampPlus->Location = System::Drawing::Point(232, 144);
			this->rampPlus->Name = L"rampPlus";
			this->rampPlus->Size = System::Drawing::Size(60, 78);
			this->rampPlus->TabIndex = 16;
			this->rampPlus->Text = L"+";
			this->rampPlus->UseVisualStyleBackColor = false;
			this->rampPlus->Click += gcnew System::EventHandler(this, &MyForm::click_rampPlus);
			// 
			// rampMinus
			// 
			this->rampMinus->BackColor = System::Drawing::SystemColors::Highlight;
			this->rampMinus->FlatAppearance->BorderColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(192)),
				static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->rampMinus->FlatAppearance->BorderSize = 0;
			this->rampMinus->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->rampMinus->Font = (gcnew System::Drawing::Font(L"Microsoft JhengHei UI", 20.25F));
			this->rampMinus->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->rampMinus->Location = System::Drawing::Point(15, 144);
			this->rampMinus->Name = L"rampMinus";
			this->rampMinus->Size = System::Drawing::Size(60, 78);
			this->rampMinus->TabIndex = 17;
			this->rampMinus->Text = L"-";
			this->rampMinus->UseVisualStyleBackColor = false;
			this->rampMinus->Click += gcnew System::EventHandler(this, &MyForm::click_rampMinus);
			// 
			// curMinus
			// 
			this->curMinus->BackColor = System::Drawing::SystemColors::Highlight;
			this->curMinus->FlatAppearance->BorderColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(192)),
				static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->curMinus->FlatAppearance->BorderSize = 0;
			this->curMinus->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->curMinus->Font = (gcnew System::Drawing::Font(L"Microsoft JhengHei UI", 20.25F));
			this->curMinus->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->curMinus->Location = System::Drawing::Point(15, 238);
			this->curMinus->Name = L"curMinus";
			this->curMinus->Size = System::Drawing::Size(60, 78);
			this->curMinus->TabIndex = 21;
			this->curMinus->Text = L"-";
			this->curMinus->UseVisualStyleBackColor = false;
			this->curMinus->Click += gcnew System::EventHandler(this, &MyForm::click_curMinus);
			// 
			// curPlus
			// 
			this->curPlus->BackColor = System::Drawing::SystemColors::Highlight;
			this->curPlus->FlatAppearance->BorderColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(192)),
				static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->curPlus->FlatAppearance->BorderSize = 0;
			this->curPlus->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->curPlus->Font = (gcnew System::Drawing::Font(L"Microsoft JhengHei UI", 20.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->curPlus->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->curPlus->Location = System::Drawing::Point(232, 238);
			this->curPlus->Name = L"curPlus";
			this->curPlus->Size = System::Drawing::Size(60, 78);
			this->curPlus->TabIndex = 20;
			this->curPlus->Text = L"+";
			this->curPlus->UseVisualStyleBackColor = false;
			this->curPlus->Click += gcnew System::EventHandler(this, &MyForm::click_curPlus);
			// 
			// curValue
			// 
			this->curValue->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->curValue->ForeColor = System::Drawing::SystemColors::Highlight;
			this->curValue->Location = System::Drawing::Point(68, 282);
			this->curValue->Name = L"curValue";
			this->curValue->Size = System::Drawing::Size(175, 31);
			this->curValue->TabIndex = 19;
			this->curValue->Text = L"CURRENT";
			this->curValue->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// curTitle
			// 
			this->curTitle->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->curTitle->ForeColor = System::Drawing::SystemColors::ControlText;
			this->curTitle->Location = System::Drawing::Point(68, 238);
			this->curTitle->Name = L"curTitle";
			this->curTitle->Size = System::Drawing::Size(175, 31);
			this->curTitle->TabIndex = 18;
			this->curTitle->Text = L"AMPLITUDE";
			this->curTitle->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// fqMinus
			// 
			this->fqMinus->BackColor = System::Drawing::SystemColors::Highlight;
			this->fqMinus->FlatAppearance->BorderColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(192)),
				static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->fqMinus->FlatAppearance->BorderSize = 0;
			this->fqMinus->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->fqMinus->Font = (gcnew System::Drawing::Font(L"Microsoft JhengHei UI", 20.25F));
			this->fqMinus->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->fqMinus->Location = System::Drawing::Point(15, 330);
			this->fqMinus->Name = L"fqMinus";
			this->fqMinus->Size = System::Drawing::Size(60, 78);
			this->fqMinus->TabIndex = 25;
			this->fqMinus->Text = L"-";
			this->fqMinus->UseVisualStyleBackColor = false;
			this->fqMinus->Click += gcnew System::EventHandler(this, &MyForm::click_fqMinus);
			// 
			// fqPlus
			// 
			this->fqPlus->BackColor = System::Drawing::SystemColors::Highlight;
			this->fqPlus->FlatAppearance->BorderColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(192)),
				static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->fqPlus->FlatAppearance->BorderSize = 0;
			this->fqPlus->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->fqPlus->Font = (gcnew System::Drawing::Font(L"Microsoft JhengHei UI", 20.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->fqPlus->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->fqPlus->Location = System::Drawing::Point(232, 330);
			this->fqPlus->Name = L"fqPlus";
			this->fqPlus->Size = System::Drawing::Size(60, 78);
			this->fqPlus->TabIndex = 24;
			this->fqPlus->Text = L"+";
			this->fqPlus->UseVisualStyleBackColor = false;
			this->fqPlus->Click += gcnew System::EventHandler(this, &MyForm::click_fqPlus);
			// 
			// fqValue
			// 
			this->fqValue->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->fqValue->ForeColor = System::Drawing::SystemColors::Highlight;
			this->fqValue->Location = System::Drawing::Point(68, 374);
			this->fqValue->Name = L"fqValue";
			this->fqValue->Size = System::Drawing::Size(175, 31);
			this->fqValue->TabIndex = 23;
			this->fqValue->Text = L"HZ";
			this->fqValue->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// fqTitle
			// 
			this->fqTitle->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->fqTitle->ForeColor = System::Drawing::SystemColors::ControlText;
			this->fqTitle->Location = System::Drawing::Point(68, 330);
			this->fqTitle->Name = L"fqTitle";
			this->fqTitle->Size = System::Drawing::Size(175, 31);
			this->fqTitle->TabIndex = 22;
			this->fqTitle->Text = L"FREQUENCY";
			this->fqTitle->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// statusDebug
			// 
			this->statusDebug->Location = System::Drawing::Point(22, 539);
			this->statusDebug->Name = L"statusDebug";
			this->statusDebug->Size = System::Drawing::Size(213, 13);
			this->statusDebug->TabIndex = 28;
			this->statusDebug->Text = L"status debug text here";
			// 
			// stimButton
			// 
			this->stimButton->BackColor = System::Drawing::SystemColors::Highlight;
			this->stimButton->FlatAppearance->BorderColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(192)),
				static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->stimButton->FlatAppearance->BorderSize = 0;
			this->stimButton->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->stimButton->Font = (gcnew System::Drawing::Font(L"Microsoft JhengHei UI", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->stimButton->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->stimButton->Location = System::Drawing::Point(28, 416);
			this->stimButton->Name = L"stimButton";
			this->stimButton->Size = System::Drawing::Size(251, 78);
			this->stimButton->TabIndex = 31;
			this->stimButton->Text = L"START/STOP STIMULATION";
			this->stimButton->UseVisualStyleBackColor = false;
			this->stimButton->Click += gcnew System::EventHandler(this, &MyForm::click_stimButton);
			// 
			// currentTitle
			// 
			this->currentTitle->Location = System::Drawing::Point(309, 262);
			this->currentTitle->Name = L"currentTitle";
			this->currentTitle->Size = System::Drawing::Size(121, 16);
			this->currentTitle->TabIndex = 33;
			this->currentTitle->Text = L"CURRENT METHOD:";
			this->currentTitle->TextAlign = System::Drawing::ContentAlignment::TopRight;
			// 
			// nextTitle
			// 
			this->nextTitle->Location = System::Drawing::Point(309, 278);
			this->nextTitle->Name = L"nextTitle";
			this->nextTitle->Size = System::Drawing::Size(121, 28);
			this->nextTitle->TabIndex = 34;
			this->nextTitle->Text = L"METHOD FOR NEXT EXERCISE:";
			this->nextTitle->TextAlign = System::Drawing::ContentAlignment::TopRight;
			// 
			// currentLabel
			// 
			this->currentLabel->Location = System::Drawing::Point(436, 262);
			this->currentLabel->Name = L"currentLabel";
			this->currentLabel->Size = System::Drawing::Size(121, 16);
			this->currentLabel->TabIndex = 35;
			this->currentLabel->Text = L"method name here";
			// 
			// methodBox
			// 
			this->methodBox->AllowDrop = true;
			this->methodBox->AutoCompleteCustomSource->AddRange(gcnew cli::array< System::String^  >(3) {
				L"xi+sd*0.5", L"xi+sd*0.33",
					L"Another"
			});
			this->methodBox->FormattingEnabled = true;
			this->methodBox->Items->AddRange(gcnew cli::array< System::Object^  >(3) { L"Xi+SD*0.5", L"Xi+SD*0.33", L"Another" });
			this->methodBox->Location = System::Drawing::Point(436, 281);
			this->methodBox->Name = L"methodBox";
			this->methodBox->Size = System::Drawing::Size(136, 21);
			this->methodBox->TabIndex = 36;
			this->methodBox->SelectedIndexChanged += gcnew System::EventHandler(this, &MyForm::methodBox_SelectedIndexChanged);
			// 
			// methodLabel
			// 
			this->methodLabel->BackColor = System::Drawing::Color::LimeGreen;
			this->methodLabel->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->methodLabel->Location = System::Drawing::Point(318, 238);
			this->methodLabel->Name = L"methodLabel";
			this->methodLabel->Size = System::Drawing::Size(254, 20);
			this->methodLabel->TabIndex = 37;
			this->methodLabel->Text = L"CHOOSE METHOD";
			this->methodLabel->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// velMinus
			// 
			this->velMinus->BackColor = System::Drawing::SystemColors::Highlight;
			this->velMinus->FlatAppearance->BorderColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(192)),
				static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->velMinus->FlatAppearance->BorderSize = 0;
			this->velMinus->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->velMinus->Font = (gcnew System::Drawing::Font(L"Microsoft JhengHei UI", 20.25F));
			this->velMinus->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->velMinus->Location = System::Drawing::Point(305, 424);
			this->velMinus->Name = L"velMinus";
			this->velMinus->Size = System::Drawing::Size(60, 78);
			this->velMinus->TabIndex = 41;
			this->velMinus->Text = L"-";
			this->velMinus->UseVisualStyleBackColor = false;
			this->velMinus->Click += gcnew System::EventHandler(this, &MyForm::click_velMinus);
			// 
			// velPlus
			// 
			this->velPlus->BackColor = System::Drawing::SystemColors::Highlight;
			this->velPlus->FlatAppearance->BorderColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(192)),
				static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->velPlus->FlatAppearance->BorderSize = 0;
			this->velPlus->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->velPlus->Font = (gcnew System::Drawing::Font(L"Microsoft JhengHei UI", 20.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->velPlus->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->velPlus->Location = System::Drawing::Point(512, 424);
			this->velPlus->Name = L"velPlus";
			this->velPlus->Size = System::Drawing::Size(60, 78);
			this->velPlus->TabIndex = 40;
			this->velPlus->Text = L"+";
			this->velPlus->UseVisualStyleBackColor = false;
			this->velPlus->Click += gcnew System::EventHandler(this, &MyForm::click_velPlus);
			// 
			// velValue
			// 
			this->velValue->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->velValue->ForeColor = System::Drawing::SystemColors::Highlight;
			this->velValue->Location = System::Drawing::Point(358, 468);
			this->velValue->Name = L"velValue";
			this->velValue->Size = System::Drawing::Size(158, 31);
			this->velValue->TabIndex = 39;
			this->velValue->Text = L"isVelocity value";
			this->velValue->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// velTitle
			// 
			this->velTitle->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->velTitle->ForeColor = System::Drawing::SystemColors::ControlText;
			this->velTitle->Location = System::Drawing::Point(358, 424);
			this->velTitle->Name = L"velTitle";
			this->velTitle->Size = System::Drawing::Size(158, 31);
			this->velTitle->TabIndex = 38;
			this->velTitle->Text = L"isVelocity";
			this->velTitle->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// exerciseLabel
			// 
			this->exerciseLabel->BackColor = System::Drawing::Color::LimeGreen;
			this->exerciseLabel->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->exerciseLabel->Location = System::Drawing::Point(318, 144);
			this->exerciseLabel->Name = L"exerciseLabel";
			this->exerciseLabel->Size = System::Drawing::Size(254, 20);
			this->exerciseLabel->TabIndex = 46;
			this->exerciseLabel->Text = L"CHOOSE EXERCISE";
			this->exerciseLabel->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// exerciseBox
			// 
			this->exerciseBox->AllowDrop = true;
			this->exerciseBox->AutoCompleteCustomSource->AddRange(gcnew cli::array< System::String^  >(5) {
				L"Lower-leg flexion", L"Upper-leg extension",
					L"Electrical circuit", L"Some weird exercise", L"Christians\' ideas here"
			});
			this->exerciseBox->FormattingEnabled = true;
			this->exerciseBox->Items->AddRange(gcnew cli::array< System::Object^  >(5) {
				L"Lower-leg flexion", L"Upper-leg extension",
					L"Electrical circuit", L"Some weird exercise", L"Christians\' ideas here"
			});
			this->exerciseBox->Location = System::Drawing::Point(436, 187);
			this->exerciseBox->Name = L"exerciseBox";
			this->exerciseBox->Size = System::Drawing::Size(136, 21);
			this->exerciseBox->TabIndex = 45;
			this->exerciseBox->SelectedIndexChanged += gcnew System::EventHandler(this, &MyForm::exerciseBox_SelectedIndexChanged);
			// 
			// currentExLabel
			// 
			this->currentExLabel->Location = System::Drawing::Point(436, 168);
			this->currentExLabel->Name = L"currentExLabel";
			this->currentExLabel->Size = System::Drawing::Size(121, 16);
			this->currentExLabel->TabIndex = 44;
			this->currentExLabel->Text = L"exercise name here";
			// 
			// nextExTitle
			// 
			this->nextExTitle->Location = System::Drawing::Point(309, 184);
			this->nextExTitle->Name = L"nextExTitle";
			this->nextExTitle->Size = System::Drawing::Size(121, 28);
			this->nextExTitle->TabIndex = 43;
			this->nextExTitle->Text = L"METHOD FOR NEXT EXERCISE:";
			this->nextExTitle->TextAlign = System::Drawing::ContentAlignment::TopRight;
			// 
			// currentExTitle
			// 
			this->currentExTitle->Location = System::Drawing::Point(309, 168);
			this->currentExTitle->Name = L"currentExTitle";
			this->currentExTitle->Size = System::Drawing::Size(121, 16);
			this->currentExTitle->TabIndex = 42;
			this->currentExTitle->Text = L"CURRENT EXERCISE:";
			this->currentExTitle->TextAlign = System::Drawing::ContentAlignment::TopRight;
			// 
			// velCurrent
			// 
			this->velCurrent->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->velCurrent->ForeColor = System::Drawing::Color::DarkRed;
			this->velCurrent->Location = System::Drawing::Point(358, 521);
			this->velCurrent->Name = L"velCurrent";
			this->velCurrent->Size = System::Drawing::Size(158, 31);
			this->velCurrent->TabIndex = 47;
			this->velCurrent->Text = L"isVelocity value";
			this->velCurrent->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::ButtonHighlight;
			this->ClientSize = System::Drawing::Size(584, 561);
			this->Controls->Add(this->velCurrent);
			this->Controls->Add(this->exerciseLabel);
			this->Controls->Add(this->exerciseBox);
			this->Controls->Add(this->currentExLabel);
			this->Controls->Add(this->nextExTitle);
			this->Controls->Add(this->currentExTitle);
			this->Controls->Add(this->velMinus);
			this->Controls->Add(this->velPlus);
			this->Controls->Add(this->velValue);
			this->Controls->Add(this->velTitle);
			this->Controls->Add(this->methodLabel);
			this->Controls->Add(this->methodBox);
			this->Controls->Add(this->currentLabel);
			this->Controls->Add(this->nextTitle);
			this->Controls->Add(this->currentTitle);
			this->Controls->Add(this->stimButton);
			this->Controls->Add(this->statusDebug);
			this->Controls->Add(this->fqMinus);
			this->Controls->Add(this->fqPlus);
			this->Controls->Add(this->fqValue);
			this->Controls->Add(this->fqTitle);
			this->Controls->Add(this->curMinus);
			this->Controls->Add(this->curPlus);
			this->Controls->Add(this->curValue);
			this->Controls->Add(this->curTitle);
			this->Controls->Add(this->rampMinus);
			this->Controls->Add(this->rampPlus);
			this->Controls->Add(this->rampValue);
			this->Controls->Add(this->rampTitle);
			this->Controls->Add(this->thButton);
			this->Controls->Add(this->statusTitle);
			this->Controls->Add(this->statusLabel);
			this->Controls->Add(this->statusMsg);
			this->Controls->Add(this->stimTitle);
			this->Controls->Add(this->exerciseTitle);
			this->Name = L"MyForm";
			this->Text = L"Sense-And-Stimulating";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->bindingSource1))->EndInit();
			this->ResumeLayout(false);

		}

		void InitializeSASGUI(void)
		{
			// Internal handling
			bSetupPressed = false;
			trainStart = false;
			//this->Icon = gcnew System::Drawing::Icon(L"logo_square.ico");
			this->methodBox->SelectedIndex = 0;
			this->exerciseBox->SelectedIndex = 0;
			// Communication
			Move3_gui = Move3_none;
			user_gui = User_none;
			// Lists
			statusList[0] = "Initialization";
			statusList[1] = "Setting threshold";
			statusList[2] = "EMG monitoring";
			statusList[3] = "Tigger FES";
			statusList[4] = "Stop FES";
			statusList[5] = "Finish program";
			statusList[6] = "Manual calibration";
			statusList[7] = "Automatic calibration\n- stimulating";
			statusList[8] = "Automatic calibration\n- resting";
			statusList[9] = "Exercise finished";

			methodList[0] = "Xi+SD*0.5";
			methodList[1] = "Xi+SD*0.3";
			methodList[2] = "Another";

			// for isMoving testing
			isVelocity = 0.8;
			// Update display
			std::stringstream tempValue;
			tempValue << std::setprecision(2) << GL_UI.isVelocity_limit << " mms/s";
			string tempString = tempValue.str();
			this->velValue->Text = gcnew String(tempString.c_str());

			// For further development
			exerciseList[0] = "Lower-leg flexion";
			exerciseList[1] = "Upper-leg extension";
			exerciseList[2] = "Electrical circuit";
			exerciseList[3] = "Some weird exercise";
			exerciseList[4] = "Christians\' ideas here";

		}
#pragma endregion

// ---------------- Buttons being pressed ----------------
	// Start background worker
	private: System::Void start_pressed(System::Object^ sender, System::EventArgs^ e) {
		this->backgroundWorker1->CancelAsync();

	}

	private: System::Void click_trainButton(System::Object^ sender, System::EventArgs^ e) {
		user_gui = User_st;
	}

	private: System::Void click_thButton(System::Object^ sender, System::EventArgs^ e) {
		user_gui = User_th;
	}

	private: System::Void click_repeatButton(System::Object^ sender, System::EventArgs^ e) {
		user_gui = User_rep;
	}

	private: System::Void click_newButton(System::Object^ sender, System::EventArgs^ e) {
		user_gui = User_new;
	}

	private: System::Void click_rampPlus(System::Object^ sender, System::EventArgs^ e) {
		Move3_gui = Move3_ramp_more;
	}

	private: System::Void click_rampMinus(System::Object^ sender, System::EventArgs^ e) {
		Move3_gui = Move3_ramp_less;
	}

	private: System::Void click_curPlus(System::Object^ sender, System::EventArgs^ e) {
		Move3_gui = Move3_incr;
	}

	private: System::Void click_curMinus(System::Object^ sender, System::EventArgs^ e) {
		Move3_gui = Move3_decr;
	}

	private: System::Void click_fqPlus(System::Object^ sender, System::EventArgs^ e) {
		Move3_gui = Move3_Hz_mr;
	}

	private: System::Void click_fqMinus(System::Object^ sender, System::EventArgs^ e) {
		Move3_gui = Move3_Hz_ls;
	}

	private: System::Void click_stimButton(System::Object^ sender, System::EventArgs^ e) {

		if ((state == st_calM || state == st_running) && !GL_UI.stimActive && GL_UI.playPause)
		{
			// Start stimulation command
			Move3_gui = Move3_start;
		}
		else
		{
			// Stop stimulation command
			Move3_gui = Move3_stop;
		}

	}

	private: System::Void click_setButton(System::Object^ sender, System::EventArgs^ e) {

		if (state == st_init)
		{
			// Start manual calibration
			user_gui = User_CM;
		}
		else
		{
			// Manual calibration has already started
			Move3_gui = Move3_done;
		}
		// Internal handling
		if (state == st_calM && !bSetupPressed)
		{
			bSetupPressed = true;
		}

	}

	private: System::Void methodBox_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e) {
		// Update the selected value
		nextIndex = this->methodBox->SelectedIndex;
	}

	private: System::Void exerciseBox_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e) {
		// Update the selected value
		nextExercise = this->exerciseBox->SelectedIndex;
	}

	// For isMoving testing
	private: System::Void click_velMinus(System::Object^ sender, System::EventArgs^ e) {
		isVelocity -= isDelta;
		// Update SAS
		GL_UI.isVelocity_limit = isVelocity;
		// Update display
		std::stringstream tempValue;
		tempValue << std::setprecision(2) << GL_UI.isVelocity_limit << " mms/s";
		string tempString = tempValue.str();
		this->velValue->Text = gcnew String(tempString.c_str());
	}
	private: System::Void click_velPlus(System::Object^ sender, System::EventArgs^ e) {
		isVelocity += isDelta;
		// Update SAS
		GL_UI.isVelocity_limit = isVelocity;
		// Update display
		std::stringstream tempValue;
		tempValue << std::setprecision(2) << GL_UI.isVelocity_limit << " mms/s";
		string tempString = tempValue.str();
		this->velValue->Text = gcnew String(tempString.c_str());
	}

// ---------------- Background worker: be careful when modifying these ----------------
			   // This function runs on a separate thread from the UI
	private: System::Void launchBackground(System::Object^ sender, DoWorkEventArgs^ e)
	{
		// Separate threads cannot access the IU fields, thus the report progress is needed
		while (!GL_UI.END_GUI) {
			dummy++;
			// by updating this field, its associated function shall be called as well (updateBackground)
			this->backgroundWorker1->ReportProgress(dummy);
			System::Threading::Thread::Sleep(UPDATE_PERIOD_MS);
		}

		// If the operation was canceled,
		// set the DoWorkEventArgs.Cancel property to true.
		if (this->backgroundWorker1->CancellationPending)
		{
			e->Cancel = true;
		}
	}

		   // This event handler demonstrates how to interpret runs when the background must finish
	private: System::Void endBackground(System::Object^ sender, RunWorkerCompletedEventArgs^ e)
	{
		// The operation completed normally.
		//string msg = String.Format("Result = {0}", e.Result);
		//System::Windows::Forms::MessageBox::Show(msg);
		//this->backLabel->Text = L"Result = {0}";
		this->statusLabel->Text = L"Backgroundworker finish. Close program.";
	}

	private: System::Void updateBackground(System::Object^ sender, ProgressChangedEventArgs^ e)
	{
		// This function can actually access and modify the IU variables 
		char intStr[32];
		// -------------------- Update status (SAS -> GUI) --------------------
		int local_status = (int)GL_UI.status;
		state = GL_UI.status;
		this->statusLabel->Text = gcnew String(statusList[local_status].c_str());
		this->statusMsg->Text = gcnew String(GL_UI.screenMessage.c_str());
		// show stimulation parameters
		std::stringstream tempValue;
		tempValue << std::setprecision(2) << GL_UI.ramp << " POINTS";
		string tempString = tempValue.str();
		this->rampValue->Text = gcnew String(tempString.c_str());

		//tempValue = to_string(GL_UI.current);
		//tempValue += " mA";q
		std::stringstream tempValueCur;
		tempValueCur << std::setprecision(4) << GL_UI.current << " mA";
		tempString = tempValueCur.str();
		this->curValue->Text = gcnew String(tempString.c_str());

		std::stringstream tempValueFq;
		tempValueFq << std::setprecision(2) << GL_UI.frequency << " Hz";
		tempString = tempValueFq.str();
		this->fqValue->Text = gcnew String(tempString.c_str());

		// -------------------- Update requests (GUI->SAS) --------------------
		bUser = (user_gui != User_none);
		bMove3 = (Move3_gui != Move3_none);

		// Set down the buttons commands and flags if the global var was updated 
		if (bUser) {
			GL_UI.User_hmi = user_gui;
			user_gui = User_none;
		}
		if (bMove3) {
			GL_UI.Move3_hmi = Move3_gui;
			Move3_gui = Move3_none;
		}

		GL_UI.next_method = (threshold_Type)nextIndex;
		GL_UI.next_exercise = (exercise_Type)nextExercise;

		// Close background task in the next loop
		if (GL_UI.END_GUI) {
			this->backgroundWorker1->CancelAsync();
		}

		// -------------------- Dynamic update (Colors and styles) --------------------
		// - default blue color: Highlight
		// - need to be pressed: green -> LimeGreen
		// - inactive? grey -> ButtonShadow
		// - STOP (safety-related important) red -> Crimson

		// Debug stuff
		itoa(GL_UI.status, intStr, 10);
		this->statusDebug->Text = gcnew String(string(intStr).c_str());

		// Start/Stop stimulation
		if (GL_UI.playPause)
		{
			// Normal behavior
			if ((state == st_calM || state == st_running) && !GL_UI.stimActive && !GL_UI.main_thEN)
			{
				this->stimButton->BackColor = System::Drawing::Color::LimeGreen;
				if (state == st_calM)
				{
					this->stimButton->Text = L"START STIMULATION";
				}
				else
				{
					this->stimButton->Text = L"RESUME STIMULATION";
				}
				
			}
			else
			{
				this->stimButton->Text = L"STOP STIMULATION";
				if (GL_UI.stimActive)
				{
					this->stimButton->BackColor = System::Drawing::Color::Crimson;
				}
				else
				{
					this->stimButton->BackColor = System::Drawing::SystemColors::Highlight;
				}
			}
		}
		else
		{
			// Button disabled
			this->stimButton->BackColor = System::Drawing::SystemColors::InactiveCaption;
			this->stimButton->Text = L"STIMULATION DISABLED";
		}


		// Set threshold and method
		if (state == st_th && !GL_UI.recReq && GL_UI.main_thEN)
		{
			this->thButton->BackColor = System::Drawing::Color::LimeGreen;
			this->methodLabel->BackColor = System::Drawing::Color::LimeGreen;
		}
		else
		{
			this->thButton->BackColor = System::Drawing::SystemColors::Highlight;
			this->methodLabel->BackColor = System::Drawing::SystemColors::ButtonHighlight;
		}

		this->currentLabel->Text = gcnew String(methodList[(int)GL_UI.method].c_str());

		// Select exercise
		if (state == st_init || state == st_repeat)
		{
			this->exerciseLabel->BackColor = System::Drawing::Color::LimeGreen;
		}
		else
		{
			this->exerciseLabel->BackColor = System::Drawing::SystemColors::ButtonHighlight;
		}

		this->currentExLabel->Text = gcnew String(exerciseList[(int)GL_UI.exercise].c_str());

		// For isVelocity testing
		std::stringstream tempVelocity;
		tempVelocity << std::setprecision(7) << GL_UI.isVelocity << " mm/s";
		tempString = tempVelocity.str();
		this->velCurrent->Text = gcnew String(tempString.c_str());
	}

};
}
