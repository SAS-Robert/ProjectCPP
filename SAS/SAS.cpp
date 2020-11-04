/** C++ Application for the RehaMove3 and RehaIngest devices
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
 *
*/
#define _CRT_SECURE_NO_WARNINGS
#pragma warning( disable : 4507 34 )
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
#include "SASLIB.h"
#include "Iir.h"

using namespace std;

// ------------------------- Devices handling --------------------------------
int one_to_escape = 0;
//bool main_start = false;
bool stim_ready = false;
bool rec_ready = false;
RehaMove3_Req_Type Move3_cmd = Move3_none;
RehaMove3_Req_Type Move3_hmi = Move3_none;
RehaIngest_Req_Type Inge_key = Inge_none;
RehaIngest_Req_Type Inge_hmi = Inge_none;

state_Type state_process = st_init;

// Files handler to store recorded data
ofstream fileRAW, fileFILTERS;
ofstream fileVALUES;
string SASName4, SASName5, SASName6;
// files
char date[15];
string file_dir;
string date_s;
string format(".txt");
string init2b("CUL_leg_iir_");
// ------------------------- Connection  ------------------------
UDPClient ROBERT;
int udp_cnt = 0;
TCPServer SCREEN;
ofstream msgData;
RehaMove3_type stimulator_device;
RehaIngest_type recorder_device;
ROB_Type screen_status;
bool IsMoving = false, EOPR = false;
double rec_threshold = 0;

// Filters from iir.h library. All the filters require the following parameters:
// - Nr of order
// - Sampling rate
// - Central frequency (Hz)
// - Frequency width (Hz)
// - Chebyshev II: also Stop band attenuation (dB)
// - Most of these parameters are calculated from Matlab's original scripts
const int samplingrate = 1000;

const int orderButty = 4;
double Low_Hz = 20;
double High_Hz = 300;
const double B_Fq = (High_Hz+Low_Hz)/2;
const double B_Fqw = (High_Hz-Low_Hz);
Iir::Butterworth::BandPass<orderButty> Butty;
std::vector<double> Butty_result;
//Cheby50 = designfilt('bandstopiir', 'PassbandFrequency1', 47,
//        'StopbandFrequency1', 49, 'StopbandFrequency2', 51,
//        'PassbandFrequency2', 53, 'PassbandRipple1', 1,
//        'StopbandAttenuation', 60, 'PassbandRipple2', 1,
//        'SampleRate', 1000, 'DesignMethod', 'cheby2');
const double C50_Fq = 50;
double C50_Pfq1 = 47;
double C50_Sfq1 = 49;
double C50_Sfq2 = 51;
double C50_Pfq2 = 53;
double C50_PSfq1 = (C50_Sfq1+C50_Pfq1)/2;
double C50_PSfq2 = (C50_Pfq2+C50_Sfq2)/2;

const double C50_Fqw = (C50_PSfq2-C50_PSfq1);
const double C50_dB = 60;
const int order50 = 10;
Iir::ChebyshevII::BandStop<order50> Cheby50;
std::vector<double> C50_result;

//Cheby100 = designfilt('bandstopiir', 'PassbandFrequency1', 97,
//          'StopbandFrequency1', 99, 'StopbandFrequency2', 101,
//          'PassbandFrequency2', 103, 'PassbandRipple1', 1,
//          'StopbandAttenuation', 60, 'PassbandRipple2', 1,
//          'SampleRate', 1000, 'DesignMethod', 'cheby2');
const double C100_Fq = 100;
double C100_Pfq1 = 97;
double C100_Sfq1 = 99;
double C100_Sfq2 = 101;
double C100_Pfq2 = 103;
double C100_PSfq1 = (C100_Sfq1+C100_Pfq1)/2;
double C100_PSfq2 = (C100_Pfq2+C100_Sfq2)/2;
const double C100_Fqw = (C100_PSfq2-C100_PSfq1);
const double C100_dB = 60;
const int order100 = 20;
Iir::ChebyshevII::BandStop<order100> Cheby100;
std::vector<double> C100_result;

std::vector<double> Filter_result;

int ROB_rep = 0;
ROB_Type wololo;
int TCP_rep = 20;

// ------------------------------ Associated with functions -------------------
//Tic-toc time
time_t tstart, tend, tstart_ing, tend_ing, tstart_move, tend_move;
double toc_lim = 2;
int task1 = 0;
int task2 = 0;
bool data_start = false;
bool data_printed = false;
// Measuring time:
//-time 1 = from when threshold has been passed until the stimulator starts (FES_cnt)
//-time 2 = filtering perfomance (EMG_tic)
//-time 3 = process perfomance: state machine + recorder + stimulator time
//-time 4 = interface perfomance: keyboard + UDP + TCP time
ofstream time1_f;
ofstream time2_f;
ofstream time3_f;
ofstream time4_f;
std::vector<double> time1_v;
std::vector<double> time2_v;
std::vector<double> time2_v2;
std::vector<double> time3_v;
std::vector<double> time3_v2;
std::vector<double> time4_v;
std::chrono::duration<double> time1_diff;
std::chrono::duration<double> time2_diff;
std::chrono::duration<double> time3_diff;
std::chrono::duration<double> time4_diff;
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
std::chrono::duration<double> auto_diff_stim;
std::chrono::duration<double> auto_diff_rec;
auto auto_start_rec = std::chrono::steady_clock::now();
auto auto_end_rec = std::chrono::steady_clock::now();

string time1_s("CUL_leg_time1_");
string time2_s("CUL_leg_time2_");
string time3_s("CUL_leg_time3_");
string time4_s("CUL_leg_time4_");

bool stim_fl0 = false, stim_fl1 = false, stim_fl2 = false, stim_fl3 = false, stim_fl4 = false;
bool rec_fl0 = false, rec_fl1 = false, rec_fl2 = false, rec_fl3 = false, rec_fl4 = false;
unsigned long long int processed = 0;
unsigned long long int th_time = 3; // 3 seconds
unsigned long long int th_nr = th_time * samplingrate; // amount of samples for threshold
double th_discard = samplingrate * 1;                // discard first filtered samples from the threshold

struct device_to_device {
    bool start = false;
    bool end = false;
    bool th = false;
    bool ready = false;
} rec_status, MAIN_to_all, stim_status;
//MAIN_to_all: the main function writes here and all threads read
//rec_status: Ingest writes on the variable, Move3 reads it

// ---------------------------- Functions declaration  -----------------------------
// Threads and devices function hearders
static void thread_stimulation();
static void thread_recording();

void tic();
bool toc();
bool TCP_decode(char* message, RehaMove3_Req_Type& stimulator, RehaIngest_Req_Type& recorder, ROB_Type& status, int& rep);

//Communication between threads
void keyboard();
void stimulation_set(RehaMove3_Req_Type& code);
//void stimulation_set(RehaMove3_Req_Type code, Smpt_ml_channel_config* current_val, Smpt_ml_channel_config next_val);

// Example threads
void thread_t1();
void thread_t2();
//void create_file();
bool dummy_tcp = false;
bool dummy_valid = false;
void thread_connect();
//float process_data_MM();
static double process_data_iir(unsigned long long int N_len);
static double process_th(unsigned long long int N_len);

// ------------------------------ Main  -----------------------------

int main(int argc, char* argv[]) {
    // initialize files names
    get_dir(argc, argv, file_dir);
    generate_date(date); //get current date/time in format YYMMDD_hhmm
    date_s = convertToString(date,sizeof(date));
    init2b = file_dir + "CUL_leg_filter_"+ date_s.c_str()  +  ".txt";
    time1_s = file_dir + time1_s + date_s.c_str()  +  ".txt";
    time2_s = file_dir + time2_s + date_s.c_str()  +  ".txt";
    time3_s = file_dir + time3_s + date_s.c_str()  +  ".txt";
    time4_s = file_dir + time4_s + date_s.c_str() + ".txt";
    SASName6 = file_dir + "CUL_leg_th_" + date_s.c_str() + ".txt";
    // Start filters
    Butty.setup(samplingrate, B_Fq, B_Fqw);
    Cheby50.setup(samplingrate, C50_Fq, C50_Fqw, C50_dB);
    Cheby100.setup(samplingrate, C100_Fq, C100_Fqw, C100_dB);

    // other dummy stuff

    int ch;
    dummy_valid = false;
    printf("Do you wanna use the TCP connection to the Touch Panel? [Y/N]: ");
    while (!dummy_valid) {
        ch = _getch();
        ch = toupper(ch);
        if (ch == 'Y') {
            dummy_tcp = true;
        }
        dummy_valid = (ch == 'Y') || (ch == 'N');
        printf("%c ", ch);
    }

    dummy_valid = false;

    // Flushing input and output buffers
    cout.flush();
    fflush(stdin);
    std::cout << "\n===================================" << endl;
    std::cout << " Communication start up " << endl;
    std::cout << "===================================" << endl;

    // Starting UPD Connection
    std::cout << "Starting connection with ROBERT and Touch Screen\n";
    //strcpy(ROBERT.SERVERc, "127.0.0.1");  //This is an address for testing
    ROBERT.display = true;               //Chosen not to show messages during messages exchange
    do {
        ROBERT.start();
    } while (ROBERT.error);
    ROBERT.display = false;
    if (dummy_tcp) {
        SCREEN.start();
        SCREEN.waiting();
    }
    else {
        printf("TCP Connection skipped. Using keyboard instead.\n\n");
    }

    std::cout << "===================================" << endl;
    if (dummy_tcp) {
        std::cout << "---> TCP/UDP controllers:\n-T = Show/hide TCP messages.\n-U = Show/hide UDP messages.\n\n";
    }
    else {
        std::cout << "---> UDP controllers:\n-U = Show/hide UDP messages.\n\n";
    }
    if (!dummy_tcp) {
        std::cout << "---> RehaMove3 controllers:\n-A = Reduce Ramp.\n-D = Increase ramp.\n-W = Increase current.\n-S = Decrease current.\n";
        std::cout << "-Q = Stop stimulation.\n-E = Restore initial stimulation values.\n\n";
        std::cout << "---> Process controllers:\n-G = Increase repetitions nr.\n-H = Decrease repetitions nr.\n\n\n";
    }
    std::cout << "Press any key to set threshold and start SAS.\nThen press 0 to finish the process.\n";
    std::cout << "===================================" << endl;

    _getch();

    std::thread Interface(thread_connect);

    MAIN_to_all.start = true;
    //SCREEN.display = true;
    printf("State machine = initialization\n");

    // State machine change: set threshold -> wait for release???
    while(!MAIN_to_all.end && (state_process != st_end)) {
        time3_start = std::chrono::steady_clock::now();

        thread_recording();
        thread_stimulation();

        switch (state_process) {
        case st_init:
            if (rec_status.ready && stim_status.ready) {
                printf("State machine change: initialization -> set threshold\n");
                state_process = st_th;
            }
            break;

        case st_th:
            if (rec_status.th) {
                printf("State machine change: set threshold -> wait for release\n");
                state_process = st_wait;
                tic();
            }
            break;

        case st_wait:
            if (rec_status.start) {
                state_process = st_running;
                printf("State machine change: wait for release -> stimulating\n");
            }
            if (ROBERT.Reached && ROBERT.valid_msg) {
                // Increase repetitions
                ROB_rep++;
                std::cout << "\n---> Repetition nr." << ROB_rep << " completed of " << TCP_rep << " <--- " << endl;
                if (ROB_rep < TCP_rep) {
                    std::cout << "Waiting for robot to return to start position." << endl;
                }
                printf("State machine change: stimulating -> stop\n");
                state_process = st_stop;
            }
            break;

        case st_running:
            if (ROBERT.Reached && ROBERT.valid_msg) {
                // Increase repetitions
                ROB_rep++;
                std::cout << "\n---> Repetition nr." << ROB_rep << " completed of " << TCP_rep << " <--- " << endl;
                if (ROB_rep < TCP_rep) {
                    std::cout << "Waiting for robot to return to start position." << endl;
                }
                printf("State machine change: stimulating -> stop\n");
                state_process = st_stop;
            }
            break;

        case st_stop:
            // Check nr of repetitions and devices
            if (ROB_rep < TCP_rep && stim_status.ready && rec_status.ready && !ROBERT.Reached && !ROBERT.isMoving){
                std::cout << "Starting next repetition" << endl;
                state_process = st_wait;
            }
            else if (ROB_rep >= TCP_rep && stim_status.ready && rec_status.ready) {
                std::cout << "Exercise finished. Waiting for program to end." << endl;
                state_process = st_end;
            }
            break;
        }// State machine

        Sleep(40);
        time3_end = std::chrono::steady_clock::now();

        if(state_process != st_init){
        time3_diff = time3_end - time3_start;
        time3_v.push_back((double)time3_diff.count());
        time3_v2.push_back(state_process);

        }
    }

    // Finish devices
    state_process = st_end;
    thread_recording();
    thread_stimulation();
    fileFILTERS.close();
    fileVALUES.close();
    // Safe here the time samples on a file
    time1_f.open(time1_s);
    if (time1_f.is_open()) {
        for (int k = 0; k < time1_v.size(); k++) {
            time1_f << time1_v[k] << " ; " << endl;
        }
        printf("Main: time measurement t1 saved in file.\n");
    }
    else {
        printf("Main: Data t1 could not be saved.\n");
    }
    time1_f.close();

    time2_f.open(time2_s);
    if (time2_f.is_open()) {
        for (int k = 0; k < time2_v.size(); k++) {
            time2_f << time2_v[k] << endl;
        }
        printf("Main: time measurement t2 saved in file.\n");
    }
    else {
        printf("Main: Data t2 could not be saved.\n");
    }
    time2_f.close();

    time3_f.open(time3_s);
    if (time3_f.is_open() && time3_v.size()>=2) {
        for (int k = 0; k < time3_v.size(); k++) {
            time3_f << time3_v[k] <<", "<< time3_v2[k] << ";" << endl;
        }
        printf("Main: time measurement t3 saved in file.\n");
    }
    else {
        printf("Main: Data t3 could not be saved.\n");
    }
    time3_f.close();

    time4_f.open(time4_s);
    if (time4_f.is_open() && time4_v.size() >= 2) {
        for (int k = 0; k < time4_v.size(); k++) {
            time4_f << time4_v[k] << ";" << endl;
        }
        printf("Main: time measurement t4 saved in file.\n");
    }
    else {
        printf("Main: Data t3 could not be saved.\n");
    }
    time4_f.close();
    // Waiting for other thread to finish
    Interface.join();

    ROBERT.end();
    if (dummy_tcp) { SCREEN.end(); }

    std::cout << "\n--------------------------\nHej Hej!\n--------------------------\n";
    // Flushing input and output buffers
    cout.flush();
    fflush(stdin);

    // testing
        // not original main
    SASName4 = file_dir + "CUL_leg_raw_" + date_s.c_str() + ".txt";

    fileRAW.open(SASName4);        // This is the data vector accumulation
    for (int j = 0; j < recorder_emg1.size(); j++) {
        fileRAW << "0.0, " << recorder_emg1[j] << ", 2.0, 3.0, 4.0" << "\n";
    }

    fileRAW.close();

    return 0;
}

// ---------------------------- Functions definition  --------------------------
void tic() {
    tstart = time(0);
}

bool toc() {
    tend = time(0);
    double diff = difftime(tend, tstart);
    bool done = (diff >= toc_lim);
    return done;
}
/*
bool TCP_decode(char* message, RehaMove3_Req_Type& stimulator, RehaIngest_Req_Type& recorder, ROB_Type& status, int& rep) {
    int length = strlen(message);
    char start_msg[7] = "SCREEN";
    char finish_msg[7] = "ENDTCP";
    char delim_msg = 59;  // ASCII Semicolon ; code
    char end_msg = 59;
    bool valid_msg = false;

    int comp = 0, pos = 0, pos_cont = 0, j = 0;
    bool comp_b = false, valid1 = false, valid2 = false, valid3 = false, charac = false, delimt = false;
    bool check_end = false;
    //Looking for beginning of the message:
    while ((j < length) && !valid_msg) {
        // Beginning of the string found
        if (((message[j] == start_msg[0]) || (message[j] == finish_msg[0])) && (pos_cont <= 1)) { //start_msg[0]){
            comp_b = true;
            pos_cont = 0;
            comp = 0;
            pos = j;
            if (message[j] == finish_msg[0]) {
                check_end = true;
            }
        }
        // If the beginning is found, start storing the "SCREEN" identifier
        if ((comp_b) && (pos_cont < 7)) {
            if (!check_end) {
                comp = comp + (message[j] - start_msg[j - pos]);
            }
            else {
                comp = comp + (message[j] - finish_msg[j - pos]);
            }
            pos_cont++;
        }
        // After 6 iteractions, verify that the identifier was found, then compare fields
        if ((pos_cont >= 6) && (comp == 0)) {
            if (!check_end) {
                // Checking delimiters
                delimt = (message[pos + 6] == delim_msg) && (message[pos + 8] == delim_msg) && (message[pos + 10] == delim_msg);
                // Checking commands
                valid1 = (message[pos + 7] >= '0') || (message[pos + 7] <= '9');
                valid2 = (message[pos + 9] >= '0') || (message[pos + 9] <= '9');
                // Checking rep value
                charac = (message[pos + 11] >= 'A') && (message[pos + 11] <= 'Z');
                valid_msg = (comp == 0) && delimt && valid1 && valid2 && charac;
                //printf("RESULT: delimiters = %d, valid cmd = %d %d, valid rep = %d, MSG = %d. ", delimt, valid1, valid2, charac, valid_msg);
            }
            else {
                valid_msg = (comp == 0);
            }
        }
        else if (pos_cont >= 6) {
            comp_b = false; // If not found_set up back
            check_end = false;
        }
        j++;
    }

    if (!check_end) {
        //Convert from char to int values
        int move_value = message[pos + 7] - '0';
        int ingest_value = message[pos + 9] - '0';

        int get_status = message[pos + 11];
        unsigned long long int rep_nr = 0;

        if (get_status == 'R') { // Only necessary to decode the rep number if R
            int rep_unit100 = message[pos + 12] - '0';
            int rep_unit10 = message[pos + 13] - '0';

            if (message[pos + 13] == delim_msg) {
                rep_nr = rep_unit100;
            }
            else if (message[pos + 14] == delim_msg) {
                rep_nr = rep_unit100 * 10 + rep_unit10;
            }
            else {
                int rep_unit1 = message[pos + 14] - '0';
                rep_nr = rep_unit100 * 100 + rep_unit10 * 10 + rep_unit1;
            }
            valid_msg = valid_msg && (rep_nr >= 0) && (rep_nr <= 999); // Check values
            //printf("funct result: move_key = %d, ingest_key = %d, rep_nr = %d.\n ", move_value, ingest_value, rep_nr);
        }

        //Outputs
        if (valid_msg) {
            stimulator = (RehaMove3_Req_Type)move_value;
            recorder = (RehaIngest_Req_Type)ingest_value;
            //status[0] = get_status;
            status = (ROB_Type)get_status;
            if (get_status == 'R') {
                rep = rep_nr;
            }
        }
    } // !check_end
    else {
        if (valid_msg) {
            SCREEN.finish = true;
        }
    }

    return valid_msg;
}
*/

void keyboard() {
    int ch;
    ch = _getch();
    ch = toupper(ch);
    printf("---> Key pressed: %c <---\n", ch);
    switch (ch) {
    case 'T':
        SCREEN.display = !SCREEN.display;
        if (SCREEN.display && dummy_tcp) {
            printf("Showing TCP messages and status.\n");
        }
        else if (dummy_tcp) {
            printf("Not showing TCP messages and status.\n");
        }
        break;
    case 'U':
        ROBERT.display = !ROBERT.display;
        if (ROBERT.display) {
            printf("Showing UDP messages and status.\n");
        }
        else {
            printf("Not showing UDP messages and status.\n");
        }
        break;
    case 'A':
        if (!dummy_tcp) { Move3_hmi = Move3_ramp_less; }
        break;
    case 'D':
        if (!dummy_tcp) { Move3_hmi = Move3_ramp_more; }
        break;
    case 'G':
        if (!dummy_tcp) {
            TCP_rep++;
            if (TCP_rep >= 10) {
                TCP_rep = 10;
            }
            std::cout << "Rep nr. modified to " << TCP_rep << endl;
        }
        break;
    case 'H':
        if (!dummy_tcp) {
            TCP_rep--;
            if (TCP_rep < ROB_rep) {
                TCP_rep = ROB_rep + 1;
                std::cout << "Rep nr. cannot be lower than the current progress amount. ";
            }
            std::cout << "Rep nr. modified to " << TCP_rep << endl;
        }
        break;
    case 'M':
        // Modify threshold
        if (!dummy_tcp) { Inge_key = Inge_decr; }
        break;
    case 'P':
        // Modify threshold
        if (!dummy_tcp) { Inge_key = Inge_incr; }
        break;
    case 'Q':
        if (!dummy_tcp) { Move3_hmi = Move3_stop; }
        break;
    case 'E':
        if (!dummy_tcp) { Move3_hmi = Move3_start; }
        break;
    case 'S':
        if (!dummy_tcp) { Move3_hmi = Move3_decr; }
        break;
    case 'W':
        if (!dummy_tcp) { Move3_hmi = Move3_incr; }
        break;
    case '0':
        MAIN_to_all.end = true;
        break;
    }
}

//void stimulation_set(RehaMove3_Req_Type code, Smpt_ml_channel_config* current_val, Smpt_ml_channel_config next_val);{
void stimulation_set(RehaMove3_Req_Type& code) {
    Smpt_ml_channel_config next_val;
    next_val = stimulator_device.stim;
    // current_Val = real values on the stimulator
    // next_val = values that are going to be assigned
    // code = from the keyboard

    /* Set the stimulation pulse */
    // stim.points[0].current = 50;
    // stim.points[0].time = 200;
    // stim.points[1].time = 100;
    // stim.points[2].current = -50;
    // stim.points[2].time = 200;


    switch (code) {
        // User interface changes
    case Move3_ramp_more:
        next_val.number_of_points++;
        next_val.ramp++;
        break;
    case Move3_ramp_less:
        next_val.number_of_points--;
        next_val.ramp--;
        break;
    case Move3_decr:
        next_val.points[0].current = next_val.points[0].current - 0.1;
        next_val.points[2].current = next_val.points[2].current + 0.1;
        break;
    case Move3_incr:
        next_val.points[0].current = next_val.points[0].current + 0.1;
        next_val.points[2].current = next_val.points[2].current - 0.1;
        break;
        // Process profiles
    //  case Move3_stop:
    //      // Stimulation values
    //      next_val.number_of_points = 10;  //* Set the number of points
    //      next_val.ramp = 10;              //* Three lower pre-pulses
    //      next_val.period = 20;           //* Frequency: 50 Hz
    //      // Set the stimulation pulse
    //      next_val.points[0].current = 0;
    //      next_val.points[0].time = 200;
    //      next_val.points[1].time = 200;
    //      next_val.points[2].current = 0;
    //      next_val.points[2].time = 200;
    //      break;
    // break;
    // case Move3_start:
    //     // Update values
    //     next_val.number_of_points = 3;
    //     next_val.ramp = 3;
    //     next_val.period = 20;
    //     next_val.points[0].current = 5;
    //     next_val.points[0].time = 200;
    //     next_val.points[1].time = 200;
    //     next_val.points[2].current = -5;
    //     next_val.points[2].time = 200;
    //     break;
    }
    // Checking max and min possible values:
    if (next_val.number_of_points > 5) {
        next_val.number_of_points = 5;
    }
    else if (next_val.number_of_points <= 0) {
        next_val.number_of_points = 0;
    }

    if (next_val.ramp > 5) {
        next_val.ramp = 5;
    }
    else if (next_val.ramp <= 0) {
        next_val.ramp = 0;
    }

    if (next_val.points[0].current >= 10) {
        next_val.points[0].current = 10;
        next_val.points[2].current = -10;
    }
    else if (next_val.points[0].current <= 0) {
        next_val.points[0].current = 0;
        next_val.points[2].current = 0;
    }

    stimulator_device.stim = next_val;
    if ((code != Move3_stop) && (code != Move3_start)) {
        printf("RehaMove3 message: Stimulation update -> current = %2.1f, ramp points = %d, ramp value = %d\n", stimulator_device.stim.points[0].current, stimulator_device.stim.number_of_points, stimulator_device.stim.ramp);
        //%2.1f is the format to show a float number, 2.1 means 2 units and 1 decimal
    }
    // else if (code == Move3_stop){
    //   printf("RehaMove3 message: Stimulation stopped\n");
    // }else{
    //   printf("RehaMove3 message: Stimulation values set to initial values (but not sent to device, chiiiiiiiill )\n");
    // }
    code = Move3_none;
    Move3_cmd = Move3_none;
    Move3_hmi = Move3_none;
}


void thread_connect() {
    bool decode_successful;

    while (!MAIN_to_all.end && (state_process != st_end)) {
        time4_start = std::chrono::steady_clock::now();

        one_to_escape = _kbhit();
        if (one_to_escape != 0) {
            keyboard();
        }
        // UDP Update:
        udp_cnt++;
        sprintf(ROBERT.message, "%d;STATUS", udp_cnt);
        ROBERT.get();
        // TCP Stuff
        /*
        if (!SCREEN.finish && dummy_tcp) {
            //printf(" -- checking... -- \n");
            SCREEN.check();
            // Receive
            if (SCREEN.new_message) {
                decode_successful = TCP_decode(SCREEN.recvbuf, Move3_hmi, Inge_key, wololo, TCP_rep);
                if (decode_successful) {
                    // Only update values  if the message was not "ENDTCP"
                    if (!SCREEN.finish) {
                        // switch(Inge_key){
                        //   case Inge_decr:
                        //       gain_th = gain_th + 0.1;
                        //       break;
                        //       case Inge_incr:
                        //       gain_th = gain_th-0.1;
                        //       break;
                        // }
                        if (SCREEN.display) { printf("TCP received: move_key = %d, ingest_key = %d, satus = %c, rep_nr = %d \n ", Move3_hmi, Inge_key, wololo, TCP_rep); }
                    }
                    // Send
                    memset(SCREEN.senbuf, '\0', 512);
                    sprintf(SCREEN.senbuf, "SAS;%2.1f;%1.1f;%1.1f;", stimulator_device.stim.points[0].current, stimulator_device.stim.ramp, gain_th);
                    SCREEN.stream();
                }
                else if (!decode_successful && SCREEN.display) {
                    printf("TCP received message not valid.\n");
                }
            }
        }*/
        // TCP Stuff
        Sleep(150);
        time4_end = std::chrono::steady_clock::now();
        time4_diff = time4_end - time4_start;
        time4_v.push_back((double)time4_diff.count());
    } // while loop
} // thread

double process_data_iir(unsigned long long int v_size) {
    time2_start = std::chrono::steady_clock::now();

    double mean = 0, temp = 0, sd = 0;
    unsigned long long int i = 0;
    unsigned long long int N_len = v_size - processed;
    // Filtering + calculate mean
    for (i = processed; i < v_size; ++i)                           // Loop for the length of the array
    {
        // Filter data - Christian's
        Butty_result.push_back(Butty.filter(recorder_emg1[i]));
        C50_result.push_back(Cheby50.filter(Butty_result[i]));
        C100_result.push_back(Cheby100.filter(C50_result[i]));
        // Saving data
        fileFILTERS << recorder_emg1[i] << "," << Butty_result[i] << "," << C50_result[i] << "," << C100_result[i] << "\n";
        // Calculating mean of retified EMG
        if (C100_result[i] > 0) {
            temp = C100_result[i];
        }
        else {
            temp = -C100_result[i];
        }
        mean = mean + temp;
    }
    mean = mean / N_len;
    // Saving results stuff
    fileVALUES << mean << ", 0.0, " << processed << "," << v_size << "," << N_len << "\n";
    // Update amount of processed data
    processed = i;

    // Save on another file here?

    time2_end = std::chrono::steady_clock::now();
    time2_diff = time2_end - time2_start;
    time2_v.push_back((double)time2_diff.count());

    return mean;
}

double process_th(unsigned long long int v_size) {
    time2_start = std::chrono::steady_clock::now();
    std::cout << "Init th " ;
    double mean = 0, temp = 0, sd = 0, value = 0;
    unsigned long long int i = 0;
    int th_limit = (int)th_discard;
    unsigned long long int N_len = v_size - (int)th_discard;
    std::cout << " parameter v_size = " << v_size << ", th_discard = " << th_discard << ", N_len = " << N_len << ", processed = " << processed << endl;
    // Filtering + calculate mean
    for (i = processed; i < v_size; ++i)                           // Loop for the length of the array
    {
        // Filter data - Christian's
        Butty_result.push_back(Butty.filter(recorder_emg1[i]));
        C50_result.push_back(Cheby50.filter(Butty_result[i]));
        C100_result.push_back(Cheby100.filter(C50_result[i]));

        // Saving data
        fileFILTERS << recorder_emg1[i] << "," << Butty_result[i] << "," << C50_result[i] << "," << C100_result[i] << "\n";

        // Only calculating mean after the first 0.2 seconds
        if (i >= th_discard) {
            // Calculating mean of retified EMG
            if (C100_result[i] > 0) {
                temp = C100_result[i];
            }
            else {
                temp = -C100_result[i];
            }
            mean = mean + temp;
        }

    }
    mean = mean / N_len;
    // Calculate standard deviation
    temp = 0;
    for (i = th_limit; i < v_size; ++i)
    {
        if (C100_result[i] > 0) {
            temp = C100_result[i];
        }
        else {
            temp = -C100_result[i];
        }
        sd += pow(temp - mean, 2);
    }
    sd = sqrt(sd / N_len);

    // Calculate final threshold value
    value = mean + (sd / 2);

    fileVALUES << mean << "," << sd << "," << th_discard << "," << v_size << "," << value << "\n";

    // Update amount of processed data
    processed = i;

    time2_end = std::chrono::steady_clock::now();
    time2_diff = time2_end - time2_start;
    time2_v.push_back((double)time2_diff.count());

    return value;
}
//================================================
void thread_stimulation()
{
    // Local Variables
    switch (state_process) {
    case st_init:
        // initialization
        stimulator_device.port_name_rm = "COM6";
        stimulator_device.init();
        stim_status.ready = stimulator_device.ready;
        break;

    case st_wait:
        // do stuff
        if (Move3_hmi != Move3_none) {
            stimulation_set(Move3_hmi);
        }
        break;

    case st_running:
        // do stuff
        if ((Move3_hmi != Move3_none) && (Move3_hmi != Move3_stop)) {
            stimulation_set(Move3_hmi);
            stim_fl1 = false;
        }

        if (Move3_hmi == Move3_stop) {
            stimulator_device.pause();
            std::cout<<"RehaMove3 message: Stimulator stopped."<<endl;
            stim_fl1 = true;
            Move3_hmi = Move3_none;
        }
        if (!stim_fl1) {
            stimulator_device.update();
        }
        // things to do only once
        if (!stim_fl2) {
            std::cout << "Reha Move3 message: Stimulating.\n";
            time1_end = std::chrono::steady_clock::now();
            time1_diff = time1_end - time1_start;
            time1_v.push_back((double)time1_diff.count());
            stim_fl2 = true;
            stim_status.ready = false;
        }
        //  stimulator_device.update();

        break;

    case st_stop:
        // Stop process and restore initial values
        if (!stim_fl3) { // Stop stimulator
            stimulator_device.pause();
            stim_fl3 = true;
        }
        else {
        // Move3_cmd = Move3_start;
          // stimulation_set(Move3_cmd);
           stim_fl0 = false;
           stim_fl1 = false;
           stim_fl2 = false;
           stim_status.ready = true;
        }

        break;

    case st_end:
        if (stimulator_device.ready) {
            stimulator_device.end();
        }
        break;
    } // switch case

} //void thread_ml_stimulation
//================================================
void thread_recording()
{
    int iterator = 0;
    unsigned long long int sample_nr = 0;
    double mean = 0;

    // record here?
    /*
    if ((state_process!=st_init)&&(state_process!=st_end)){
        recorder_device.record();
    }
    */

    switch (state_process) {
    case st_init:
        recorder_emg1.clear();
        Butty_result.clear();
        C50_result.clear();
        C100_result.clear();
        processed = 0;

        recorder_device.port_name_ri = "COM4";
        recorder_device.init();
        Sleep(4);
        recorder_device.start();
        rec_status.ready = recorder_device.ready;
        //std::cout << "Reha Ingest message: Waiting for start.\n";
        break;

    case st_th:
        recorder_device.record();
        if (recorder_device.data_received && recorder_device.data_start && !fileFILTERS.is_open() && !fileFILTERS.is_open())
            {
                fileFILTERS.open(init2b);
                fileVALUES.open(SASName6);
        }

        sample_nr = recorder_emg1.size();
        if ( sample_nr >= th_nr) {
            rec_threshold = process_th(sample_nr);
            std::cout << "Reha Ingest message: threshold = " << rec_threshold << endl;
            //rec_threshold = 10;
            rec_status.th = true;
        }
        break;

    case st_wait:
        recorder_device.record();
        sample_nr = recorder_emg1.size();
        if (sample_nr - processed >= 100) {
            mean = process_data_iir(sample_nr);

            if ((mean >= rec_threshold)&&!rec_status.start && !ROBERT.isMoving && ROBERT.valid_msg && toc()){
                printf("Reha Ingest message: threshold overpassed.\n");
                rec_status.start = true;
                time1_start = std::chrono::steady_clock::now();
            }
        }
        break;

    case st_running:
        recorder_device.record();
        sample_nr = recorder_emg1.size();
        if (sample_nr - processed >= 100) {
            mean = process_data_iir(sample_nr);
        }
        rec_status.ready = false;
        rec_status.start = false;
        break;

    case st_stop:
        recorder_device.record();
        sample_nr = recorder_emg1.size();
        if (sample_nr - processed >= 100) {
            mean = process_data_iir(sample_nr);
        }
        rec_status.ready = true;
        break;

    case st_end:
        if (recorder_device.ready) {
            recorder_device.end();
        }
        break;
    }

}
