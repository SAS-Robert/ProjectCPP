// SAS Application
//#include "SASLIB.hpp"
// User interface

#include "MyForm.h"

// Testing multithreading before adding the SAS APP
#include <thread>
#include <random>
#include <string>
// multithreading testing

using namespace System;
using namespace System::IO;
using namespace System::Threading;
using namespace System::ComponentModel;
using namespace System::Windows::Forms;

//[STAThreadAttribute]


//void Main(array<String^>^ args) {
void Main() {
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	DesktopApp02::MyForm gui;
	Application::Run(% gui);
}



// Example from Microsoft Docs in C# converted to C++
/*
namespace BackgroundWorkerExample
{
    public ref class Form1 : System::Windows::Forms::Form
    {
        // Var declaration
    public:
        System::ComponentModel::Container^ components = null;
        System::ComponentModel::BackgroundWorker^ backgroundWorker1;
        System::Windows::Forms::Button^ startBtn;
        System::Windows::Forms::Button^ cancelBtn;
        System::Windows::Forms::MessageBox^ displayStuff; // New
        BackgroundWorker bw; // Background worker

    private:
        //void backgroundWorker1_DoWork(Object sender, EventArgs e)
        void backgroundWorker1_DoWork(Object sender, DoWorkEventArgs  e)
        {
            // Do not access the form's BackgroundWorker reference directly.
            // Instead, use the reference provided by the sender parameter.
            //BackgroundWorker bw; // = sender();// as BackgroundWorker;

            // Extract the argument.
            int arg = (int) e.Argument;

            // Start the time-consuming operation.
            e.Result = TimeConsumingOperation(arg);

            // If the operation was canceled by the user,
            // set the DoWorkEventArgs.Cancel property to true.
            if (bw.CancellationPending)
            {
                e.Cancel= true;
            }
        }

        // This event handler demonstrates how to interpret
        // the outcome of the asynchronous operation implemented
        // in the DoWork event handler.
    private:
        void backgroundWorker1_RunWorkerCompleted(
            Object sender,
            RunWorkerCompletedEventArgs e)
        {
            if (e.Cancelled)
            {
                // The user canceled the operation.
                //MessageBox->Show("Operation was canceled");
                this->displayStuff->Show("Operation was canceled");
            }
            else if (e.Error)
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

        // This method models an operation that may take a long time
        // to run. It can be cancelled, it can raise an exception,
        // or it can exit normally and return a result. These outcomes
        // are chosen randomly.
    private:
        int TimeConsumingOperation(int sleepPeriod)
        {
            int result = 0;

            //Random rand();// = new Random();
            
            random_device rand;
            mt19937 gen(rand());

            while (!bw.CancellationPending)
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

    private:
        void startBtn_Click(Object sender, EventArgs e)
        {
            this->backgroundWorker1->RunWorkerAsync(2000);
        }

    private:
        void cancelBtn_Click(Object sender, EventArgs e)
        {
            this->backgroundWorker1->CancelAsync();
        }

        /// <summary>
        /// Required designer variable.
        /// </summary>


    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    protected:
        ~Form1()
        {
            if (components)
            {
                delete components;
            }
        }

#pragma region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
    private:
        void InitializeComponent()
        {
            this->backgroundWorker1 = (gcnew System::ComponentModel::BackgroundWorker());
            this->startBtn = (gcnew System::Windows::Forms::Button());
            this->cancelBtn = (gcnew System::Windows::Forms::Button());
            this->displayStuff = (gcnew System::Windows::Forms::MessageBox());
            this->SuspendLayout();
            //
            // backgroundWorker1
            //
            this->backgroundWorker1->WorkerSupportsCancellation = true;
            this->backgroundWorker1->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &Form1::backgroundWorker1_DoWork);
            this->backgroundWorker1->RunWorkerCompleted += gcnew System::ComponentModel::RunWorkerCompletedEventHandler(&backgroundWorker1_RunWorkerCompleted);
            //
            // startBtn
            //
            this->startBtn->Location = System::Drawing::Point(12, 12);
            this->startBtn->Name = "startBtn";
            this->startBtn->Size = System::Drawing::Size(75, 23);
            this->startBtn->TabIndex = 0;
            this->startBtn->Text = "Start";
            this->startBtn->Click += gcnew System::EventHandler(this, &Form1::startBtn_Click);
            //
            // cancelBtn
            //
            this->cancelBtn->Location = System::Drawing::Point(94, 11);
            this->cancelBtn->Name = "cancelBtn";
            this->cancelBtn->Size = System::Drawing::Size(75, 23);
            this->cancelBtn->TabIndex = 1;
            this->cancelBtn->Text = "Cancel";
            this->cancelBtn->Click += gcnew System::EventHandler(this, &Form1::cancelBtn_Click);
            // New --------------------------
            // displayStuff
            //
            // Form1
            //
            this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(183, 49);
            this->Controls->Add(this->cancelBtn);
            this->Controls->Add(this->startBtn);
            this->Name = "Form1";
            this->Text = "Form1";
            this->ResumeLayout(false);
        }

    public:
        Form1()
        {
            InitializeComponent();
        }
#pragma endregion


    };

    public class Program
    {
    private:
        Program()
        {
        }

        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application::EnableVisualStyles();
            Application::Run(gcnew Form1());
        }
    };
}
*/

// Some random example that I found from the internet
/*
void OnRunWorkerCompleted(System::Object^ sender, System::ComponentModel::RunWorkerCompletedEventArgs^ e);
void SearchTree(DirectoryInfo^ root);
void OnProgressChanged(System::Object^ sender, System::ComponentModel::ProgressChangedEventArgs^ e);
void OnDoWork(System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e);

ref class DataPacket
{
public:
    String^ fileName;
};

int Main(array<System::String^>^ args)
{
    System::ComponentModel::BackgroundWorker^ bgw;
    bgw = gcnew System::ComponentModel::BackgroundWorker();
    bgw->WorkerReportsProgress = true;
    bgw->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(&OnDoWork);
    bgw->ProgressChanged += gcnew System::ComponentModel::ProgressChangedEventHandler(&OnProgressChanged);
    bgw->RunWorkerCompleted += gcnew System::ComponentModel::RunWorkerCompletedEventHandler(&OnRunWorkerCompleted);

    bgw->RunWorkerAsync();

    Console::ReadKey();
    return 0;
}

void SendFiles()
{

}

void OnDoWork(System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e)
{
    DirectoryInfo^ root = gcnew DirectoryInfo("E:\\Test");
    SearchTree(root);
}

void OnProgressChanged(System::Object^ sender, System::ComponentModel::ProgressChangedEventArgs^ e)
{
    //throw gcnew System::NotImplementedException();
}

void OnRunWorkerCompleted(System::Object^ sender, System::ComponentModel::RunWorkerCompletedEventArgs^ e)
{
    //throw gcnew System::NotImplementedException();
}

void SearchTree(DirectoryInfo^ root)
{
    DataPacket^ dp = gcnew DataPacket();

    array<FileInfo^>^ files = nullptr;
    array<DirectoryInfo^>^ subDirs = nullptr;

    try
    {
        files = root->GetFiles();
    }
    catch (Exception^ ex)
    {
        //
    }

    if (files != nullptr)
    {
        for each (FileInfo ^ file in files)
        {
            dp->fileName = file->Name;
            //Here i want report progress with ReportProgress(0, dp); but how? in gui i know how, but in console no :/
            //
            //
        }
    }

    subDirs = root->GetDirectories();

    for each (DirectoryInfo ^ dirs in subDirs)
    {
        SearchTree(dirs);
    }
}
*/