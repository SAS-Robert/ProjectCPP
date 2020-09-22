#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <windows.h>
// Standard C++ and C libraries
#include <conio.h>
#include <stdlib.h>
#include <cstdlib>
#include <iomanip>
#include <complex>
#include <stdio.h>
#include <ctime>
// User-defined libraries
#include "smpt_ml_client.h"
#include "smpt_dl_client.h"
#include "smpt_definitions.h"
#include "SAS.h"

//=========================== UDP Connection ===============================

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
//===========================Hasomed fcn ===============================

void fill_ml_init(Smpt_device* const device, Smpt_ml_init* const ml_init)
{
    /* Clear ml_init struct and set the data */
    smpt_clear_ml_init(ml_init);
    ml_init->packet_number = smpt_packet_number_generator_next(device);
}

void fill_ml_update(Smpt_device* const device, Smpt_ml_update* const ml_update)
{
    /* Clear ml_update and set the data */
    smpt_clear_ml_update(ml_update);
    ml_update->enable_channel[Smpt_Channel_Red] = true;  /* Enable channel red */
    ml_update->packet_number = smpt_packet_number_generator_next(device);

    ml_update->channel_config[Smpt_Channel_Red].number_of_points = 3;  /* Set the number of points */
    ml_update->channel_config[Smpt_Channel_Red].ramp = 3;              /* Three lower pre-pulses   */
    ml_update->channel_config[Smpt_Channel_Red].period = 20;           /* Frequency: 50 Hz */

    /* Set the stimulation pulse */
    /* First point, current: 20 mA, positive, pulse width: 200 �s */
    ml_update->channel_config[Smpt_Channel_Red].points[0].current = 50;
    ml_update->channel_config[Smpt_Channel_Red].points[0].time = 200;

    /* Second point, pause 100 �s */
    ml_update->channel_config[Smpt_Channel_Red].points[1].time = 100;

    /* Third point, current: -20 mA, negative, pulse width: 200 �s */
    ml_update->channel_config[Smpt_Channel_Red].points[2].current = -50;
    ml_update->channel_config[Smpt_Channel_Red].points[2].time = 200;
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
    dl_init->ads129x.config1 = 131;
    dl_init->ads129x.config2 = 0;
    dl_init->ads129x.config3 = 252;
    dl_init->ads129x.rld_sensn = 2;
    dl_init->ads129x.rld_sensp = 2;

    dl_init->gain_ch1 = Smpt_Dl_Init_Gain_Ch1_20x;
    dl_init->live_data_mode_flag = true;
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
void handleGetAckReceived(Smpt_device* const device, const Smpt_ack& ack)
{
    Smpt_dl_get get_ack;
    smpt_send_dl_get(device, &get_ack);
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
