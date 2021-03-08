/** C++ Application for the SAS mode
 *
 * author: Carolina Gomez <cgs@lifescience-robotics.com>
 *
 * Please notice that the libraries are linked to the directories address on
 * my own computer. Other users should change these propierties, so it can
 * compile the project.
 *
 * This application runs the following processes:
 *
 * Serial connection and handeling the RehaMove3 device.
 * Serial connection and handeling the RehaIngest device.
 * UPD Connection with ROBERT
 * TCP Connection with the Touch Panel
 * Filter and process EMG data from RehaIngest
 *
*/
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
#include "SASLIB.hpp"

using namespace std;

// ------------------------- Main global variables --------------------------------
int GL_keyPressed = 0;
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
User_Req_Type User_cmd = User_none;

// ------------------------- Devices handling --------------------------------
bool stim_ready = false, rec_ready = false, stim_abort = false;

// char PORT_STIM[5] = "COM6";   // Laptop
char PORT_STIM[5] = "COM3";     // Robot
RehaMove3 stimulator;

// char PORT_REC[5] = "COM4";    // Laptop
char PORT_REC[5] = "COM4";      // Robot
RehaIngest recorder;

// Flag variables
bool stim_fl0 = false, stim_fl1 = false, stim_fl2 = false, stim_fl3 = false, stim_fl4 = false;
bool rec_fl0 = false, rec_fl1 = false, rec_fl2 = false, rec_fl3 = false, rec_fl4 = false;
bool main_fl0 = false, main_fl1 = false, main_init = false;

// ------------------------- UDP / TCP  ------------------------
int udp_cnt = 0;

char ROBOT_IP_E[15] = "127.0.0.1";
char ROBOT_IP[15] = "172.31.1.147";
uint32_t ROBOT_PORT = 30007;
UdpClient robert(ROBOT_IP, ROBOT_PORT);

char SCREEN_ADDRESS[15] = "127.0.0.1";
char SCREEN_PORT[15] = "30002";
// TcpServer screen(SCREEN_PORT); // Using TCP-IP protocol
UdpServer screen(SCREEN_ADDRESS, SCREEN_PORT); // Using UDP-IP protocol

// TCP Stuff
int ROB_rep = 0;
ROB_Type TCP_rob;
int TCP_rep = 2;
string screenMessage = "";

ROB_Type screen_status;
bool start_train = false, hmi_repeat = false, hmi_new = false;

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
string file_dir, th_s, date_s, filter_s;
string time1_s, time2_s, time3_s, time4_s;

ofstream fileLOGS;

// ---------------------------- Functions declaration  -----------------------------
// Dummies
bool GL_tcpActive = true;

// Interface functions
void get_keyboard();
void connect_thread();
void run_gui();
// Devices functions
void modify_stimulation(RehaMove3_Req_Type &code, Smpt_Channel sel_ch);
static void stimulating_sas();
static void recording_sas();
// files functions
void start_files();
void end_files();

// ------------------------------ Main  -----------------------------
int main(int argc, char *argv[])
{
    // Local variables
    bool devicesReady;
    bool robotReady;
    // initialize files names
    get_dir(argc, argv, file_dir);
    start_files();

    // Flushing input and output buffers
    cout.flush();
    fflush(stdin);


    std::cout << "\n===================================" << endl;
    std::cout << " Communication start up " << endl;
    std::cout << "===================================" << endl;

    // Starting UPD Connection
    std::cout << "Starting connection with ROBERT and Touch Screen\n";
    robert.display = true; //Chosen not to show messages during messages exchange
    do
    {
        robert.start();
    } while (robert.error);
    robert.display = false;

    screen.start();
    //screen.waiting(); // Only for TcpServer
    GL_keyPressed = 0;


    std::cout << "===================================" << endl;
    if (GL_tcpActive)
    {
        std::cout << "---> TCP/UDP controllers:\n-T = Show/hide TCP messages.\n-U = Show/hide UDP messages.\n\n";
    }
    else
    {
        std::cout << "---> UDP controllers:\n-U = Show/hide UDP messages.\n\n";
        std::cout << "---> Process controllers:\n-G = Increase repetitions nr.\n-H = Decrease repetitions nr.\n\n\n";
    }
    std::cout << "--->Instructions: \n1. Press any key to start SAS program.\n";
    std::cout << "2. Select 2 for Automatic calibration and 3 to skip to Manual calibration.\n";
    std::cout << "3. Finish calibration by pressing X.\n";
    std::cout << "4. Press 4 to start threshold measurement.\n";
    std::cout << "5. Press key nr.1 to start training once the threshold is measured.\n";
    std::cout << "6. Programe will run until all repetitions are completed. To interrupt process, press 0.\n";
    std::cout << "===================================" << endl;

    std::thread Interface(connect_thread);
    std::thread GUI(run_gui);

    MAIN_to_all.start = true;
    MAIN_to_all.ready = true;

    screenMessage = "Starting up stimulator and recorder.";
    std::cout << screenMessage << endl;

    // main process
    while (!MAIN_to_all.end && (GL_state != st_end))
    {
        control_thread(MAIN_THREAD, THREAD_START, GL_state);

        recording_sas();

        // State machine
        switch (GL_state)
        {
         case st_init:
            devicesReady = rec_status.ready && stim_status.ready;
            if (devicesReady && !stimA_active && !stimM_active && !main_init)
             {
                 screenMessage = "Devices ready. \nPress Set stimulation \nto set stimulator parameters.";
                 std::cout << screenMessage << endl;
                 main_init = true;
            }
            if (devicesReady && stimA_active)
            {
                std::cout << "===================================" << endl;
                screenMessage = "Starting automatic callibration...";
                std::cout << screenMessage << endl;

                GL_state = st_calA_go;
                toc_lim = 5;
                tic();
            }
            if (devicesReady && stimM_active)
            {
                std::cout << "===================================" << endl;
                screenMessage = "Set stimulation started.";
                std::cout << screenMessage << endl;
                GL_state = st_calM;

                if (!GL_tcpActive)
                {
                    std::cout << "---> Stimulator controllers:\n-A = Reduce Ramp.\n-D = Increase ramp.\n-W = Increase current.\n-S = Decrease current.\n";
                    std::cout << "-Q = Stop stimulation.\n-E = Re-start stimulation.\n-X = end current calibration (manual or automatic).\n\n";
                    std::cout << "===================================" << endl;
                }
            }       
            main_fl0 = false;
         break;
         
        // Stimulation calibration process
        case st_calA_go:
            // Stimulation too weak: nothing happens
            if (toc() && !stimA_start_b && !stim_auto_done)
            {
                toc_lim = 5;
                screenMessage = "Auto-calibration : Endpoint not reached, break of " + to_string(toc_lim) + "s.";
                std::cout << screenMessage << endl;

                GL_state = st_calA_stop;
                tic();
                calCycle_nr++; // New for testing
            }
            // Stimulation not strong enough: it moves, but it does not reach the end
            // in this case it re-starts the tic, but if at the end it does not Reach it, it'll set a longer break
            if (!toc() && robert.isMoving && !robert.Reached && !stimA_start_b)
            {
                toc_lim = 7;
                stimA_start_b = true;
                screenMessage = "Auto-calibration: Movement detected, stimulation total time will be set to " + to_string(toc_lim) + "s.";
                std::cout << screenMessage << endl;
            }
            if (toc() && stimA_start_b && !robert.Reached)
            {
                GL_state = st_calA_stop;
                toc_lim = 10;
                tic();
                calCycle_nr++; // New for testing
                
                screenMessage = "Auto-calibration : Stopping movement and break of " + to_string(toc_lim) + "s.";
                std::cout << screenMessage << endl;
            }
            // Stimulation values OK: Reached End Point
            if (stim_auto_done && toc())
            {
                //printf("SAS test: Reached End Point, %2.2f s. Switching to manual callibration.\n", toc_lim);
                screenMessage = "SAS test: Reached End Point. \nSwitching to manual callibration.";
                std::cout << screenMessage << endl;

                if (!GL_tcpActive)
                {
                    std::cout << "===================================" << endl;
                    std::cout << "---> Stimulator controllers:\n-A = Reduce Ramp.\n-D = Increase ramp.\n-W = Increase current.\n-S = Decrease current.\n";
                    std::cout << "-Q = Stop stimulation.\n-E = Re-start stimulation.\n-X = end current calibration (manual or automatic).\n\n";
                    std::cout << "===================================" << endl;
                }
                GL_state = st_calM;
            }
            // Quit automatic calibration
            if (stim_userX && !stimulator.active)
            {
                GL_state = st_calM;
                screenMessage = "Quitting automatic callibration. \nStarting manual...";
                std::cout << screenMessage << endl;

                if (!GL_tcpActive)
                {
                    std::cout << "===================================" << endl;
                    std::cout << "---> Stimulator controllers:\n-A = Reduce Ramp.\n-D = Increase ramp.\n-W = Increase current.\n-S = Decrease current.\n";
                    std::cout << "-Q = Stop stimulation.\n-E = Re-start stimulation.\n-X = end current calibration (manual or automatic).\n\n";
                    std::cout << "===================================" << endl;
                }
            }
            break;

        case st_calA_stop:
            // Quit automatic calibration
            if (stim_userX && !stimulator.active)
            {
                GL_state = st_calM;
                screenMessage = "Quitting automatic callibration. \nStarting manual...";
                std::cout << screenMessage << endl;

                if (!GL_tcpActive)
                {
                    std::cout << "===================================" << endl;
                    std::cout << "---> Stimulator controllers:\n-A = Reduce Ramp.\n-D = Increase ramp.\n-W = Increase current.\n-S = Decrease current.\n";
                    std::cout << "-Q = Stop stimulation.\n-E = Re-start stimulation.\n-X = end current calibration (manual or automatic).\n\n";
                    std::cout << "===================================" << endl;
                }
            }
            // Abort automatic calibration if it has run for too long
            else if ((calCycle_nr >= CAL_CYCLE_LIM) || (stimulator.stim[Smpt_Channel_Red].points[0].current >= CAL_CUR_LIM))
            {
                GL_state = st_calM;
                // why has the calibration failed
                if (calCycle_nr >= CAL_CYCLE_LIM)
                {
                    //printf("SAS PROGRAMME: Automatic callibration failed after %d tries. Switching to manual...\n", calCycle_nr);
                    screenMessage = "Automatic callibration failed after " + to_string(calCycle_nr) + " tries. \nSwitching to manual...";
                    std::cout << screenMessage << endl;
                }
                else
                {
                    //printf("SAS PROGRAMME: Automatic callibration failed. Stimulation has reached the maximum current of %2.2f mA. Switching to manual...\n", CAL_CUR_LIM);
                    screenMessage = "Automatic callibration failed. Stimulation has reached the maximum current of " + to_string(CAL_CUR_LIM) + "mA. \nSwitching to manual...";
                    std::cout << screenMessage << endl;
                }

                if (!GL_tcpActive)
                {
                    std::cout << "===================================" << endl;
                    std::cout << "---> Stimulator controllers:\n-A = Reduce Ramp.\n-D = Increase ramp.\n-W = Increase current.\n-S = Decrease current.\n";
                    std::cout << "-Q = Stop stimulation.\n-E = Re-start stimulation.\n-X = end current calibration (manual or automatic).\n\n";
                    std::cout << "===================================" << endl;
                }
            }
            else if (!stimulator.active && toc())
            {
                GL_state = st_calA_go;
                toc_lim = 5;
                tic();
                //printf("SAS test: Finish cycle nr.%d. Starting next cycle with a stim time of %2.2f s.\n", calCycle_nr, toc_lim);
                screenMessage = "Finish cycle nr." + to_string(calCycle_nr) + ". \nStarting next cycle with a stim time of " + to_string(toc_lim) + "s.";
                std::cout << screenMessage << endl;
            }
            break;

        case st_calM:
            if (rec_status.ready && rec_status.req && stim_done)
            {
                screenMessage = "Setting threshold...";
                std::cout << screenMessage << endl;
                GL_state = st_th;
            }
            break;

        // Normal SAS process
        case st_th:
            if (rec_status.th)
            {
                if (GL_tcpActive)
                {
                    //printf("SAS PROGRAMME: Threshold saved. Press start training button. Waiting for stimulator to be triggered.\n");
                    screenMessage = "Threshold saved. \nPress start training button. \nWaiting for stimulator to be triggered.";
                    std::cout << screenMessage << endl;
                }
                else
                {
                    printf("Threshold saved. \nPress 1 to start training. \nWaiting for stimulator to be triggered.\n");
                }
                std::cout << "\n===================================" << endl;
                // Update exercise
                GL_exercise_old = GL_exercise;
                GL_thMethod_old = GL_thMethod;

                GL_state = st_wait;
            }
            break;

        case st_wait:
            if(stim_abort)
            {
                // Exercise has been aborted
                ROB_rep++;
                //printf("SAS PROGRAMME: Abort exercise.\n");
                //std::cout << "\n---> Repetition nr." << ROB_rep << " completed of " << TCP_rep << " <--- " << endl;
                screenMessage = "Exercise aborted. \nCompleted " + to_string(ROB_rep) +  " repetitions of " + to_string(TCP_rep);
                std::cout << screenMessage << endl;

                GL_state = st_stop;
                fileLOGS << "2.0, " << GL_processed << "\n";
            }
            else if (rec_status.start)
            {
                // normal trigger
                GL_state = st_running;
                screenMessage = "Stimulator triggered";
                std::cout << screenMessage << endl;

                fileLOGS << "1.0, " << GL_processed << "\n";
            }
            if (robert.Reached && robert.valid_msg)
            {
                // Increase repetitions
                ROB_rep++;
                screenMessage = "End of Point reached. \nCompleted " + to_string(ROB_rep) + " repetitions of " + to_string(TCP_rep);

                if (ROB_rep < TCP_rep)
                {
                    screenMessage += ". \nWaiting for robot to return to start position.";
                }
                std::cout << screenMessage << endl;

                GL_state = st_stop;
                fileLOGS << "2.0, " << GL_processed << "\n";
            }
            break;

        case st_running:
            if (stim_abort)
            {
                // Exercise has been aborted
                ROB_rep++;
                screenMessage = "Exercise aborted. \nCompleted " + to_string(ROB_rep) + " repetitions of " + to_string(TCP_rep);
                std::cout << screenMessage << endl;
                GL_state = st_stop;
                fileLOGS << "3.0, " << GL_processed << "\n";
            }
            if (robert.Reached && robert.valid_msg)
            {
                // Increase repetitions
                ROB_rep++;
                screenMessage = "End of Point reached. \nCompleted " + to_string(ROB_rep) + " repetitions of " + to_string(TCP_rep);
                if (ROB_rep < TCP_rep)
                {
                    screenMessage += ". \nWaiting for robot to return to start position.";
                }
                std::cout << screenMessage << endl;

                GL_state = st_stop;
                fileLOGS << "3.0, " << GL_processed << "\n";
            }
            break;

        case st_stop:
            devicesReady = stim_status.ready && rec_status.ready;
            robotReady = !robert.Reached && !robert.isMoving && robert.valid_msg;
            // Check nr of repetitions and devices
            if (ROB_rep < TCP_rep && devicesReady && robotReady && !stim_abort)
            {
                std::cout << "\n===================================" << endl;
                screenMessage = "Starting next repetition";
                std::cout << screenMessage << endl;

                GL_state = st_wait;
                fileLOGS << "4.0, " << GL_sampleNr << "\n";
            }
            else if ((ROB_rep >= TCP_rep || stim_abort) && stim_status.ready && rec_status.ready)
            {
                std::cout << "\n===================================" << endl;
                screenMessage =  "Exercise finished. \nDo another exercise or finish the program.";
                std::cout << screenMessage << endl;
                //GL_state = st_end;
                GL_exercise_old = GL_exercise;
                GL_thMethod_old = GL_thMethod;
                GL_state = st_repeat;
            }
            break;

        case st_repeat:
                // Reset process variables: this needs to be better structured
                ROB_rep = 0;
                main_init = false;
                devicesReady = rec_status.ready && stim_status.ready && !stim_abort;
                robotReady = !robert.isMoving && !robert.Reached && robert.valid_msg;

                if (MAIN_to_all.end && devicesReady)
                {
                    end_files();
                    GL_state = st_end;
                }
                else if (hmi_repeat && devicesReady && robotReady) {
                    // Repeat same type of exercise
                    if (!rec_status.req && !main_fl0) {
                        screenMessage = "Waiting for Threshold button.";
                        std::cout << screenMessage << endl;

                        main_fl0 = true;
                        main_fl1 = false;
                    }
                    else if (rec_status.req) {
                        startup_filters();
                        hmi_repeat = false;
                        hmi_new = false;
                        main_fl0 = false;
                        main_fl1 = false;

                        end_files();
                        start_files();

                        GL_exercise = GL_exercise_old;
                        GL_thMethod = GL_thMethod;
                        GL_state = st_th;
                    }
                }
                else if (hmi_new && devicesReady && robotReady) {
                    // Do a new type of exercise
                    startup_filters();
                    hmi_repeat = false;
                    hmi_new = false;

                    end_files();
                    start_files();

                    GL_state = st_init;
                }

                if ((hmi_repeat || hmi_new) && devicesReady && !robotReady && !main_fl1)
                {
                    screenMessage += "\nWaiting for robot to be at start position";
                    std::cout << screenMessage << endl;
                    main_fl1 = true;
                }
            break;
        } // State machine

        stimulating_sas();
        // Process handling: if the robot-connection gets lost
        if (robert.error_lim && MAIN_to_all.ready)
        {
            bool jump_cal = (GL_state == st_calA_go) || (GL_state == st_calA_stop);
            bool jump_run = (GL_state == st_running) || (GL_state == st_wait);
            bool wait_cal = (GL_state == st_init) || (GL_state == st_calM) || (GL_state == st_th);

            if (jump_cal)
            {
                screenMessage = "Connection to the robot lost. \nSwitching to manual calibration.";
                std::cout << screenMessage << endl;
                GL_state = st_calM;
            }
            else if (jump_run)
            {
                screenMessage = "Connection to the robot lost. \nStopping current repetition";
                std::cout << screenMessage << endl;
                GL_state = st_stop;
            }
            else if (wait_cal)
            {
                screenMessage = "Connection to the robot lost. \nProcess will stop after calibration and threshold set up";
                std::cout << screenMessage << endl;
            }
            else
            {
                screenMessage = "Connection to the robot lost. \nProcess will stop after calibration and threshold set up";
                std::cout << screenMessage << endl;
            }
            MAIN_to_all.ready = false;
        }
        else if (!robert.error_lim && !MAIN_to_all.ready)
        {
            screenMessage = "Connection to the robot restored";
            std::cout << screenMessage << endl;

            MAIN_to_all.ready = true;
        }

        // Controlling thread cycle time
        control_thread(MAIN_THREAD, THREAD_END, GL_state);

        time3_end = std::chrono::steady_clock::now();
        if (GL_state != st_init)
        {
            time3_diff = time3_end - th1_st;
            time3_v.push_back((double)time3_diff.count());
            time3_v2.push_back(GL_state);
        }
    }

    // Finish devices
    GL_state = st_end;
    recording_sas();
    stimulating_sas();
    // Waiting for other thread to finish
    Interface.join();
    GUI.join();

    // Close files if they are still open
    if (fileFILTERS.is_open())
    {
        end_files();
    }

    robert.end();
    if (GL_tcpActive)
    {
        screen.end();
    }

    std::cout << "\n--------------------------\nHej Hej!\n--------------------------\n";
    // Flushing input and output buffers
    cout.flush();
    fflush(stdin);

    return 0;
}


// ---------------------------- Interface function definitions --------------------------

void get_keyboard()
{
    int ch;
    ch = _getch();
    ch = toupper(ch);
    printf("---> Key pressed: %c <---\n", ch);
    switch (ch)
    {
    // Display options
    case 'T':
        screen.display = !screen.display;
        if (screen.display && GL_tcpActive)
        {
            printf("Showing screen IP-socket messages and status.\n");
        }
        else if (GL_tcpActive)
        {
            printf("Not showing screen IP-socket messages and status.\n");
        }
        break;
    case 'U':
        robert.display = !robert.display;
        if (robert.display)
        {
            printf("Showing robot IP-socket messages and status.\n");
        }
        else
        {
            printf("Not showing robot IP-socket messages and status.\n");
        }
        break;
    case 'I':
        stimulator.display = !stimulator.display;
        if (stimulator.display)
        {
            printf("Showing stimulator messages.\n");
        }
        else
        {
            printf("Not showing stimulator messages.\n");
        }
        break;
    case 'Y':
        recorder.display = !recorder.display;
        if (recorder.display)
        {
            printf("Showing recorder messages.\n");
        }
        else
        {
            printf("Not showing recorder messages.\n");
        }
        break;
        // Repetition options
    case 'G':
        if (!GL_tcpActive)
        {
            TCP_rep++;
            if (TCP_rep >= 100)
            {
                TCP_rep = 100;
            }
            std::cout << "Rep nr. modified to " << TCP_rep << endl;
        }
        break;
    case 'H':
        if (!GL_tcpActive)
        {
            TCP_rep--;
            if (TCP_rep < ROB_rep)
            {
                TCP_rep = ROB_rep + 1;
                std::cout << "Rep nr. cannot be lower than the current progress amount. ";
            }
            std::cout << "Rep nr. modified to " << TCP_rep << endl;
        }
        break;
    // Stimulation options
    case 'A':
        Move3_key = Move3_ramp_less;
        break;
    case 'D':
        Move3_key = Move3_ramp_more;
        break;
    case 'S':
        Move3_key = Move3_decr;
        break;
    case 'W':
        Move3_key = Move3_incr;
        break;
    // Stimulator device
    case 'Q':
        Move3_key = Move3_stop;
        break;
    case 'E':
        Move3_key = Move3_start;
        break;
    case 'X':
        Move3_key = Move3_done;
        break;
    //New
    // Stimulation channels
    case 'V':
        if (!GL_tcpActive)
        {
            hmi_channel = Smpt_Channel_Red;
            printf("Stimulator: red channel Selected.\n");
        }
        break;
    case 'B':
        if (!GL_tcpActive)
        {
            hmi_channel = Smpt_Channel_Blue;
            printf("Stimulator: blue channel Selected.\n");
        }
        break;
    case 'N':
        if (!GL_tcpActive)
        {
            hmi_channel = Smpt_Channel_Black;
            printf("Stimulator: black channel Selected.\n");
        }
        break;
    case 'M':
        if (!GL_tcpActive)
        {
            hmi_channel = Smpt_Channel_White;
            printf("Stimulator: white channel Selected.\n");
        }
        break;
    case 'R':
        Move3_key = Move3_en_ch;
        break;

    // SAS programme
    case '1':
        if(GL_state == st_wait)
        {
            start_train = true;
            fileLOGS << "5.0, " << GL_processed << "\n";
            std::cout << "Start training pressed." << endl;
        }
        break;
    case '2':
        if (GL_state == st_init)
        {
            stimA_active = true;
            stimM_active = false;
            std::cout << "Automatic callibration selected." << endl;
        }
        break;
    case '3':
        if (GL_state == st_init)
        {
            stimA_active = false;
            stimM_active = true;
            std::cout << "Manual callibration selected." << endl;
        }
        break;
    case '4':
        if ((GL_state == st_calM || GL_state == st_repeat) )
        {
            rec_status.req = true;
            std::cout << "Start threshold." << endl;
        }
        break;
    case '5':
        if (GL_state == st_repeat)
        {
            hmi_repeat = true;
            std::cout << "Selected: repeat same type of exercise." << endl;
        }
        break;

    case '6':
        if (GL_state == st_repeat)
        {
            hmi_new = true;
            std::cout << "Selected: start a new exercise." << endl;
        }
        break;

    case '0':
        MAIN_to_all.end = true;
        break;
    }
}

void connect_thread()
{
    while (!MAIN_to_all.end && (GL_state != st_end))
    {
        // normal thread
        control_thread(INTERFACE_THREAD, THREAD_START, GL_state);
        GL_keyPressed = _kbhit();
        if (GL_keyPressed != 0)
        {
            get_keyboard();
        }
        // UDP Update:
        udp_cnt++;
        sprintf(robert.message, "%d;STATUS", udp_cnt);
        robert.get();

        // Controlling thread cycle time
        control_thread(INTERFACE_THREAD, THREAD_END, GL_state);

        time4_end = std::chrono::steady_clock::now();
        time4_diff = time4_end - th2_st;

        // Only saving result if the state machine is not potentially closing/opening files
        if (GL_state != st_repeat)
        {
            time4_v.push_back((double)time4_diff.count());
        }

        // Close file in case exercise has been finished

    } // while loop
} // thread

void run_gui()
{
    // to send: stimulation parameters + exercise settings + current status -> guiMsg
    char stimMsg[BUFLEN], setMsg[BUFLEN], disMsg[BUFLEN], guiMsg[BUFLEN];
    bool decode_successful;
    while (!MAIN_to_all.end && (GL_state != st_end) && !screen.finish)
    {
        // Receive
        screen.check();

        if (!screen.error && GL_tcpActive)
        {
            // Normal procedure
            decode_successful = decode_gui(screen.recvbuf, Move3_hmi, User_cmd, TCP_rob, TCP_rep, screen.finish, hmi_channel, GL_exhmi, GL_thhmi);

            if (decode_successful)
            {
                if (screen.display)
                {
                    printf("From SCREEN received: move_key = %d, user_cmd = %d, satus = %c, rep_nr = %d, sel_ch = %d, sel_ex = %d, sel_th = %d \n ", Move3_hmi, User_cmd, TCP_rob, TCP_rep, hmi_channel, GL_exhmi, GL_thhmi);
                }
                // Actions related to the User_cmd
                switch (User_cmd)
                {
                case User_st:
                    if (GL_state == st_wait)
                    {
                        start_train = true;
                        fileLOGS << "5.0, " << GL_processed << "\n";
                        std::cout << "Start training pressed." << endl;
                    }
                    break;
                case User_CA:
                    if (GL_state == st_init)
                    {
                        stimA_active = true;
                        stimM_active = false;
                        std::cout << "Automatic callibration selected." << endl;
                    }
                    break;
                case User_CM:
                    if (GL_state == st_init)
                    {
                        stimA_active = false;
                        stimM_active = true;
                        std::cout << "Manual callibration selected." << endl;
                    }
                    break;
                case User_th:
                    if (GL_state == st_calM || (GL_state == st_repeat && hmi_repeat))
                    {
                        rec_status.req = true;
                        std::cout << "Start threshold." << endl;
                    }
                    break;
                case User_rep:
                    if (GL_state == st_repeat && !hmi_new && !hmi_repeat)
                    {
                        hmi_repeat = true;
                        screenMessage = "Selected: repeat same type of exercise.";
                        std::cout << screenMessage << endl;
                    }
                    break;
                case User_new:
                    if (GL_state == st_repeat && !hmi_new && !hmi_repeat)
                    {
                        hmi_new = true;
                        screenMessage = "Selected: start a new exercise.";
                        std::cout << screenMessage << endl;
                    }
                    break;
                }

                if ((GL_state == st_repeat || GL_state == st_init)&& !hmi_repeat)
                {
                    GL_exercise = GL_exhmi;
                    GL_thMethod = GL_thhmi;
                }
            }
            else if (!decode_successful && screen.display)
            {
                printf("SCREEN received message not valid.\n");
            }
            // Wait for stimulator to update values before sending
            if (decode_successful && (Move3_hmi != Move3_none))
            {
                bool update_values_TCP;
                bool update_state = (GL_state == st_calM) || (GL_state == st_wait) || (GL_state == st_running);
                do
                {
                    Sleep(5);
                    update_values_TCP = (Move3_hmi == Move3_decr) || (Move3_hmi == Move3_incr) || (Move3_hmi == Move3_ramp_more) || (Move3_hmi == Move3_ramp_less);
                } while (update_values_TCP && update_state);
            }

            // Send if something was received
            sprintf(stimMsg,"%2.1f;%d;%2.1f;%d;%d;", stimulator.stim[hmi_channel].points[0].current, stimulator.stim[hmi_channel].ramp, stimulator.fq[hmi_channel], stimulator.stim_act[hmi_channel], stimulator.active);
            sprintf(setMsg, "%d;%d;%d;", start_train, GL_exercise, GL_thMethod);
            sprintf(disMsg, "%d;%d;%s;", ROB_rep, GL_state, screenMessage.c_str());
            memset(screen.senbuf, '\0', 512);
            sprintf(screen.senbuf, "SAS;%s%s%s", stimMsg, setMsg, disMsg);
            screen.stream();
        }
        else if(screen.error_lim && GL_tcpActive)
        {
            GL_tcpActive = false;
            printf("SAS PROGRAMME: Connection to the screen lost. Switching to keyboard.\n");
        }
        else if (!screen.error && !GL_tcpActive)
        {
            GL_tcpActive = true;
            printf("SAS PROGRAMME: Connection to the screen restored. Switching back to Touch Panel.\n");
        }

        // TCP Stuff
        Sleep(250);
    }
    // End of the Program if the screen has been closed
    if (screen.finish && GL_tcpActive)
    {
        printf("Screen App has been closed. SAS program will finish too.\n");
        MAIN_to_all.end = true;
    }
}

// ---------------------------- File function definitions --------------------------

void start_files()
{
    // initialize files names
    generate_date(date); //get current date/time in format YYMMDD_hhmm
    date_s = convert_to_string(date, sizeof(date));
    filter_s = file_dir + folder + Sname + "_filter_" + date_s.c_str() + ".txt";
    time1_s = file_dir + folder + Sname + "_time1_" + date_s.c_str() + ".txt";
    time2_s = file_dir + folder + Sname + "_time2_" + date_s.c_str() + ".txt";
    time3_s = file_dir + folder + Sname + "_time3_" + date_s.c_str() + ".txt";
    time4_s = file_dir + folder + Sname + "_time4_" + date_s.c_str() + ".txt";
    th_s = file_dir + folder + Sname + "_th_" + date_s.c_str() + ".txt";
    string LOGS_s = file_dir + folder + Sname + "_log_" + date_s.c_str() + ".txt";
    fileLOGS.open(LOGS_s);
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

// ---------------------------- Devices function definitions  --------------------------

void modify_stimulation(RehaMove3_Req_Type &code, Smpt_Channel sel_ch)
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
            next_val.period = MS_TO_HZ/next_fq;
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
    printf("RehaMove3 message: Stimulation update -> current = %2.2f, period = %2.7f, frequency = %2.2f\n", stimulator.stim[sel_ch].points[0].current, stimulator.stim[sel_ch].period, stimulator.fq[sel_ch]);
        //%2.1f is the format to show a float number, 2.1 means 2 units and 1 decimal
    //}
    // Update commands
    code = Move3_none;
    Move3_cmd = Move3_none;
    Move3_hmi = Move3_none;
    Move3_key = Move3_none;
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
            stimulator.init(PORT_STIM, GL_exercise);
        }
        stim_status.ready = stimulator.ready;
        break;

    // Stimulator calibration process
    case st_calA_go:
        if (((Move3_hmi == Move3_stop || Move3_key == Move3_stop) && stimulator.active) || (Move3_hmi == Move3_done || Move3_key == Move3_done))
        {
            stimulator.pause();
            if (Move3_hmi == Move3_done || Move3_key == Move3_done)
            {
                stim_userX = true; // Request to quit
            }

            Move3_hmi = Move3_none;
            Move3_key = Move3_none;
        }
        if ((Move3_hmi != Move3_stop && Move3_key != Move3_stop) && !stim_auto_done && !stim_userX)
        {
            stimulator.update();
            //stimulator.update(Smpt_Channel_Red);
        }

        // If movement has been previously detected and the end point is reached
        if (stimA_start_b && robert.Reached && !stim_auto_done)
        {
            stimulator.pause();
            stim_auto_done = !stimulator.active;
            screenMessage = "Stimulation: automatic calibration done";
            std::cout << screenMessage << endl;
        }

        break;

    case st_calA_stop:
        // Stop stimulator and update values
        if (stimulator.active)
        {
            stimulator.pause();
            Move3_cmd = Move3_incr;
            // if keep going?
            modify_stimulation(Move3_cmd, Smpt_Channel_Red);
        }
        if ((Move3_hmi == Move3_done || Move3_key == Move3_done) && !stim_userX)
        {
            stimulator.pause();
            stim_userX = true; // Request to quit
            Move3_hmi = Move3_none;
            Move3_key = Move3_none;
        }
        break;

    case st_calM:
        Move3_user_key = (Move3_key != Move3_done) && (Move3_key != Move3_stop) && (Move3_key != Move3_none) && (Move3_key != Move3_start) && (Move3_key != Move3_en_ch);
        Move3_user_gui = (Move3_hmi != Move3_done) && (Move3_hmi != Move3_stop) && (Move3_hmi != Move3_none) && (Move3_hmi != Move3_start) && (Move3_hmi != Move3_en_ch);
        //Update stimulation parameters if a key associated with parameters has been pressed
        if (Move3_user_gui)
        {
            modify_stimulation(Move3_hmi, hmi_channel);
        }
        else if (Move3_user_key)
        {
            modify_stimulation(Move3_key, hmi_channel);
        }

        // Enable / disable channels. By default, only red channel is avaliable.
        if (Move3_hmi == Move3_en_ch || Move3_key == Move3_en_ch)
        {
            stimulator.enable(hmi_channel, !stimulator.stim_act[hmi_channel]);
            Move3_hmi = Move3_none;
        }

        // Quit
        if (((Move3_hmi == Move3_stop || Move3_key == Move3_stop|| stim_timeout) && stimulator.active) || (Move3_hmi == Move3_done || Move3_key == Move3_done))
        {
            stimulator.pause();
            stim_done = !stimulator.active && (Move3_hmi == Move3_done || Move3_key == Move3_done);
            Move3_hmi = Move3_none;
            Move3_key = Move3_none;
            if (stim_done)
            {

                if (GL_tcpActive)
                {
                    screenMessage = "Stimulation set up finished. \nPress set threshold";
                    std::cout << screenMessage << endl;
                }
                else
                {
                    std::cout << "--> Press 4 for set threshold <---" << endl;
                }
            }
            else
            {
                screenMessage = "Stimulation stopped";
                std::cout << screenMessage << endl;
            }
        }

        if ((stimulator.active || Move3_hmi == Move3_start) && !stim_timeout)
        {
            stimulator.update2(hmi_channel);
            //stimulator.update(Smpt_Channel_Red);
        }
        break;

    // Normal SAS process
    case st_wait:
        stim_fl3 = false;
        stim_fl4 = false;
        // modify parameters if a button is pressed
        Move3_user_key = (Move3_key != Move3_done) && (Move3_key != Move3_stop) && (Move3_key != Move3_none) && (Move3_key != Move3_start) && (Move3_key != Move3_en_ch);
        Move3_user_gui = (Move3_hmi != Move3_done) && (Move3_hmi != Move3_stop) && (Move3_hmi != Move3_none) && (Move3_hmi != Move3_start) && (Move3_hmi != Move3_en_ch);

        if (Move3_user_gui)
        {
            modify_stimulation(Move3_hmi, Smpt_Channel_Red);
        }
        else if (Move3_user_key)
        {
            modify_stimulation(Move3_key, Smpt_Channel_Red);
        }

        // Abort button
        if ((Move3_key == Move3_done) || (Move3_hmi == Move3_done) && !stimulator.active)
        {
            stim_abort = true;
            Move3_hmi = Move3_none;
            Move3_key = Move3_none;
        }
        break;

    case st_running:
        // modify parameters if a button is pressed
        Move3_user_key = (Move3_key != Move3_done) && (Move3_key != Move3_stop) && (Move3_key != Move3_none) && (Move3_key != Move3_start) && (Move3_key != Move3_en_ch);
        Move3_user_gui = (Move3_hmi != Move3_done) && (Move3_hmi != Move3_stop) && (Move3_hmi != Move3_none) && (Move3_hmi != Move3_start) && (Move3_hmi != Move3_en_ch);

        if (Move3_user_gui)
        {
            modify_stimulation(Move3_hmi, Smpt_Channel_Red);
            stim_fl1 = false;
        }
        else if (Move3_user_key)
        {
            modify_stimulation(Move3_key, Smpt_Channel_Red);
            stim_fl1 = false;
        }

        if ((Move3_hmi == Move3_stop || Move3_key == Move3_stop || stim_timeout || Move3_hmi == Move3_done || Move3_key == Move3_done) && stimulator.active)
        {
            stimulator.pause();
            screenMessage = "Stimulation stopped";
            std::cout << screenMessage << endl;
            stim_fl1 = true;
            // abort exercise
            if (Move3_hmi == Move3_done || Move3_key == Move3_done){
                stim_abort = true;
            }
            Move3_hmi = Move3_none;
            Move3_key = Move3_none;
        }
        if (!stim_fl1 && !stim_timeout)
        {
            stimulator.update();
            //stimulator.update(Smpt_Channel_Red);
        }
        // things to do only once
        if (!stim_fl2)
        {
            screenMessage = "Stimulation active";
            std::cout << screenMessage << endl;

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
        
        if (hmi_new) {
            set_stimulation(GL_exercise, stimulator.stim[Smpt_Channel_Red], INIT_FQ);
            stim_status.ready = true;
        }
        else {
            stim_status.ready = hmi_repeat;
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
            screenMessage = "Stimulation timeout";
            std::cout << screenMessage << endl;
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
            if (recorder.display) { std::cout << "Reha Ingest message: Starting recorder." << endl; }
            Sleep(2500);
            recorder.init(PORT_REC);
            if (recorder.found) { recorder.start(); }

            // This is just in case the user needs to move the stuff around
            if (!recorder.ready) {
                Sleep(2500);
            }
        }
        startup_filters();
        rec_status.ready = recorder.ready;
    break;

    case st_calA_go:
        // Discard data
        recorder.record();
        recorder_emg1.clear();
    break;

    case st_calA_stop:
        // Discard data
        recorder.record();
        recorder_emg1.clear();
    break;

    case st_calM:
        // Discard data
        recorder.record();
        recorder_emg1.clear();
    break;
    case st_th:
        recorder.record();
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
                    temp_value = process_th(GL_sampleNr, recorder_emg1, 0.5);
                    break;
                case th_SD03:
                    temp_value = process_th(GL_sampleNr, recorder_emg1, 2);
                    break;
                default:
                    temp_value = process_th(GL_sampleNr, recorder_emg1, 3);
            }

            //printf("%d\n", temp_value);         // debugging stuff
            if (GL_sampleNr >= TH_DISCARD)
            {
                THRESHOLD = THRESHOLD + temp_value;
            }
            if (GL_processed >= TH_NR)
            {
                THRESHOLD = THRESHOLD / (GL_sampleNr - GL_thDiscard);
                screenMessage = "EMG activity: threshold = " + to_string(THRESHOLD);
                std::cout << screenMessage << endl;

                //std::cout << "Reha Ingest message: threshold = " << THRESHOLD << ", old m = " << old_value[0] << ", old nr = " << old_nr[0] << endl;
                rec_status.th = true;
                rec_status.req = false;
            }
        }
        break;

    case st_wait:
        recorder.record();
        GL_sampleNr = recorder_emg1.size();
        if (GL_sampleNr - GL_processed >= SAMPLE_LIM)
        {
            mean = process_data_iir(GL_sampleNr, recorder_emg1);

            st_wait_jump = !rec_status.start && !robert.isMoving && robert.valid_msg && start_train;

            if ((mean >= THRESHOLD) && (GL_thWaitCnt > TH_WAIT) && st_wait_jump)
            {
                screenMessage = "EMG activity: threshold overpassed";
                std::cout << screenMessage << endl;
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
            else if ((GL_thWaitCnt >= TH_WAIT) && !rec_fl2)
            {
                screenMessage = "EMG activity: now you can push";
                std::cout << screenMessage << endl;
                rec_fl2 = true;
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
