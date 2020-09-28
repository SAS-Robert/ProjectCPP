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

// Standard windows libraries
/*#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <windows.h>*/
// Standard C++ and C libraries
#include <iostream>
#include <fstream>
#include <conio.h>
#include <stdlib.h>
#include <cstdlib>
#include <iomanip>
#include <complex>
#include <stdio.h>
#include <thread>
#include <ctime>
#include <sstream>
#include <ctype.h>
// User-defined libraries
#include "smpt_ml_client.h"
#include "smpt_dl_client.h"
#include "smpt_definitions.h"
#include "BiQuad.h"
#include "bandStop.h"
#include "notch100.h"
#include "notch50.h"
#include "SAS.h"
#include "iir.h"

using namespace std;

/* EDIT: Change to the virtual com port of your device */
const char* port_name_rm = "COM3";
const char* port_name_ri = "COM4";
int one_to_escape = 0;
//bool main_start = false;
bool Move3_ready = false;
bool Ingest_ready = false;
RehaMove3_Req_Type Move3_key = Move3_none;
std::vector<float> channel_2;			// Original data being stored
std::vector<float> channel_raw;		//Raw data to be filtered while recording
std::vector<float> channel_filter;//Filtered data

// Files handler to store recorded data
ofstream fileRAW;
ofstream fileFILTERS;
//ofstream file3;
ofstream msgData;

//Tic-toc time
time_t tstart, tend;
double toc_lim = 3;

//UPD connection settings
UDPClient ROBERT;
// Example threads
void thread_ml_t1();
void thread_ml_t2();

int task1 = 0;
int task2 = 0;
bool data_start = false;
bool data_printed = false;
string init1("file1_");
string init2("file2_");
//string init3("file3_");
string format(".txt");

// Threads and devices function hearders
static void thread_ml_stimulation(const char* port_name);
static void thread_ml_recording(const char* port_name);
static void handle_dl_packet_global(Smpt_device* const device);
void handleSendLiveDataReceived(Smpt_device* const device, const Smpt_ack& ack);
void tic();
bool toc();
//Communication between threads
void keyboard();
void stimulation_user(RehaMove3_Req_Type code, Smpt_ml_channel_config* current_val, Smpt_ml_channel_config next_val);
Smpt_ml_channel_config stim;

struct device_to_device{
  bool start = false;
  bool end = false;
  bool treshold = false;
} Ingest_to_Move, MAIN_to_all;
//MAIN_to_all: the main function writes here and all threads read
//Ingest_to_Move: Ingest writes on the variable, Move3 reads it

int main()
{
  // Flushing input and output buffers
  cout.flush();
  fflush(stdin);
  std::cout << "==================================="<< endl;
  std::cout << "-Starting up devices and communication."<<endl;
  std::cout << "==================================="<< endl;
  strcpy(ROBERT.SERVERc, "127.0.0.1");  //This is an address for testing
  ROBERT.display = false;               //Chosen not to show messages during messages exchange
  // Starting UPD Connection
  std::cout << "Starting connection\n";
  do{
    ROBERT.start();
  }while(ROBERT.error);

//  std::thread RehaMove3(thread_ml_stimulation, port_name_rm);
    // std::thread RehaIngest(thread_ml_recording, port_name_ri);
    std::thread RehaMove3(thread_ml_t1);
    std::thread RehaIngest(thread_ml_t2);

  //wait for both devices to be ready
  while((!Move3_ready)||(!Ingest_ready)){
    Sleep(1000);
  }
  std::cout << "==================================="<< endl;
  // Using multi threading
  std::cout << "-Using multiple threads.\nPress any key to start process.\nThen press 0 to finish.\n";
  _getch();
  std::cout << "==================================="<< endl;

  std::cout << "RehaMove3 controllers:\n-A = Reduce Ramp.\n-D = Increase ramp.\n-W = Increase current.\n-S = Decrease current.\n";


  MAIN_to_all.start = true;

  //Saving UDP messages
  if(ROBERT.display){
    char date[15];
    generate_date(date); //get current date/time in format YYMMDD_hhmm
    string init1("UDP_");
    init1.append(date);
    init1.append(".txt");
    msgData.open(init1);
  }

  while (!MAIN_to_all.end){
     //while(true){
	   ROBERT.get();
  // If any errors were internally given, the program will re-strart the communication
    if(ROBERT.error){
      ROBERT.end();
      ROBERT.start();
    }else if(ROBERT.display){
      msgData<<ROBERT.buf<<endl;
    }
    Sleep(10);
    one_to_escape = _kbhit();
      if (one_to_escape != 0){
        keyboard();
      }
  }
  // Waiting for threads to finish
  RehaMove3.join();
  RehaIngest.join();

  ROBERT.end();

  std::cout << "==================================="<< endl;
  std::cout << "-Number of iterations:\nThread RehaMove3 = " << task1 <<"\nThread RehaIngest = " << task2 << endl;
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
      switch(ch){
        case 'A':
          Move3_key = Move3_ramp_less;
          break;
        case 'W':
          Move3_key = Move3_incr;
          break;
        case 'S':
          Move3_key = Move3_decr;
          break;
        case 'D':
          Move3_key = Move3_ramp_more;
          break;
        case '0':
          MAIN_to_all.end = true;
        break;
      }
      printf("---> Key pressed: %c <---\n",ch);
}

//void stimulation_user(RehaMove3_Req_Type code, Smpt_ml_channel_config* current_val, Smpt_ml_channel_config next_val);{
void stimulation_user(RehaMove3_Req_Type code){
   Smpt_ml_channel_config next_val;
   next_val = stim;
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

   stim = next_val;
   printf("RehaMove3 message: Stimulation update -> current = %2.1f, ramp points = %d, ramp value = %d\n",stim.points[0].current,stim.number_of_points,stim.ramp);
   //%2.1f is the format to show a float number, 2.1 means 2 units and 1 decimal
   Move3_key = Move3_none;
}
//Example threads: this is only for some code testing.
void thread_ml_t1(){
std::cout <<"RehaMove3 doing stuff\n";
Move3_ready = true;

  while (!MAIN_to_all.end){
    task1++;
    Sleep(1000);
    if(task1>=10){
      std::cout<<"RehaMove3 task 1\n";
      task1 = 0;
    }
  }

}

void tic(){
  tstart = time(0);
  /*
  time_t rawtime;
  struct tm * timeinfo;
  char buffer[10];
  time (&rawtime);
  // Get current time
  timeinfo = localtime (&rawtime);
  strftime(buffer, sizeof(buffer),"%H:%M:%S", timeinfo);
  std::cout<<"Hours="<<buffer[0]<<"-"<<buffer[1]<<", min="<<buffer[3]<<"-"<<buffer[4]<<", sec="<<buffer[6]<<"-"<<buffer[7]<<endl;
  // Convert and save time in numerical values
  int hour = (buffer[0]-'0')*10+buffer[1]-'0';
  int min = (buffer[3]-'0')*10+buffer[4]-'0';
  int sec = (buffer[6]-'0')*10+buffer[7]-'0';
  tic_toc.hr = hour;
  tic_toc.min = min;
  tic_toc.sec = sec;
  tic_toc.en = true;
  std::cout<<"Tic: sec = "<<tic_toc.sec<<", min = "<<tic_toc.min<<", hr = "<<tic_toc.hr<<endl;
  */
}

bool toc(){
  tend = time(0);
  double diff = difftime(tend, tstart);
  bool done = (diff>=toc_lim);
  return done;
  /*
  time_t rawtime;
  struct tm * timeinfo;
  char buffer[10];
  time (&rawtime);
  // Get current time
  timeinfo = localtime (&rawtime);
  strftime(buffer, sizeof(buffer),"%H:%M:%S", timeinfo);
  std::cout<<"Hours="<<buffer[0]<<"-"<<buffer[1]<<", min="<<buffer[3]<<"-"<<buffer[4]<<", sec="<<buffer[6]<<"-"<<buffer[7]<<endl;
  // Convert and save time in numerical values
  int hour = (buffer[0]-'0')*10+buffer[1]-'0';
  int min = (buffer[3]-'0')*10+buffer[4]-'0';
  int sec = (buffer[6]-'0')*10+buffer[7]-'0';
  // Compare if time has passed:
//  bool hr_done = (tic_toc.hr_lim > )
*/
}

void thread_ml_t2(){
  Ingest_ready = true;

  std::cout<<"Sample thread 2 for thesting."<<endl;
  int i = 0;
  tic();
  do{
    i++;
    std::cout<<"toc "<<toc()<<endl;
    Sleep(1000);
  }while(i<=10);


  while (!MAIN_to_all.end){
    task2++;
    Sleep(1000);
    if(task2>=10){
      //std::cout<<"RehaIngest task 2\n";
      task2 = 0;
      Ingest_to_Move.start = true;
    }
  }

  /*
  time_t rawtime;
  struct tm * timeinfo;
  char buffer1[5];
  char buffer2[100];
  //char output;
  time (&rawtime);
  timeinfo = localtime (&rawtime);
  strftime(buffer1, sizeof(buffer1),"%S", timeinfo);
  strftime(buffer2, sizeof(buffer2),"%H:%M:%S", timeinfo);
  std::cout << "Complete time: " << buffer2 << "\nSeconds: " << buffer1 << endl;
  std::cout<<"Hours="<<buffer2[0]<<"-"<<buffer2[1]<<", min="<<buffer2[3]<<"-"<<buffer2[4]<<", sec="<<buffer2[6]<<"-"<<buffer2[7]<<endl;
  //printf("Complete time 2: %s\nSeconds 2: %s", buffer2, buffer1);
  // Conversion to int
//  int hour = ((int)buffer2[]
  int hour = (buffer2[0]-'0')*10+buffer2[1]-'0';
  int min = (buffer2[3]-'0')*10+buffer2[4]-'0';
  int sec = (buffer2[6]-'0')*10+buffer2[7]-'0';
  std::cout<<"Sec int = "<<sec<<", min int = "<<min<<", hr int = "<<hour<<endl;
  */
  /*for(int i=0; i < 15; ++i){
    outStr[i] = buffer[i];
  }*/
}
//================================================
void thread_ml_recording(const char* port_name)
{
  string thread_msg = "Reha Ingest message: ";
  uint8_t packet_number = 0;
  Smpt_device device_ri = { 0 };
  Smpt_ml_init ml_init = { 0 };           /* Struct for ml_init command */
  //Process variables
  bool smpt_port = false, smpt_check = false, smpt_stop = false, smpt_next = false;
  bool smpt_end = false;
	int limit_samples = 10000;
	// Filters:
	bandStopType* bandStop = bandStop_create();
	std::vector<float> BandStop_result;
	notch50Type* notch50 = notch50_create();
	std::vector<float> notch50_result;
	notch100Type* notch100 = notch100_create();
	std::vector<float> notch100_result;

  /* First step */
  std::cout << "Reha Ingest message: Setting communication...\n";
  while(!smpt_next){
    smpt_check = smpt_check_serial_port(port_name_ri);
    smpt_port = smpt_open_serial_port(&device_ri, port_name_ri);
    packet_number = smpt_packet_number_generator_next(&device_ri);
    smpt_stop = smpt_send_dl_stop(&device_ri, packet_number);
    smpt_next = smpt_check && smpt_port && smpt_stop;
    if(!smpt_next){
      std::cout << "Error - Reha Ingest: Device not found. Turn it on and/or check connection.\n";
      smpt_stop = smpt_send_dl_stop(&device_ri, packet_number);
    	smpt_close_serial_port(&device_ri);
      Sleep(5000); // waits for 5 seconds to give you time to regret your life
      std::cout << "Reha Ingest message: Retrying...\n";
    }
    if(one_to_escape != 0){
      smpt_end = true;
      break; }
  }
  /*2nd-3rd-4th steps if the user has not stopped the process*/
  if(!smpt_end){
    /* Clean the input buffer */
    while (smpt_new_packet_received(&device_ri))
    {
        handle_dl_packet_global(&device_ri);
    }
    /* Second step: enable device */
    std::cout << thread_msg << "Enabling and initializing device...\n";
    Smpt_dl_power_module dl_power_module = { 0 };
    fill_dl_power_module(&device_ri, &dl_power_module);
    smpt_send_dl_power_module(&device_ri, &dl_power_module);

    /* wait, because the enabling takes some time (normal up to 4ms) */
    Sleep(10);

    while (smpt_new_packet_received(&device_ri))
    {
        handle_dl_packet_global(&device_ri);
    }
    /* Third step: initialize device*/
    Smpt_dl_init dl_init = { 0 };
    fill_dl_init(&device_ri, &dl_init);
    smpt_send_dl_init(&device_ri, &dl_init);

    Sleep(10);

    while (smpt_new_packet_received(&device_ri))
    {
        handle_dl_packet_global(&device_ri);
    }
    Sleep(10);
    // Every step is needed for ini.
		//---------------------------------------------
    // Waiting here for start from main:
    /* send measurement start cmd*/
    packet_number = smpt_packet_number_generator_next(&device_ri);
    smpt_send_dl_start(&device_ri, packet_number);

    std::cout << thread_msg << "Device ready.\n";
    Ingest_ready = true;
    while(!MAIN_to_all.start){
      Sleep(500);
    }
    //---------------------------------------------
        /* Fourth step*/

    int iterator = 0;
		// Starting files:
		char date[15];
		generate_date(date); //get current date/time in format YYMMDD_hhmm
		init1.append(date);
		init1.append(format);
		init2.append(date);
		init2.append(format);

    //Data loop
    //std::cout << thread_msg << "Preparing.\n";
    std::cout << thread_msg << "RehaMove3 start will be released at iteration number 3000.\n";
    while (!MAIN_to_all.end)
		//while (iterator < 10)
    {
        while (smpt_new_packet_received(&device_ri))
        {
            handle_dl_packet_global(&device_ri);
						if(data_start && !data_printed){
              std::cout<<thread_msg<<"Data available at iteration number "<<iterator<<endl;
							std::cout << thread_msg << "Recording data.\n";
							data_printed = true;
              fileRAW.open(init1);
              fileFILTERS.open(init2);
              task2=1;
						}
        }
        iterator++;
        // Live filtering currently inactive until the issue is fixed
        /*
        task2=channel_raw.size();
        //std::cout << thread_msg <<"Data size: "<<channel_raw.size()<<endl;
				//Live filtering here
				if(task2 >= limit_samples && data_start){
					for (int i = 0; i < channel_raw.size(); ++i)                           // Loop for the length of the array
					{
							bandStop_writeInput(bandStop, channel_raw[i]);              // Write one sample into the filter
							BandStop_result.push_back(bandStop_readOutput(bandStop));        // Read one sample from the filter and store it in the array.
							notch50_writeInput(notch50, BandStop_result[i]);
							notch50_result.push_back(bandStop_readOutput(bandStop));
							notch100_writeInput(notch100, notch50_result[i]);
							notch100_result.push_back(bandStop_readOutput(bandStop));
  				}

          // Saving filtered data from filters
        //  std::cout << thread_msg <<"Data size: "<<BandStop_result.size()<<", "<<notch50_result.size()<<", "<<notch100_result.size()<<endl;
          for(int i = 0; i < BandStop_result.size(); ++i){
              fileFILTERS <<BandStop_result[i]<<","<<notch50_result[i]<< ","<<notch100_result[i]<<"\n";
          }
					//Preparing for next sample set
					task2 = 0;
					channel_raw.clear();
					channel_filter.clear();
					BandStop_result.clear();
					notch50_result.clear();
					notch100_result.clear();

				}
        // Communication with RehaMove3 thread
        if((iterator>=3000) && !Ingest_to_Move.start){
          Ingest_to_Move.start = true;
          std::cout << thread_msg << "stimulation release sent.\n";
        }
        Sleep(1);
        */
    }
  }/*2nd-3rd-4th steps*/
  /*fifth step*/
  std::cout << thread_msg << "Ending process.\n";
  packet_number = smpt_packet_number_generator_next(&device_ri);
  smpt_port = smpt_send_dl_stop(&device_ri, packet_number);
  smpt_port = smpt_close_serial_port(&device_ri);
  smpt_check = smpt_check_serial_port(port_name_ri);

  fileRAW.close();
  fileFILTERS.close();

	// End filters:
	bandStop_destroy(bandStop);
	notch50_destroy(notch50);
	notch100_destroy(notch100);

} //void thread_ml_recording
//================================================
void thread_ml_stimulation(const char* port_name)
{
	Smpt_device device = { 0 };
	Smpt_ml_init ml_init = { 0 };           // Struct for ml_init command *
	Smpt_ml_update ml_update = { 0 };       // Struct for ml_update command
	Smpt_ml_get_current_data ml_get_current_data = { 0 };
	UINT i = 0;
  bool smpt_port = false, smpt_check = false, smpt_next = false;
  bool smpt_end = false, smpt_get = false;
  uint8_t packet;
  int turn_on = 0; //Time if the device gets turned on in the middle of the process
  //-------------------------------------------------------------------------------------
  // Stimulation values
  printf("RehaMove3 message: Stimulation initial values -> current = %2.2f, ramp points = %d, ramp value = %d\n",stim.points[0].current,stim.number_of_points,stim.ramp);
  stim.number_of_points = 3;  /* Set the number of points */
  stim.ramp = 3;              /* Three lower pre-pulses   */
  stim.period = 20;           /* Frequency: 50 Hz */
  /* Set the stimulation pulse */
  stim.points[0].current = 50;
  stim.points[0].time = 200;
  stim.points[1].time = 100;
  stim.points[2].current = -50;
  stim.points[2].time = 200;
  //-------------------------------------------------------------------------------------
  // Start Process
  std::cout << "Reha Move3 message: Initializing device...\n";
  while(!smpt_next){
    smpt_check = smpt_check_serial_port(port_name);
  	smpt_port = smpt_open_serial_port(&device, port_name);
    // Request ID Data
    packet = smpt_packet_number_generator_next(&device);
    smpt_send_get_device_id(&device, packet);

    fill_ml_init(&device, &ml_init);
    smpt_send_ml_init(&device, &ml_init);
    fill_ml_update(&device, &ml_update, stim);
    smpt_send_ml_update(&device, &ml_update);
    fill_ml_get_current_data(&device, &ml_get_current_data);
    // This last command check if it's received all the data requested
    smpt_get = smpt_send_ml_get_current_data(&device, &ml_get_current_data);
    //std::cout << "Before while: get="<<smpt_get << endl;

    // smpt_next = go to next step -> Process running
    smpt_next = smpt_check && smpt_port && smpt_get;
    if(!smpt_next){
      smpt_send_ml_stop(&device, smpt_packet_number_generator_next(&device));
    	smpt_close_serial_port(&device);
      if(smpt_check){
        std::cout << "Error - Reha Move3: Device does not respond. Turn it on or restart it.\n";
        turn_on = 2500;
      }else{
        std::cout << "Error - Reha Move3: Device not found. Check connection.\n";
      }
      Sleep(5000); // waits for 5 seconds to give you time to regret your life
      std::cout << "Reha Move 3 message: Retrying...\n";
      smpt_port = true;
    }
    if(one_to_escape != 0){
      smpt_end = true;
      break; }
  }

  // Run Process
	if(!smpt_end){
    Sleep(turn_on); // wait for it to be properly started
    smpt_port = false;
    fill_ml_init(&device, &ml_init);
    smpt_send_ml_init(&device, &ml_init);
    //---------------------------------------------
    // Waiting here for start from main:
    std::cout << "Reha Move3 message: Device ready.\n";
    Move3_ready = true;
    while(!MAIN_to_all.start){
      Sleep(500);
    }
    //---------------------------------------------
    //Wait for run from RehaIngest
    std::cout << "Reha Move3 message: Waiting for start.\n";
    while(!Ingest_to_Move.start){
      Sleep(100);
    }

    std::cout << "Reha Move3 message: Stimulating.\n";
  	while ((!MAIN_to_all.end)&&(!smpt_end))
  	{
      // New part: setting up values:
      if(Move3_key != Move3_none){
        stimulation_user(Move3_key);
      }
  		fill_ml_update(&device, &ml_update, stim);
  		smpt_send_ml_update(&device, &ml_update);
  		while (i <= 600)
  		{
  			fill_ml_get_current_data(&device, &ml_get_current_data);
  			smpt_send_ml_get_current_data(&device, &ml_get_current_data);
  			Sleep(1);
  			i++;
  		}

  		fill_ml_get_current_data(&device, &ml_get_current_data);
  		smpt_send_ml_get_current_data(&device, &ml_get_current_data);

      task1++;
      Sleep(100);
  	}
  }//if(!smpt_end)

	//Finish process
	std::cout << "Reha Move3 message: Ending process.\n";
//  No need to repeat this, since the connectio was successfully stablished and
// in case something went wrong, it'd get fixed on the next step
    smpt_send_ml_stop(&device, smpt_packet_number_generator_next(&device));
    if(!smpt_port){
      smpt_port = smpt_close_serial_port(&device);
      smpt_check = smpt_check_serial_port(port_name); // it must be available after closing
    }
    if(smpt_check){std::cout << "Reha Move3 message: Process successfully finished.\n";}

} //void thread_ml_stimulation
//================================================
static void handle_dl_packet_global(Smpt_device* const device)
{
    Smpt_ack ack;
    smpt_last_ack(device, &ack);
    Smpt_Cmd cmd = (Smpt_Cmd)ack.command_number;
    switch (cmd)
    {
    case Smpt_Cmd_Dl_Power_Module_Ack:
    {
        handlePowerModuleAckReceived(device, ack);
        break;
    }
    case Smpt_Cmd_Dl_Get_Ack:
    {
        handleGetAckReceived(device, ack);
        break;
    }
    case Smpt_Cmd_Dl_Init_Ack:
    {
        handleInitAckReceived(device, ack);
        break;
    }
    case Smpt_Cmd_Dl_Start_Ack:
    {
        //handleStartAckReceived(device, ack);
        break;
    }
    case Smpt_Cmd_Dl_Stop_Ack:
    {
        handleStopAckReceived(device, ack);
        break;
    }
    case Smpt_Cmd_Dl_Send_Live_Data:
    {
        handleSendLiveDataReceived(device, ack);
        break;
    }
    default:
    {
        break;
    }
    }
}

void handleSendLiveDataReceived(Smpt_device* const device, const Smpt_ack& ack)
{
    Smpt_dl_send_live_data live_data;
    float values[5] = { 0 };
    smpt_get_dl_send_live_data(device, &live_data);
    if(!data_start){ data_start=true;}
    //Data samples
    for (int i = 0; i < live_data.n_channels; i++)
    {
        //std::cout << i << "\n";
        values[i] = live_data.electrode_samples[i].value;
        // Modification
        //std::cout << live_data.electrode_samples[i].value << "\n";
        //std::cout << values[1] << "\n";
        if (i == 1) {
            channel_2.push_back(values[1]);
						channel_raw.push_back(values[1]);
						//channel_filter.push_back(values[1]);
        }
        //Sleep(10);
    }
    //uint32_t timeDiff = live_data.time_offset - m_lastTimeOffset;
    values[4] = (float)live_data.time_offset;

    if (fileRAW.is_open())
    {
        //value[0] : channel 1, bioimpedance measurement
        //value[1] : channel 2, emg 1 measurement
        //value[2] : channel 3, emg 2
        //value[3] : channel 4, analog signal.
        //value[4] : time_ofset between last sample and actual sample
        fileRAW << values[0] << ", " << values[1] << ", " << values[2] << ", " << values[3] << ", " << values[4] << "\n";
    }

}
