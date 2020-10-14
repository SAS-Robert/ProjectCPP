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
//#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <conio.h>
#include <stdlib.h>
//#include <cstdlib>
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
#include "bandStop.h"
#include "notch50.h"
#include "notch100.h"
//#include "iir.h"
using namespace std;

// ------------------------- Devices handling --------------------------------
int one_to_escape = 0;
//bool main_start = false;
bool stim_ready = false;
bool rec_ready = false;
RehaMove3_Req_Type Move3_cmd = Move3_none;
RehaMove3_Req_Type Move3_hmi= Move3_none;
RehaIngest_Req_Type Inge_key = Inge_none;
RehaIngest_Req_Type Inge_hmi = Inge_none;

state_Type state_process = st_init;
std::vector<float> channel_filter;//Filtered data

// Files handler to store recorded data
ofstream fileRAW;
ofstream fileFILTERS;
//ofstream file3;
// ------------------------- Connection and devices  ------------------------
UDPClient ROBERT;
int udp_cnt = 0 ;
TCPServer SCREEN;
ofstream msgData;
RehaMove3_type stimulator_device;
RehaIngest_type recorder_device;
ROB_Type screen_status;
float gain_th = 2.5;
int ROB_rep = 0;
ROB_Type wololo;
int TCP_rep = 3;
// ------------------------------ Associated with functions -------------------
//Tic-toc time
time_t tstart, tend, tstart_ing, tend_ing, tstart_move, tend_move;
double toc_lim = 3;
int task1 = 0;
int task2 = 0;
bool data_start = false;
bool data_printed = false;
// Measuring time:
//-time 1 = from when threshold has been passed until the stimulator starts (FES_cnt)
//-time 2 = filtering perfomance (EMG_tic)
//-time 3 = recorder perfomance: collect samples + process them + filter them + get Robert status
ofstream time1_f;
ofstream time2_f;
ofstream time3_f;
std::vector<double> time1_v;
std::vector<double> time2_v;
std::vector<double> time3_v;
std::chrono::duration<double> time1_diff;
std::chrono::duration<double> time2_diff;
std::chrono::duration<double> time3_diff;
auto time1_start = std::chrono::steady_clock::now();
auto time1_end = std::chrono::steady_clock::now();
auto time2_start = std::chrono::steady_clock::now();
auto time2_end = std::chrono::steady_clock::now();
auto time3_start = std::chrono::steady_clock::now();
auto time3_end = std::chrono::steady_clock::now();
// files
char date[15];
string file_dir;
string init1("file_raw_");
string init2("file_filtered_");

string time1_s("time1_");
string time2_s("time2_");
string time3_s("time3_");

string format(".txt");
struct device_to_device{
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
void create_file();
bool dummy_udp = false, dummy_tcp = false;
bool dummy_valid = false;;
// ------------------------------ Main  -----------------------------

int main(int argc, char *argv[]) {
  bool decode_successful;

  // initialize files names
  get_dir(argc, argv, file_dir);
  generate_date(date); //get current date/time in format YYMMDD_hhmm
  init1.insert(0,file_dir);
  init2.insert(0,file_dir);
  time1_s.insert(0,file_dir);
  time2_s.insert(0,file_dir);
  time3_s.insert(0,file_dir);
  init1.append(date);
  init1.append(format);
  init2.append(date);
  init2.append(format);
  time1_s.append(date);
  time1_s.append(format);
  time2_s.append(date);
  time2_s.append(format);
  time3_s.append(date);
  time3_s.append(format);

  // other dummy stuff
  dummy_valid = false;
  printf("Do you wanna use the UDP connection to Robert's controller? [Y/N]: ");
  int ch;
  while (!dummy_valid){
    ch = _getch();
    ch = toupper( ch );
    if (ch == 'Y'){
      dummy_udp = true;
    }
    dummy_valid = (ch == 'Y') || (ch == 'N');
    printf("%c ",ch);
  }
  ROBERT.display = dummy_udp;
  dummy_valid = false;
  printf("\nDo you wanna use the TCP connection to the Touch Panel? [Y/N]: ");
  while (!dummy_valid){
    ch = _getch();
    ch = toupper( ch );
    if (ch == 'Y'){
      dummy_tcp = true;
    }
    dummy_valid = (ch == 'Y') || (ch == 'N');
    printf("%c ",ch);
  }
  //printf("dummy tcp = %d\n", dummy_tcp);
  // Flushing input and output buffers
  cout.flush();
  fflush(stdin);
  std::cout << "\n==================================="<< endl;
  std::cout << " Communication start up "<<endl;
  std::cout << "==================================="<< endl;

  if(dummy_udp){
    // Starting UPD Connection
    std::cout << "Starting connection with ROBERT and Touch Screen\n";
    strcpy(ROBERT.SERVERc, "127.0.0.1");  //This is an address for testing
//    ROBERT.display = false;               //Chosen not to show messages during messages exchange
     do{
       ROBERT.start();
     }while(ROBERT.error);
     // Send first message
     strcpy(ROBERT.message, "0;STATUS;");
     ROBERT.get();

  }else{
    printf("UDP Connection skipped\n\n");
  }

  if(dummy_tcp){
    SCREEN.start();
    SCREEN.waiting();
  }else{
    printf("TCP Connection skipped. Using keyboard instead.\n\n");
  }

  std::cout << "==================================="<< endl;
  // Using multi threading
  std::cout << "Press any key to start process.\nThen press 0 to finish.\n";
  // _getch();
  std::cout << "==================================="<< endl;
  if(dummy_udp && dummy_tcp){
    std::cout << "---> TCP/UDP controllers:\n-T = Show/hide TCP messages.\n-U = Show/hide UDP messages.\n\n";
  } else if (dummy_udp){
    std::cout << "---> UDP controllers:\n-U = Show/hide UDP messages.\n\n";
  } else if (dummy_tcp){
    std::cout << "---> TCP controllers:\n-T = Show/hide TCP messages.\n\n";
  }
  if (!dummy_tcp){
    std::cout << "---> RehaMove3 controllers:\n-A = Reduce Ramp.\n-D = Increase ramp.\n-W = Increase current.\n-S = Decrease current.\n\n";
    std::cout << "---> RehaIngest controllers:\n-P = Increase threshold gain.\n-M = Decrease threshold gain.\n\n\n\r";
  }

  _getch();

  std::thread stimulation(thread_stimulation);
  std::thread recording(thread_recording);
  // std::thread stimulation(thread_t1);
  // std::thread recording(thread_t2);

  //std::cout << "---> '3' Manual release for stimulation in case RehaIngest is not functional.\n";

  MAIN_to_all.start = true;
  SCREEN.display = true;
  printf("State machine = initialization\n");

  while (!MAIN_to_all.end){
    Sleep(1);
    one_to_escape = _kbhit();
      if (one_to_escape != 0 ){
        keyboard();
      }

      // TCP Stuff
      if (!SCREEN.finish && dummy_tcp){
        //printf(" -- checking... -- \n");
        SCREEN.check();
        // Receive
        if(SCREEN.new_message){
          decode_successful = TCP_decode(SCREEN.recvbuf, Move3_hmi, Inge_key,wololo,TCP_rep);
          if(decode_successful){
            // Only update values  if the message was not "ENDTCP"
            if(!SCREEN.finish){
              //stimulation_set(Move3_hmi);
              // Dummy for the gain
              switch(Inge_key){
                case Inge_decr:
                    gain_th = gain_th + 0.1;
                    break;
                case Inge_incr:
                    gain_th = gain_th-0.1;
                    break;
              }
              if(SCREEN.display){ printf("TCP received: move_key = %d, ingest_key = %d, satus = %c, rep_nr = %d \n ", Move3_hmi, Inge_key, wololo, TCP_rep);}
            }
            // Send
            memset(SCREEN.senbuf, '\0', 512);
            sprintf(SCREEN.senbuf, "SAS;%2.1f;%1.1f;%1.1f;", stimulator_device.stim.points[0].current, stimulator_device.stim.ramp, gain_th);
            SCREEN.stream();
          }else if (!decode_successful && SCREEN.display){
            printf("TCP received message not valid.\n");
          }
        }
      }
      // TCP Stuff

      // Biiiiiiiiiiig global State machine
      switch(state_process){
        case st_init:
          if(stim_status.ready && rec_status.ready){
            printf("State machine change: initialization -> set threshold\n");
            state_process = st_th;  // Go to set threshold
          }
        break;

        case st_th:
          // Some condition here to check the threshold has been set
          if(rec_status.th){
            printf("State machine change: set threshold -> wait for release\n");
            state_process = st_wait;
          }
        break;

        case st_wait:
          if(rec_status.start){
            state_process = st_running;
            printf("State machine change: wait for release -> stimulating\n");
          }
        break;

        case st_running:
          if(ROBERT.Reached && ROBERT.valid_msg){
            // Increase repetitions
            ROB_rep++;
            std::cout<<"\n---> Repetition nr."<<ROB_rep<<" completed of "<< TCP_rep <<" <--- "<<endl;
            if(ROB_rep<TCP_rep){
              std::cout<<"Waiting for robot to return to start position."<<endl;
            }
//            printf("State machine change: stimulating -> stop\n");
            state_process = st_stop;
            }
        break;

        case st_stop:
        // Check nr of repetitions and devices
          if(ROB_rep<TCP_rep && stim_status.ready && rec_status.ready && !ROBERT.Reached){// && !ROBERT.isMoving){
            std::cout<<"Starting next repetition"<<endl;
            state_process = st_wait;
          }else if(ROB_rep>=TCP_rep && stim_status.ready && rec_status.ready){
            std::cout << "Exercise finished. Waiting for program to end." << endl;
            state_process = st_end;
          }
        break;
      }// State machine


  }
  // Waiting for threads to finish
  stimulation.join();
  recording.join();

  if(dummy_udp){ROBERT.end();}
  if(dummy_tcp){SCREEN.end();}

  std::cout << "\n--------------------------\nHej Hej!\n--------------------------\n";
  // Flushing input and output buffers
  cout.flush();
  fflush(stdin);
	return 0;
}

// ---------------------------- Functions definition  --------------------------
void tic(){
  tstart = time(0);
}

bool toc(){
  tend = time(0);
  double diff = difftime(tend, tstart);
  bool done = (diff>=toc_lim);
  return done;
}

bool TCP_decode(char* message, RehaMove3_Req_Type& stimulator, RehaIngest_Req_Type& recorder, ROB_Type& status, int& rep){
  int length = strlen(message);
  char start_msg[7] = "SCREEN";
  char finish_msg[7] ="ENDTCP";
  char delim_msg = 59;  // ASCII Semicolon ; code
  char end_msg = 59;
  bool valid_msg = false;

  int comp = 0, pos = 0, pos_cont = 0, j = 0;
  bool comp_b = false, valid1 = false, valid2 = false, valid3 = false, charac = false, delimt = false;
  bool check_end = false;
  //Looking for beginning of the message:
  while ((j < length) && !valid_msg) {
    // Beginning of the string found
    if (((message[j] == start_msg[0]) || (message[j] == finish_msg[0]))&& (pos_cont <= 1)) { //start_msg[0]){
      comp_b = true;
      pos_cont = 0;
      comp = 0;
      pos = j;
      if(message[j] == finish_msg[0]){
        check_end = true;
      }
    }
    // If the beginning is found, start storing the "SCREEN" identifier
    if ((comp_b) && (pos_cont < 7)) {
      if(!check_end){
        comp = comp + (message[j] - start_msg[j - pos]);
      }else{
        comp = comp + (message[j] - finish_msg[j - pos]);
      }
      pos_cont++;
    }
    // After 6 iteractions, verify that the identifier was found, then compare fields
    if ((pos_cont >= 6) && (comp == 0)) {
      if(!check_end){
      // Checking delimiters
      delimt = (message[pos + 6] == delim_msg) && (message[pos + 8] == delim_msg) && (message[pos + 10] == delim_msg);
      // Checking commands
      valid1 = (message[pos + 7] >= '0') || (message[pos + 7] <= '9');
      valid2 = (message[pos + 9] >= '0') || (message[pos + 9] <= '9');
      // Checking rep value
      charac = (message[pos + 11] >= 'A') && (message[pos + 11] <= 'Z');
      valid_msg = (comp == 0) && delimt && valid1 && valid2 && charac;
      //printf("RESULT: delimiters = %d, valid cmd = %d %d, valid rep = %d, MSG = %d. ", delimt, valid1, valid2, charac, valid_msg);
      }else{
      valid_msg = (comp == 0);
      }
    }
    else if (pos_cont >= 6) {
      comp_b = false; // If not found_set up back
      check_end = false;
    }
    j++;
  }

  if(!check_end){
  //Convert from char to int values
  int move_value = message[pos + 7] - '0';
  int ingest_value = message[pos + 9] - '0';

  int get_status = message[pos + 11];
  unsigned long long int rep_nr = 0;

  if (get_status == 'R'){ // Only necessary to decode the rep number if R
    int rep_unit100 = message[pos + 12] - '0';
    int rep_unit10 = message[pos + 13] - '0';

    if (message[pos + 13] == delim_msg){
      rep_nr = rep_unit100;
    } else if (message[pos + 14] == delim_msg){
      rep_nr = rep_unit100*10 + rep_unit10;
    }else{
      int rep_unit1 = message[pos + 14] - '0';
      rep_nr = rep_unit100*100 + rep_unit10*10 + rep_unit1;
    }
    valid_msg = valid_msg && (rep_nr>=0) && (rep_nr<=999); // Check values
    //printf("funct result: move_key = %d, ingest_key = %d, rep_nr = %d.\n ", move_value, ingest_value, rep_nr);
  }

    //Outputs
    if (valid_msg){
      stimulator = (RehaMove3_Req_Type) move_value;
      recorder = (RehaIngest_Req_Type) ingest_value;
      //status[0] = get_status;
      status = (ROB_Type)get_status;
      if(get_status == 'R'){
        rep = rep_nr;
      }
    }
  } // !check_end
  else {
    if (valid_msg){
       SCREEN.finish = true;
     }
  }

  return valid_msg;
}

void keyboard(){
      int ch;
      ch = _getch();
      ch = toupper( ch );
      printf("---> Key pressed: %c <---\n", ch);
      switch(ch){
           case 'T':
             SCREEN.display = !SCREEN.display;
             if (SCREEN.display && dummy_tcp) {
                 printf("Showing TCP messages and status.\n");
             }
             else if (dummy_tcp){
                 printf("Not showing TCP messages and status.\n");
             }
           break;
          case 'U':
            ROBERT.display = !ROBERT.display;
            if (ROBERT.display && dummy_udp) {
                printf("Showing UDP messages and status.\n");
            }
            else if (dummy_udp) {
                printf("Not showing UDP messages and status.\n");
            }
            break;
        case 'A':
          if (!dummy_tcp){Move3_hmi = Move3_ramp_less;}
          break;
        case 'D':
            if (!dummy_tcp){Move3_hmi = Move3_ramp_more;}
        break;
        case 'M':
            // Modify threshold
          if (!dummy_tcp){Inge_key = Inge_decr;}
        break;
        case 'P':
          // Modify threshold
          if (!dummy_tcp){Inge_key = Inge_incr;}
          break;
        case 'S':
          if (!dummy_tcp){Move3_hmi = Move3_decr;}
        break;
        case 'W':
          if (!dummy_tcp){Move3_hmi = Move3_incr;}
        break;
        case '0':
          MAIN_to_all.end = true;
        break;
      }
}

//void stimulation_set(RehaMove3_Req_Type code, Smpt_ml_channel_config* current_val, Smpt_ml_channel_config next_val);{
void stimulation_set(RehaMove3_Req_Type& code){
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


   switch(code){
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
       next_val.points[0].current = next_val.points[0].current-0.1;
       next_val.points[2].current = next_val.points[2].current+0.1;
       break;
     case Move3_incr:
       next_val.points[0].current = next_val.points[0].current+0.1;
       next_val.points[2].current = next_val.points[2].current-0.1;
       break;
     // Process profiles
     case Move3_stop:
       next_val.points[0].current = 0;
       next_val.points[2].current = 0;
       break;
     case Move3_start:
       next_val.points[0].current = 5;
       next_val.points[2].current = 5;
       next_val.ramp = 3;
       next_val.number_of_points = 3;
       break;
   }
   // Checking max and min possible values:
   if(next_val.number_of_points>5){
     next_val.number_of_points = 5;
   }else if(next_val.number_of_points<2){
     next_val.number_of_points = 2;
   }

   if(next_val.ramp>5){
     next_val.ramp = 5;
   }else if(next_val.ramp<2){
     next_val.ramp = 2;
   }

   if(next_val.points[0].current>=10){
     next_val.points[0].current = 10;
     next_val.points[2].current = -10;
   }else if (next_val.points[0].current<=0){
     next_val.points[0].current = 0;
     next_val.points[2].current = 0;
   }

   stimulator_device.stim = next_val;
   if((code != Move3_stop) && (code != Move3_start)){
   printf("RehaMove3 message: Stimulation update -> current = %2.1f, ramp points = %d, ramp value = %d\n", stimulator_device.stim.points[0].current, stimulator_device.stim.number_of_points, stimulator_device.stim.ramp);
   //%2.1f is the format to show a float number, 2.1 means 2 units and 1 decimal
 }else if (code == Move3_stop){
   printf("RehaMove3 message: Stimulation stopped\n");
 }else{
   printf("RehaMove3 message: Stimulation values set to initial values (but not sent to device, chiiiiiiiill )\n");
 }
   code = Move3_none;
   Move3_cmd = Move3_none;
   Move3_hmi = Move3_none;
}

//Example threads: this is only for some code testing.
void thread_t1(){
//std::cout <<"RehaMove3 doing stuff\n";
stim_status.ready = true;

  while (!MAIN_to_all.end){
    task1++;
    Sleep(1000);
    if(task1>=10){
      //std::cout<<"RehaMove3 task 1\n";
      task1 = 0;
    }
  }

}
void thread_t2(){
  std::cout<<"Reha Ingest: Sample thread for testing."<<endl;
  rec_status.ready = true;
  while (!MAIN_to_all.end){
    task2++;

    if(task2>=10){
      //std::cout<<"RehaIngest task 2\n";
      if(state_process == st_wait){
        Sleep(3000);
        printf("Thread released.\n");
        rec_status.start = true;

      }else{
        printf("Thread release deleted.\n");
        rec_status.start = false;
        task2 = 0;
      }
      Sleep(1);
    }
    else{
      Sleep(1000);
    }
  }
}

//================================================
void thread_stimulation()
{
    // Dummy Variables
    std::chrono::duration<double> elapsed_seconds;
    int chrono_loop = 0;
    float taverage = 0;
    auto auto_start = std::chrono::steady_clock::now();
    bool print_once = false;
    // initialization
    stimulator_device.port_name_rm = "COM3";
    stimulator_device.abort = (one_to_escape!=0);
    stimulator_device.init();
    stim_status.ready = stimulator_device.ready;

    auto auto_end = std::chrono::steady_clock::now();
    elapsed_seconds = auto_end-auto_start;
    std::cout << "Reha Move3 init time = " << elapsed_seconds.count() << " s\n";

    std::cout << "Reha Move3 message: Waiting for start.\n";

    while((state_process!=st_end) && !MAIN_to_all.end){ // process
      switch(state_process){
          case st_wait:
            // do stuff
            print_once = false;
            if(Move3_hmi != Move3_none){
              stimulation_set(Move3_hmi);
              //Move3_hmi = Move3_none;
            }
          break;

          case st_running:
            // do stuff
            if(Move3_hmi != Move3_none){
              stimulation_set(Move3_hmi);
            }
            stimulator_device.update();
            // things to do only once
            if(!print_once){
              std::cout << "Reha Move3 message: Stimulating.\n";
              time1_end = std::chrono::steady_clock::now();
              time1_diff = time1_end - time1_start;
              time1_v.push_back((double)time1_diff.count());
              print_once = true;
              stim_status.ready = false;
            }
          break;

          case st_stop:     // Stop process and restore initial values
            if(print_once){
              Move3_cmd = Move3_stop;
              stimulation_set(Move3_cmd);
              stimulator_device.update();

              Move3_cmd = Move3_start;
              stimulation_set(Move3_cmd);

              stim_status.ready = true;
              print_once = false;
            }
      } // switch case

    } // while process

    auto_start = std::chrono::steady_clock::now();
    stimulator_device.end();
    auto_end = std::chrono::steady_clock::now();
    elapsed_seconds = auto_end-auto_start;
    std::cout << "Reha Move3 finish time = " << elapsed_seconds.count() << " s\n";

    // Safe here the time samples on a file
    time1_f.open(time1_s);
    if(time1_f.is_open()){
    for(int k = 0; k < time1_v.size(); k++){
      time1_f << time1_v[k] <<" ; "<< endl;
    }
    printf("RehaMove3: time measurement t1 saved in file.\n");
  }else{
    printf("RehaMove3: Data t1 could not be saved.\n");
  }
    time1_f.close();
} //void thread_ml_stimulation
//================================================
void thread_recording()
{
	// Filters:
	bandStopType* bandStop = bandStop_create();
	std::vector<float> BandStop_result;
	notch50Type* notch50 = notch50_create();
	std::vector<float> notch50_result;
	notch100Type* notch100 = notch100_create();
	std::vector<float> notch100_result;

  // Dummy Variables
  int chrono_loop = 0;
  float taverage = 0, trecord = 0;

  recorder_device.port_name_ri = "COM4";
  recorder_device.init();
  recorder_device.start();
  rec_status.ready = recorder_device.ready;
  std::cout << "Reha Ingest message: Waiting for start.\n";

    int iterator = 0;
    int sample_nr = 0;
    bool print_once = false;
    bool set_th = false, modify_th = false;
    bool set_value = false, tstart = false;
    float acc = 0;
    float threshold, mean_th;
    float mean = 0, temp;
    bool files_opened = false;

    //wait for start
    while (state_process != st_th){
      Sleep(1);
    }
    //printf("State machine is now threshold in thread?\n");

    while((state_process!=st_end) && !MAIN_to_all.end){
      // Collect data
      recorder_device.record();

      if (recorder_device.data_received && recorder_device.data_start && !files_opened) {
        fileRAW.open(init1);
        fileFILTERS.open(init2);
        files_opened = true;
        //std::cout << "Files opened"<< endl;
        tic();
      }
      if (recorder_device.data_received && recorder_device.data_start) {
        fileRAW << recorder_device.data << "\n";
      }

        iterator++;
        sample_nr=recorder_device.channel_raw.size();
        //if (recorder_device.data_start && recorder_device.data_received) { std::cout << "RegaIngest -> Data size: " << recorder_device.channel_raw.size() << endl; }

        // State machine conditions
        set_th = (toc() && recorder_device.data_start && !rec_status.th);

        modify_th = ((state_process == st_wait)&& (Inge_key!=Inge_none) && (!rec_status.start));
        set_value = (sample_nr >= recorder_device.limit_samples && recorder_device.data_start && rec_status.th && (!rec_status.start));

        // Update Robert Variables
        if(set_th || set_value){
          if(dummy_udp){ // if the connection is active
            udp_cnt++;
            sprintf(ROBERT.message, "%d;STATUS;", udp_cnt);
            ROBERT.get();
          }

      }

        if(set_th || set_value){
          //printf("FD ");
          acc = 0;
          int N_len;
          N_len = recorder_device.channel_raw.size();
          for (int i = 0; i < N_len; ++i)                           // Loop for the length of the array
          {
              // Filter data
              bandStop_writeInput(bandStop, recorder_device.channel_raw[i]);              // Write one sample into the filter
              BandStop_result.push_back(bandStop_readOutput(bandStop));        // Read one sample from the filter and store it in the array.
              notch50_writeInput(notch50, BandStop_result[i]);
              notch50_result.push_back(bandStop_readOutput(bandStop));
              notch100_writeInput(notch100, notch50_result[i]);
              notch100_result.push_back(bandStop_readOutput(bandStop));
              // Saving data
              fileFILTERS <<recorder_device.channel_raw[i] << "," << BandStop_result[i]<<","<<notch50_result[i]<< ","<<notch100_result[i]<<"\n";
              // Calculating mean of retified EMG
              if(notch100_result[i]>0){
                temp=notch100_result[i];
              }else{
                temp=-notch100_result[i];
              }
              mean = mean + (temp/N_len);
          }
          //Preparing for next sample set
          task2 = 0;
          recorder_device.channel_raw.clear();
          channel_filter.clear();
          BandStop_result.clear();
          notch50_result.clear();
          notch100_result.clear();
        }

        // State machine process
        switch (state_process) {
          case st_th:
                if(set_th && !rec_status.th){
                  mean_th = mean;
                  threshold = mean_th*gain_th;
                  std::cout<<"Reha Ingest message: Threshold set to -> resting mean = "<<mean_th<<", gain = "<<gain_th<<", th value = "<<threshold<<endl;
                  rec_status.th = true;
                }
          break;

          case st_wait:
              // Modifying threshold value before starting to stimulate
                if(modify_th){
                  switch(Inge_key){
                    case Inge_decr:
                        gain_th = gain_th + 0.1;
                        break;
                    case Inge_incr:
                        gain_th = gain_th-0.1;
                        break;
                  }
                  Inge_key = Inge_none;
                  threshold = mean_th*gain_th;
                  std::cout<< "Reha Ingest message: Threshold modified to -> gain = "<<gain_th<<", th value = "<<threshold<<endl;
                }

                // Communication with RehaMove3 thread and main
                if(set_value){
                  rec_status.start = (mean> threshold) && (!ROBERT.isMoving); // && (!ROBERT.error);
                  std::cout<< "Reha Ingest message: comparing -> threshold = "<<threshold<<" <> measured value = "<<mean<<endl;
                  if(!print_once){
                    time1_start = std::chrono::steady_clock::now();
                    printf("Release sent. state will change to running.\n");
                    print_once = !print_once;
                  }
                }
          break;

          case st_running:
              rec_status.ready = false;
              rec_status.start = false;
              // Wait here for RehaMove to Finish
          break;

         case st_stop:
          // Nothing to do. Ready for next repetition
           rec_status.ready = true;
         break;
        }

      // Others
      if ((Inge_key!=Inge_none) && rec_status.start){
        std::cout<<"Reha Ingest message: You cannot change the threshold after starting to stimulate."<<endl;
        Inge_key=Inge_none;
      }


      Sleep(1);
    }


  recorder_device.end();


  fileRAW.close();
  fileFILTERS.close();

	bandStop_destroy(bandStop);
	notch50_destroy(notch50);
	notch100_destroy(notch100);

} //void thread_ml_recording
