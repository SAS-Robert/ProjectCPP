/** CPP Aplication for the RehaMove3 and RehaIngest devices
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

using namespace std;

// ------------------------- Devices handling --------------------------------
int one_to_escape = 0;
//bool main_start = false;
bool Move3_ready = false;
bool Ingest_ready = false;
RehaMove3_Req_Type Move3_key = Move3_none;
RehaIngest_Req_Type Inge_key = Inge_none;

state_Type state = st_init;
std::vector<float> channel_filter;//Filtered data

// Files handler to store recorded data
ofstream fileRAW;
ofstream fileFILTERS;
//ofstream file3;
// ------------------------- Connection and devices  ------------------------
UDPClient ROBERT;
TCPServer SCREEN;
ofstream msgData;
RehaMove3_type stimulator_device;
RehaIngest_type recorder_device;

// ------------------------------ Associated with functions -------------------
//Tic-toc time
time_t tstart, tend, tstart_ing, tend_ing, tstart_move, tend_move;
double toc_lim = 3;
int task1 = 0;
int task2 = 0;
bool data_start = false;
bool data_printed = false;
string init1("file_raw_");
string init2("file_filtered_");
//string init3("file3_");
string format(".txt");
struct device_to_device{
  bool start = false;
  bool end = false;
  bool treshold = false;
} Ingest_to_Move, MAIN_to_all;
//MAIN_to_all: the main function writes here and all threads read
//Ingest_to_Move: Ingest writes on the variable, Move3 reads it

// ---------------------------- Functions declaration  -----------------------------
// Threads and devices function hearders
static void thread_stimulation();
static void thread_recording();

void tic();
bool toc();
//Communication between threads
void keyboard();
void stimulation_user(RehaMove3_Req_Type code, Smpt_ml_channel_config* current_val, Smpt_ml_channel_config next_val);

// Example threads
void thread_t1();
void thread_t2();

// ------------------------------ Main  -----------------------------

int main() {
  // Flushing input and output buffers
  cout.flush();
  fflush(stdin);
  std::cout << "==================================="<< endl;
  std::cout << "-Starting up devices and communication."<<endl;
  std::cout << "==================================="<< endl;
  // Starting UPD Connection
  std::cout << "Starting connection with ROBERT and Touch Screen\n";
  strcpy(ROBERT.SERVERc, "127.0.0.1");  //This is an address for testing
  ROBERT.display = false;               //Chosen not to show messages during messages exchange
   do{
     ROBERT.start();
   }while(ROBERT.error);

	SCREEN.start();


	SCREEN.end();

  std::thread stimulation(thread_stimulation);
  std::thread recording(thread_recording);
  // std::thread stimulation(thread_t1);
  // std::thread recording(thread_t2);

  //wait for both devices to be ready
  while((!Move3_ready)||(!Ingest_ready)){
    Sleep(1000);
  }
  std::cout << "==================================="<< endl;
  // Using multi threading
  std::cout << "-Using multiple threads.\nPress any key to start process.\nThen press 0 to finish.\n";
  _getch();
  std::cout << "==================================="<< endl;
  std::cout << "---> TCP/UDP controllers:\n-T = Show/hide TCP messages.\n-U = Show/hide UDP messages.\n\n";
  std::cout << "---> RehaMove3 controllers:\n-A = Reduce Ramp.\n-D = Increase ramp.\n-W = Increase current.\n-S = Decrease current.\n\n";
  std::cout << "---> RehaIngest controllers:\n-P = Increase threshold gain.\n-M = Decrease threshold gain.\n\n\n\r";
  //std::cout << "---> '3' Manual release for stimulation in case RehaIngest is not functional.\n";

  MAIN_to_all.start = true;


  while (!MAIN_to_all.end){
    Sleep(200);
    one_to_escape = _kbhit();
      if (one_to_escape != 0){
        keyboard();
      }
  }
  // Waiting for threads to finish
  stimulation.join();
  recording.join();

  ROBERT.end();
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

void keyboard(){
      int ch;
      ch = _getch();
      ch = toupper( ch );
      printf("---> Key pressed: %c <---\n", ch);
      switch(ch){
           case 'T':
             SCREEN.display = !SCREEN.display;
             if (SCREEN.display) {
                 printf("Showing TCP messages and status.\n");
             }
             else {
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
          Move3_key = Move3_ramp_less;
          break;
        case 'D':
            Move3_key = Move3_ramp_more;
        break;
        case 'M':
            // Modify threshold
          Inge_key = Inge_decr;
        break;
        case 'P':
          // Modify threshold
          Inge_key = Inge_incr;
          break;
        case 'S':
          Move3_key = Move3_decr;
        break;
        case 'W':
          Move3_key = Move3_incr;
        break;
        // case '3':
        //   Ingest_to_Move.start = true;
        //   printf("RehaMove3 released manually.\n");
        // break;
        case '0':
          MAIN_to_all.end = true;
        break;
      }
}

//void stimulation_user(RehaMove3_Req_Type code, Smpt_ml_channel_config* current_val, Smpt_ml_channel_config next_val);{
void stimulation_user(RehaMove3_Req_Type code){
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
     case Move3_ramp_more:
       next_val.number_of_points++;
       next_val.ramp++;
       break;
     case Move3_ramp_less:
       next_val.number_of_points--;
       next_val.ramp--;
       break;
     case Move3_decr:
       next_val.points[0].current = next_val.points[0].current-1;
       next_val.points[2].current = next_val.points[2].current+1;
       break;
     case Move3_incr:
       next_val.points[0].current = next_val.points[0].current+1;
       next_val.points[2].current = next_val.points[2].current-1;
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

   if(next_val.points[0].current>60){
     next_val.points[0].current = 60;
     next_val.points[2].current = -60;
   }else if (next_val.points[0].current<10){
     next_val.points[0].current = 10;
     next_val.points[2].current = -10;
   }

   stimulator_device.stim = next_val;
   printf("RehaMove3 message: Stimulation update -> current = %2.1f, ramp points = %d, ramp value = %d\n", stimulator_device.stim.points[0].current, stimulator_device.stim.number_of_points, stimulator_device.stim.ramp);
   //%2.1f is the format to show a float number, 2.1 means 2 units and 1 decimal
   Move3_key = Move3_none;
}

//Example threads: this is only for some code testing.
void thread_t1(){
//std::cout <<"RehaMove3 doing stuff\n";
Move3_ready = true;

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
  Ingest_ready = true;
  while (!MAIN_to_all.end){
    task2++;
    if(task2>=10){
      //std::cout<<"RehaIngest task 2\n";
      task2 = 0;
      Ingest_to_Move.start = true;
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

    stimulator_device.port_name_rm = "COM3";
    stimulator_device.abort = (one_to_escape!=0);
    stimulator_device.init();
    Move3_ready = stimulator_device.ready;

    auto auto_end = std::chrono::steady_clock::now();
    elapsed_seconds = auto_end-auto_start;
    std::cout << "Reha Move3 init time = " << elapsed_seconds.count() << " s\n";

    while(!MAIN_to_all.start){
      Sleep(500);
    }
    //Wait for run from RehaIngest
    std::cout << "Reha Move3 message: Waiting for start.\n";
    while(!Ingest_to_Move.start&&!MAIN_to_all.end){
      Sleep(100);
    }
    if(!MAIN_to_all.end){std::cout << "Reha Move3 message: Stimulating.\n";}

  	while ((!MAIN_to_all.end)&&(!stimulator_device.smpt_end))
  	{
      auto_start = std::chrono::steady_clock::now();

      // New part: setting up values:
      if(Move3_key != Move3_none){
        stimulation_user(Move3_key);
      }
      stimulator_device.update();

      auto_end = std::chrono::steady_clock::now();
      elapsed_seconds = auto_end - auto_start;
      chrono_loop++;
      taverage = taverage + ((float)elapsed_seconds.count());
      Sleep(100);
  	}
  taverage = taverage/((float)chrono_loop);
  std::cout << "Reha Move3 average run time = " << taverage << " s\n";

  auto_start = std::chrono::steady_clock::now();

  stimulator_device.end();

  auto_end = std::chrono::steady_clock::now();
  elapsed_seconds = auto_end - auto_start;
  std::cout << "Reha Move3 finish time = " << elapsed_seconds.count() << " s\n";

} //void thread_ml_stimulation
//================================================
void thread_recording()
{
  // Start process
	// Filters:
	bandStopType* bandStop = bandStop_create();
	std::vector<float> BandStop_result;
	notch50Type* notch50 = notch50_create();
	std::vector<float> notch50_result;
	notch100Type* notch100 = notch100_create();
	std::vector<float> notch100_result;

  // Dummy Variables
  std::chrono::duration<double> elapsed_seconds;
  int chrono_loop = 0;
  float taverage = 0, trecord = 0;
  auto auto_start = std::chrono::steady_clock::now();

  recorder_device.port_name_ri = "COM4";
  recorder_device.init();
  recorder_device.start();

  auto auto_end = std::chrono::steady_clock::now();
  elapsed_seconds = auto_end-auto_start;
  std::cout << "Reha Ingest start-up time = " << elapsed_seconds.count() << " s\n";

  Ingest_ready = recorder_device.ready;
  while(!MAIN_to_all.start){
    Sleep(500);
  }

    int iterator = 0;
		// Starting files:
		char date[15];
		generate_date(date); //get current date/time in format YYMMDD_hhmm
		init1.append(date);
		init1.append(format);
		init2.append(date);
		init2.append(format);
    bool dummy = false;
    bool set_th = false, modify_th = false;
    bool set_value = false, tstart = false;
    float acc = 0;
    float threshold, mean_th, gain_th = 2.5;
    float mean = 0, temp;
    bool files_opened = false;

    while (!MAIN_to_all.end)
    {
      auto_start = std::chrono::steady_clock::now();

      // Collect data
      recorder_device.record();
      auto auto_record = std::chrono::steady_clock::now();

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
        task2=recorder_device.channel_raw.size();
        //if (recorder_device.data_start && recorder_device.data_received) { std::cout << "RegaIngest -> Data size: " << recorder_device.channel_raw.size() << endl; }

        // State machine conditions
        set_th = (toc() && recorder_device.data_start && (state == st_init));
        modify_th = ((state == st_wait)&& (Inge_key!=Inge_none) && (!Ingest_to_Move.start));
        set_value = (task2 >= recorder_device.limit_samples && recorder_device.data_start && (state != st_init) && (!Ingest_to_Move.start));

        // Update Robert Variables
        if(set_th || set_value){
          ROBERT.get();

          if(ROBERT.error){
            ROBERT.end();
            ROBERT.start();
          }else if(ROBERT.display && msgData.is_open()){
            msgData<<ROBERT.buf<<endl;
          }
      }

        if((set_th || set_value)){   // && !ROBERT.error){
          //printf("Filtering data.\n");
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

      //  if((set_th || set_value || modify_th) && !ROBERT.error){
        // State machine process
        switch(state){
          case st_init:
                if(set_th){
                  mean_th = mean;
                  threshold = mean_th*gain_th;
                  std::cout<<"Reha Ingest message: state=init. Threshold settings: resting mean = "<<mean_th<<", gain = "<<gain_th<<", th value = "<<threshold<<endl;
                  state = st_wait;
                  //iterator = 1;
                }
          break;

          case st_wait:
              // Modifying threshold value before starting
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
                std::cout<< "Reha Ingest message: state=wait. Threshold modified to gain = "<<gain_th<<", th value = "<<threshold<<endl;
              }

              // Communication with RehaMove3 thread
              if(set_value){
                Ingest_to_Move.start = (mean> threshold) && (!ROBERT.isMoving); // && (!ROBERT.error);
                std::cout<< "Reha Ingest message: state=wait. Comparing to threshold: "<<mean<<", result: "<<(mean> threshold)<<", ROBERT is moving? "<<ROBERT.isMoving<<endl;
                if(Ingest_to_Move.start){
                  state = st_running;
                  printf("Release sent. state will change to running.\n");
                }
              }
          break;

          case st_running:
              // Wait here for finish?
          break;

        }

      // Others
      if ((Inge_key!=Inge_none) && Ingest_to_Move.start){
        std::cout<<"Reha Ingest message: You cannot change the threshold after starting to stimulate."<<endl;
        Inge_key=Inge_none;
      }

      if (recorder_device.data_received && recorder_device.data_start) {
        auto_end = std::chrono::steady_clock::now();
        elapsed_seconds = auto_record - auto_start;
        trecord = trecord + ((float)elapsed_seconds.count());
        elapsed_seconds = auto_end - auto_start;
        taverage = taverage + ((float)elapsed_seconds.count());
        chrono_loop++;
      }

      Sleep(1);
    }
  trecord = trecord/((float)chrono_loop);
  taverage = taverage/((float)chrono_loop);
  std::cout << "Reha Ingest average: collect data time = "<<trecord<<", total run time = " << taverage << " s\n";

  // Finish everything:
  auto_start = std::chrono::steady_clock::now();

  recorder_device.end();

  auto_end = std::chrono::steady_clock::now();
  elapsed_seconds = auto_end - auto_start;
  std::cout << "Reha Ingest finish time = " << elapsed_seconds.count() << " s\n";

  fileRAW.close();
  fileFILTERS.close();

	bandStop_destroy(bandStop);
	notch50_destroy(notch50);
	notch100_destroy(notch100);

} //void thread_ml_recording
