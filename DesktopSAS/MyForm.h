#pragma once
#include <random>
#include <string>

namespace DesktopSAS {

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
			//
			//TODO: Add the constructor code here
			//
			int dummy = 0;
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
	public: System::Windows::Forms::Button^ start_Button;
	     	System::ComponentModel::BackgroundWorker^ backgroundWorker1;
			System::Windows::Forms::Label^ message1;
			System::Windows::Forms::Label^ backLabel;
			int dummy;

	protected:

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->start_Button = (gcnew System::Windows::Forms::Button());
			this->backgroundWorker1 = (gcnew System::ComponentModel::BackgroundWorker());
			this->message1 = (gcnew System::Windows::Forms::Label());
			this->backLabel = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// start_Button
			// 
			this->start_Button->Location = System::Drawing::Point(13, 13);
			this->start_Button->Name = L"start_Button";
			this->start_Button->Size = System::Drawing::Size(109, 23);
			this->start_Button->TabIndex = 0;
			this->start_Button->Text = L"Press me!";
			this->start_Button->UseVisualStyleBackColor = true;
			this->start_Button->Click += gcnew System::EventHandler(this, &MyForm::start_pressed);
			// 
			// backgroundWorker1
			// 
			this->backgroundWorker1->WorkerSupportsCancellation = true;
			this->backgroundWorker1->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &MyForm::backgroundWorker1_DoWork);
			this->backgroundWorker1->RunWorkerCompleted += gcnew System::ComponentModel::RunWorkerCompletedEventHandler(this, &MyForm::backgroundWorker1_RunWorkerCompleted);
			// 
			// message1
			// 
			this->message1->Location = System::Drawing::Point(13, 84);
			this->message1->Name = L"message1";
			this->message1->Size = System::Drawing::Size(213, 13);
			this->message1->TabIndex = 1;
			this->message1->Text = L"message1";
			// 
			// backLabel
			// 
			this->backLabel->Location = System::Drawing::Point(13, 119);
			this->backLabel->Name = L"backLabel";
			this->backLabel->Size = System::Drawing::Size(213, 13);
			this->backLabel->TabIndex = 2;
			this->backLabel->Text = L"backgroundWorker Text";
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(284, 261);
			this->Controls->Add(this->backLabel);
			this->Controls->Add(this->message1);
			this->Controls->Add(this->start_Button);
			this->Name = L"MyForm";
			this->Text = L"MyForm";
			this->ResumeLayout(false);

		}
#pragma endregion
	// Start background worker
	private: System::Void start_pressed(System::Object^ sender, System::EventArgs^ e) {
		this->message1->Text = L"Started backgroundWorker";
		this->backgroundWorker1->RunWorkerAsync(2000);
	}
	// End background worker
	private: System::Void cancel_pressed(System::Object^ sender, System::EventArgs^ e) {
		this->message1->Text = L"Close BackgroundWorker";
		// Close background task
		this->backgroundWorker1->CancelAsync();
	}

	private: System::Void backgroundWorker1_DoWork(System::Object^ sender, DoWorkEventArgs^ e)
	{
		// Do not access the form's BackgroundWorker reference directly.
		// Instead, use the reference provided by the sender parameter.
		//BackgroundWorker bw; // = sender();// as BackgroundWorker;

		dummy++;
		this->backLabel->Text = L"backgroundWorker was here";
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
			this->backLabel->Text = L"Operation was canceled";
		}
		else if (e->Error)
		{
			// There was an error during the operation.
			//string msg = "An error occurred: {0}" + e.Error.Message;
			//MessageBox.Show(msg);
			this->backLabel->Text = L"An error occurred: {0}";
		}
		else
		{
			// The operation completed normally.
			//string msg = String.Format("Result = {0}", e.Result);
			//System::Windows::Forms::MessageBox::Show(msg);
			this->backLabel->Text = L"Result = {0}";
		}
	}

		   int TimeConsumingOperation(int sleepPeriod)
		   {
			   int result = 0;


			   while (!this->backgroundWorker1->CancellationPending)
			   {
				   bool exit = false;
				   switch (dummy)
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
