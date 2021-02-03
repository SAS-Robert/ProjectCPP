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

// ------------------------------------------------------------------------

using namespace std;

// ------------------ Variables for process control ------------------
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
    Move3_en_ch = 8,     //Enable/disable channel
    Move3_Hz_mr = 9,     // Increase frequency
    Move3_Hz_ls = 10,    // Decrease Frequency
    Move3_us_mr = 11,    // Increase pulse width
    Move3_us_ls = 12,    // Decrease pulse width
} RehaMove3_Req_Type;

typedef enum
{
    Inge_none = 0, // Nothing to do
    Inge_incr = 1, // Increase threshold gain
    Inge_decr = 2, // Decrease threshold gain
} RehaIngest_Req_Type;

typedef enum
{
    pause = 'P', // 'P' = pause by user-request
    rep = 'R',   // 'R' = repetition active
    start = 'S', // 'S' = start of repetition
} ROB_Type;

typedef enum
{
    st_init = 0,       // Nothing to do
    st_th = 1,         // set threshold
    st_wait = 2,       // Waiting to overcome threshold
    st_running = 3,    // Stimulating
    st_stop = 4,       // Done 1 seq, waiting for next
    st_end = 5,        // Setting threshold
    st_testM = 6,      // Run stimulation test (manual)
    st_testA_go = 7,   // Run stimulation test (automatic) -> apply current
    st_testA_stop = 8, // Run stimulation test (automatic) -> stop stim for recovery
} state_Type;

typedef enum
{
    User_none = 0, // Nothing to do
    User_CM = 1,   // Manual calibration
    User_CA = 2,   // Automatic calibration
    User_X = 3,    // Quit calibration
    User_th = 4,   // Start threshold
    User_st = 5,   // Start training
} User_Req_Type;

// ------------------ Global variables ------------------
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
const double TH2_CYCLE = 0.15;
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
    char buffer[15];
    //char output;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M", timeinfo);
    for (int i = 0; i < 15; ++i)
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

void tic()
{
    tstart = std::chrono::steady_clock::now();
}

bool toc()
{
    tend = std::chrono::steady_clock::now();
    tstart_tend_diff = tend - tstart;

    bool done = (((double)tstart_tend_diff.count()) >= toc_lim);
    return done;
}

void control_thread(int thread_nr, bool start, state_Type state)
{
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
            //Sleep(10);
            if ((th1_d < TH1_CYCLE) && (state >= st_wait) && (state <= st_stop))
            {
                th1_d = (TH1_CYCLE - th1_d) * 1000;
                th1_sleep = 1 + (int)th1_d;
                Sleep(th1_sleep);
            }
            else
            {
                Sleep(5);
                th1_sleep = 0;
            }
            break;

        case 2:
            th2_fn = std::chrono::steady_clock::now();
            th2_diff = th2_fn - th2_st;
            th2_d = ((double)th2_diff.count());
            Sleep(100);
            break;
        default:
            Sleep(20);
            break;
        }

    }

}
// ------------------------------------------------------------------------
#endif