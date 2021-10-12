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
#include "SASLIBshared.h"
#include "SASLIBgui.h"
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
    bool th2 = false;
    bool ready = false;
    bool req = false;
    bool error = false;
} rec_status, MAIN_to_all, stim_status;
//MAIN_to_all: the main function writes here and all threads read

// Exercise and method settings

threshold_Type GL_thhmi = th_SD05;

// User options
RehaMove3_Req_Type Move3_cmd = Move3_none;
RehaMove3_Req_Type Move3_hmi = Move3_none;
RehaMove3_Req_Type Move3_key = Move3_none;
User_Req_Type User_cmd = User_none, user_gui = User_none;

// ------------------------- Devices handling --------------------------------
bool stim_abort = false, stimAvailable = false, recAvailable = false;
double fixed_value = 0;

//char PORT_STIM[5] = "COM6";   // Laptop
char PORT_STIM[5] = "COM6";     // Robot
RehaMove3 stimulator;
int countPort = 0;

//char PORT_REC[5] = "COM4";    // Laptop
char PORT_REC[5] = "COM4";      // Robot
RehaIngest recorder;
int GL_iterator = 0;

// Flag variables
// Important ones:
//  - stim_fl1: true if the stimulation is stopped
//  - stim_fl2: true if the stimulation is on
bool stim_fl0 = false, stim_fl1 = false, stim_fl2 = false, stim_fl3 = false, stim_fl4 = false;
bool rec_fl0 = false, rec_fl1 = false, rec_fl2 = false, rec_fl3 = false, rec_fl4 = false, rec_fl5 = false;;
bool main_fl0 = false, main_fl1 = false, main_init = false, main_1stSet = false, main_fl2 = false;;
bool main_thEN = false, main_force_repeat = false;
// ------------------------- UDP / TCP  ------------------------
int udp_cnt = 0;

char ROBOT_IP_E[15] = "127.0.0.1";
char ROBOT_IP[15] = "172.31.1.147";
uint32_t ROBOT_PORT = 30009;
UdpClient robert(ROBOT_IP_E, ROBOT_PORT);

char SCREEN_ADDRESS[15] = "127.0.0.1"; // main screen IP address
char SCREEN_PORT[15] = "30002";
char SCREEN_EMG_PORT[15] = "30005";
char SCREEN_DATA_PORT[15] = "30011";
// TcpServer screen(SCREEN_PORT); // Using TCP-IP protocol
UdpServer screen(SCREEN_ADDRESS, SCREEN_PORT); // Using UDP-IP protocol 
UdpServer screenEMG(SCREEN_ADDRESS, SCREEN_EMG_PORT); // Using UPD-IP protocol for sending EMG permanently when recording
UdpServer screenData(SCREEN_ADDRESS, SCREEN_DATA_PORT);
tcp_msg_Type screen_status;

bool start_train = false;

// ------------------------- Stimulator calibration  ------------------------
// This value will be changed on the SAS interface run-time if needed >> default = CH1
Smpt_Channel hmi_channel = Smpt_Channel_Black; 
emgCh_Type emgCH = emgCh0;
// State process variables
bool stim_done = false,
stim_userX = false, stim_auto_done = false, stim_pause = false;

// Stimulation timing settings
double stimA_cycle = 8.0; // how long the stimulator is allowed to be active (seconds)
std::chrono::duration<double> stimA_diff;
auto stimA_start = std::chrono::steady_clock::now();
auto stimA_end = std::chrono::steady_clock::now();
bool stimA_start_b = false, stimA_end_b = false;
bool stimA_active = false, stimM_active = false;
bool stim_timing = false, stim_timeout = false;

// Assist as need flags and timing - AAN feature 
int time2trigger = 10; // seconds to trigger for starting FES
int time2complete = 5; // seconds to complete exercise with FES
std::chrono::duration<double> time2trigger_diff;
std::chrono::duration<double> time2complete_diff;
auto time2trigger_start = std::chrono::steady_clock::now();
auto time2trigger_end = std::chrono::steady_clock::now();
auto time2complete_start = std::chrono::steady_clock::now();
auto time2complete_end = std::chrono::steady_clock::now();
bool mech_as_needed = false; // AAN prerequisites: Active return + assist in completion + SASFlag = true
bool velocity_trigg = false; // AAN prerequisites: Velocity needs to be lower than 7.5mm/s in e.g. 2s
bool trigger_timeout = false, complete_timeout = false; // Flags for timing handling of AAN

// ------------------------------ Timing -------------------
// Dummies - Measuring time:
//-time 1 = from when threshold has been passed until the stimulator starts (FES_cnt)
//-time 2 = filtering perfomance (EMG_tic)
//-time 3 = process perfomance: state machine + recorder + stimulator time
//-time 4 = interface perfomance: keyboard + UDP + TCP time
ofstream time3_f;
double time3_d, time3_v2_d;
std::vector<double> time3_v, time3_v2;
std::chrono::duration<double> time3_diff, time4_diff;
auto time3_start = std::chrono::steady_clock::now();
auto time3_end = std::chrono::steady_clock::now();

// Files variables for names and handling
char date[DATE_LENGTH];
// Location on the SAS computer
//char folder[DATE_LENGTH] = "C:\\Users\\User\\Documents\\SASData\\"; // Old version
//char folder[DATE_LENGTH] = "C:\\SAS Interface\\SASData\\";
char folder[DATE_LENGTH] = "C:\\Users\\AAS\\Documents\\LSR\\Toni\\SAS_Data\\Trash\\";
//char folder[DATE_LENGTH] = "C:\\Users\\Kasper Leerskov\\Downloads\\SASData\\";
char Sname[DATE_LENGTH] = "subject";
char file_dir[256], th_s[256], date_s[256], filter_s[256], logs_s[256], stim_s[256];
char time3_s[256];

ofstream fileLOGS, stimFile;
bool dummyMain = false, dummyMain2 = false;
// ---------------------------- Functions declaration  -----------------------------
bool GL_tcpActive = true;

// Sub-function messages
char msg_modify[512], msg_stimulating[512], msg_recording[512], msg_main_char[512];

// Interface functions
string msg_connect = "", msg_main = "", msg_extGui = "", msg_gui = "", msg_connect_2 = "";
double live_data, live_previous = -11;
bool connect_thread_ready = false, run_extGui_ready = false;
void sendData_thread();
void sendEmgData_thread();
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

void mainSAS_thread();

[STAThread]
//void Main(array<String^>^ args)

int Main()
{
    // Start threads 
    std::thread stateMachine(mainSAS_thread);
    std::thread robot(robot_thread);
    std::thread extGUI(screen_thread);
    std::thread sendtoScreen(sendEmgData_thread);
    std::thread sendData(sendData_thread);

    // Run local GUI
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);

    SASv30::MyForm form;
    Application::Run(% form);

    // Force to all the other threads to close when the local GUI is closed
    MAIN_to_all.end = true;
    GL_UI.END_GUI = true;
    // Wait for the other threads to join
    robot.join();
    sendtoScreen.join();
    sendData.join();
    stateMachine.join();
    //extGUI.join();

    return 0;
}

// ---------------------------- Interface function definitions --------------------------
void update_local_variables() {
    // TODO: include all local variables and check they are being updated
    GL_exercise = screen.exercise;
    GL_thMethod = screen.method;
    emgCH = screen.channel;
}

void update_localGui() {
    // ------------- sas -> gui ------------- 
    // Only used for debugging purposes and see whether the workflow after the merge with SCREEN works
    // status
    GL_UI.status = GL_state;
    GL_UI.END_GUI = MAIN_to_all.end;

    // Exercise settings updated from SCREEN instead of local interface variables
    GL_UI.recReq = rec_status.req;
    GL_UI.main_thEN = main_thEN;
    GL_UI.method = GL_thMethod;
    GL_UI.exercise = GL_exercise;

    // Emg channel
    GL_UI.channel = emgCH;
    GL_UI.channelReady = stimulator.ch_ready;

    // stimulation parameters
    GL_UI.stimActive = stimulator.active;
    GL_UI.current = stimulator.stim[hmi_channel].points[0].current;
    GL_UI.ramp = stimulator.stim[hmi_channel].ramp;
    GL_UI.frequency = stimulator.fq[hmi_channel];
    GL_UI.playPause = robert.playPause;

    // Threshold variables
    GL_UI.th1 = rec_status.th;
    GL_UI.th2 = rec_status.th2;

    // stimulator and recorder ports
    GL_UI.recReady = recorder.ready;// && !rec_status.error;
    GL_UI.stimReady = stimAvailable && !stim_status.error;

    // ------------- gui -> sas -------------
    // Keys
    Move3_key = GL_UI.Move3_hmi;
    user_gui = GL_UI.User_hmi;

    // Logical operations based on the user commands - What is this ?? (12.10.21)
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

/* ---------------------------- File function definitions --------------------------
- fileFILTERS: values regarding the EMG recorded
- fileVALUES: values of the method used, the mean, Threshold, v_size, N_len and exercise
- fileLOGS: events happening 
- stimFILE: values of the stimulation applied
- time3_f: time samples 
*/
void start_files()
{
    // initialize files names
    generate_date(date); //get current date/time in format YYMMDD_hhmm

    sprintf(filter_s, "%s%s_filter_%s.txt", folder, Sname, date);
    sprintf(time3_s, "%s%s_time_%s.txt", folder, Sname, date);
    sprintf(th_s, "%s%s_th_%s.txt", folder, Sname, date);
    sprintf(logs_s, "%s%s_log_%s.txt", folder, Sname, date);
    sprintf(stim_s, "%s%s_stim_%s.txt", folder, Sname, date);

    fileLOGS.open(logs_s);
    stimFile.open(stim_s);
}

void end_files()
{
    fileFILTERS.close();
    fileVALUES.close();
    fileLOGS.close();
    stimFile.close();
    // Safe here the time samples on a file
    time3_f.open(time3_s);
    if (time3_f.is_open() && time3_v.size() >= 2)
    {
        for (int k = 0; k < time3_v.size(); k++)
        {
            time3_f << time3_v[k] << ", " << time3_v2[k] << ";" << endl;
        }
        // printf("Main: time measurement t3 saved in file.\n");
    }
    else
    {
        printf("Main: Data t3 could not be saved.\n");
    }
    time3_f.close();

    time3_v.clear();
    time3_v2.clear();
}

/* ---------------------------- Threads function definitions --------------------------
The are several threads running in parallel for handling the data communication in between
SAS - control software of the robot - the screen. There is a UDP protocol listening for the input
into SAS from the control software and another one listening from the screen. There is another UDP
for live-stream sending EMG data to the screen. 
*/

void sendData_thread() 
{
    // Initial values of the variables in the SCREEN
    bool stimulating_previous = false, disconnected_previous = false;
    double threshold_saved = 0.0;

    screenData.display = true;
    do
    {
        screenData.start();
    } while (screenData.error && !MAIN_to_all.end);
    screenData.display = true;

    screenData.check();

    // main loop of the sending thread
    while (!MAIN_to_all.end && (GL_state != st_end))
    {
        bool stimulating_now = stimulator.active;
        bool disconnection_now = (stim_status.error || rec_status.error);
        double threshold_now = THRESHOLD;

        if (stimulating_now != stimulating_previous)
        {
            ostringstream now_str;
            now_str << boolalpha << stimulating_now;
            screenData.streamCommands("STIM_STATUS;" + now_str.str());
            stimulating_previous = stimulating_now;
        }

        if (threshold_now != threshold_saved)
        {
            string now_th;
            now_th = to_string(threshold_now);
            screenData.streamCommands("THRESHOLD_VALUE;" + now_th);
            threshold_saved = threshold_now;
        }

        if (disconnection_now != disconnected_previous)
        {
            ostringstream disc_now;
            disc_now << boolalpha << disconnection_now;
            screenData.streamCommands("DISCONNECTED;" + disc_now.str());
            disconnected_previous = disconnection_now;
        }
    }

    screenData.end();
}

// Thread for the UDP communication for sending - screen is the UdpServer
void sendEmgData_thread()
{
    screenEMG.display = true;
    do
    {
        screenEMG.start();
    } while (screenEMG.error && !MAIN_to_all.end);
    screenEMG.display = true;

    screenEMG.check();

    // main loop of the sending thread
    while (!MAIN_to_all.end && (GL_state != st_end))
    {
        if (GL_state != st_stop && GL_state != st_repeat)
        {
            if (live_data != live_previous)
            {
                screenEMG.stream(live_data);
                live_previous = live_data;
            }
            else
            {
                // Do nothing
            }
        }
    }

    screenEMG.end();
}

// Thread for the UDP communication - robert is the UdpSever
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
        if (GL_state == st_th) // !robert.legSaved &&
        {
            // st_th is the safest moment to get the leg weight data
            udp_cnt++;
            sprintf(robert.message, "1;WEIGHT", udp_cnt);
            robert.getWeight();
        }
        else
        {
            udp_cnt++;
            sprintf(robert.message, "%d;STATUS", udp_cnt);
            robert.get();
        }
        // local GUI update
        update_localGui();

        // Controlling thread cycle time
        System::Threading::Thread::Sleep(control_thread(INTERFACE_THREAD, THREAD_END, GL_state));

        msg_connect = robert.displayMsg;

    } // running loop

    // finish thread
    robert.end();
} // thread

// Thread for the UDP communication for receiving - screen is the UdpClient
void screen_thread()
{
    // to send: stimulation parameters + exercise settings + current status -> guiMsg
    // char stimMsg[BUFLEN], setMsg[BUFLEN], disMsg[BUFLEN], guiMsg[BUFLEN];
    // char itoaNr[32];
    bool decode_successful;
    char longMsg[BUFLEN];
    // Start
    screen.display = true;
    screen.start();
    run_extGui_ready = true;
    screen.display = true;
    // Running -> Need to put back: && (GL_state != st_end)
    while (!MAIN_to_all.end && !screen.finish)
    {
        // Receive
        screen.check();

        if (!screen.error && GL_tcpActive)
        {
            decode_successful = decode_successful = decode_screen(screen.recvbuf, screen.finish, screen.playPause, screen.res_level, screen.pulse_width,
                screen.amplitude, screen.frequency, screen.exercise, screen.method, screen.trigger_gain, screen.start_stop,
                screen.auto_trigger, screen.time_vel_th, screen.vel_th, screen.stim_port, screen.rec_port, screen.channel, screen.velocity,
                screen_status);

            if (decode_successful)
            {
                update_local_variables();
                update_localGui();
                if (screen.display)
                {
                    sprintf(longMsg, "Message from SCREEN received");
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

/* ---------------------------- MAIN ALGORITHM  --------------------------
This thread handles the main algorithm of SAS. Here a state machine loop is used.
*/
void mainSAS_thread()
{
    // ------------- Start up -------------
    // Local variables
    bool devicesReady;
    bool robotReady;
    // initialize files names and filters
    start_files();
    startup_filters();

    // Flushing input and output buffers
    cout.flush();
    fflush(stdin);

    MAIN_to_all.start = true;
    MAIN_to_all.ready = true;

    //msg_main = "Starting up stimulator and recorder.";
    msg_main = "Starting up connection with ROBERT.";
    // Waiting for the connection in the other threads to be stablished
    while ((!connect_thread_ready || !run_extGui_ready) && !MAIN_to_all.end) {
        System::Threading::Thread::Sleep(200);
        msg_main = "";
        if (!connect_thread_ready) {
            msg_main += "Waiting for connection with robot.\n";
        }
        if (!run_extGui_ready) {
            msg_main += "Waiting for screen server to be set up.\n";
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
                msg_main = "Set up stimulation parameters.\n- Complete first set to start exercise.";
                GL_state = st_calM;
            }
            else if (devicesReady && !screen_status == start)
            {
                msg_main = "Stimulator and recorder ready.\n- Select and exercise.\n";
                msg_main += "- Plan on ROBERT a calibration set + at least one exercise set.";
            }
            else if (!devicesReady)
            {
                msg_main = "\nRehaMove3: ";
                msg_main += msg_stimulating;
                msg_main += "\nRehaIngest: ";
                msg_main += msg_recording;
            }

            //GL_exercise = GL_UI.next_exercise;
            GL_exercise = screen.exercise;
            break;

        case st_calM:
            // New: set up done when the first set up finishes
            if (rec_status.ready && screen_status == setDone && !stimulator.active)
            {
                msg_main = "Calibration set completed. Waiting to start exercise set.";
                statusList[(int)st_th] = "On hold";
                GL_state = st_th;
            }
            else if (screen_status == exDone || screen_status == msgEnd)
            {
                GL_state = st_init;
            }
            else if (screen_status != setDone && screen_status != exDone)
            {
                msg_main = "Set up stimulation parameters.\n";
                //msg_main += msg_stimulating; // add the stimulator messages
                msg_main += "\n- Complete first set to start exercise.";
            }
            break;

            // Normal SAS process
        case st_th:
            // New: wait until the user is at the beginning of a repetition
            if (screen_status == start && !main_thEN)
            {
                statusList[(int)st_th] = "Setting threshold";
                msg_main = "Select a method and press RECORD THRESHOLD";
                main_thEN = true;
            }

            if (rec_status.th) // && screen_status == repStart
            {
                //differenciate bettween single-th and MVC methods
                if ((GL_thMethod == th_MVC05 || GL_thMethod == th_MVC10) && GL_UI.set_MVC)
                {
                    GL_state = st_mvc;
                    msg_main = "Setting MVC - 2nd threshold.";
                }
                else if (GL_thMethod != th_MVC05 && GL_thMethod != th_MVC10) {
                    GL_state = st_wait;
                    msg_main = "Threshold saved. Press patient button to begin exercise.";
                    // Update exercise settings
                    robert.playPause = false; // start the first set with the stimulation disabled
                    main_1stSet = false;
                    main_thEN = false;
                }
                else if ((GL_thMethod == th_MVC05 || GL_thMethod == th_MVC10) && !GL_UI.set_MVC)
                {
                    msg_main = "Threshold saved. Press Set MVC for 2nd threshold.";
                }
            }
            else if (rec_status.req && !rec_status.th)
            {
                statusList[(int)st_th] = "Recording threshold";
                msg_main = "Recording threshold.";
            }
            else if (!rec_status.req && !rec_status.th)
            {
                //msg_main = "Choose a method and press SET THRESHOLD";
                GL_thMethod = GL_UI.next_method;
            }

            // Abort
            if (screen_status == exDone || screen_status == msgEnd || rec_status.error)
            {
                GL_state = st_repeat;
                main_force_repeat = rec_status.error;
            }
            break;


            // New MVC methods state
        case st_mvc:

            if (rec_status.th2)
            {
                msg_main = "MVC threshold set. Press patient button to begin exercise.";
                // Update exercise settings
                robert.playPause = false; // start the first set with the stimulation disabled
                main_1stSet = false;
                main_thEN = false;
                GL_UI.set_MVC = false;
                GL_state = st_wait;
            }
            else
            {
                msg_main = "Recording 2nd threshold.";
            }

            // Abort
            if (screen_status == exDone || screen_status == msgEnd || rec_status.error)
            {
                GL_state = st_repeat;
                main_force_repeat = rec_status.error;
            }
            break;

        case st_wait:
            // screen message on the run
            if (robert.playPause)
            {
                msg_main = "Waiting for trigger.";
            }
            else if (!robert.playPause)
            {
                msg_main = "Press patient button to allow stimulation.";
                time2trigger_start = std::chrono::steady_clock::now(); // AAN timer
            }

            if (screen_status == exDone || screen_status == msgEnd || rec_status.error)
            {
                // Exercise has been aborted
                msg_main = "Exercise done.";

                GL_state = st_repeat;
                fileLOGS << "2.0, " << GL_processed << "\n";
                main_force_repeat = rec_status.error;
            }
            else if (rec_status.start && robert.playPause)
            {
                // normal trigger in any case (normal SAS or AAN)
                GL_state = st_running;
                msg_main = "Stimulation triggered";
                fileLOGS << "1.0, " << GL_processed << "\n";
                time2complete_start = std::chrono::steady_clock::now();
                complete_timeout = false;
            }
            else if (mech_as_needed && !rec_status.start && robert.playPause)
            {
                // analyse the timing for the AAN timeout
                time2trigger_end = std::chrono::steady_clock::now();
                time2trigger_diff = time2trigger_end - time2trigger_start;
                if (time2trigger_diff.count() >= time2trigger)
                {
                    //OutputDebugString(std::to_string(time2trigger_diff.count()).c_str());
                    // AAN not triggered in time
                    GL_state = st_running;
                    msg_main = "Stimulation not triggered. Timeout.";
                    fileLOGS << "5.0, " << GL_processed << "\n";
                    // Inmediately after, FES need to be activated - flag updated
                    trigger_timeout = true;
                    time2complete_start = std::chrono::steady_clock::now();
                }
                complete_timeout = false;
            }
            // The end point has been reached before the stimulation has been triggered
            else if (screen_status == repEnd)
            {
                // Patient has reached end of repetition
                msg_main = "End-Point reached";

                if (robert.Reached || robert.isMoving)
                {
                    msg_main += ". Waiting for robot to return to start position.";
                }

                GL_state = st_stop;
                fileLOGS << "2.0, " << GL_processed << "\n";
            }

            break;

        case st_running:
            // screen message on the run
            if (!(screen_status == exDone || screen_status == msgEnd) && !robert.Reached)
            {
                msg_main = msg_stimulating;
            }

            if ((screen_status == exDone || screen_status == msgEnd || rec_status.error) && !stimulator.active)
            {
                // Exercise has been aborted
                msg_main = "Exercise done";

                GL_state = st_repeat;
                fileLOGS << "3.0, " << GL_processed << "\n";
                main_force_repeat = rec_status.error;
            }
            // End Point reached or set finished 
            else if (((robert.Reached && robert.valid_msg) || screen_status == setDone) && !stimulator.active)
            {
                // Patient has reached end of repetition
                msg_main = "End-Point reached";

                if (robert.Reached || robert.isMoving)
                {
                    msg_main += ". Waiting for robot to return to start position.";
                }

                GL_state = st_stop;

                fileLOGS << "3.0, " << GL_processed << "\n";
            }
            // Completion timeout for AAN
            else if (mech_as_needed && stimulator.active && (!robert.Reached && robert.valid_msg))
            {
                // analyse the timing for the AAN timeout
                time2complete_end = std::chrono::steady_clock::now();
                time2complete_diff = time2complete_end - time2complete_start;
                if ((double)time2complete_diff.count() >= time2complete)
                {
                    complete_timeout = true;
                    // Mechanical assistance needed for completion
                    msg_main = "Mechanical assistance activated. Waiting to complete exercise.";
                }
            }
            break;

        case st_stop:
            // Display message
            msg_main = "Reached end-point. Waiting for robot to return to the start.";
            complete_timeout = false;
            trigger_timeout = false;
            //velocity_trigg = false;

            devicesReady = stim_status.ready && (rec_status.ready || rec_status.error);
            robotReady = !robert.Reached && !robert.isMoving && robert.valid_msg;
            // For GUI testing: screen_status == repStart && devicesReady, taken out && robotReady
            // For robot testing ? = devicesReady && robotReady
            // Next repetition
            if (screen_status == repStart && devicesReady && robert.valid_msg)
            {
                msg_main = "Starting next repetition";

                GL_state = st_wait;
                fileLOGS << "4.0, " << GL_sampleNr << "\n";
            }
            // No more repetitions coming
            else if ((screen_status == setDone || screen_status == msgEnd || screen_status == exDone || rec_status.error) && devicesReady)
            {
                GL_UI.hmi_repeat = false;
                msg_main = "Set finished. Waiting for next set or next exercise.";
                GL_state = st_repeat;
                robert.legSaved = false;
                main_force_repeat = rec_status.error;
            }

            break;

        case st_repeat:
            main_1stSet = false;
            main_init = false;
            complete_timeout = false;
            trigger_timeout = false;
            //velocity_trigg = false;
            devicesReady = rec_status.ready && stim_status.ready && !stim_status.error && !rec_status.error;
            robotReady = robert.valid_msg; // && !robert.Reached 

            // 1. Finish program (if required)
            if (MAIN_to_all.end && devicesReady)
            {
                msg_main = "Finish program";
                end_files();
                GL_state = st_end;
            }
            // 2. Next set (keep going)
            // this is not available if the connection to the recorder has been lost
            else if (screen_status == start && devicesReady && !main_force_repeat)
            {
                msg_main = "Starting next set.";
                GL_state = st_wait;
            }
            // 3. Repeat exercise with the same settings
            // check here if the person presses the button before the screen updates to == start?
            else if (screen_status == repeat && devicesReady && robotReady)
            {
                msg_main = "Repeating exercise.";
                main_thEN = false;
                GL_UI.hmi_repeat = true;
                GL_UI.set_MVC = false;
                // Repeat same type of exercise
                main_fl0 = false;
                main_fl1 = false;

                end_files();
                startup_filters();
                start_files();

                statusList[(int)st_th] = "On hold";
                GL_state = st_th;
                main_force_repeat = false;
            }
            // 4. Exercise done. Go back to the beginning
            else if (screen_status == exDone && devicesReady && robotReady)
            {
                msg_main = "Exercise finished.";
                end_files();
                startup_filters();
                start_files();
                GL_state = st_init;
                main_force_repeat = false;
            }

            break;
        } // State machine

        stimulating_sas();

        // Controlling thread cycle time
        System::Threading::Thread::Sleep(control_thread(MAIN_THREAD, THREAD_END, GL_state));

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
    sprintf(msg_modify, "RehaMove3 message: Stimulation update -> current = %2.2f, period = %2.7f, frequency = %2.2f\n", stimulator.stim[sel_ch].points[0].current, stimulator.stim[sel_ch].period, stimulator.fq[sel_ch]);
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
    bool screen_stop = false, robot_stop = true, devAvailable = true;
    
    // checking device
    if (GL_state != st_init)
    {
        devAvailable = stimulator.checkStatus();
    }

    // set error flag
    if (!devAvailable && !stim_status.error)
    {
        sprintf(msg_stimulating, "Device connection lost");
        stim_status.error = true;
    }

    // normal run
    if (devAvailable && !stim_status.error)
    {
        switch (GL_state)
        {
        case st_init:
            // initialization
            // Select Stimulation Channel = Recording Channel
            if (!stimulator.ch_ready)
            {
                emgCH = GL_UI.next_channel;
            }
            if (emgCH == emgCh1)
            {
                hmi_channel = Smpt_Channel_Black;
                stimulator.channel = hmi_channel;
                stimulator.ch_ready = true;
            }
            else if (emgCH == emgCh2)
            {
                hmi_channel = Smpt_Channel_White;
                stimulator.channel = hmi_channel;
                stimulator.ch_ready = true;
            }
            else if (emgCH == emgCh0) // Channel not selected yet
            {
                stimulator.ch_ready = false;
            }
            if (!stimulator.ready && stimulator.ch_ready)
            {
                // Select port from the GUI
                if (GL_UI.PORT_STIM[3] >= '1' && GL_UI.PORT_STIM[3] <= '9')
                {
                    PORT_STIM[3] = GL_UI.PORT_STIM[3];
                }
                countPort++;
                // Connect to the device and initialize settings
                sprintf(msg_stimulating, "Starting stimulator on port %s, try nr %d", PORT_STIM, countPort);
                load_stim_settings();
                stimulator.display = true;
                stimulator.init(PORT_STIM, GL_exercise);
                //sprintf(msg_stimulating, "%s", stimulator.displayMsg);
                stim_fl1 = false;
            }
            else if(stimulator.ready && stimulator.ch_ready)
            {
                stimulator.fq[hmi_channel] = INIT_FQ;
                set_stimulation(GL_exercise, stimulator.stim[hmi_channel], INIT_FQ);
                sprintf(msg_stimulating, "Stimulator ready");
            }
            stim_status.ready = stimulator.ready;
            break;

        case st_calM:
            // Do once at the beginning
            if (!stim_fl2)
            {
                sprintf(msg_stimulating, " ");
                stim_fl2 = true;
                // load initial values
                stimulator.fq[hmi_channel] = INIT_FQ;
                set_stimulation(GL_exercise, stimulator.stim[hmi_channel], INIT_FQ);
            }
            Move3_user_key = (Move3_key != Move3_done) && (Move3_key != Move3_stop) && (Move3_key != Move3_none) && (Move3_key != Move3_start);
            //Update stimulation parameters if a key associated with parameters has been pressed
            if (Move3_user_key)
            {
                modify_stimulation(Move3_key, hmi_channel);
            }
            screen_stop = (screen_status == setDone) || (screen_status == exDone);
            robot_stop = !robert.playPause || robert.Reached;
            // Quit
            if (((Move3_key == Move3_stop || stim_timeout || screen_stop || robot_stop || rec_status.error) && stimulator.active))
            {
                stimulator.pause();
                stim_done = !stimulator.active && stim_timeout;

                // screen message
                if (Move3_key == Move3_stop || screen_status == setDone)
                {
                    sprintf(msg_stimulating, "Stimulation stopped");
                }
                else if (!robert.playPause)
                {
                    sprintf(msg_stimulating, "Stimulation stopped by the patient");
                }
                else if (stim_timeout)
                {
                    sprintf(msg_stimulating, "Stimulation timeout");
                }

                Move3_hmi = Move3_none;
                Move3_key = Move3_none;
            }

            if ((stimulator.active || Move3_hmi == Move3_start || Move3_key == Move3_start) && !stim_timeout && robert.playPause)
            {
                if (Move3_hmi == Move3_start || Move3_key == Move3_start)
                {
                    stimulator.stim_act[hmi_channel] = true;
                }
                sprintf(msg_stimulating, "Stimulation active");

                stimulator.update2(hmi_channel);
            }
            break;

            // There are no actions available while setting the threshold (st_th, st_mvc)

            // Normal SAS process
        case st_wait:
            stim_fl0 = false;
            stim_fl1 = false;
            stim_fl2 = false;
            stim_fl3 = false;
            stim_fl4 = false;
            stim_status.ready = true;
            stim_done = false;
            trigger_timeout = false;
            // modify parameters if a button is pressed
            Move3_user_key = (Move3_key != Move3_done) && (Move3_key != Move3_stop) && (Move3_key != Move3_none) && (Move3_key != Move3_start) && (Move3_key != Move3_en_ch);

            if (Move3_user_key)
            {
                modify_stimulation(Move3_key, hmi_channel);
            }
            break;

        case st_running:
            // modify parameters if a button is pressed
            Move3_user_key = (Move3_key != Move3_done) && (Move3_key != Move3_stop) && (Move3_key != Move3_none) && (Move3_key != Move3_start);

            if (Move3_user_key)
            {
                modify_stimulation(Move3_key, hmi_channel);
                Move3_hmi = Move3_none;
                Move3_key = Move3_none;
            }

            screen_stop = (screen_status == setDone) || (screen_status == exDone) || (screen_status == msgEnd);
            robot_stop = !robert.playPause || robert.Reached;
            // Stop stimulation
            if ((Move3_hmi == Move3_stop || Move3_key == Move3_stop || screen_stop || stim_timeout || robot_stop || rec_status.error) && stimulator.active)
            {
                stimulator.pause();
                stim_fl1 = true;
                stim_pause = !robert.playPause;
                // abort exercise
                stim_abort = screen_stop;
                // screen message
                if (Move3_hmi == Move3_stop || Move3_key == Move3_stop)
                {
                    sprintf(msg_stimulating, "Stimulation stopped");
                }
                else if (!robert.playPause)
                {
                    sprintf(msg_stimulating, "Stimulation stopped by the patient");
                }
                else if (stim_timeout)
                {
                    sprintf(msg_stimulating, "Stimulation timeout");
                }

                Move3_hmi = Move3_none;
                Move3_key = Move3_none;
            }

            // Stimulate
            if (robert.playPause && !stim_timeout && !stim_fl1)
            {
                // AAN
                if (mech_as_needed)
                {
                    if (trigger_timeout)
                    {
                        // Activates stim right away
                        sprintf(msg_stimulating, "Stimulation active due to timeout without trigger");
                        stimulator.update2(hmi_channel);   
                    }
                    else if(!trigger_timeout)
                    {
                        // Waiting for velocity trigger - FES only if needed
                        if (velocity_trigg)
                        {
                            sprintf(msg_stimulating, "Velocity timeout reached");
                            stimulator.update2(hmi_channel);
                        }
                        else if (!velocity_trigg)
                        {
                            // Do nothing and wait for end or timeout
                            sprintf(msg_stimulating, "Velocity timeout not reached yet");

                            // The exercise can be completed without assistance at this point
                            // stim or mechanical assistance not activated yet
                        }
                    }
                }
                // Normal SAS
                else
                {
                    sprintf(msg_stimulating, "Stimulation active due to trigger");
                    stimulator.update2(hmi_channel);
                }
            }

            // Maybe re-take the stimulation if the play is pressed again?
            if (robert.playPause && stim_fl1 && stim_pause && Move3_key == Move3_start)
            {
                trigger_timeout = false;
                stim_pause = false;
                stim_fl1 = false;
                sprintf(msg_stimulating, "Stimulation re-enabled");
            }
            else if (robert.playPause && stim_fl1 && !stim_pause && Move3_key == Move3_start)
            {
                trigger_timeout = false;
                stim_fl1 = false;
                sprintf(msg_stimulating, "Stimulation resumed");
            }

            // things to do only once
            if (!stim_fl2)
            {
                sprintf(msg_stimulating, "Stimulation active");
                stim_fl2 = true;
                stim_status.ready = false;
            }

            // Recording stimulation
            if (stimulator.active)
            {
                // stimFile 
                // stimFile << (float)stimulator.stim[hmi_channel].points[0].current << ", " << (int)stimulator.stim[hmi_channel].ramp << ", " << (float)stimulator.fq[hmi_channel] << ", ";
                // stimFile << GL_exercise << ", " << robert.isVelocity << ", " << robert.legWeight << ", " << screen.level << ", " << GL_processed << "\n";
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
                trigger_timeout = false;
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
            trigger_timeout = false;
            stim_status.ready = (screen_status != exDone);

            if (screen_status == exDone) {
                stimulator.fq[hmi_channel] = INIT_FQ;
                set_stimulation(GL_exercise, stimulator.stim[hmi_channel], INIT_FQ);
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
    
    }
    
    // Stimulator connection lost
    if (!devAvailable && stim_status.error)
    {
        sprintf(msg_stimulating, "Device connection lost");
        if (stimulator.ready)
        {
            stimulator.end();
            countPort = 0;
        }
        if (!stimulator.ready)
        {
            // Select port from the GUI
            if (GL_UI.PORT_STIM[3] >= '1' && GL_UI.PORT_STIM[3] <= '9')
            {
                PORT_STIM[3] = GL_UI.PORT_STIM[3];
            }
            countPort++;
            // Connect to the device and initialize settings
            sprintf(msg_stimulating, "Starting stimulator on port %s, try nr %d", PORT_STIM, countPort);
            stimulator.display = true;
            stimulator.init(PORT_STIM, GL_exercise);
            stim_status.error = !stimulator.ready;
        }
    }
    
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
        }
    }
    if (!stimulator.active && stim_timing)
    {
        stim_timing = false;
        stim_timeout = false;
    }


    // For the GUI
    stimAvailable = stimulator.ready || (GL_state != st_init && devAvailable);
    //stimulator.ch_ready = stimulator.ready;

}
//================================================

void recording_sas()
{
    double mean = 0, mean2 = 0, temp_value = 0, mvc = 0, mvc2 = 0;
    double static value = 0, value_cnt = 0;
    bool st_wait_jump = false, devAvailable = false, received_data;
    //recording = &received_data;
    unsigned long long int N_len = 0;


    // normal process
    if (!rec_status.error)
    {
        switch (GL_state)
        {

        case st_init:

            if (!recorder.ready)
            {
                // Select port from the GUI
                if (GL_UI.PORT_REC[3] >= '1' && GL_UI.PORT_REC[3] <= '9')
                {
                    PORT_REC[3] = GL_UI.PORT_REC[3];            // this is maybe too redundant
                }
                // normal start up
                sprintf(msg_recording, "Starting recorder on port %s.", PORT_REC);
                Sleep(250);
                recorder.display = true;
                recorder.init_hasomed(PORT_REC);
                if (recorder.found) { recorder.start(); }

                // This is just in case the user needs to move the stuff around
                //if (!recorder.ready) {
                    //Sleep(2500);
                //}
                if (recorder.ready)
                {
                    sprintf(msg_recording, "Recorder ready");
                }
            }
            if (recorder.ready && recorder.found)
            {
                received_data = recorder.record();
            }
            rec_status.ready = recorder.data_start && recorder.ready && recorder.found;

            break;

        case st_calM:
            received_data = recorder.record();
            if (emgCH == emgCh1)
            {
                GL_sampleNr = recorder_emg1.size();
                //string msg = "From EMG1, GL_sampleNr = " + to_string(GL_sampleNr) + "\n";
                //OutputDebugString(msg.c_str()); // print
            }
            else if (emgCH == emgCh2)
            {
                GL_sampleNr = recorder_emg2.size();
                //string msg = "From EMG2, GL_sampleNr = " + to_string(GL_sampleNr) + "\n";
                //OutputDebugString(msg.c_str()); // print
            }

            N_len = GL_sampleNr - GL_processed;
            if (N_len >= SAMPLE_LIM)
            {
                // Get mean
                if (emgCH == emgCh1)
                {
                    //int tr;
                    live_data = process_data_iir(GL_sampleNr, recorder_emg1, stimFile, placeholder);
                    //tr = process_th_proper_mean(GL_sampleNr);
                    //string msg = "Live data = " + to_string(GL_sampleNr) + "\n";
                    //OutputDebugString(msg.c_str()); // print
                }
                else if (emgCH == emgCh2)
                {
                    //int tr;
                    live_data = process_data_iir(GL_sampleNr, recorder_emg2, stimFile, placeholder);
                    //tr = process_th_proper_mean(GL_sampleNr);
                    //string msg = "Live data = " + to_string(GL_sampleNr) + "\n";
                    //OutputDebugString(msg.c_str()); // print
                }
            }

            // This data discarding need to be done somewhere else so that in calM state it can be actually 
            // calibrated by playing around with the graph in the SCREEN interface - Following lines out-commented
            // Discard data - Old
            //recorder_emg1.clear();
            //recorder_emg2.clear();
            //MEAN = 0;
            //MVC = 0;
            //THRESHOLD = 0;
            //rec_fl4 = false;
            //rec_fl5 = false;
            break;

        case st_th:
            received_data = recorder.record();
            if (!main_thEN)
            {
                // ---- Comes from recording SAS - st_calM
                recorder_emg1.clear();
                recorder_emg2.clear();
                MEAN = 0;
                MVC = 0;
                THRESHOLD = 0;
                rec_fl4 = false;
                rec_fl5 = false;
                // ----
            }
            // Set threshold button has been pressed
            if (rec_status.req && main_thEN)
            {
                //tic();
                if (recorder.data_received && recorder.data_start && (!fileFILTERS.is_open() || !fileVALUES.is_open()))
                {
                    fileFILTERS.open(filter_s);
                    fileVALUES.open(th_s);
                }

                // EMG
                if (emgCH == emgCh1)
                {
                    GL_sampleNr = recorder_emg1.size();
                }
                else if (emgCH == emgCh2)
                {
                    GL_sampleNr = recorder_emg2.size();
                }
                
                N_len = GL_sampleNr - GL_processed;
                if (N_len >= SAMPLE_LIM)
                {
                    // Get mean
                    if (emgCH == emgCh1)
                    {
                        temp_value = process_th_mean(GL_sampleNr, recorder_emg1);
                        live_data = temp_value;
                    }
                    else if (emgCH == emgCh2)
                    {
                        temp_value = process_th_mean(GL_sampleNr, recorder_emg2);
                        live_data = temp_value;
                    }

                    if (GL_sampleNr >= TH_DISCARD && !rec_status.th)
                    {
                        THRESHOLD = THRESHOLD + temp_value;
                    }
                    if (GL_processed >= TH_NR && !rec_status.th)
                    {
                        unsigned long long int tot_len = GL_sampleNr - GL_thDiscard;
                        double proper_mean;
                        MEAN = MEAN / tot_len;
                        // Select threshold method 
                        switch (GL_thMethod)
                        {
                        case th_SD05:
                            proper_mean = process_th_proper_mean(GL_sampleNr);
                            live_data = proper_mean;
                            THRESHOLD = process_th_sd(GL_sampleNr, proper_mean, 3);
                            break;
                        case th_SD03:
                            proper_mean = process_th_proper_mean(GL_sampleNr);
                            live_data = proper_mean;
                            THRESHOLD = process_th_sd(GL_sampleNr, proper_mean, 2);
                            break;
                        default:
                            THRESHOLD = MEAN;
                        }

                        //tendEMG = toc();
                        //timeEFile << tendEMG << "\n";
                        sprintf(msg_recording, "EMG activity: method = %d, threshold = %2.6f", GL_thMethod, THRESHOLD);
                        rec_status.th = true;
                        rec_status.req = false;
                    }
                }

            }
            else if (rec_status.th == false && (GL_thMethod != th_MVC05 || GL_thMethod != th_MVC10))
            {
                // Discard data until button is pressed
                recorder_emg1.clear();
                recorder_emg2.clear();
            }
            else if (rec_status.th == true && (GL_thMethod == th_MVC05 || GL_thMethod == th_MVC10) && !GL_UI.set_MVC)
            {
                if (emgCH == emgCh1)
                {
                    GL_sampleNr = recorder_emg1.size();
                }
                else if (emgCH == emgCh2)
                {
                    GL_sampleNr = recorder_emg2.size();
                }
                //sprintf(msg_recording, "GL_sampleNr %d", GL_sampleNr);
                if (GL_sampleNr - GL_processed >= SAMPLE_LIM)
                {
                    placeholder.current = stimulator.stim[hmi_channel].points[0].current;
                    placeholder.ramp = stimulator.stim[hmi_channel].ramp;
                    placeholder.fq = stimulator.fq[hmi_channel];
                    placeholder.isVelocity = robert.isVelocity;
                    placeholder.legWeight = robert.legWeight;
                    placeholder.screenLevel = screen.res_level;
                    if (emgCH == emgCh1)
                    {
                        mean = process_data_iir(GL_sampleNr, recorder_emg1, stimFile, placeholder); // returned value sent to the main SCREEN for live streaming
                        live_data = mean;
                    }
                    else if (emgCH == emgCh2)
                    {
                        mean = process_data_iir(GL_sampleNr, recorder_emg2, stimFile, placeholder); // returned value sent to the main SCREEN for live streaming
                        live_data = mean;
                    }
                }
            }
            break;

        case st_mvc:
            received_data = recorder.record();
            if (!rec_fl5 && GL_UI.set_MVC)
            {
                GL_processed_MVC = GL_processed + 2 * TH_NR;
                //sprintf(msg_recording, "MCV samples needed = %d, , Current processed samples =  %d", GL_processed_MVC, GL_processed);
                rec_fl5 = true;
                GL_iterator = 0;
            }

            if (emgCH == emgCh1)
            {
                GL_sampleNr = recorder_emg1.size();
            }
            else if (emgCH == emgCh2)
            {
                GL_sampleNr = recorder_emg2.size();
            }
            //sprintf(msg_recording, "GL_sampleNr %d", GL_sampleNr);
            if (GL_sampleNr - GL_processed >= SAMPLE_LIM)
            {
                GL_iterator++;
                unsigned long long int sampleCounter = GL_processed;
                //sprintf(msg_recording, "MCV GL_iterator %d, samples needed = %d, Current processed samples =  %d, GL_sampleNr = %d", GL_iterator, GL_processed_MVC, sampleCounter, GL_sampleNr);
                if (emgCH == emgCh1)
                {
                    mvc = process_th_mvc(GL_sampleNr, recorder_emg1);
                }
                else if (emgCH == emgCh2)
                {
                    mvc = process_th_mvc(GL_sampleNr, recorder_emg2);
                }
                //std::cout << "Processed, now pointer on " << GL_processed << endl;

                if (GL_processed >= GL_processed_MVC)
                {
                    // Select threshold method 
                    switch (GL_thMethod)
                    {
                    case th_MVC05:
                        THRESHOLD = MEAN + mvc * 0.05;
                        break;
                    case th_MVC10:
                        THRESHOLD = MEAN + mvc * 0.10;
                        break;
                    default:
                        THRESHOLD = MEAN;
                    }

                    sprintf(msg_recording, "EMG activity: threshold =  %3.6f", THRESHOLD);
                    
                    //sprintf(msg_recording, "MEAN = %3.6f, THRESHOLD = %3.6f", MEAN, THRESHOLD);
                    rec_status.th2 = true;
                    rec_status.req = false;
                }
            }
            break;


        case st_wait:
            received_data = recorder.record();
            if (emgCH == emgCh1)
            {
                GL_sampleNr = recorder_emg1.size();
            }
            else if (emgCH == emgCh2)
            {
                GL_sampleNr = recorder_emg2.size();
            }
            
            if (GL_sampleNr - GL_processed >= SAMPLE_LIM)
            {
                placeholder.current = stimulator.stim[hmi_channel].points[0].current;
                placeholder.ramp = stimulator.stim[hmi_channel].ramp;
                placeholder.fq = stimulator.fq[hmi_channel];
                placeholder.isVelocity = robert.isVelocity;
                placeholder.legWeight = robert.legWeight;
                placeholder.screenLevel = screen.res_level;
                if (emgCH == emgCh1)
                {
                    mean = process_data_iir(GL_sampleNr, recorder_emg1, stimFile, placeholder);
                    live_data = mean;
                }
                else if (emgCH == emgCh2)
                {
                    mean = process_data_iir(GL_sampleNr, recorder_emg2, stimFile, placeholder);
                    live_data = mean;
                }

                // Original for software 3.0:
                // st_wait_jump = !rec_status.start && !robert.isMoving && robert.valid_msg; //  && start_train
                // For GUI testing
                //st_wait_jump = !rec_status.start && robert.playPause;
                // Final version
                robert.isMoving = robert.isVelocity >= GL_UI.isVelocity_limit;
                st_wait_jump = !rec_status.start && !robert.isMoving && robert.valid_msg && robert.playPause;

                if (mean >= THRESHOLD && (GL_thWaitCnt > TH_WAIT) && st_wait_jump)
                {
                    sprintf(msg_recording, "EMG activity: threshold exceeded");
                    rec_status.start = true;
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
            received_data = recorder.record();
            if (emgCH == emgCh1)
            {
                GL_sampleNr = recorder_emg1.size();
            }
            else if (emgCH == emgCh2)
            {
                GL_sampleNr = recorder_emg2.size();
            }
            
            if (GL_sampleNr - GL_processed >= SAMPLE_LIM)
            {
                placeholder.current = stimulator.stim[hmi_channel].points[0].current;
                placeholder.ramp = stimulator.stim[hmi_channel].ramp;
                placeholder.fq = stimulator.fq[hmi_channel];
                placeholder.isVelocity = robert.isVelocity;
                placeholder.legWeight = robert.legWeight;
                placeholder.screenLevel = screen.res_level;
                if (emgCH == emgCh1)
                {
                    mean = process_data_iir(GL_sampleNr, recorder_emg1, stimFile, placeholder);
                    live_data = mean;
                }
                else if (emgCH == emgCh2)
                {
                    mean = process_data_iir(GL_sampleNr, recorder_emg2, stimFile, placeholder);
                    live_data = mean;
                }
            }
            rec_status.ready = false;
            rec_status.start = false;
            break;

        case st_stop:
            GL_thWaitCnt = 0;
            rec_fl2 = false;
            received_data = recorder.record();
            if (emgCH == emgCh1)
            {
                GL_sampleNr = recorder_emg1.size();
            }
            else if (emgCH == emgCh2)
            {
                GL_sampleNr = recorder_emg2.size();
            }
            
            if (GL_sampleNr - GL_processed >= SAMPLE_LIM)
            {
                placeholder.current = stimulator.stim[hmi_channel].points[0].current;
                placeholder.ramp = stimulator.stim[hmi_channel].ramp;
                placeholder.fq = stimulator.fq[hmi_channel];
                placeholder.isVelocity = robert.isVelocity;
                placeholder.legWeight = robert.legWeight;
                placeholder.screenLevel = screen.res_level;

                if (emgCH == emgCh1)
                {
                    mean = process_data_iir(GL_sampleNr, recorder_emg1, stimFile, placeholder);
                    live_data = mean;
                }
                else if (emgCH == emgCh2)
                {
                    mean = process_data_iir(GL_sampleNr, recorder_emg2, stimFile, placeholder);
                    live_data = mean;
                }
            }
            rec_status.ready = true;
            rec_status.th = false;
            rec_status.th2 = false;
            rec_fl5 = false;
            rec_fl4 = false;
            break;

        case st_repeat:
            // Discard data between sets
            received_data = recorder.record();
            if (emgCH == emgCh1)
            {
                GL_sampleNr = recorder_emg1.size();
            }
            else if (emgCH == emgCh2)
            {
                GL_sampleNr = recorder_emg2.size();
            }
            
            if (GL_sampleNr - GL_processed >= SAMPLE_LIM)
            {
                placeholder.current = stimulator.stim[hmi_channel].points[0].current;
                placeholder.ramp = stimulator.stim[hmi_channel].ramp;
                placeholder.fq = stimulator.fq[hmi_channel];
                placeholder.isVelocity = robert.isVelocity;
                placeholder.legWeight = robert.legWeight;
                placeholder.screenLevel = screen.res_level;
                if (emgCH == emgCh1)
                {
                    mean = process_data_iir(GL_sampleNr, recorder_emg1, stimFile, placeholder);
                    live_data = mean;
                }
                else if (emgCH == emgCh2)
                {
                    mean = process_data_iir(GL_sampleNr, recorder_emg2, stimFile, placeholder);
                    live_data = mean;
                }

            }
            recorder_emg1.clear();
            recorder_emg2.clear();
            rec_status.th = false;
            rec_status.th2 = false;
            rec_status.req = false;
            rec_status.ready = true;
            break;

        case st_end:
            if (recorder.ready)
            {
                recorder.end();
            }
            break;
        }

    }

    // checking
    //recAvailable = (GL_state == st_init || GL_state == st_end) || ((received_data && recorder.data_start) || (!recorder.data_start && recorder.ready));
    recAvailable = (GL_state == st_init || GL_state == st_end) || ((received_data && recorder.data_start) || (recorder.found && recorder.ready)); // NEW handling for REHAINGEST disconnection
    //sprintf(msg_recording, "recAva %d, status %d, received %d, start %d", recAvailable, (GL_state == st_init || GL_state == st_end), (received_data&& recorder.data_start), (!recorder.data_start&& recorder.ready));


    // checking if there is an error
    if (!recAvailable && !rec_status.error)
    {
        sprintf(msg_recording, "Device connection lost");
        rec_status.error = true;
        countPort = 0;
        if (recorder.ready)
        {
            recorder.end();
        }
    }

    // restore connection if lost
    // restore only when state is at the end so it does not consume time
    if (rec_status.error && (GL_state == st_init || GL_state == st_repeat || GL_state == st_calM))
    {  
        if (!recorder.ready)
        {
            // Select port from the GUI
            if (GL_UI.PORT_REC[3] >= '1' && GL_UI.PORT_REC[3] <= '9')
            {
                PORT_REC[3] = GL_UI.PORT_REC[3];            // this is maybe too redundant
            }
            // normal start up
            sprintf(msg_recording, "Re-start manually the recorder. Reconnecting recorder on port %s.", PORT_REC);
            Sleep(2500);
            recorder.display = true;
            recorder.init_hasomed(PORT_REC);
            if (recorder.found) { 
                recorder.start(); 
            }

            // This is just in case the user needs to move the stuff around
            //if (!recorder.ready) 
            //{
            //    Sleep(2500);
            //}
        }
        if (recorder.ready && recorder.found)
        {
            sprintf(msg_recording, "Restarting recorder...");
            received_data = recorder.record();
        }

        if (recorder.data_start && recorder.ready && recorder.found && (GL_state==st_repeat || GL_state==st_init || GL_state == st_calM))
        {
            sprintf(msg_recording, "Communication restored");
            rec_status.error = false;
        }
    }
    
}
