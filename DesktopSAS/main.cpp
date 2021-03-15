#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4507 34)
#include <iostream>
#include <fstream>
#include <conio.h>
#include <stdlib.h>
#include <cmath>
#include <iomanip>
#include <complex>
#include <stdio.h>
#include <thread>
#include <ctime>
#include <sstream>
#include <ctype.h>
#include <vector>
#include <complex>
#include <math.h>
// User - defined libraries
#include "sasGui.h"
#include "GUI.h"
#include "Iir.h"
#include "SASLIB.hpp"

// Small note: error "sprintf is ambiguous" can be ignored.

using namespace System;
using namespace System::Windows::Forms;

// ------------------------- Main global variables --------------------------------
state_Type GL_state = st_init;

struct device_to_device
{
	bool start = false;
	bool end = false;
	bool th = false;
	bool ready = false;
	bool req = false;
} rec_status, MAIN_to_all, stim_status;
//MAIN_to_all: the main function writes here and all threads read

// Exercise and method settings
exercise_Type GL_exercise = exCircuit, GL_exercise_old = exCircuit; // upperLeg_extexCircuit;
exercise_Type GL_exhmi = exCircuit;

threshold_Type GL_thMethod = th_SD05, GL_thMethod_old = th_SD05; // upperLeg_extexCircuit;
threshold_Type GL_thhmi = th_SD05;

// User options
RehaMove3_Req_Type Move3_cmd = Move3_none;
RehaMove3_Req_Type Move3_hmi = Move3_none;
RehaMove3_Req_Type Move3_key = Move3_none;
User_Req_Type User_cmd = User_none, user_gui = User_none;

// ------------------------- Devices handling --------------------------------
bool stim_ready = false, rec_ready = false, stim_abort = false;

char PORT_STIM[5] = "COM9";   // Laptop
// char PORT_STIM[5] = "COM3";     // Robot
RehaMove3 stimulator;

// char PORT_REC[5] = "COM4";    // Laptop
char PORT_REC[5] = "COM4";      // Robot
RehaIngest recorder;

// Flag variables
bool stim_fl0 = false, stim_fl1 = false, stim_fl2 = false, stim_fl3 = false, stim_fl4 = false;
bool rec_fl0 = false, rec_fl1 = false, rec_fl2 = false, rec_fl3 = false, rec_fl4 = false;
bool main_fl0 = false, main_fl1 = false, main_init = false, main_1stSet = false;
bool main_thEN = false;
// ------------------------- UDP / TCP  ------------------------
int udp_cnt = 0;

char ROBOT_IP_E[15] = "127.0.0.1";
char ROBOT_IP[15] = "172.31.1.147";
uint32_t ROBOT_PORT = 30007;
UdpClient robert(ROBOT_IP_E, ROBOT_PORT);

char SCREEN_ADDRESS[15] = "127.0.0.1";
char SCREEN_PORT[15] = "30002";
// TcpServer screen(SCREEN_PORT); // Using TCP-IP protocol
UdpServer screen(SCREEN_ADDRESS, SCREEN_PORT); // Using UDP-IP protocol
tcp_msg_Type screen_status;

bool start_train = false;

// ------------------------- Stimulator calibration  ------------------------
Smpt_Channel hmi_channel = Smpt_Channel_Red;
// State process variables
bool stim_done = false,
stim_userX = false, stim_auto_done = false;

// Stimulation timing settings
double stimA_cycle = 8.0; // how long the stimulator is allowed to be active (seconds)
std::chrono::duration<double> stimA_diff;
auto stimA_start = std::chrono::steady_clock::now();
auto stimA_end = std::chrono::steady_clock::now();
bool stimA_start_b = false, stimA_end_b = false;
bool stimA_active = false, stimM_active = false;
bool stim_timing = false, stim_timeout = false;
// ------------------------------ Timing -------------------
// Dummies - Measuring time:
//-time 1 = from when threshold has been passed until the stimulator starts (FES_cnt)
//-time 2 = filtering perfomance (EMG_tic)
//-time 3 = process perfomance: state machine + recorder + stimulator time
//-time 4 = interface perfomance: keyboard + UDP + TCP time
ofstream time1_f, time2_f, time3_f, time4_f;
double time1_d, time2_d, time3_d, time3_v2_d, time4_d;
std::vector<double> time1_v, time2_v, time3_v, time3_v2, time4_v;
std::chrono::duration<double> time1_diff, time2_diff, time3_diff, time4_diff;
auto time1_start = std::chrono::steady_clock::now();
auto time1_end = std::chrono::steady_clock::now();
auto time2_start = std::chrono::steady_clock::now();
auto time2_end = std::chrono::steady_clock::now();
auto time3_start = std::chrono::steady_clock::now();
auto time3_end = std::chrono::steady_clock::now();
auto auto_start_stim = std::chrono::steady_clock::now();
auto auto_end_stim = std::chrono::steady_clock::now();
auto time4_start = std::chrono::steady_clock::now();
auto time4_end = std::chrono::steady_clock::now();
std::chrono::duration<double> auto_diff_stim, auto_diff_rec;


// Files variables for names and handling
char date[DATE_LENGTH];
char folder[DATE_LENGTH] = "files\\";
char Sname[DATE_LENGTH] = "subject";
char file_dir[256], th_s[256], date_s[256], filter_s[256], logs_s[256];
char time1_s[256], time2_s[256], time3_s[256], time4_s[256];

ofstream fileLOGS;
bool dummyMain = false, dummyMain2 = false;
// ---------------------------- Functions declaration  -----------------------------
// Dummies
bool GL_tcpActive = true;

// Sub-function messages
char msg_modify[512], msg_stimulating[512], msg_recording[512], msg_main_char[512];

// Interface functions
string msg_connect = "", msg_main = "", msg_extGui = "", msg_gui = "";
bool connect_thread_ready = false, run_extGui_ready = false;
void robot_thread();
void screen_thread();
void update_localGui();
// Devices functions
void modify_stimulation(RehaMove3_Req_Type& code, Smpt_Channel sel_ch);
static void stimulating_sas();
static void recording_sas();
// files functions
void start_files();
void end_files();

// ------------------------------ Main  -----------------------------
void mainSAS_thread();

[STAThread]
//void Main(array<String^>^ args)
void Main()
{
    // Start threads 
	std::thread stateMachine(mainSAS_thread);
	std::thread robot(robot_thread);
	std::thread extGUI(screen_thread);

	// Run local GUI
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);

	DesktopSAS::MyForm form;
	Application::Run(% form);

    // Force to all the other threads to close when the local GUI is closed
    MAIN_to_all.end = true;
    GL_UI.END_GUI = true;
	// Wait for the other threads to join
	robot.join();
	extGUI.join();
	stateMachine.join();

}
// ---------------------------- Interface function definitions --------------------------
void update_localGui() {
    // ------------- sas -> gui -------------
    // status
    GL_UI.status = GL_state;
    GL_UI.screenMessage = "Info: ";
    GL_UI.screenMessage += msg_main;
    GL_UI.screenMessage += "\nUDP status: ";
    GL_UI.screenMessage += msg_connect;
    GL_UI.screenMessage += "\nTCP status: ";
    GL_UI.screenMessage += msg_extGui;
    //GL_UI.screenMessage += "\nSimulator: ";
    //GL_UI.screenMessage += msg_stimulating;
    //GL_UI.screenMessage += "\nRecorder: ";
    //GL_UI.screenMessage += msg_recording;
    GL_UI.END_GUI = MAIN_to_all.end;

    // Exercise settings
    GL_UI.recReq = rec_status.req;
    GL_UI.main_thEN = main_thEN;
    GL_UI.method = GL_thMethod;

    // stimulation parameters
    GL_UI.stimActive = stimulator.active;
    GL_UI.current = stimulator.stim[Smpt_Channel_Red].points[0].current;
    GL_UI.ramp = stimulator.stim[Smpt_Channel_Red].ramp;
    GL_UI.frequency = stimulator.fq[Smpt_Channel_Red];

    // ------------- gui -> sas -------------
    // Keys
    Move3_key = GL_UI.Move3_hmi;
    user_gui = GL_UI.User_hmi;
   
    // Logical operations based on the user commands 
    switch (user_gui)
    {
    case User_th:
        if (GL_state == st_th)
        {
            rec_status.req = true;
            msg_gui = "Start threshold.";
        }
        break;
    }

    // Pull down command flags
    user_gui = User_none;
    GL_UI.User_hmi = User_none;
    GL_UI.Move3_hmi = Move3_none;

    // For isMoving testing
    GL_UI.isVelocity = robert.isVelocity;

}

void robot_thread()
{
    // start up
    robert.display = true; //Chosen not to show messages during messages exchange
    do
    {
        msg_connect = robert.displayMsg;
        robert.start();
    } while (robert.error && !MAIN_to_all.end);
    robert.display = true;

    connect_thread_ready = true;
    msg_connect = "Connection to robot-IP stablished.";
    // running loop
    while (!MAIN_to_all.end && (GL_state != st_end))
    {
        // normal thread
        control_thread(INTERFACE_THREAD, THREAD_START, GL_state);

        // UDP Update:
        udp_cnt++;
        sprintf(robert.message, "%d;STATUS", udp_cnt);
        robert.get();

        // local GUI update
        update_localGui();

        // Controlling thread cycle time
        System::Threading::Thread::Sleep(control_thread(INTERFACE_THREAD, THREAD_END, GL_state));

        time4_end = std::chrono::steady_clock::now();
        time4_diff = time4_end - th2_st;

        // Only saving result if the state machine is not potentially closing/opening files
        if (GL_state != st_repeat)
        {
            time4_v.push_back((double)time4_diff.count());
        }

        msg_connect = robert.displayMsg;

    } // running loop

    // finish thread
    robert.end();
} // thread

void screen_thread()
{
    // to send: stimulation parameters + exercise settings + current status -> guiMsg
    char stimMsg[BUFLEN], setMsg[BUFLEN], disMsg[BUFLEN], guiMsg[BUFLEN];
    bool decode_successful;
    char itoaNr[32];
    char longMsg[BUFLEN];
    // Start
    screen.display = true;
    screen.start();
    run_extGui_ready = true;
    screen.display = true;
    // Running -> Need to put back: && (GL_state != st_end)
    while (!MAIN_to_all.end  && !screen.finish)
    {
        // Receive
        screen.check();

        if (!screen.error && GL_tcpActive)
        {
            decode_successful = decode_extGui(screen.recvbuf, screen.finish, screen.playPause, screen_status);

            if (decode_successful)
            {
                if (screen.display)
                {
                    sprintf(longMsg, "From SCREEN received: finish = %d, button= %d, status = %d, msg = %s ", screen.finish, screen.playPause, screen_status, screen.recvbuf);
                    msg_extGui = string(longMsg).c_str();
                }
                // Actions related to what is has been received
                if (screen.finish)
                {
                    MAIN_to_all.end = true;
                    break;
                }
            }
            else if (!decode_successful && screen.display)
            {
                sprintf(longMsg, "SCREEN received message not valid: %s\n ", screen.recvbuf);
                msg_extGui = longMsg;
            }

        }
        else if (screen.error_lim && GL_tcpActive)
        {
           // GL_tcpActive = false;
            msg_extGui = "SAS PROGRAMME: Connection to the screen lost.";
        }
        else if (!screen.error && !GL_tcpActive)
        {
           // GL_tcpActive = true;
            msg_extGui = "SAS PROGRAMME: Connection to the screen restored.";
        }

        // TCP Stuff
        System::Threading::Thread::Sleep(250);
    }
    // End of the Program if the screen has been closed
    msg_extGui = "End screen";
    if (screen.finish)
    {
        //&& GL_tcpActive
        msg_extGui = "Screen App has been closed. SAS program will finish too.";
        MAIN_to_all.end = true;
    }
    // Finish screen socket before closing thread
    screen.end();
}

// ---------------------------- File function definitions --------------------------
void start_files()
{
    // initialize files names
    generate_date(date); //get current date/time in format YYMMDD_hhmm
    
    sprintf(filter_s, "%s%s_filter_%s.txt", folder, Sname, date);
    sprintf(time1_s, "%s%s_time1_%s.txt", folder, Sname, date);
    sprintf(time2_s, "%s%s_time2_%s.txt", folder, Sname, date);
    sprintf(time3_s, "%s%s_time3_%s.txt", folder, Sname, date);
    sprintf(time4_s, "%s%s_time4_%s.txt", folder, Sname, date);
    sprintf(th_s, "%s%s_th_%s.txt", folder, Sname, date);
    sprintf(logs_s, "%s%s_log_%s.txt", folder, Sname, date);
    
    fileLOGS.open(logs_s);

}

void end_files()
{
    fileFILTERS.close();
    fileVALUES.close();
    fileLOGS.close();
    // Safe here the time samples on a file
    time1_f.open(time1_s);
    if (time1_f.is_open())
    {
        for (int k = 0; k < time1_v.size(); k++)
        {
            time1_f << time1_v[k] << " ; " << endl;
        }
        printf("Main: time measurement t1 saved in file.\n");
    }
    else
    {
        printf("Main: Data t1 could not be saved.\n");
    }
    time1_f.close();

    time2_f.open(time2_s);
    if (time2_f.is_open())
    {
        for (int k = 0; k < time2_v.size(); k++)
        {
            time2_f << time2_v[k] << endl;
        }
        printf("Main: time measurement t2 saved in file.\n");
    }
    else
    {
        printf("Main: Data t2 could not be saved.\n");
    }
    time2_f.close();

    time3_f.open(time3_s);
    if (time3_f.is_open() && time3_v.size() >= 2)
    {
        for (int k = 0; k < time3_v.size(); k++)
        {
            time3_f << time3_v[k] << ", " << time3_v2[k] << ";" << endl;
        }
        printf("Main: time measurement t3 saved in file.\n");
    }
    else
    {
        printf("Main: Data t3 could not be saved.\n");
    }
    time3_f.close();

    time4_f.open(time4_s);
    if (time4_f.is_open() && time4_v.size() >= 2)
    {
        for (int k = 0; k < time4_v.size(); k++)
        {
            time4_f << time4_v[k] << ";" << endl;
        }
        printf("Main: time measurement t4 saved in file.\n");
    }
    else
    {
        printf("Main: Data t3 could not be saved.\n");
    }
    time4_f.close();

    time1_v.clear();
    time2_v.clear();
    time3_v.clear();
    time3_v2.clear();
    time4_v.clear();
}

// ---------------------------- MAIN ALGORITHM  --------------------------

void mainSAS_thread()
{
    // ------------- Start up -------------
    // Local variables
    bool devicesReady;
    bool robotReady;
    // initialize files names
    start_files();

    // Flushing input and output buffers
    cout.flush();
    fflush(stdin);

    MAIN_to_all.start = true;
    MAIN_to_all.ready = true;

    msg_main = "Starting up stimulator and recorder.";

    // Waiting for the connection in the other threads to be stablished
    while ((!connect_thread_ready || !run_extGui_ready)&&!MAIN_to_all.end) {
        System::Threading::Thread::Sleep(200);
        GL_UI.screenMessage = "";
        if (!connect_thread_ready) {
            GL_UI.screenMessage += "Waiting for connection with robot.\n";
        }
        if (!run_extGui_ready) {
            GL_UI.screenMessage += "Waiting for screen server to be set up.\n";
        }
    }

    // ------------- State machine loop -------------
    while (!MAIN_to_all.end && GL_state != st_end)
    {
        control_thread(MAIN_THREAD, THREAD_START, GL_state);

        recording_sas();

        // State machine
        switch (GL_state)
        {
        case st_init:
            devicesReady = rec_status.ready && stim_status.ready;
            // New: no more automatic calibration
            // when the first sets starts, it allows to set up the stimulation
            // Edit: choosing method on the threshold state
            if (devicesReady && screen_status == start)
            {
                msg_main = "Setting up stimulation. Finish set to start exercise.";
                GL_state = st_calM;
            }
            else if (devicesReady && !screen_status == start)
            {
                msg_main = "Devices ready. Waiting for robot to start.";
            }
            break;

        case st_calM:
            // New: set up done when the first set up finishes
            if (rec_status.ready && screen_status == setDone && !stimulator.active)
            {
                msg_main = "Finished calibration set. Waiting to start 2nd set";
                GL_state = st_th;
            }
            break;

            // Normal SAS process
        case st_th:
            // New: wait until the user is at the beginning of a repetition
            if (screen_status == start && !main_thEN)
            {
                msg_main = "2nd set started. Select a method and press SET THRESHOLD";
                main_thEN = true;
            }
            if (rec_status.th) // && screen_status == repStart
            {
                msg_main = "Threshold saved. Press start training button. Waiting for stimulator to be triggered.";
                // Update exercise settings
                //GL_thMethod_old = GL_thMethod;

                GL_state = st_wait;
                main_1stSet = false;
                main_thEN = false;
            }
            else if (!rec_status.req && rec_status.th)
            {
                //msg_main = "Choose a method and press SET THRESHOLD";
                GL_thMethod = GL_UI.next_method;
            }
            break;

        case st_wait:
            if (screen_status == exDone)
            {
                // Exercise has been aborted
                msg_main = "Received: exercise_done";

                GL_state = st_stop;
                fileLOGS << "2.0, " << GL_processed << "\n";
            }
            else if (rec_status.start)
            {
                // normal trigger
                GL_state = st_running;

                msg_main = "Stimulator triggered";

                fileLOGS << "1.0, " << GL_processed << "\n";

            }
            // For GUI testing:  screen_status == repEnd
            // Final version: (robert.Reached && robert.valid_msg)
            else if (screen_status == repEnd)
            {
                // Patient has reached end of repetition
                msg_main = "Received: end of repetition";

                if (robert.Reached || robert.isMoving)
                {
                    msg_main += ". Waiting for robot to return to start position.";
                }

                GL_state = st_stop;
                fileLOGS << "2.0, " << GL_processed << "\n";
            }
            break;

        case st_running:
            if (screen_status == exDone)
            {
                // Exercise has been aborted
                msg_main = "Received: exercise_done";

                GL_state = st_stop;
                fileLOGS << "3.0, " << GL_processed << "\n";
            }
            // For GUI testing:  screen_status == repEnd
            // Final version: (robert.Reached && robert.valid_msg)
            else if (screen_status == repEnd)
            {
                // Patient has reached end of repetition
                msg_main = "Received: end of repetition";

                if (robert.Reached || robert.isMoving)
                {
                    msg_main += ". Waiting for robot to return to start position.";
                }

                GL_state = st_stop;
                fileLOGS << "3.0, " << GL_processed << "\n";
            }
            break;

        case st_stop:
            devicesReady = stim_status.ready && rec_status.ready;
            robotReady = !robert.Reached && !robert.isMoving && robert.valid_msg;
            // For GUI testing: screen_status == repStart && devicesReady, taken out && robotReady
            // For robot testing ? = devicesReady && robotReady
            // Next repetition
            if (screen_status == repStart)
            {
                msg_main = "Starting next repetition";

                GL_state = st_wait;
                fileLOGS << "4.0, " << GL_sampleNr << "\n";
            }
            // No more repetitions coming
            else if ((screen_status == setDone) && stim_status.ready && rec_status.ready)
            {
                GL_UI.hmi_repeat = false;
                msg_main = "Set finished. Waiting for next set or for exercise to finish.";
                //GL_state = st_end;
                GL_exercise_old = GL_exercise;
                GL_thMethod_old = GL_thMethod;
                GL_state = st_repeat;
            }
            break;

        case st_repeat:
            main_1stSet = false;
            main_init = false;
            devicesReady = rec_status.ready && stim_status.ready;
            robotReady = robert.valid_msg; // !robert.isMoving && !robert.Reached && 

            // 1. Finish program (if required)
            if (MAIN_to_all.end && devicesReady)
            {
                msg_main = "Finish program";
                end_files();
                GL_state = st_end;
            }
            // 2. Next set (keep going)
            // Maybe modify to: (screen_status == start || screen_status == repStart)
            //For GUI testing: taken out && robotReady
            else if (screen_status == start && devicesReady)
            {
                msg_main = "Starting next set.";
                GL_state = st_wait;
            }
            // 3. Repeat exercise with the same settings
            // check here if the person presses the button before the screen updates to == start?
            //For GUI testing: taken out && robotReady
            else if (screen_status == repeat && devicesReady)
            {
                msg_main = "Repeat exercise.";
                GL_UI.hmi_repeat = true;
                // Repeat same type of exercise
                startup_filters();
                main_fl0 = false;
                main_fl1 = false;

                end_files();
                start_files();

                GL_state = st_th;
            }
            // 4. Exercise done. Go back to the beginning
            //For GUI testing: taken out && robotReady
            else if (screen_status == exDone && devicesReady)
            {
                msg_main = "Exercise finished.";
                startup_filters();
                end_files();
                start_files();
                GL_state = st_init;
            }

            break;
        } // State machine

        stimulating_sas();
        // GUI testing: robot error has been taken out
        /*
        // Process handling: if the robot-connection gets lost
        if (robert.error_lim && MAIN_to_all.ready)
        {
            bool jump_cal = (GL_state == st_calA_go) || (GL_state == st_calA_stop);
            bool jump_run = (GL_state == st_running) || (GL_state == st_wait);
            bool wait_cal = (GL_state == st_init) || (GL_state == st_calM) || (GL_state == st_th);

            if (jump_cal)
            {
                msg_main = "Connection to the robot lost. Switching to manual calibration.";
                GL_state = st_calM;
            }
            else if (jump_run)
            {
                msg_main = "Connection to the robot lost. Stopping current repetition";
                GL_state = st_stop;
            }
            else if (wait_cal)
            {
                msg_main = "Connection to the robot lost. Process will stop after calibration and threshold set up";
            }
            else
            {
                msg_main = "Connection to the robot lost. \nProcess will stop after calibration and threshold set up";
            }
            MAIN_to_all.ready = false;
        }
        else if (!robert.error_lim && !MAIN_to_all.ready)
        {
            msg_main = "Connection to the robot restored";
            MAIN_to_all.ready = true;
        }
        */
        // Controlling thread cycle time
        System::Threading::Thread::Sleep(control_thread(MAIN_THREAD, THREAD_END, GL_state));

        time3_end = std::chrono::steady_clock::now();
        if (GL_state != st_init)
        {
            time3_diff = time3_end - th1_st;
            time3_v.push_back((double)time3_diff.count());
            time3_v2.push_back(GL_state);
        }
    }


	// ------------- End thread -------------
    // Finish devices
    GL_state = st_end;
    recording_sas();
    stimulating_sas();

    // (Just in case) notify all threads the end of the program 
    GL_UI.END_GUI = true;
    MAIN_to_all.end = true;

    // Close files if they are still open
    if (fileFILTERS.is_open())
    {
        end_files();
    }
    // Flushing input and output buffers
    cout.flush();
    fflush(stdin);
}

// ---------------------------- Devices function definitions  --------------------------

void modify_stimulation(RehaMove3_Req_Type& code, Smpt_Channel sel_ch)
{
    // current_Val = real values on the stimulator
    // next_val = values that are going to be assigned
    // code = command (what to change)
    Smpt_ml_channel_config next_val = stimulator.stim[sel_ch];
    double next_fq = stimulator.fq[sel_ch];
    //next_val = stimulator.stim[Smpt_Channel_Red]; // Old
    float Dcurr = 0.0, DHz = 0.0;
    uint8_t Dramp = 0, Dnr = 0;
    bool autoCal_process = (GL_state == st_calA_go) || (GL_state == st_calA_stop); // Automatic cal in progress

    // Automatic calibration
    if (autoCal_process)
    {
        // Select type of increments
        if (next_val.points[0].current <= 20)
        {
            Dcurr = D_CUR_HIGH;
            next_val.ramp = D_RAMP_LOW;
            next_val.number_of_points = D_RAMP_LOW;
        }
        else if (next_val.points[0].current <= 30)
        {
            Dcurr = D_CUR_MED;
            next_val.ramp = D_RAMP_MED;
            next_val.number_of_points = D_RAMP_MED;
        }
        else if (next_val.points[0].current <= 40)
        {
            Dcurr = D_CUR_LOW;
            next_val.ramp = D_RAMP_HIGH;
            next_val.number_of_points = D_RAMP_HIGH;
        }
    }
    // Manual calibration or standard process
    else {
        Dcurr = D_CUR_MAN;
        Dramp = D_RAMP_MAN;
        Dnr = D_POINT_MAN;
        DHz = D_FQ_MAN;
    }

    // Apply increments 
    switch (code)
    {
    case Move3_ramp_more:
        next_val.number_of_points += Dnr;
        next_val.ramp += Dramp;
        break;
    case Move3_ramp_less:
        next_val.number_of_points -= Dnr;
        next_val.ramp -= Dramp;
        break;
    case Move3_decr:
        next_val.points[0].current -= Dcurr;
        next_val.points[2].current += Dcurr;
        break;
    case Move3_incr:
        next_val.points[0].current += Dcurr;
        next_val.points[2].current -= Dcurr;
        break;
    case Move3_Hz_mr:
        next_fq += DHz;
        next_val.period = MS_TO_HZ / next_fq;
        break;
    case Move3_Hz_ls:
        next_fq -= DHz;
        next_val.period = MS_TO_HZ / next_fq;
        break;
    }

    // Checking max and min possible values:
    if (next_val.ramp > MAX_STIM_RAMP)
    {
        next_val.ramp = MAX_STIM_RAMP;
        next_val.number_of_points = MAX_STIM_RAMP;
    }
    else if (next_val.ramp < MIN_STIM_RAMP)
    {
        next_val.ramp = MIN_STIM_RAMP;
        next_val.number_of_points = MIN_STIM_RAMP;
    }

    if (next_val.points[0].current > MAX_STIM_CUR)
    {
        next_val.points[0].current = MAX_STIM_CUR;
        next_val.points[2].current = -MAX_STIM_CUR;
    }
    else if (next_val.points[0].current < MIN_STIM_CUR)
    {
        next_val.points[0].current = MIN_STIM_CUR;
        next_val.points[2].current = MIN_STIM_CUR;
    }

    if (next_fq > MAX_STIM_FQ)
    {
        next_fq = MAX_STIM_FQ;
        next_val.period = MS_TO_HZ / next_fq;
    }
    else if (next_fq < MIN_STIM_FQ)
    {
        next_fq = MIN_STIM_FQ;
        next_val.period = MS_TO_HZ / next_fq;
    }
    // Update values
    stimulator.stim[sel_ch] = next_val;
    stimulator.fq[sel_ch] = next_fq;
    // stimulator.stim[Smpt_Channel_Red] = next_val; // Old
    //if ((code != Move3_stop) && (code != Move3_start) && (screen.display))
    //{
    sprintf(msg_modify,"RehaMove3 message: Stimulation update -> current = %2.2f, period = %2.7f, frequency = %2.2f\n", stimulator.stim[sel_ch].points[0].current, stimulator.stim[sel_ch].period, stimulator.fq[sel_ch]);
    //%2.1f is the format to show a float number, 2.1 means 2 units and 1 decimal
//}
// Update commands
    code = Move3_none;
    Move3_cmd = Move3_none;
    Move3_key = Move3_none;
    GL_UI.Move3_hmi = Move3_none;
}

void stimulating_sas()
{
    // Local Variables
    bool Move3_user_req = false, Move3_user_gui = false, Move3_user_key = false;

    switch (GL_state)
    {
    case st_init:
        // initialization
        if (!stimulator.ready)
        {
            // Choose an exercise
            load_stim_settings();
            stimulator.display = true;
            stimulator.init(PORT_STIM, GL_exercise);
            stim_fl1 = false;
        }
        else
        {
            sprintf(msg_stimulating, "Reha Move3 ready");
        }
        stim_status.ready = stimulator.ready;
        break;

    case st_calM:
        Move3_user_key = (Move3_key != Move3_done) && (Move3_key != Move3_stop) && (Move3_key != Move3_none) && (Move3_key != Move3_start) && (Move3_key != Move3_en_ch);
        //Update stimulation parameters if a key associated with parameters has been pressed
        if (Move3_user_key)
        {
            modify_stimulation(Move3_key, hmi_channel);
        }

        // Quit
        if (((Move3_key == Move3_stop  || stim_timeout || screen_status == setDone) && stimulator.active) || !screen.playPause)
        {
            stimulator.pause();
            stim_done = !stimulator.active && stim_timeout;
            Move3_hmi = Move3_none;
            Move3_key = Move3_none;
            if (stim_done)
            {
                sprintf(msg_stimulating, "Stimulation set up finished. \nPress set threshold");
                msg_main = "Stimulation set up finished. \nPress set threshold";
                
            }
            else
            {
                sprintf(msg_stimulating, "Stimulation stopped");
                msg_main = "Stimulation stopped";
            }
        }

        if ((stimulator.active || Move3_hmi == Move3_start || Move3_key == Move3_start) && !stim_timeout)
        {
            stimulator.update();
        }
        break;

        // Normal SAS process
    case st_wait:
        stim_fl0 = false;
        stim_fl1 = false;
        stim_fl2 = false;
        stim_fl3 = false;
        stim_fl4 = false;
        stim_status.ready = true;
        stim_done = false;
        // modify parameters if a button is pressed
        Move3_user_key = (Move3_key != Move3_done) && (Move3_key != Move3_stop) && (Move3_key != Move3_none) && (Move3_key != Move3_start) && (Move3_key != Move3_en_ch);

       if (Move3_user_key)
       {
           modify_stimulation(Move3_key, Smpt_Channel_Red);
       }
        break;

    case st_running:
        // modify parameters if a button is pressed
        Move3_user_key = (Move3_key != Move3_done) && (Move3_key != Move3_stop) && (Move3_key != Move3_none) && (Move3_key != Move3_start) && (Move3_key != Move3_en_ch);

        if (Move3_user_key)
        {
            modify_stimulation(Move3_key, Smpt_Channel_Red);
            stim_fl1 = false;
        }
        
        // Stop stimulation
        
        if ((Move3_hmi == Move3_stop || Move3_key == Move3_stop || stim_timeout) && stimulator.active || screen_status == exDone || !screen.playPause)
        {
            stimulator.pause();
            sprintf(msg_stimulating, "Stimulation stopped");
            msg_main = "Stimulation stopped";
            stim_fl1 = true;
            // abort exercise
            if (screen_status == exDone) {
                stim_abort = true;
            }
            Move3_hmi = Move3_none;
            Move3_key = Move3_none;
        }
        if (screen.playPause && !stim_timeout && !stim_fl1) // !stim_fl1 necessary?
        {
            stimulator.update();
        }
        
        // things to do only once
        if (!stim_fl2)
        {
            sprintf(msg_stimulating, "Stimulation active");
            msg_main = "Stimulation active";

            time1_end = std::chrono::steady_clock::now();
            time1_diff = time1_end - time1_start;
            time1_v.push_back((double)time1_diff.count());

            stim_fl2 = true;
            stim_status.ready = false;
        }
        
        break;

    case st_stop:
        // Stop process and restore initial values
        if (stimulator.active)
        { // Stop stimulator
            stimulator.pause();
            fileLOGS << "6.0, " << GL_processed << "\n";
        }
        else
        {
            stim_fl0 = false;
            stim_fl1 = false;
            stim_fl2 = false;
            stim_fl3 = false;
            stim_status.ready = true;
        }

        break;

    case st_repeat:
        calCycle_nr = 0;
        stimA_active = false;
        stimM_active = false;
        stim_userX = false;
        stim_auto_done = false;
        stim_done = false;
        start_train = false;
        stim_abort = false;

        if (screen_status == exDone) {
            set_stimulation(GL_exercise, stimulator.stim[Smpt_Channel_Red], INIT_FQ);
            stim_status.ready = true;
        }
        else {
            stim_status.ready = true;
        }


        break;

    case st_end:
        if (stimulator.ready)
        {
            stimulator.end();
        }
        break;
    } // switch case

    // Stimulation time control
    if (!stim_timing && stimulator.active)
    {
        stimA_start = std::chrono::steady_clock::now();
        stim_timing = true;
    }
    if (stim_timing && stimulator.active && !stim_timeout)
    {
        stimA_end = std::chrono::steady_clock::now();
        stimA_diff = stimA_end - stimA_start;
        stim_timeout = ((double)stimA_diff.count()) >= stimA_cycle;
        if (stim_timeout)
        {
            sprintf(msg_stimulating, "Stimulation timeout");
            msg_main = "Stimulation timeout";
        }
    }
    if (!stimulator.active && stim_timing)
    {
        stim_timing = false;
        stim_timeout = false;
    }

} //void thread_ml_stimulation
//================================================

void recording_sas()
{
    int iterator = 0;
    double mean = 0, temp_value = 0;
    double static value = 0, value_cnt = 0;
    bool st_wait_jump = false;

    switch (GL_state)
    {

    case st_init:
        if (!recorder.ready)
        {
            if (recorder.display) { sprintf(msg_recording,"Reha Ingest message: Starting recorder."); }
            Sleep(2500);
            recorder.display = true;
            recorder.init(PORT_REC);
            if (recorder.found) { recorder.start(); }

            // This is just in case the user needs to move the stuff around
            if (!recorder.ready) {
                Sleep(2500);
            }
            else
            {
                sprintf(msg_recording, "Reha Ingest ready");
            }
        }
        startup_filters();
        rec_status.ready = recorder.ready;
        break;

    case st_calM:
        // Discard data
        recorder.record();
        recorder_emg1.clear();
        break;

    case st_th:
        recorder.record();
        // Set threshold button has been pressed
        if (rec_status.req && main_thEN)
        {
            if (recorder.data_received && recorder.data_start && !fileFILTERS.is_open())
            {
                //printf("Data found\n");         // debugging stuff
                fileFILTERS.open(filter_s);
                fileVALUES.open(th_s);
            }

            GL_sampleNr = recorder_emg1.size();
            if (GL_sampleNr - GL_processed >= SAMPLE_LIM)
            {
                // Select threshold method 
                switch (GL_thMethod)
                {
                case th_SD05:
                    temp_value = process_th_SD05(GL_sampleNr, recorder_emg1);
                    break;
                case th_SD03:
                    temp_value = process_th_SD03(GL_sampleNr, recorder_emg1);
                    break;
                default:
                    temp_value = process_th_XX(GL_sampleNr, recorder_emg1);
                }

                //printf("%d\n", temp_value);         // debugging stuff
                if (GL_sampleNr >= TH_DISCARD)
                {
                    THRESHOLD = THRESHOLD + temp_value;
                }
                if (GL_processed >= TH_NR)
                {
                    THRESHOLD = THRESHOLD / (GL_sampleNr - GL_thDiscard);
                    sprintf(msg_recording, "EMG activity: method = %d, threshold = %2.6f", GL_thMethod, THRESHOLD);
                    sprintf(msg_main_char, "EMG activity: method = %d, threshold = %2.6f", GL_thMethod, THRESHOLD);
                    msg_main = msg_main_char;
                    rec_status.th = true;
                    rec_status.req = false;
                }
            }
        }
        else
        { 
            // Discard data until button is pressed
            recorder_emg1.clear();
        }
        break;

    case st_wait:
        recorder.record();
        GL_sampleNr = recorder_emg1.size();
        if (GL_sampleNr - GL_processed >= SAMPLE_LIM)
        {
            mean = process_data_iir(GL_sampleNr, recorder_emg1);

             // Original for software 3.0:
             // st_wait_jump = !rec_status.start && !robert.isMoving && robert.valid_msg; //  && start_train
             // For GUI testing
             //st_wait_jump = !rec_status.start;
             // Final version
             st_wait_jump = !rec_status.start && (robert.isVelocity < GL_UI.isVelocity_limit) && robert.valid_msg;


            if ((mean >= THRESHOLD) && (GL_thWaitCnt > TH_WAIT) && st_wait_jump)
            {
                sprintf(msg_recording, "EMG activity: threshold overpassed");
                msg_main = "EMG activity: threshold overpassed";
                rec_status.start = true;
                time1_start = std::chrono::steady_clock::now();
                rec_fl2 = false;
            }
            else if (mean < THRESHOLD)
            {
                GL_thWaitCnt++;
            }
            else if ((mean >= THRESHOLD) && (GL_thWaitCnt < TH_WAIT))
            {
                GL_thWaitCnt = 0;
                rec_fl2 = false;
            }
        }
        break;

    case st_running:
        recorder.record();
        GL_sampleNr = recorder_emg1.size();
        if (GL_sampleNr - GL_processed >= SAMPLE_LIM)
        {
            mean = process_data_iir(GL_sampleNr, recorder_emg1);
        }
        rec_status.ready = false;
        rec_status.start = false;
        break;

    case st_stop:
        GL_thWaitCnt = 0;
        rec_fl2 = false;
        recorder.record();
        GL_sampleNr = recorder_emg1.size();
        if (GL_sampleNr - GL_processed >= SAMPLE_LIM)
        {
            mean = process_data_iir(GL_sampleNr, recorder_emg1);
        }
        rec_status.ready = true;
        rec_status.th = false;
        break;

    case st_repeat:
        // Discard data
        recorder.record();
        recorder_emg1.clear();
        break;

    case st_end:
        if (recorder.ready)
        {
            recorder.end();
        }
        break;
    }
}
