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

// ------------------------- Main global variables --------------------------------

int one_to_escape = 0;
state_Type state_process = st_init;

struct device_to_device {
    bool start = false;
    bool end = false;
    bool th = false;
    bool ready = false;
} rec_status, MAIN_to_all, stim_status;
//MAIN_to_all: the main function writes here and all threads read
//rec_status: Ingest writes on the variable, Move3 reads it

// Dummies
char date[15];
string file_dir;
string date_s;
string filter_s;

// ------------------------- Filtering  ------------------------
// Filters from iir.h library. All the filters require the following parameters:
// - Nr of order
// - Sampling rate
// - Central frequency (Hz)
// - Frequency width (Hz)
// - Most of these parameters are calculated from Matlab's original scripts
const int samplingrate = 1000;

const int orderButty = 4;
double Low_Hz = 20;
double High_Hz = 300;
const double B_Fq = (High_Hz + Low_Hz) / 2;
const double B_Fqw = (High_Hz - Low_Hz);
Iir::Butterworth::BandPass<orderButty> Butty;
std::vector<double> Butty_result;

const double B50_Fq = 50;
const double B100_Fq = 100;
const double B150_Fq = 150;
const double B200_Fq = 200;
const double B250_Fq = 250;

const double B50_100_Fqw = 10;
const int orderB50_100 = 2;

Iir::Butterworth::BandStop<orderB50_100> B50;
std::vector<double> B50_result;

Iir::Butterworth::BandStop<orderB50_100> B100;
std::vector<double> B100_result;

Iir::Butterworth::BandStop<orderB50_100> B150;
std::vector<double> B150_result;

Iir::Butterworth::BandStop<orderB50_100> B200;
std::vector<double> B200_result;

Iir::Butterworth::BandStop<orderB50_100> B250;
std::vector<double> B250_result;

// ------------------------- Devices handling --------------------------------

bool stim_ready = false;
bool rec_ready = false;
double rec_threshold = 0;

RehaMove3_Req_Type Move3_cmd = Move3_none;
RehaMove3_Req_Type Move3_hmi = Move3_none;
RehaIngest_Req_Type Inge_key = Inge_none;
RehaIngest_Req_Type Inge_hmi = Inge_none;

char port_stim[5] = "COM6";
RehaMove3 stimulator_device(port_stim);

char port_rec[5] = "COM4";
RehaIngest recorder_device(port_rec);

// Other variables
bool stim_fl0 = false, stim_fl1 = false, stim_fl2 = false, stim_fl3 = false, stim_fl4 = false;
bool rec_fl0 = false, rec_fl1 = false, rec_fl2 = false, rec_fl3 = false, rec_fl4 = false;
unsigned long long int processed = 0;
unsigned long long int th_time = 3; // 3 seconds
unsigned long long int th_nr = th_time * samplingrate; // amount of samples for threshold
double th_discard = samplingrate * 1.1;                // discard first filtered samples from the threshold
double th_discard_nr = 0;
int th_wait = 3, th_wait_cnt = 0;                                       // amount of mean sets before triggering

// Dummies
// Files handler to store recorded data
ofstream fileRAW, fileFILTERS;
ofstream fileVALUES, fileLOGS;
string SASName4, SASName5, th_s;

// ------------------------- UDP / TCP  ------------------------
char robot_IP_e[15] = "127.0.0.1";
char robot_IP[15] = "172.31.1.147";
uint32_t robot_port = 30007;
int udp_cnt = 0;
UDPClient ROBERT;

char screen_port[15] = "30002";
TCPServer SCREEN;

// TCP Stuff
int ROB_rep = 0;
ROB_Type wololo;
int TCP_rep = 20;

ROB_Type screen_status;
bool start_train = false;
unsigned long long int sample_nr = 0;
// ------------------------------ Timing -------------------
// Threads cycles
auto th1_st = std::chrono::steady_clock::now();
auto th1_fn = std::chrono::steady_clock::now();
std::chrono::duration<double> th1_diff;
const double th1_cycle = 0.1;
double th1_d = 0;
int th1_sleep = 1;
auto th2_st = std::chrono::steady_clock::now();
auto th2_fn = std::chrono::steady_clock::now();
std::chrono::duration<double> th2_diff;
const double th2_cycle = 0.15;
double th2_d = 0.0;
int th2_sleep = 1;

// Dummies - Measuring time:
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

string time1_s;
string time2_s;
string time3_s;
string time4_s;
char folder[256] = "demo_18Nov\\";
char Sname[256] = "subject1";
// ---------------------------- Functions declaration  -----------------------------
// Dummies
bool dummy_tcp = false;
bool dummy_valid = false;
// Various
//Tic-toc time
time_t tstart, tend;
double toc_lim = 2;
void tic();
bool toc();

// Interface functions
//bool TCP_decode(char* message, RehaMove3_Req_Type& stimulator, RehaIngest_Req_Type& recorder, ROB_Type& status, int& rep);
void keyboard();
void thread_connect();

// Devices functions
void stimulation_set(RehaMove3_Req_Type& code);
static void thread_stimulation();

static double process_data_iir(unsigned long long int N_len);
static double process_th(unsigned long long int N_len);
static void thread_recording();

// ------------------------------ Main  -----------------------------
int main(int argc, char* argv[]) {
    // initialize files names
    get_dir(argc, argv, file_dir);
    generate_date(date); //get current date/time in format YYMMDD_hhmm
    date_s = convertToString(date,sizeof(date));
    filter_s = file_dir + folder + Sname + "_filter_" + date_s.c_str() + ".txt";
    time1_s = file_dir + folder + Sname + "_time1_" + date_s.c_str()  +  ".txt";
    time2_s = file_dir + folder + Sname + "_time2_" + date_s.c_str()  +  ".txt";
    time3_s = file_dir + folder + Sname + "_time3_" + date_s.c_str()  +  ".txt";
    time4_s = file_dir + folder + Sname + "_time4_" + date_s.c_str() + ".txt";
    th_s = file_dir + folder + Sname + "_th_" + date_s.c_str() + ".txt";
    string LOGS_s = file_dir + folder + Sname + "_log_" + date_s.c_str() + ".txt";
    fileLOGS.open(LOGS_s);
    // Start filters
    Butty.setup(samplingrate, B_Fq, B_Fqw);
    B50.setup(samplingrate, B50_Fq, B50_100_Fqw);
    B100.setup(samplingrate, B100_Fq, B50_100_Fqw);
    B150.setup(samplingrate, B150_Fq, B50_100_Fqw);
    B200.setup(samplingrate, B200_Fq, B50_100_Fqw);
    B250.setup(samplingrate, B250_Fq, B50_100_Fqw);
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
    ROBERT.display = true;               //Chosen not to show messages during messages exchange
    do {
        ROBERT.start(robot_IP, robot_port);
    } while (ROBERT.error);
    ROBERT.display = false;
    if (dummy_tcp) {
        SCREEN.start(screen_port);
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
    std::cout << "--->Instructions: \n1. Press any key to start SAS and set the threshold.\n2. Press key nr.1 to start training.\n3. Then press 0 to finish the programme.\n";
    std::cout << "===================================" << endl;

    _getch();

    std::thread Interface(thread_connect);

    MAIN_to_all.start = true;

    printf("SAS PROGRAMME: starting up devices.\n");

    one_to_escape = 0;

    while(!MAIN_to_all.end && (state_process != st_end)) {
        th1_st = std::chrono::steady_clock::now();
        thread_recording();
        thread_stimulation();

        switch (state_process) {
        case st_init:
            if (rec_status.ready && stim_status.ready) {
                printf("SAS PROGRAMME: setting threshold...\n");
                state_process = st_th;
            }
            break;

        case st_th:
            if (rec_status.th) {
                printf("SAS PROGRAMME: Threshold saved. Waiting for stimulator to be triggered.\n");
                std::cout << "\n===================================" << endl;
                state_process = st_wait;
                tic();
            }
            break;

        case st_wait:
            if (rec_status.start) {
                state_process = st_running;
                printf("SAS PROGRAMME: Stimulator triggered\n");
                fileLOGS << "1.0, " << processed << "\n";
            }
            if (ROBERT.Reached && ROBERT.valid_msg) {
                // Increase repetitions
                ROB_rep++;
                printf("SAS PROGRAMME: End of Point reached.\n");
                std::cout << "\n---> Repetition nr." << ROB_rep << " completed of " << TCP_rep << " <--- " << endl;
                if (ROB_rep < TCP_rep) {
                    std::cout << "Waiting for robot to return to start position." << endl;
              }
                state_process = st_stop;
                fileLOGS << "2.0, " << processed << "\n";
          }
            break;

        case st_running:
            if (ROBERT.Reached && ROBERT.valid_msg) {
                // Increase repetitions
                ROB_rep++;
                printf("SAS PROGRAMME: End of Point reached.\n");
                std::cout << "\n---> Repetition nr." << ROB_rep << " completed of " << TCP_rep << " <--- " << endl;
                if (ROB_rep < TCP_rep) {
                    std::cout << "Waiting for robot to return to start position." << endl;
                }
                state_process = st_stop;
                fileLOGS << "3.0, " << processed << "\n";
          }
            break;

        case st_stop:
            // Check nr of repetitions and devices
            if (ROB_rep < TCP_rep && stim_status.ready && rec_status.ready && !ROBERT.Reached && !ROBERT.isMoving){
                std::cout << "\n===================================" << endl;
                std::cout << "SAS PROGRAMME: Starting next repetition" << endl;
                state_process = st_wait;
                fileLOGS << "4.0, " << sample_nr << "\n";
            }
            else if (ROB_rep >= TCP_rep && stim_status.ready && rec_status.ready) {
                std::cout << "\n===================================" << endl;
                std::cout << "SAS PROGRAMME: Exercise finished. Program will finish." << endl;
                state_process = st_end;
            }
            break;
        }// State machine

        Sleep(10);

        // Controlling thread cycle time
        th1_fn = std::chrono::steady_clock::now();
        th1_diff = th1_fn - th1_st;
        th1_d = ((double)th1_diff.count())+0.015;
        if ((th1_d < th2_cycle)&& (state_process != st_init)) {
            th1_d = (th1_cycle - th1_d) * 1000;
            th1_sleep = 1 + (int)th1_d;

            Sleep(th1_sleep);
        }
        else {
            Sleep(10);
            th1_sleep = 0;
        }

    time3_end = std::chrono::steady_clock::now();
    if (state_process != st_init) {
        time3_diff = time3_end - th1_st;
        time3_v.push_back((double)time3_diff.count());
        time3_v2.push_back(state_process);
    }

    }

    // Finish devices
    state_process = st_end;
    thread_recording();
    thread_stimulation();
    // Waiting for other thread to finish
    Interface.join();

    fileFILTERS.close();
    fileVALUES.close();

    fileLOGS.close();
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


    ROBERT.end();
 //   if (dummy_tcp) { SCREEN.end(); }

    std::cout << "\n--------------------------\nHej Hej!\n--------------------------\n";
    // Flushing input and output buffers
    cout.flush();
    fflush(stdin);

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

// ---------------------------- Functions interface  --------------------------
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
    case '1':
        if (state_process == st_wait) {
            start_train = true;
            fileLOGS << "5.0, " << processed << "\n";
            std::cout << "Start training pressed." << endl;
        }
        break;
    case '0':
        MAIN_to_all.end = true;
        break;
    }
}

void thread_connect() {
    bool decode_successful;

    while (!MAIN_to_all.end && (state_process != st_end)) {
        th2_st = std::chrono::steady_clock::now();

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
        Sleep(10);

        // Controlling thread cycle time
        th2_fn = std::chrono::steady_clock::now();
        th2_diff = th2_fn - th2_st;
        th2_d = ((double)th2_diff.count())+0.015;
        if (th2_d < th2_cycle) {
            th2_d = (th2_cycle- th2_d)*1000;
            th2_sleep = 1+(int)th2_d;

            Sleep(th2_sleep);
        }

        time4_end = std::chrono::steady_clock::now();
        time4_diff = time4_end - th2_st;
        time4_v.push_back((double)time4_diff.count());
    } // while loop
} // thread
// ---------------------------- Functions devices  --------------------------

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

void thread_stimulation()
{
    // Local Variables
    switch (state_process) {
    case st_init:
        // initialization
        stimulator_device.init();
        // Update values
        stimulator_device.stim.number_of_points = 3;  //* Set the number of points
        stimulator_device.stim.ramp = 3-;              //* Three lower pre-pulses
        stimulator_device.stim.period = 20;           //* Frequency: 50 Hz
        stimulator_device.stim.points[0].current = 25.0;
        stimulator_device.stim.points[0].time = 200;
        stimulator_device.stim.points[1].time = 200;
        stimulator_device.stim.points[2].current = -25.0;
        stimulator_device.stim.points[2].time = 200;

        stim_status.ready = stimulator_device.ready;
        break;

    case st_wait:
        stim_fl3 = false;
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
            fileLOGS << "6.0, " << processed << "\n";
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
double process_data_iir(unsigned long long int v_size) {
    time2_start = std::chrono::steady_clock::now();

    double mean = 0, temp = 0, sd = 0;
    unsigned long long int i = 0;
    unsigned long long int N_len = v_size - processed;
    // Filtering + calculate mean
    for (i = processed; i < v_size; ++i)                           // Loop for the length of the array
    {
        // Filter data
        Butty_result.push_back(Butty.filter(recorder_emg1[i]));
        B50_result.push_back(B50.filter(Butty_result[i]));
        B100_result.push_back(B100.filter(B50_result[i]));
        B150_result.push_back(B150.filter(B100_result[i]));
        B200_result.push_back(B200.filter(B150_result[i]));
        B250_result.push_back(B250.filter(B200_result[i]));
        // Saving data
        fileFILTERS << recorder_emg1[i] << "," << Butty_result[i] << "," << B50_result[i] << "," << B100_result[i] <<"," << B150_result[i] << "," << B200_result[i] << "," << B250_result[i] << "\n";
        // Calculating mean of retified EMG
        if (B100_result[i] > 0) {
            temp = B250_result[i];
        }
        else {
            temp = -B250_result[i];
        }
        mean = mean + temp;
    }
    mean = mean / N_len;
    // Saving results
    fileVALUES << mean << ", 0.0, " << processed << "," << v_size << "," << N_len << "\n";
    // Update amount of processed data
    processed = i;

    time2_end = std::chrono::steady_clock::now();
    time2_diff = time2_end - time2_start;
    time2_v.push_back((double)time2_diff.count());

    return mean;
}

double process_th(unsigned long long int v_size) {
    time2_start = std::chrono::steady_clock::now();
    double mean = 0, temp = 0, sd = 0, value = 0;
    unsigned long long int i = 0;
    int th_limit = (int)th_discard;
    unsigned long long int N_len = v_size - processed;
    // Filtering + calculate mean
    for (i = processed; i < v_size; ++i)                           // Loop for the length of the array
    {
        // Filter data
        Butty_result.push_back(Butty.filter(recorder_emg1[i]));
        B50_result.push_back(B50.filter(Butty_result[i]));
        B100_result.push_back(B100.filter(B50_result[i]));
        B150_result.push_back(B150.filter(B100_result[i]));
        B200_result.push_back(B200.filter(B150_result[i]));
        B250_result.push_back(B250.filter(B200_result[i]));
        // Saving data
        fileFILTERS << recorder_emg1[i] << "," << Butty_result[i] << "," << B50_result[i] << "," << B100_result[i] << "," << B150_result[i] << "," << B200_result[i] << "," << B250_result[i] << "\n";
        // Calculating mean of retified EMG
        if (B100_result[i] > 0) {
            temp = B250_result[i];
        }
        else {
            temp = -B250_result[i];
        }
        mean = mean + temp;
    }
    mean = mean / N_len;
    if (v_size > th_discard) {
        // Calculate standard deviation
        temp = 0;
        for (i = processed; i < v_size; ++i)
        {
            if (B250_result[i] > 0) {
                temp = B250_result[i];
            }
            else {
                temp = -B250_result[i];
            }
            sd += pow(temp - mean, 2);
        }
        sd = sqrt(sd / N_len);
        // Calculate final threshold value
        value = (mean + sd/2) * N_len;
    }
    else {
        th_discard_nr = v_size;
    }

    if (processed <= 10) {
        fileVALUES << mean << "," << sd << "," << th_discard << "," << v_size << "," << value << "\n";
    }
    else {
        fileVALUES << mean << "," << sd << "," << rec_threshold << "," << v_size << "," << value << "\n";
    }
    // Update amount of processed data
    processed = i;

    time2_end = std::chrono::steady_clock::now();
    time2_diff = time2_end - time2_start;
    time2_v.push_back((double)time2_diff.count());

    return value;
}

void thread_recording()
{
    int iterator = 0;
  //
    double mean = 0, temp_value = 0;
    double static value = 0, value_cnt = 0;
    bool st_wait_jump = false;

    switch (state_process) {
    case st_init:
        recorder_emg1.clear();
        Butty_result.clear();
        B50_result.clear();
        B100_result.clear();
        processed = 0;

        recorder_device.init();
        Sleep(4);
        recorder_device.start();
        rec_status.ready = recorder_device.ready;
        break;

    case st_th:
        recorder_device.record();
        if (recorder_device.data_received && recorder_device.data_start && !fileFILTERS.is_open() && !fileFILTERS.is_open())
            {
                fileFILTERS.open(filter_s);
                fileVALUES.open(th_s);
        }

        sample_nr = recorder_emg1.size();
        if ( sample_nr - processed >= 100) {
            temp_value = process_th(sample_nr);
            if (sample_nr >=th_discard){
              rec_threshold = rec_threshold + temp_value;
            }
            if(processed >= th_nr){
              rec_threshold = rec_threshold / (sample_nr - th_discard_nr);
              //rec_threshold = 2.0;
              std::cout << "Reha Ingest message: threshold = " << rec_threshold << endl;
              rec_status.th = true;
            }
        }
        break;

    case st_wait:
        recorder_device.record();
        sample_nr = recorder_emg1.size();
        if (sample_nr - processed >= 100) {
            mean = process_data_iir(sample_nr);

            st_wait_jump =!rec_status.start && !ROBERT.isMoving && ROBERT.valid_msg && start_train;

            if ((mean >= rec_threshold)&&(th_wait_cnt > th_wait)&& st_wait_jump){
                printf("Reha Ingest message: threshold overpassed.\n", th_wait_cnt);
                rec_status.start = true;
                time1_start = std::chrono::steady_clock::now();
                rec_fl2 = false;
            }else if(mean<rec_threshold){
              th_wait_cnt++;
            }else if((mean>=rec_threshold)&&(th_wait_cnt<th_wait)){
              th_wait_cnt = 0;
              rec_fl2 = false;
            }else if((th_wait_cnt>=th_wait)&&!rec_fl2){
              printf("Reha Ingest message: Now you can push.\n");
              rec_fl2 = true;
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
        th_wait_cnt = 0;
        rec_fl2 = false;
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
