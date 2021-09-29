/** Library with basic functionalities
  *
*/

#ifndef SASLIBbasic_H_
#define SASLIBbasic_H_

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <complex>
#include <ctime>
#include <string>
#include <thread>

// ------------------------------------------------------------------------

using namespace std;

// ------------------ Variables for process control ------------------
// Types for internal process handling
typedef enum
{
    st_setup = -1,    // Before init, select exercise and re-do stuff
    st_init = 0,       // Start up
    st_th = 1,         // Setting threshold
    st_wait = 2,       // Waiting to overcome threshold
    st_running = 3,    // Stimulating
    st_stop = 4,       // Done 1 seq, waiting for next
    st_end = 5,        // Finish program
    st_calM = 6,      // Stimulation manual calibration
    st_calA_go = 7,   // Stimulation automatic calibration -> stimulator active
    st_calA_stop = 8, // Stimulation automatic calibration -> resting period
    st_repeat = 9,    // Repeat an exercise
    st_mvc = 10,    // set MVC threshold
} state_Type;

typedef enum
{
    log_trigger = 1,    // stimulator triggered
    log_noTgr   = 2,    // end of repetition (without triggering stimulator)
    log_end     = 3,    // end of repetition (with stimulator)
    log_start   = 4,    // start repetition
    log_train   = 5,    // start training pressed (no longer in use)
    log_stop    = 6,    // sending stop command to stimulator
    log_stopUser= 7,    // stimulator stopped by user
} log_type;

typedef enum
{
    exInvalid = -1,     // Invalid exercise
    kneeExt = 0,        // Knee extension
    kneeFlex = 1,       // Knee flexion
    plantFlex = 2,      // Planta flexion
    dorFlex = 3,        // Dorsal flexion
    exOther = 4,        // Because why not
} exercise_Type;

typedef enum
{
    th_SD05 = 0,        // SD*0.5 = Half of the standard deviation
    th_SD03 = 1,        // SD*0.3 = Third of the standard deviation
    th_MVC05 = 2,       // MVC*0.05
    th_MVC10 = 3,       // MVC*0.10
} threshold_Type;

typedef enum
{
    emgCh0 = 0,         // Channel not selected - it first need to be selected for the program to run 
    emgCh1 = 1,         // EMG Channel 1: Red connector/ black stim need to be connected
    emgCh2 = 2,         // EMG Channel 2: Blue connector/ white stim need to be conected
} emgCh_Type;

// User options for stimulation and process
typedef enum
{
    Move3_none = 0,      // Nothing to do
    Move3_incr = 1,      // Increment current
    Move3_decr = 2,      // Reduce current
    Move3_ramp_more = 3, // Increase ramp
    Move3_ramp_less = 4, // Reduce ramp
    Move3_stop = 5,      // Stop Stimulating
    Move3_start = 6,     // Stimulate with initial values
    Move3_done = 7,      // Finish callibration
    Move3_Hz_mr = 8,     // Increase frequency
    Move3_Hz_ls = 9,    // Decrease Frequency
    Move3_us_mr = 10,    // Increase pulse width
    Move3_us_ls = 11,    // Decrease pulse width
    Move3_en_ch = 12,     //Enable/disable channel
} RehaMove3_Req_Type;

typedef enum
{
    User_none = 0, // Nothing to do
    User_CM = 1,   // Manual calibration
    User_CA = 2,   // Automatic calibration
    User_X = 3,    // Quit calibration
    User_th = 4,   // Start threshold
    User_st = 5,   // Start training
    User_ex = 6,   // User must select an exercise
    User_rep = 7,  // Repeat same exercise
    User_new = 8,  // Do a new type of exercise
} User_Req_Type;

typedef enum
{
    exDone = 0,     // Finish everything
    start = 1,       // Exercise has started
    repeat = 2,     // Repeat exercise
    pause = 3,      // Play/Pause button = in pause
    play = 4,     // Play/Pause button = in play 
    setDone = 5,    // Repetitions-set finished
    repStart = 6,   // Start repetition
    repEnd = 7,     // End repetition
    finish = 8,     // Close socket
    msgEnd = 9,   // exercise has been aborted
    msg_none = 10, // You messed up somewhere
    res1 = 11,       // A resistance value has been received
    res2 = 12,
    res3 = 13,
    res4 = 14,
    res5 = 15,
    res6 = 16,
    res7 = 17,
    res8 = 18,
    res9 = 19,
    res10 = 20,
} tcp_msg_Type;

const int MSG_AMOUNT = (int)res10 + 1;
// ------------------ Global variables ------------------
const int DATE_LENGTH = 256;
// Threads cycles
auto th1_st = std::chrono::steady_clock::now();
auto th1_fn = std::chrono::steady_clock::now();
std::chrono::duration<double> th1_diff;
const double TH1_CYCLE = 0.03;
double th1_d = 0;
int th1_sleep = 1;
const int MAIN_THREAD = 1;
const bool THREAD_START = true, THREAD_END = false;

auto th2_st = std::chrono::steady_clock::now();
auto th2_fn = std::chrono::steady_clock::now();
std::chrono::duration<double> th2_diff;
const double TH2_CYCLE = 0.1;
double th2_d = 0.0;
int th2_sleep = 1;
const int INTERFACE_THREAD = 2;

//Tic-toc time
auto tstart = std::chrono::steady_clock::now();
auto tend = std::chrono::steady_clock::now();
std::chrono::duration<double> tstart_tend_diff;
double toc_lim = 5.0; // sec

// ------------------ Functions definition ------------------
void generate_date(char* outStr)
{
    //void do not return values and a char array is a message
    //So the function gets a pointer to a global char array to write the date
    time_t rawtime;
    struct tm* timeinfo;
    char buffer[DATE_LENGTH];
    //char output;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", timeinfo);
    for (int i = 0; i < DATE_LENGTH; ++i)
    {
        outStr[i] = buffer[i];
    }
}

string convert_to_string(char *a, int size)
{
    int i;
    string s = "";
    for (i = 0; i < size - 1; i++)
    {
        s = s + a[i];
    }
    return s;
}

void get_dir(int argc, char *argv[], string &Outdir)
{
    //Gets the current directory of the programme, but for files
    std::stringstream sst(argv[0]);
    char delimeter = '\\'; //for Windows
    std::string part, full = "", part_prev = "";
    while (std::getline(sst, part, delimeter))
    {
        full += part_prev;
        part_prev = part + delimeter;
    }

    // Cleaning unnecessary parts
    std::string t = full;
    std::string s = "\\Release";
    std::string::size_type i = t.find(s);
    if (i != std::string::npos)
        t.erase(i, s.length());

    s = "\\Debug";
    std::string::size_type j = t.find(s);
    if (j != std::string::npos)
        t.erase(j, s.length());

    s = "\\x64";
    std::string::size_type k = t.find(s);
    if (k != std::string::npos)
        t.erase(k, s.length());

    // Output
    full = t;
    Outdir = full;
}

// Las funciones de tic-toc solo se hicieron porque Christian echa mucho de menos Matlab
void tic()
{
    tstart = std::chrono::steady_clock::now();
}

double toc()
{
    tend = std::chrono::steady_clock::now();
    tstart_tend_diff = tend - tstart;

    //bool done = (((double)tstart_tend_diff.count()) >= toc_lim);
    return tstart_tend_diff.count();
}

int control_thread(int thread_nr, bool start, state_Type state)
// New: it returns how long the thread must sleep
{
    int output = 0;
    if (start){
        // Start thread: start counting time
        switch(thread_nr){
            case 1:
                th1_st = std::chrono::steady_clock::now();
                break;
            case 2:
                th2_st = std::chrono::steady_clock::now();
                break;
        }
    }else{
        // End thread: checks the current cycle time
        switch (thread_nr)
        {
        case 1:
            th1_fn = std::chrono::steady_clock::now();
            th1_diff = th1_fn - th1_st;
            th1_d = ((double)th1_diff.count()) + 0.0015;
            if ((th1_d < TH1_CYCLE) && (state >= st_wait) && (state <= st_stop))
            {
                th1_d = (TH1_CYCLE - th1_d) * 1000;
                th1_sleep = 1 + (int)th1_d;
                output = th1_sleep;
            }
            else
            {
                output = 25;
                th1_sleep = 0;
            }
            break;

        case 2:
            th2_fn = std::chrono::steady_clock::now();
            th2_diff = th2_fn - th2_st;
            th2_d = ((double)th2_diff.count());
            output = 100;
            break;
        default:
            output = 20;
            break;
        }

    }

    return output;
}
// ------------------------------------------------------------------------
#endif