#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
// Standard C++ and C libraries
#include <conio.h>
#include <stdlib.h>
#include <cstdlib>
#include <iomanip>
#include <complex>
#include <stdio.h>
#include <ctime>
#include "SASLIB.h"
// Hasomed
#include "smpt_ml_client.h"
#include "smpt_dl_client.h"
#include "smpt_definitions.h"
//

//========================== Millesaceous ==================================
//New functions
void generate_date(char* outStr){
//void do not return values and a char array is a message
//So the function gets a pointer to a global char array to write the date
  time_t rawtime;
  struct tm * timeinfo;
  char buffer[15];
  //char output;
  time (&rawtime);
  timeinfo = localtime (&rawtime);
  strftime(buffer, sizeof(buffer),"%Y%m%d_%H%M", timeinfo);
  for(int i=0; i < 15; ++i){
    outStr[i] = buffer[i];
  }
}

string convertToString(char* a, int size)
{
    int i;
    string s = "";
    for (i = 0; i < size-1; i++) {
        s = s + a[i];
    }
    return s;
}

void get_dir(int argc, char *argv[], string& Outdir){
  //Gets the current directory of the programme, but for files
  std::stringstream sst(argv[0]);
  char delimeter='\\'; //for Windows
  std::string part, full="", part_prev="";
  while (std::getline(sst, part, delimeter))
  {
      full+=part_prev;
      part_prev=part+delimeter;
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

bool UDP_decode(char* message, bool& value1, bool& value2)
{
  char b_true[6]="true";
  char b_false[6]="false";
  int comp_t = 0, comp_f=0, pos = 0, pos_cont = 0;
  bool valid1 = false, valid2 = false, valid_msg = false, b_v1=false, b_v2= false;
  // Decode 1st part of the message:
  for(int j=0; j<=3; j++){
    comp_t = comp_t + message[j]-b_true[j];
    comp_f = comp_f + message[j]-b_false[j];
  }
  if((comp_t==0)||(comp_f==0)){
    valid1 = true;
  }
  // Decode 2nd part of the message:
  if(comp_t==0){
    pos = 5;
    b_v1 = true;
  }else{
    pos = 6;
  }
  comp_t = 0;
  comp_f = 0;
  for(int j=0; j<=3; j++){
    comp_t = comp_t + message[j+pos]-b_true[j];
    comp_f = comp_f + message[j+pos]-b_false[j];
  }
  if(comp_t==0){
    valid2 = true;
    b_v2 = true;
  }
  else if(comp_f==0){
    valid2 = true;
  }
  // Outputs:
  valid_msg = valid1 && valid2;

  if (valid_msg){
    value1 = b_v1;
    value2 = b_v2;
  }
  return valid_msg;
}

bool TCP_decode(char* message, RehaMove3_Req_Type& stimulator, User_Req_Type& user, ROB_Type& status, int& rep, bool& finished) {
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
        int user_value = message[pos + 9] - '0';

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
        }

        //Outputs
        if (valid_msg) {
            stimulator = (RehaMove3_Req_Type)move_value;
            user = (User_Req_Type)user_value;
            //status[0] = get_status;
            status = (ROB_Type)get_status;
            if (get_status == 'R') {
                rep = rep_nr;
            }
        }
    } // !check_end
    else {
        if (valid_msg) {
            finished = true;
        }
    }

    return valid_msg;
}
//=========================== Hasomed fcn ===============================
void fill_ml_init(Smpt_device* const device, Smpt_ml_init* const ml_init)
{
    /* Clear ml_init struct and set the data */
    smpt_clear_ml_init(ml_init);
    ml_init->packet_number = smpt_packet_number_generator_next(device);
}

void fill_ml_update(Smpt_device* const device, Smpt_ml_update* const ml_update, Smpt_ml_channel_config values)
{
    /* Clear ml_update and set the data */
    smpt_clear_ml_update(ml_update);
    ml_update->enable_channel[Smpt_Channel_Red] = true;  /* Enable channel red */
    ml_update->packet_number = smpt_packet_number_generator_next(device);

    ml_update->channel_config[Smpt_Channel_Red].number_of_points = values.number_of_points;
    ml_update->channel_config[Smpt_Channel_Red].ramp = values.ramp;
    ml_update->channel_config[Smpt_Channel_Red].period = values.period;
    ml_update->channel_config[Smpt_Channel_Red].points[0].current = values.points[0].current;
    ml_update->channel_config[Smpt_Channel_Red].points[0].time = values.points[0].time;
    ml_update->channel_config[Smpt_Channel_Red].points[1].time = values.points[1].time;
    ml_update->channel_config[Smpt_Channel_Red].points[2].current = values.points[2].current;
    ml_update->channel_config[Smpt_Channel_Red].points[2].time = values.points[2].time;
}

void fill_ml_get_current_data(Smpt_device* const device, Smpt_ml_get_current_data* const ml_get_current_data)
{
    ml_get_current_data->packet_number = smpt_packet_number_generator_next(device);
    ml_get_current_data->data_selection[Smpt_Ml_Data_Stimulation] = true; /* get stimulation data */
}

void fill_dl_init(Smpt_device* const device, Smpt_dl_init* const dl_init)
{
    dl_init->ads129x.ch1set = 16;
    dl_init->ads129x.ch2set = 0;
    dl_init->ads129x.ch3set = 0;
    dl_init->ads129x.ch4set = 0;
    dl_init->ads129x.config1 = 133; // 131 = 4KHz, 133 = 1 KHz, 134 = 500 SPS
    dl_init->ads129x.config2 = 0;
    dl_init->ads129x.config3 = 252;
    dl_init->ads129x.rld_sensn = 2;
    dl_init->ads129x.rld_sensp = 2;

    dl_init->gain_ch1 = Smpt_Dl_Init_Gain_Ch1_20x;
    dl_init->live_data_mode_flag = true;
    // Currently there are no pre-filtering options available
    dl_init->filter = Smpt_Dl_Dsp_Filter_off;    /* = 0 no filter is activated*/
}

void fill_dl_power_module(Smpt_device* const device, Smpt_dl_power_module* const dl_power_module)
{
    dl_power_module->hardware_module = Smpt_Dl_Hardware_Module_Measurement;
    dl_power_module->switch_on_off = true;

}

void handleInitAckReceived(Smpt_device* const device, const Smpt_ack& ack)
{
    Smpt_dl_init_ack init_ack;
    smpt_get_dl_init_ack(device, &init_ack);
}

void handlePowerModuleAckReceived(Smpt_device* const device, const Smpt_ack& ack)
{
    Smpt_dl_power_module_ack power_module_ack;
    smpt_get_dl_power_module_ack(device, &power_module_ack);
}

void handleStopAckReceived(Smpt_device* const device, const Smpt_ack& ack)
{
    Smpt_dl_power_module_ack power_module_ack;
    smpt_get_dl_power_module_ack(device, &power_module_ack);
}
