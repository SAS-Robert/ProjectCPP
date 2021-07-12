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
		int dummy, nextIndex, nextExercise, nextRecP, nextStimP;
		const int UPDATE_PERIOD_MS = 100;
		// SAS <-> GUI fields
		RehaMove3_Req_Type Move3_gui;
		User_Req_Type user_gui;
		state_Type state;
		bool bUser, bMove3, bSetupPressed, bSingle_mth;
		bool trainStart, stimA_active, stimM_active, gui_repeat, gui_new;
		//char intStr[32];

		// For isMoving testing
		double isVelocity;
		const double isDelta = 1;
		
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

		System::Windows::Forms::Button^ stimButton;
		System::Windows::Forms::Label^ nextTitle;
		System::Windows::Forms::ComboBox^ methodBox;

		System::Windows::Forms::Button^ velMinus;
		System::Windows::Forms::Button^ velPlus;
		System::Windows::Forms::Label^ velValue;
		System::Windows::Forms::Label^ velTitle;

		System::Windows::Forms::ComboBox^ exerciseBox;
		System::Windows::Forms::Label^ nextExTitle;

		System::Windows::Forms::Label^ velCurrent;
		System::Windows::Forms::BindingSource^ bindingSource1;
		System::Windows::Forms::Label^ stimInfo;
		System::Windows::Forms::Label^ additionalTitle;






	public: System::Windows::Forms::Label^ addLabelFixed;
	public: System::Windows::Forms::Button^ windowButton;
	public: System::Windows::Forms::ComboBox^ recPBox;

	public: System::Windows::Forms::Label^ recPortTitle;
	public: System::Windows::Forms::ComboBox^ stimPBox;


	public: System::Windows::Forms::Label^ stimPortTitle;
	public: System::Windows::Forms::Label^ testText;



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
			this->stimButton = (gcnew System::Windows::Forms::Button());
			this->nextTitle = (gcnew System::Windows::Forms::Label());
			this->methodBox = (gcnew System::Windows::Forms::ComboBox());
			this->velMinus = (gcnew System::Windows::Forms::Button());
			this->velPlus = (gcnew System::Windows::Forms::Button());
			this->velValue = (gcnew System::Windows::Forms::Label());
			this->velTitle = (gcnew System::Windows::Forms::Label());
			this->exerciseBox = (gcnew System::Windows::Forms::ComboBox());
			this->nextExTitle = (gcnew System::Windows::Forms::Label());
			this->velCurrent = (gcnew System::Windows::Forms::Label());
			this->bindingSource1 = (gcnew System::Windows::Forms::BindingSource(this->components));
			this->stimInfo = (gcnew System::Windows::Forms::Label());
			this->additionalTitle = (gcnew System::Windows::Forms::Label());
			this->addLabelFixed = (gcnew System::Windows::Forms::Label());
			this->windowButton = (gcnew System::Windows::Forms::Button());
			this->recPBox = (gcnew System::Windows::Forms::ComboBox());
			this->recPortTitle = (gcnew System::Windows::Forms::Label());
			this->stimPBox = (gcnew System::Windows::Forms::ComboBox());
			this->stimPortTitle = (gcnew System::Windows::Forms::Label());
			//this->testText = (gcnew System::Windows::Forms::Label());
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
			this->statusMsg->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->statusMsg->Location = System::Drawing::Point(15, 49);
			this->statusMsg->Name = L"statusMsg";
			this->statusMsg->Size = System::Drawing::Size(556, 102);
			this->statusMsg->TabIndex = 4;
			this->statusMsg->Text = L"status message here";
			// 
			// statusLabel
			// 
			this->statusLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 20.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->statusLabel->Location = System::Drawing::Point(301, 4);
			this->statusLabel->Name = L"statusLabel";
			this->statusLabel->Size = System::Drawing::Size(284, 36);
			this->statusLabel->TabIndex = 5;
			this->statusLabel->Text = L"status Label here";
			this->statusLabel->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// statusTitle
			// 
			this->statusTitle->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 20.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->statusTitle->ForeColor = System::Drawing::SystemColors::Highlight;
			this->statusTitle->Location = System::Drawing::Point(7, 9);
			this->statusTitle->Name = L"statusTitle";
			this->statusTitle->Size = System::Drawing::Size(288, 31);
			this->statusTitle->TabIndex = 6;
			this->statusTitle->Text = L"PROGRAM STATUS:";
			// 
			// stimTitle
			// 
			this->stimTitle->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->stimTitle->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 20.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->stimTitle->ForeColor = System::Drawing::SystemColors::Highlight;
			this->stimTitle->Location = System::Drawing::Point(1, 151);
			this->stimTitle->Name = L"stimTitle";
			this->stimTitle->Size = System::Drawing::Size(273, 491);
			this->stimTitle->TabIndex = 7;
			this->stimTitle->Text = L"STIMULATION";
			this->stimTitle->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// exerciseTitle
			// 
			this->exerciseTitle->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->exerciseTitle->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 20.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->exerciseTitle->ForeColor = System::Drawing::SystemColors::Highlight;
			this->exerciseTitle->Location = System::Drawing::Point(272, 151);
			this->exerciseTitle->Name = L"exerciseTitle";
			this->exerciseTitle->Size = System::Drawing::Size(357, 491);
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
			this->thButton->Font = (gcnew System::Drawing::Font(L"Microsoft JhengHei UI", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->thButton->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->thButton->Location = System::Drawing::Point(285, 507);
			this->thButton->Name = L"thButton";
			this->thButton->Size = System::Drawing::Size(327, 78);
			this->thButton->TabIndex = 10;
			this->thButton->Text = L"SET THRESHOLD";
			this->thButton->UseVisualStyleBackColor = false;
			this->thButton->Click += gcnew System::EventHandler(this, &MyForm::click_thButton);
			// 
			// rampTitle
			// 
			this->rampTitle->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->rampTitle->ForeColor = System::Drawing::SystemColors::Highlight;
			this->rampTitle->Location = System::Drawing::Point(61, 205);
			this->rampTitle->Name = L"rampTitle";
			this->rampTitle->Size = System::Drawing::Size(152, 31);
			this->rampTitle->TabIndex = 14;
			this->rampTitle->Text = L"RAMP";
			this->rampTitle->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// rampValue
			// 
			this->rampValue->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->rampValue->ForeColor = System::Drawing::SystemColors::ControlText;
			this->rampValue->Location = System::Drawing::Point(70, 249);
			this->rampValue->Name = L"rampValue";
			this->rampValue->Size = System::Drawing::Size(131, 31);
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
			this->rampPlus->Location = System::Drawing::Point(206, 205);
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
			this->rampMinus->Location = System::Drawing::Point(7, 205);
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
			this->curMinus->Location = System::Drawing::Point(7, 299);
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
			this->curPlus->Location = System::Drawing::Point(206, 299);
			this->curPlus->Name = L"curPlus";
			this->curPlus->Size = System::Drawing::Size(60, 78);
			this->curPlus->TabIndex = 20;
			this->curPlus->Text = L"+";
			this->curPlus->UseVisualStyleBackColor = false;
			this->curPlus->Click += gcnew System::EventHandler(this, &MyForm::click_curPlus);
			// 
			// curValue
			// 
			this->curValue->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->curValue->ForeColor = System::Drawing::SystemColors::ControlText;
			this->curValue->Location = System::Drawing::Point(70, 343);
			this->curValue->Name = L"curValue";
			this->curValue->Size = System::Drawing::Size(131, 31);
			this->curValue->TabIndex = 19;
			this->curValue->Text = L"CURRENT";
			this->curValue->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// curTitle
			// 
			this->curTitle->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->curTitle->ForeColor = System::Drawing::SystemColors::Highlight;
			this->curTitle->Location = System::Drawing::Point(62, 299);
			this->curTitle->Name = L"curTitle";
			this->curTitle->Size = System::Drawing::Size(151, 31);
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
			this->fqMinus->Location = System::Drawing::Point(7, 391);
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
			this->fqPlus->Location = System::Drawing::Point(206, 391);
			this->fqPlus->Name = L"fqPlus";
			this->fqPlus->Size = System::Drawing::Size(60, 78);
			this->fqPlus->TabIndex = 24;
			this->fqPlus->Text = L"+";
			this->fqPlus->UseVisualStyleBackColor = false;
			this->fqPlus->Click += gcnew System::EventHandler(this, &MyForm::click_fqPlus);
			// 
			// fqValue
			// 
			this->fqValue->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->fqValue->ForeColor = System::Drawing::SystemColors::ControlText;
			this->fqValue->Location = System::Drawing::Point(70, 435);
			this->fqValue->Name = L"fqValue";
			this->fqValue->Size = System::Drawing::Size(131, 31);
			this->fqValue->TabIndex = 23;
			this->fqValue->Text = L"HZ";
			this->fqValue->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// fqTitle
			// 
			this->fqTitle->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->fqTitle->ForeColor = System::Drawing::SystemColors::Highlight;
			this->fqTitle->Location = System::Drawing::Point(62, 391);
			this->fqTitle->Name = L"fqTitle";
			this->fqTitle->Size = System::Drawing::Size(151, 31);
			this->fqTitle->TabIndex = 22;
			this->fqTitle->Text = L"FREQUENCY";
			this->fqTitle->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// stimButton
			// 
			this->stimButton->BackColor = System::Drawing::SystemColors::Highlight;
			this->stimButton->FlatAppearance->BorderColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(192)),
				static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->stimButton->FlatAppearance->BorderSize = 0;
			this->stimButton->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->stimButton->Font = (gcnew System::Drawing::Font(L"Microsoft JhengHei UI", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->stimButton->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->stimButton->Location = System::Drawing::Point(7, 507);
			this->stimButton->Name = L"stimButton";
			this->stimButton->Size = System::Drawing::Size(259, 78);
			this->stimButton->TabIndex = 31;
			this->stimButton->Text = L"START/STOP STIMULATION";
			this->stimButton->UseVisualStyleBackColor = false;
			this->stimButton->Click += gcnew System::EventHandler(this, &MyForm::click_stimButton);
			// 
			// nextTitle
			// 
			this->nextTitle->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->nextTitle->ForeColor = System::Drawing::SystemColors::Highlight;
			this->nextTitle->Location = System::Drawing::Point(283, 297);
			this->nextTitle->Name = L"nextTitle";
			this->nextTitle->Size = System::Drawing::Size(329, 26);
			this->nextTitle->TabIndex = 34;
			this->nextTitle->Text = L"SELECTED METHOD:";
			this->nextTitle->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// methodBox
			// 
			this->methodBox->AllowDrop = true;
			this->methodBox->AutoCompleteCustomSource->AddRange(gcnew cli::array< System::String^  >(4) {
				L"xi+sd*0.5", L"xi+sd*0.33",
					L"MVC05", L"MVC10"
			});
			this->methodBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 27.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->methodBox->ForeColor = System::Drawing::SystemColors::WindowText;
			this->methodBox->FormattingEnabled = true;
			this->methodBox->Items->AddRange(gcnew cli::array< System::Object^  >(4) { L"Xi+SD*0.5", L"Xi+SD*0.33", L"MVC05", L"MVC10" });
			this->methodBox->Location = System::Drawing::Point(280, 324);
			this->methodBox->Name = L"methodBox";
			this->methodBox->Size = System::Drawing::Size(332, 50);
			this->methodBox->TabIndex = 36;
			this->methodBox->SelectedIndexChanged += gcnew System::EventHandler(this, &MyForm::methodBox_SelectedIndexChanged);
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
			this->velMinus->Location = System::Drawing::Point(288, 391);
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
			this->velPlus->Location = System::Drawing::Point(552, 388);
			this->velPlus->Name = L"velPlus";
			this->velPlus->Size = System::Drawing::Size(60, 78);
			this->velPlus->TabIndex = 40;
			this->velPlus->Text = L"+";
			this->velPlus->UseVisualStyleBackColor = false;
			this->velPlus->Click += gcnew System::EventHandler(this, &MyForm::click_velPlus);
			// 
			// velValue
			// 
			this->velValue->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->velValue->ForeColor = System::Drawing::SystemColors::Highlight;
			this->velValue->Location = System::Drawing::Point(351, 435);
			this->velValue->Name = L"velValue";
			this->velValue->Size = System::Drawing::Size(195, 31);
			this->velValue->TabIndex = 39;
			this->velValue->Text = L"isVelocity value";
			this->velValue->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// velTitle
			// 
			this->velTitle->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->velTitle->ForeColor = System::Drawing::SystemColors::ControlText;
			this->velTitle->Location = System::Drawing::Point(346, 391);
			this->velTitle->Name = L"velTitle";
			this->velTitle->Size = System::Drawing::Size(200, 31);
			this->velTitle->TabIndex = 38;
			this->velTitle->Text = L"MIN. VELOCITY";
			this->velTitle->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// exerciseBox
			// 
			this->exerciseBox->AutoCompleteCustomSource->AddRange(gcnew cli::array< System::String^  >(5) {
				L"Knee extension", L"Knee flexion",
					L"Planta flexion", L"Dorsal flexion", L"Other"
			});
			this->exerciseBox->Enabled = false;
			this->exerciseBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 27.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->exerciseBox->FormattingEnabled = true;
			this->exerciseBox->Items->AddRange(gcnew cli::array< System::Object^  >(5) {
				L"Knee extension", L"Knee flexion", L"Planta flexion",
					L"Dorsal flexion", L"Other"
			});
			this->exerciseBox->Location = System::Drawing::Point(280, 230);
			this->exerciseBox->Name = L"exerciseBox";
			this->exerciseBox->Size = System::Drawing::Size(332, 50);
			this->exerciseBox->TabIndex = 45;
			this->exerciseBox->SelectedIndexChanged += gcnew System::EventHandler(this, &MyForm::exerciseBox_SelectedIndexChanged);
			// 
			// nextExTitle
			// 
			this->nextExTitle->BackColor = System::Drawing::SystemColors::ButtonHighlight;
			this->nextExTitle->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->nextExTitle->ForeColor = System::Drawing::SystemColors::Highlight;
			this->nextExTitle->Location = System::Drawing::Point(280, 203);
			this->nextExTitle->Name = L"nextExTitle";
			this->nextExTitle->Size = System::Drawing::Size(332, 30);
			this->nextExTitle->TabIndex = 43;
			this->nextExTitle->Text = L"SELECTED EXERCISE:";
			this->nextExTitle->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// velCurrent
			// 
			this->velCurrent->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->velCurrent->ForeColor = System::Drawing::Color::Transparent;
			this->velCurrent->Location = System::Drawing::Point(413, 473);
			this->velCurrent->Name = L"velCurrent";
			this->velCurrent->Size = System::Drawing::Size(158, 23);
			this->velCurrent->TabIndex = 47;
			this->velCurrent->Text = L"isVelocity value";
			this->velCurrent->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// stimInfo
			// 
			this->stimInfo->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Italic, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->stimInfo->ForeColor = System::Drawing::Color::DimGray;
			this->stimInfo->Location = System::Drawing::Point(12, 587);
			this->stimInfo->Name = L"stimInfo";
			this->stimInfo->Size = System::Drawing::Size(242, 44);
			this->stimInfo->TabIndex = 48;
			this->stimInfo->Text = L"PRESS PATIENT BUTTON TO ENABLE STIMULATION";
			this->stimInfo->TextAlign = System::Drawing::ContentAlignment::BottomCenter;
			// 
			// additionalTitle
			// 
			this->additionalTitle->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->additionalTitle->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 20.25F, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->additionalTitle->ForeColor = System::Drawing::SystemColors::Highlight;
			this->additionalTitle->Location = System::Drawing::Point(1, 641);
			this->additionalTitle->Name = L"additionalTitle";
			this->additionalTitle->Size = System::Drawing::Size(628, 124);
			this->additionalTitle->TabIndex = 49;
			this->additionalTitle->Text = L" ";
			// 
			// addLabelFixed
			// 
			this->addLabelFixed->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->addLabelFixed->ForeColor = System::Drawing::SystemColors::Highlight;
			this->addLabelFixed->Location = System::Drawing::Point(2, 648);
			this->addLabelFixed->Name = L"addLabelFixed";
			this->addLabelFixed->Size = System::Drawing::Size(139, 74);
			this->addLabelFixed->TabIndex = 54;
			this->addLabelFixed->Text = L"ADDITIONAL SETTINGS";
			this->addLabelFixed->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// windowButton
			// 
			this->windowButton->BackColor = System::Drawing::SystemColors::Highlight;
			this->windowButton->FlatAppearance->BorderColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(192)),
				static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->windowButton->FlatAppearance->BorderSize = 0;
			this->windowButton->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->windowButton->Font = (gcnew System::Drawing::Font(L"Microsoft JhengHei UI", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->windowButton->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->windowButton->Location = System::Drawing::Point(147, 646);
			this->windowButton->Name = L"windowButton";
			this->windowButton->Size = System::Drawing::Size(138, 78);
			this->windowButton->TabIndex = 55;
			this->windowButton->Text = L"RESIZE WINDOW";
			this->windowButton->UseVisualStyleBackColor = false;
			this->windowButton->Click += gcnew System::EventHandler(this, &MyForm::click_windowButton);
			// 
			// recPBox
			// 
			this->recPBox->AutoCompleteCustomSource->AddRange(gcnew cli::array< System::String^  >(9) {
				L"COM1", L"COM2", L"COM3", L"COM4",
					L"COM5", L"COM6", L"COM7", L"COM8", L"COM9"
			});
			this->recPBox->Enabled = false;
			this->recPBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 27.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->recPBox->FormattingEnabled = true;
			this->recPBox->Items->AddRange(gcnew cli::array< System::Object^  >(9) {
				L"COM1", L"COM2", L"COM3", L"COM4", L"COM5", L"COM6",
					L"COM7", L"COM8", L"COM9"
			});
			this->recPBox->Location = System::Drawing::Point(296, 674);
			this->recPBox->Name = L"recPBox";
			this->recPBox->Size = System::Drawing::Size(151, 50);
			this->recPBox->TabIndex = 57;
			this->recPBox->SelectedIndexChanged += gcnew System::EventHandler(this, &MyForm::recPBox_SelectedIndexChanged);
			// 
			// recPortTitle
			// 
			this->recPortTitle->BackColor = System::Drawing::SystemColors::ButtonHighlight;
			this->recPortTitle->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->recPortTitle->ForeColor = System::Drawing::SystemColors::Highlight;
			this->recPortTitle->Location = System::Drawing::Point(296, 647);
			this->recPortTitle->Name = L"recPortTitle";
			this->recPortTitle->Size = System::Drawing::Size(160, 30);
			this->recPortTitle->TabIndex = 56;
			this->recPortTitle->Text = L"REHA INGEST";
			this->recPortTitle->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// stimPBox
			// 
			this->stimPBox->AutoCompleteCustomSource->AddRange(gcnew cli::array< System::String^  >(9) {
				L"COM1", L"COM2", L"COM3", L"COM4",
					L"COM5", L"COM6", L"COM7", L"COM8", L"COM9"
			});
			this->stimPBox->Enabled = false;
			this->stimPBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 27.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->stimPBox->FormattingEnabled = true;
			this->stimPBox->Items->AddRange(gcnew cli::array< System::Object^  >(9) {
				L"COM1", L"COM2", L"COM3", L"COM4", L"COM5", L"COM6",
					L"COM7", L"COM8", L"COM9"
			});
			this->stimPBox->Location = System::Drawing::Point(459, 674);
			this->stimPBox->Name = L"stimPBox";
			this->stimPBox->Size = System::Drawing::Size(153, 50);
			this->stimPBox->TabIndex = 59;
			this->stimPBox->SelectedIndexChanged += gcnew System::EventHandler(this, &MyForm::stimPBox_SelectedIndexChanged);
			// 
			// stimPortTitle
			// 
			this->stimPortTitle->BackColor = System::Drawing::SystemColors::ButtonHighlight;
			this->stimPortTitle->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->stimPortTitle->ForeColor = System::Drawing::SystemColors::Highlight;
			this->stimPortTitle->Location = System::Drawing::Point(459, 647);
			this->stimPortTitle->Name = L"stimPortTitle";
			this->stimPortTitle->Size = System::Drawing::Size(153, 30);
			this->stimPortTitle->TabIndex = 58;
			this->stimPortTitle->Text = L"REHA MOVE";
			this->stimPortTitle->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// testText
			//
			/*
			this->testText->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->testText->ForeColor = System::Drawing::SystemColors::Highlight;
			this->testText->Location = System::Drawing::Point(302, 600);
			this->testText->Name = L"testText";
			this->testText->Size = System::Drawing::Size(195, 31);
			this->testText->TabIndex = 60;
			this->testText->Text = L"debug";
			this->testText->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			this->testText->Click += gcnew System::EventHandler(this, &MyForm::testText_Click);
			*/
			// 
			// MyForm
			// 
			this->AllowDrop = true;
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::ButtonHighlight;
			this->ClientSize = System::Drawing::Size(624, 731);
			this->ControlBox = false;
			//this->Controls->Add(this->testText);
			this->Controls->Add(this->stimPBox);
			this->Controls->Add(this->stimPortTitle);
			this->Controls->Add(this->recPBox);
			this->Controls->Add(this->recPortTitle);
			this->Controls->Add(this->windowButton);
			this->Controls->Add(this->addLabelFixed);
			this->Controls->Add(this->additionalTitle);
			this->Controls->Add(this->stimInfo);
			this->Controls->Add(this->velCurrent);
			this->Controls->Add(this->exerciseBox);
			this->Controls->Add(this->nextExTitle);
			this->Controls->Add(this->velMinus);
			this->Controls->Add(this->velPlus);
			this->Controls->Add(this->velValue);
			this->Controls->Add(this->velTitle);
			this->Controls->Add(this->methodBox);
			this->Controls->Add(this->nextTitle);
			this->Controls->Add(this->stimButton);
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
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"MyForm";
			this->StartPosition = System::Windows::Forms::FormStartPosition::Manual;
			this->Tag = L"0;0";
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
			this->recPBox->SelectedIndex = 3;
			this->stimPBox->SelectedIndex = 6;
			// Communication
			Move3_gui = Move3_none;
			user_gui = User_none;
			// Lists
			statusList[0] = "Initialization";
			statusList[1] = "Setting threshold";
			statusList[2] = "Waiting for trigger";
			statusList[3] = "FES triggered";
			statusList[4] = "End of repetition";
			statusList[5] = "Finish program";
			statusList[6] = "Stimulation set-up";
			statusList[7] = "Automatic calibration\n- stimulating";
			statusList[8] = "Automatic calibration\n- resting";
			statusList[9] = "Set finished";
			statusList[10] = "Seting MVC threshold";

			methodList[0] = "Xi+SD*3";
			methodList[1] = "Xi+SD*2";
			methodList[2] = "Another";
			methodList[3] = "MVC05";
			methodList[4] = "MVC10";

			// for isMoving testing
			isVelocity = 100.0;
			// Update display
			std::stringstream tempValue;

			tempValue << std::setprecision(2) << GL_UI.isVelocity_limit << " mm/100ms";
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
		// Normal run: either single-th method or MVC after setting the first threshold
		if (state == st_th && !GL_UI.recReq && !GL_UI.th1 && GL_UI.main_thEN)
		{
			user_gui = User_th;
		}
		// MCV Method: for setting the 2nd threshold
		else if (!bSingle_mth && GL_UI.th1 && state == st_th && !GL_UI.set_MVC)
		{
			GL_UI.set_MVC = true;
		}
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

		if ((state == st_calM || state == st_running) && !GL_UI.stimActive && GL_UI.playPause && GL_UI.stimReady)
		{
			// Start stimulation command
			Move3_gui = Move3_start;
		}
		else if (GL_UI.playPause)
		{
			// Stop stimulation command
			Move3_gui = Move3_stop;
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
	
	private: System::Void click_windowButton(System::Object^ sender, System::EventArgs^ e) {
		this->ClientSize = System::Drawing::Size(624, 731);
		this->Location = System::Drawing::Point(0, 0);
	}
// Select recorder and stimulator Ports

	private: System::Void recPBox_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e) {
		char intStr[8];
		nextRecP = this->recPBox->SelectedIndex;
		nextRecP++;		// the index starts on 0 but the first port is COM1

		// check limits if it is the same one as the other device
		if (nextRecP == nextStimP && nextRecP < 9)
		{
			nextRecP++;
			this->recPBox->SelectedIndex = nextRecP-1;
		}
		else if (nextRecP == nextStimP && nextRecP == 9)
		{
			nextRecP--;
			this->recPBox->SelectedIndex = nextRecP-1;
		}

		itoa(nextRecP, intStr, 10);
		GL_UI.PORT_REC[3] = intStr[0];
	}

	private: System::Void stimPBox_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e) {
		char intStr[8];
		nextStimP = this->stimPBox->SelectedIndex;
		nextStimP++;  // the index starts on 0 but the first port is COM1

		// check limits if it is the same one as the other device
		if (nextRecP == nextStimP && nextStimP < 9)
		{
			nextStimP++;
			this->stimPBox->SelectedIndex = nextStimP-1;
		}
		else if (nextRecP == nextStimP && nextStimP == 9)
		{
			nextStimP--;
			this->stimPBox->SelectedIndex = nextStimP-1;
		}

		itoa(nextStimP, intStr, 10);
		GL_UI.PORT_STIM[3] = intStr[0];
	}

	// For isMoving testing (that it was at the end shipped to Holland because why not)
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
		// Debug test
		//this->testText->Text = L"Hello world";
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

		bSingle_mth = (GL_UI.method != th_MVC10 && GL_UI.method != th_MVC05);

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

		// Update indexes in boxes
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

		// START / STOP / RESUME STIMULATION
		// i. notice text
		if (!GL_UI.playPause || !GL_UI.stimReady)
		{
			this->stimInfo->ForeColor = System::Drawing::SystemColors::InactiveCaption;
			if (!GL_UI.playPause)
			{
				this->stimInfo->Text = L"PRESS PATIENT BUTTON TO ENABLE STIMULATION";
			}
			else
			{
				this->stimInfo->Text = L"CONNECT STIMULATOR";
			}
		}
		else
		{
			this->stimInfo->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
		}
		// ii. Stimulation disabled
		if (!GL_UI.playPause || !GL_UI.stimReady)
		{
			this->stimButton->BackColor = System::Drawing::SystemColors::InactiveCaption;
			this->stimButton->Text = L"STIMULATION DISABLED";
		}
		// Start/resume stimulation
		else if ((state == st_calM || state == st_running) && !GL_UI.stimActive && GL_UI.playPause)
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
		// Stop stimulation that is active
		else if (GL_UI.playPause && GL_UI.stimActive)
		{
			this->stimButton->Text = L"STOP STIMULATION";
			this->stimButton->BackColor = System::Drawing::Color::Crimson;
		}
		// Stop stimulation in blue
		else
		{
			this->stimButton->Text = L"STOP STIMULATION";
			this->stimButton->BackColor = System::Drawing::SystemColors::Highlight;
		}


		// SELECT THRESHOLD AND RECORD IT
		// i. Method selection
		if (state == st_th && !GL_UI.recReq && !GL_UI.th1 && GL_UI.main_thEN)
		{
			this->nextTitle->BackColor = System::Drawing::Color::LimeGreen;
			this->nextTitle->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->nextTitle->Text = L"SELECT METHOD:";
			this->methodBox->Enabled = true;
		}
		else
		{
			this->nextTitle->BackColor = System::Drawing::SystemColors::ButtonHighlight;
			this->nextTitle->ForeColor = System::Drawing::SystemColors::Highlight;
			this->nextTitle->Text = L"SELECTED METHOD:";
			this->methodBox->Enabled = false;
		}

		// ii. Record threshold button
		// First: for 1st threshold (does not matter what method)
		if (state == st_th && !GL_UI.recReq && !GL_UI.th1 && GL_UI.main_thEN)
		{
			this->thButton->BackColor = System::Drawing::Color::LimeGreen;

			if (bSingle_mth)
			{
				this->thButton->Text = L"RECORD THRESHOLD";
			}
			else
			{
				this->thButton->Text = L"RECORD RESTING THRESHOLD";
			}
		}
		// Second: recording threshold
		else if ((state == st_th && GL_UI.recReq && !GL_UI.th1) || (state == st_mvc && !GL_UI.th2))
		{
			this->thButton->BackColor = System::Drawing::SystemColors::Highlight;
			this->thButton->Text = L"RECORDING...";
		}
		// Fourth: 1st threshold has been recording and it is necessary to press for the 2nd one
		else if (state == st_th && GL_UI.th1 && !bSingle_mth && !GL_UI.set_MVC)
		{
			this->thButton->BackColor = System::Drawing::Color::LimeGreen;
			this->thButton->Text = L"RECORD MVC THRESHOLD";
		}
		// TH has not been set yet or it has already been set
		else
		{
			this->thButton->BackColor = System::Drawing::SystemColors::Highlight;
			this->thButton->Text = L"THRESHOLD RECORDED";
		}
		//-----------------------------------------------------------------


		// SELECT EXERCISE
		if (state == st_init)
		{
			this->nextExTitle->BackColor = System::Drawing::Color::LimeGreen;
			this->nextExTitle->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->nextExTitle->Text = L"SELECT EXERCISE:";
			this->exerciseBox->Enabled = true;
		}
		else
		{
			this->nextExTitle->BackColor = System::Drawing::SystemColors::ButtonHighlight;
			this->nextExTitle->ForeColor = System::Drawing::SystemColors::Highlight;
			this->nextExTitle->Text = L"SELECTED EXERCISE:";
			this->exerciseBox->Enabled = false;
		}

		// SELECT STIMULATOR AND RECORDER PORTS
		this->stimPBox->Enabled = (!GL_UI.stimReady);
		this->recPBox->Enabled = (!GL_UI.recReady);

		// For isVelocity testing
		std::stringstream tempVelocity;
		tempVelocity << std::setprecision(7) << GL_UI.isVelocity << " mm/100ms";
		tempString = tempVelocity.str();
		this->velCurrent->Text = gcnew String(tempString.c_str());
	}

private: System::Void testText_Click(System::Object^ sender, System::EventArgs^ e) {

}
};
}
