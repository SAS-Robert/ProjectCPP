/** CPP Aplication for the RehaMove3 and RehaIngest devices
 *
 * author: Carolina Gomez <cgs@lifescience-robotics.com>
 *
 * Please notice that the libraries are linked to the directories address on
 * my own computer. Other users should change these propierties, so it can
 * compile the project.
 *
 * This application runs the following process:
 *
 * UPD Connection with ROBERT
 * Serial connection and handeling the RehaMove3 device.
 * Serial connection and handeling the RehaIngest device.
 *
*/

// Standard C++ and C libraries
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
// User-defined libraries
#include "smpt_ml_client.h"
#include "smpt_dl_client.h"
#include "smpt_definitions.h"
//#include "BiQuad.h"
#include "bandStop.h"
#include "notch100.h"
#include "notch50.h"
#include "SAS.h"
#include "iir.h"

using namespace std;

// ------------------------------ Devices --------------------------------------
int one_to_escape = 0;
//bool main_start = false;
bool Move3_ready = false;
bool Ingest_ready = false;
RehaMove3_Req_Type Move3_key = Move3_none;
RehaIngest_Req_Type Inge_key = Inge_none;
state_Type state = st_init;


std::vector<float> channel_2;			// Original data being stored
//std::vector<float> channel_raw;		//Raw data to be filtered while recording
std::vector<float> channel_filter;//Filtered data

// Files handler to store recorded data
ofstream fileRAW;
ofstream fileFILTERS;
//ofstream file3;
// ------------------------- Connection and devices  ------------------------
UDPClient ROBERT;
TCPClient SCREEN;
ofstream msgData;
RehaMove3_type stimulator_device;
RehaIngest_type recorder_device;

// ------------------------------ Associated with functions -------------------
//Tic-toc time
time_t tstart, tend, tstart_ing, tend_ing;
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
// ------------------------------ Functions  -----------------------------
// Example threads
void thread_ml_t1();
void thread_ml_t2();
// Threads and devices function hearders
static void thread_stimulation();
static void thread_recording();

void tic();
bool toc();
//Communication between threads
void keyboard();
void stimulation_user(RehaMove3_Req_Type code, Smpt_ml_channel_config* current_val, Smpt_ml_channel_config next_val);

int main()
{
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

  //strcpy(SCREEN.SERVERc, "127.0.0.1");  //This is an address for testing
  //SCREEN.display = false;               //Chosen not to show messages during messages exchange
 //   do{
 //     SCREEN.start();
 //   }while(SCREEN.error);

  //Saving UDP or TCP messages
  // if(ROBERT.display){
  //   char date[15];
  //   generate_date(date); //get current date/time in format YYMMDD_hhmm
  //   string init1("UDP_");
  //   init1.append(date);
  //   init1.append(".txt");
  //   msgData.open(init1);
  // }

    std::thread stimulator(thread_stimulation);
     std::thread recorder(thread_recording);
    // std::thread RehaMove3(thread_ml_t1);
    //std::thread RehaIngest(thread_ml_t2);

  //wait for both devices to be ready
  while((!Move3_ready)||(!Ingest_ready)){
    Sleep(1000);
  }
  std::cout << "==================================="<< endl;
  // Using multi threading
  std::cout << "-Using multiple threads.\nPress any key to start process.\nThen press 0 to finish.\n";
  _getch();
  std::cout << "==================================="<< endl;
  std::cout << "---> TCP/UDP controllers:\n-T = Show/hide TCP messages.\n-U = Show/hide UDP messages.\n";
  std::cout << "---> RehaMove3 controllers:\n-A = Reduce Ramp.\n-D = Increase ramp.\n-W = Increase current.\n-S = Decrease current.\n";
  std::cout << "\n---> RehaIngest controllers:\n-P = Increase threshold gain.\n-M = Decrease threshold gain.\n\n\n\r";
  std::cout << "---> '3' Manual release for stimulation in case RehaIngest is not functional.\n";

  MAIN_to_all.start = true;


  while (!MAIN_to_all.end){
    Sleep(200);
    one_to_escape = _kbhit();
      if (one_to_escape != 0){
        keyboard();
      }
  }
  // Waiting for threads to finish
  stimulator.join();
  recorder.join();

  ROBERT.end();
  SCREEN.end();

  //std::cout << "==================================="<< endl;
  //std::cout << "-Number of iterations:\nThread RehaMove3 = " << task1 <<"\nThread RehaIngest = " << task2 << endl;
  std::cout << "\n--------------------------\nHej Hej!\n--------------------------\n";
  // Flushing input and output buffers
  cout.flush();
  fflush(stdin);

return 0;

}   // Uncomment to enable original code.
//================================================
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
        case '3':
          Ingest_to_Move.start = true;
          printf("RehaMove3 released manually.\n");
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
void tic(){
  tstart = time(0);
}

bool toc(){
  tend = time(0);
  double diff = difftime(tend, tstart);
  bool done = (diff>=toc_lim);
  return done;
}

void thread_ml_t1(){
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
void thread_ml_t2(){
  std::cout<<"Reha Ingest: Sample thread for testing."<<endl;

  Ingest_ready = true;
  while (!MAIN_to_all.end){
    task2++;
    if (MAIN_to_all.start) {
        SCREEN.get();
        Sleep(3000);
    }
    if(task2>=10){
      //std::cout<<"RehaIngest task 2\n";
      task2 = 0;
      Ingest_to_Move.start = true;
    }
  }
}
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

  recorder_device.init();
    recorder_device.start();
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
      // Collect data
      recorder_device.record();
      if (recorder_device.data_received && recorder_device.data_start && !files_opened) {
        fileRAW.open(init1);
        fileFILTERS.open(init2);
        files_opened = true;
        std::cout << "Files opened"<< endl;
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

        Sleep(1);
    }

  // Finish everything:
  recorder_device.end();

  fileRAW.close();
  fileFILTERS.close();

	bandStop_destroy(bandStop);
	notch50_destroy(notch50);
	notch100_destroy(notch100);

} //void thread_ml_recording
//================================================
void thread_stimulation()
{
    stimulator_device.abort = (one_to_escape!=0);
    stimulator_device.init();
    Move3_ready = stimulator_device.ready;
    while(!MAIN_to_all.start){
      Sleep(500);
    }
    //Wait for run from RehaIngest
    std::cout << "Reha Move3 message: Waiting for start.\n";
    while(!Ingest_to_Move.start){
      Sleep(100);
    }
    std::cout << "Reha Move3 message: Stimulating.\n";
  	while ((!MAIN_to_all.end)&&(!stimulator_device.smpt_end))
  	{
      // New part: setting up values:
      if(Move3_key != Move3_none){
        stimulation_user(Move3_key);
      }
      stimulator_device.update();

      Sleep(100);
  	}
  stimulator_device.end();

} //void thread_ml_stimulation
