﻿// fixDevices.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4507 34)

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
#include <windows.h>

// ==================================================================
//#include "SASLIBdev.hpp"
#include "smpt_ml_client.h"
#include "smpt_dl_client.h"
#include "smpt_definitions.h"

// ------------------ Gobal variables ------------------
// For recorder
static std::vector<double> recorder_emg1;
static std::vector<double> recorder_emg2;

// For stimulator
const int MS_TO_HZ = 1000;

// ------------------ Functions definition ------------------
// Original Hasomed Functions
void fill_ml_init(Smpt_device* const device, Smpt_ml_init* const ml_init)
{
    /* Clear ml_init struct and set the data */
    smpt_clear_ml_init(ml_init);
    ml_init->packet_number = smpt_packet_number_generator_next(device);
}

void fill_ml_update(Smpt_device* const device, Smpt_ml_update* const ml_update, Smpt_Channel sel_ch, bool enable, Smpt_ml_channel_config values)
//void fill_ml_update(Smpt_device *const device, Smpt_ml_update *const ml_update, Smpt_ml_channel_config values)
{
    /* Clear ml_update and set the data */
    smpt_clear_ml_update(ml_update);
    //Smpt_Channel sel_ch = Smpt_Channel_Red;
    //ml_update->enable_channel[sel_ch] = true; /* Enable channel red */
    ml_update->enable_channel[sel_ch] = enable;
    ml_update->packet_number = smpt_packet_number_generator_next(device);

    ml_update->channel_config[sel_ch].number_of_points = values.number_of_points;
    ml_update->channel_config[sel_ch].ramp = values.ramp;
    ml_update->channel_config[sel_ch].period = values.period;
    ml_update->channel_config[sel_ch].points[0].current = values.points[0].current;
    ml_update->channel_config[sel_ch].points[0].time = values.points[0].time;
    ml_update->channel_config[sel_ch].points[1].time = values.points[1].time;
    ml_update->channel_config[sel_ch].points[2].current = values.points[2].current;
    ml_update->channel_config[sel_ch].points[2].time = values.points[2].time;
}

void enable_ml_update(Smpt_device* const device, Smpt_ml_update* const ml_update, Smpt_Channel sel_ch, bool enable)
{
    // Clear ml_update and set the data
    smpt_clear_ml_update(ml_update);
    //Smpt_Channel sel_ch = Smpt_Channel_Red;
    ml_update->enable_channel[sel_ch] = enable;
    ml_update->packet_number = smpt_packet_number_generator_next(device);

    ml_update->channel_config[sel_ch].number_of_points = 3;
    ml_update->channel_config[sel_ch].ramp = 3;
    ml_update->channel_config[sel_ch].period = 20;
    ml_update->channel_config[sel_ch].points[0].current = 0.0;
    ml_update->channel_config[sel_ch].points[0].time = 300;
    ml_update->channel_config[sel_ch].points[1].time = 300;
    ml_update->channel_config[sel_ch].points[2].current = 0.0;
    ml_update->channel_config[sel_ch].points[2].time = 300;
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
    dl_init->filter = Smpt_Dl_Dsp_Filter_off; /* = 0 no filter is activated*/
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

void fill_dl_poweroff_module(Smpt_device* const device, Smpt_dl_power_module* const dl_power_module)
{
    dl_power_module->hardware_module = Smpt_Dl_Hardware_Module_Measurement;
    dl_power_module->switch_on_off = false;
}

// Modified from original Hasomeds examples:
static void handleSendLiveDataReceived(Smpt_device* const device, const Smpt_ack& ack)
{
    Smpt_dl_send_live_data live_data;
    float values[5] = { 0 };
    smpt_get_dl_send_live_data(device, &live_data);

    for (int i = 0; i < live_data.n_channels; i++)
    {
        values[i] = live_data.electrode_samples[i].value;
    }
    values[4] = (float)live_data.time_offset;

    //value[0] : channel 1, bioimpedance measurement
    //value[1] : channel 2, emg 1 measurement
    //value[2] : channel 3, emg 2
    //value[3] : channel 4, analog signal.
    //value[4] : time_ofset between last sample and actual sample
    recorder_emg1.push_back((double)values[1]);
    // recorder_emg2.push_back((double)values[2]);
}

static bool handle_dl_packet_global(Smpt_device* const device)
{
    Smpt_ack ack;
    smpt_last_ack(device, &ack);
    Smpt_Cmd cmd = (Smpt_Cmd)ack.command_number;
    bool output = false;

    switch (cmd)
    {
    case Smpt_Cmd_Dl_Power_Module_Ack:
    {
        handlePowerModuleAckReceived(device, ack);
        break;
    }
    case Smpt_Cmd_Dl_Get_Ack:
    {
        break;
    }
    case Smpt_Cmd_Dl_Init_Ack:
    {
        handleInitAckReceived(device, ack);
        break;
    }
    case Smpt_Cmd_Dl_Start_Ack:
    {
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
        output = true;
        break;
    }
    default:
    {
        break;
    }
    }
    return output;
}

// ------------------ Objects definition ------------------
// Stimulator
class RehaMove3
{
private:
    char* port_name_rm;
    Smpt_device device;
    Smpt_ml_init ml_init; // Struct for ml_init command *
    Smpt_ml_get_current_data ml_get_current_data;
    Smpt_get_device_id_ack device_id_ack;
    Smpt_get_stim_status_ack device_stim_ack;
    uint8_t packet;
    int turn_on = 0; //Time if the device gets turned on in the middle of the process
    bool smpt_port, smpt_check, smpt_next, smpt_end, smpt_get, smpt_ack;
    Smpt_ml_update ml_update; // Struct for ml_update command
    char itoaNr[32];
    char ID_dev[64], ID_MOVE[64] = "170150307";
public:
    bool ready, active, display;
    //Smpt_ml_channel_config stim;
    Smpt_ml_channel_config stim[4];
    double fq[4];
    bool stim_act[4] = { true, false, false, false };
    // From main:
    bool abort, error;
    char displayMsg[512];

    // Constructor
    RehaMove3()
    {
        device = { 0 };
        ml_init = { 0 };
        ml_get_current_data = { 0 };
        smpt_port = false;
        smpt_check = false;
        smpt_next = false;
        smpt_end = false;
        smpt_get = false;
        ready = false;
        abort = false;
        active = false;
        turn_on = 2500;
        display = false;
        error = false;
    }
    // Functions
    void init(char* port)
    {
        ready = false;
        // device port
        port_name_rm = port;
        // Stimulation values
        stim_act[Smpt_Channel_Red] = false;
        stim[Smpt_Channel_Red].number_of_points = 3; // Set the number of points
        stim[Smpt_Channel_Red].ramp = 3;             // Three lower pre-pulses
        stim[Smpt_Channel_Red].period = 20;          // Frequency: 50 Hz
        // Set the stimulation pulse
        stim[Smpt_Channel_Red].points[0].current = 0;
        stim[Smpt_Channel_Red].points[0].time = 300;
        stim[Smpt_Channel_Red].points[1].time = 300;
        stim[Smpt_Channel_Red].points[2].current = 0;
        stim[Smpt_Channel_Red].points[2].time = 300;

        // Start Process
        smpt_check = smpt_check_serial_port(port_name_rm);
        smpt_port = smpt_open_serial_port(&device, port_name_rm);

        fill_ml_init(&device, &ml_init);
        smpt_send_ml_init(&device, &ml_init);
        fill_ml_update(&device, &ml_update, Smpt_Channel_Red, stim_act[Smpt_Channel_Red], stim[Smpt_Channel_Red]); // <- comment out this one
        smpt_send_ml_update(&device, &ml_update);
        fill_ml_get_current_data(&device, &ml_get_current_data);
        // This last command check if it's received all the data requested
        smpt_get = smpt_send_ml_get_current_data(&device, &ml_get_current_data);
        // This all is true when it is connected to the wrong port. Therefore it is necessary to check the device ID

        //sprintf(displayMsg, "%s smpt_check %d, smpt_port %d, smpt_get %d", port_name_rm, smpt_check, smpt_port, smpt_get);

        // Request ID Data
        packet = smpt_packet_number_generator_next(&device);
        smpt_send_get_device_id(&device, packet);
        bool getID = false;
        int getID_dummy = 0;
        Smpt_ack getID_ack = { 0 };

        while (!getID && getID_dummy < 100)
        {
            if (smpt_new_packet_received(&device))
            {
                smpt_last_ack(&device, &getID_ack);
                //packet = smpt_packet_number_generator_next(&device);
                getID = smpt_get_get_device_id_ack(&device, &device_id_ack);
            }
            Sleep(1);
            getID_dummy++;
        }

        // check device id: if the bool = 1 and the ID is not empty, the device has been found
        //printf("GET ID: bool %d, ID = %s, result = %d, schleife = %d\n", getID, device_id_ack.device_id, device_id_ack.result, getID_dummy);

        // smpt_next = connection port was successfull
        smpt_next = smpt_check && smpt_port && smpt_get && getID && (getID_dummy<90);

        if (smpt_next && !abort && !smpt_end)
        {
            Sleep(turn_on); // wait for it to be properly started
            smpt_port = false;
            fill_ml_init(&device, &ml_init);
            smpt_send_ml_init(&device, &ml_init);

            // Initialize stim channels values
            for (int k = 0; k < 4; k++)
            {
                fq[k] = 30;
                stim[k].period = (MS_TO_HZ / fq[k]);
            }
            // Start the channel 1
            stim_act[Smpt_Channel_Red] = true;
            ready = true;

            if (display) { sprintf(displayMsg, "Device RehaMove3 ready."); }
        }
        else if (!smpt_next)
        {
            smpt_send_ml_stop(&device, smpt_packet_number_generator_next(&device));
            smpt_close_serial_port(&device);
            if (smpt_check && display)
            {
                sprintf(displayMsg, "Device on port %s does not respond. Turn it on or restart it.", port_name_rm);
            }
            else if (display)
            {
                sprintf(displayMsg, "Device on port %s not found. Check connection.", port_name_rm);
            }

            smpt_port = true;
            // Retry after this
        }
        else if (abort)
        {
            smpt_end = true;
        }

    };
    bool checkStatus()
    {
        // this just returns if the devices is still there
        bool getStatus = false;
        int getID_cnt = 0;
        // Request ID Data
        packet = smpt_packet_number_generator_next(&device);
        smpt_send_get_device_id(&device, packet);
        bool getID = false;
        Smpt_ack getID_ack = { 0 };
        memset(ID_dev, '\0', 64);

        while (!getID && getID_cnt < 100)
        {
            if (smpt_new_packet_received(&device))
            {
                smpt_last_ack(&device, &getID_ack);
                getID = smpt_get_get_device_id_ack(&device, &device_id_ack);
            }
            Sleep(1);
            getID_cnt++;
        }
        sprintf(ID_dev, "%s", device_id_ack.device_id);
        bool id_comp = strcmp(ID_dev, ID_MOVE) == 0;
        getStatus = getID && id_comp;
        
        return getStatus;
    };
    void update()
    {
        fill_ml_update(&device, &ml_update, Smpt_Channel_Red, true, stim[Smpt_Channel_Red]);
        smpt_send_ml_update(&device, &ml_update);

        fill_ml_get_current_data(&device, &ml_get_current_data);
        smpt_send_ml_get_current_data(&device, &ml_get_current_data);
        active = true;
    };
    void pause()
    {
        smpt_send_ml_stop(&device, smpt_packet_number_generator_next(&device));
        fill_ml_init(&device, &ml_init);
        smpt_send_ml_init(&device, &ml_init);
        active = false;
    };
    void end()
    {
        //  No need to repeat this, since the connection was successfully stablished and
        // in case something went wrong, it'd get fixed on the next step
        smpt_send_ml_stop(&device, smpt_packet_number_generator_next(&device));
        if (!smpt_port)
        {
            smpt_port = smpt_close_serial_port(&device);
            smpt_check = smpt_check_serial_port(port_name_rm); // it must be available after closing
        }
        ready = false;
        active = false;
        if (smpt_check && display)
        {
            sprintf(displayMsg, "Reha Move3 message: Process finished.");
        }
    };
};

// Recorder
class RehaIngest
{
private:
    const char* port_name_ri;
    uint8_t packet_number = 0;
    Smpt_device device_ri = { 0 };
    Smpt_ml_init ml_init = { 0 }; // Struct for ml_init command
    Smpt_get_device_id_ack device_id_ack;
    Smpt_dl_stop_ack device_stop_ack;
    //Process variables
    bool smpt_port, smpt_check, smpt_stop, smpt_next, smpt_end;
    char ID_dev[64], ID_INGEST[64] = "190751110";
    const int ERROR_LIM = 10;

public:
    bool abort, ready, found, display, error;
    bool data_received, data_start, data_printed;
    char displayMsg[512];
    int packet_cnt = 0, error_cnt = 0;

    // Constructor
    RehaIngest()
    {
        device_ri = { 0 };
        ml_init = { 0 };
        packet_number = 0;
        smpt_port = false;
        smpt_check = false;
        smpt_next = false;
        smpt_end = false;
        smpt_stop = false;
        data_received = false;
        data_start = false;
        data_printed = false;
        ready = false;
        abort = false;
        display = false;
        found = false;
        error = false;
        error_cnt = 0;
        packet_cnt = 0;
    }
    // Functions
    void init_original(char* port)
    {
        error_cnt = 0;
        data_start = false;
        ready = false;
        found = false;
        port_name_ri = port;
        // First step
        if (display) { sprintf(displayMsg, "Reha Ingest message: Setting communication on port %s", port_name_ri); }

        smpt_check = smpt_check_serial_port(port_name_ri);
        smpt_port = smpt_open_serial_port(&device_ri, port_name_ri);
        packet_number = smpt_packet_number_generator_next(&device_ri);
        smpt_stop = smpt_send_dl_stop(&device_ri, packet_number);

        // Request ID Data
        packet_number = smpt_packet_number_generator_next(&device_ri);
        smpt_send_get_device_id(&device_ri, packet_number);
        bool getID = false;
        int getID_cnt = 0;
        Smpt_ack getID_ack = { 0 };
        memset(ID_dev, '\0', 64);

        while (!getID && getID_cnt < 100)
        {
            if (smpt_new_packet_received(&device_ri))
            {
                smpt_last_ack(&device_ri, &getID_ack);
                getID = smpt_get_get_device_id_ack(&device_ri, &device_id_ack);
            }
            Sleep(1);
            getID_cnt++;
        }
        sprintf(ID_dev, "%s", device_id_ack.device_id);
        bool id_comp = strcmp(ID_dev, ID_INGEST) == 0;

        smpt_next = smpt_check && smpt_port && smpt_stop && getID && id_comp;
        // smpt_next = connection to the device was successful

        if (smpt_next && !smpt_end && !abort) {
            if (display) { sprintf(displayMsg, "Device RehaIngest found."); }
            recorder_emg1.clear();
            found = true;
            error = false;
        }
        else if (!smpt_next)
        {
            sprintf(displayMsg, "Error - Reha Ingest: Device not found. Turn it on and/or check connection.");
            smpt_stop = smpt_send_dl_stop(&device_ri, packet_number);
            smpt_close_serial_port(&device_ri);
        }
        else if (abort)
        {
            smpt_end = true;
        }

    };
    void start()
    {
        ready = false;
        // Fourth step
        // Clean the input buffer
        while (smpt_new_packet_received(&device_ri))
        {
            handle_dl_packet_global(&device_ri);
        }
        // Second step: enable device
        if (display) { sprintf(displayMsg, "Reha Ingest message: Enabling and initializing device... "); }
        Smpt_dl_power_module dl_power_module = { 0 };
        fill_dl_power_module(&device_ri, &dl_power_module);
        smpt_send_dl_power_module(&device_ri, &dl_power_module);

        // wait, because the enabling takes some time (normal up to 4ms)
        Sleep(10);

        while (smpt_new_packet_received(&device_ri))
        {
            handle_dl_packet_global(&device_ri);
        }
        // Third step: initialize device
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
        // send measurement start cmd
        packet_number = smpt_packet_number_generator_next(&device_ri);
        smpt_send_dl_start(&device_ri, packet_number);
        ready = true;
        error = false;
        if (display) { sprintf(displayMsg, "Device ready."); }
    };

    int record_hasomed(int j)
    {
        // clean data variables before starting
        data_received = false;

        while (smpt_new_packet_received(&device_ri) && j < 300) {

            data_received = handle_dl_packet_global(&device_ri);
            if (data_received && !data_start)
            {
                data_start = true;
            }
            if (data_received)
            {
                j++;
            }
            //checking error
            if (data_start && !data_received)
            {
                error_cnt++;
            }
            if (data_start && data_received && error_cnt != 0)
            {
                error_cnt=0;
            }
            error = (error_cnt >= ERROR_LIM);
        }

        return packet_cnt;
    };

    bool record()
    {
        // clean data variables before starting
        data_received = false;

        while (smpt_new_packet_received(&device_ri)) {

            data_received = handle_dl_packet_global(&device_ri);
            if (data_received && !data_start)
            {
                data_start = true;
            }
            if (data_received)
            {
                packet_cnt++;
            }         
        }
        //checking error
        if (data_start && !data_received && error_cnt < ERROR_LIM)
        {
            error_cnt++;
        }
        else if (data_start && data_received && error_cnt != 0)
        {
            error_cnt = 0;
        } 
        error = (error_cnt >= ERROR_LIM);
        return data_received;
    };


    void record_original()
    {
        // clean data variables before starting
        data_received = false;
        while (smpt_new_packet_received(&device_ri))
        {
            data_received = handle_dl_packet_global(&device_ri);
            if (data_received && !data_start)
            {
                data_start = true;
            }
        }
    };

    void end()
    {
        packet_number = smpt_packet_number_generator_next(&device_ri);
        smpt_port = smpt_send_dl_stop(&device_ri, packet_number);
        Sleep(10);
        while (smpt_new_packet_received(&device_ri))
        {
            handle_dl_packet_global(&device_ri);
            Sleep(1); // <--- maybe add this?
        }
        smpt_port = smpt_close_serial_port(&device_ri);
        smpt_check = smpt_check_serial_port(port_name_ri);
        ready = false;
        if (display) { sprintf(displayMsg, "Reha Ingest message: Process finished."); }
    };

    bool checkStatus(int& counter)
    {
        // this just returns if the devices is still there
        bool getStatus = false;
        int getID_cnt = 0;
        // Request ID Data
        packet_number = smpt_packet_number_generator_next(&device_ri);
        smpt_send_get_device_id(&device_ri, packet_number);
        bool getID = false;
        Smpt_ack getID_ack = { 0 };
        memset(ID_dev, '\0', 64);

        while (!getID && getID_cnt < 100)
        {
            if (smpt_new_packet_received(&device_ri))
            {
                smpt_last_ack(&device_ri, &getID_ack);
                getID = smpt_get_get_device_id_ack(&device_ri, &device_id_ack);
            }
            Sleep(1);
            getID_cnt++;
        }
        sprintf(ID_dev, "%s", device_id_ack.device_id);
        bool id_comp = strcmp(ID_dev, ID_INGEST) == 0;

        getStatus = getID && id_comp;
        counter = getID_cnt;

        return getStatus;
    };


    void init(char* port)
    {
        error_cnt = 0;
        data_start = false;
        ready = false;
        found = false;
        port_name_ri = port;
        // First step
        if (display) { sprintf(displayMsg, "Reha Ingest message: Setting communication on port %s", port_name_ri); }

        smpt_check = smpt_check_serial_port(port_name_ri);
        smpt_port = smpt_open_serial_port(&device_ri, port_name_ri);

        packet_number = smpt_packet_number_generator_next(&device_ri);
        Smpt_dl_get dl_get;
        dl_get.get_type = Smpt_Dl_Get_Type_Operation_Mode;
        dl_get.packet_number = packet_number;
        smpt_send_dl_get(&device_ri, &dl_get);

        bool getMode = false;
        int getMode_cnt = 0;
        Smpt_ack getMode_ack = { 0 };
        Smpt_dl_get_ack dl_get_ack = { 0 };
        Smpt_Dl_Op_Mode OperationMode;

        while (!getMode && getMode_cnt < 100)
        {
            if (smpt_new_packet_received(&device_ri))
            {
                smpt_last_ack(&device_ri, &getMode_ack);
                smpt_get_dl_get_ack(&device_ri, &dl_get_ack);

                if (dl_get_ack.get_type == Smpt_Dl_Get_Type_Operation_Mode)
                {
                    getMode = true;
                    OperationMode = dl_get_ack.operation_mode;
                }

            }
            Sleep(1);
            getMode_cnt++;
        }
        bool sendStop = false;
        if (getMode)
        {
            sprintf(displayMsg, "Get Operation Mode sucessfull: %d\n", OperationMode);

            switch (OperationMode)
            {
            case Smpt_Dl_Op_Mode_Undefined:
            {
                sprintf(displayMsg, "!! Undefined Mode !!\n\n");
                break;
            }
            case Smpt_Dl_Op_Mode_Idle:
            {
                sprintf(displayMsg, "Device is idle\n");
                break;
            }
            case Smpt_Dl_Op_Mode_Live_Measurement_Pre:
            {
                sprintf(displayMsg, "Device is working\n");
                sendStop = true;
                break;
            }
            case Smpt_Dl_Op_Mode_Live_Measurement:
            {
                sprintf(displayMsg, "Device is working\n");
                sendStop = true;
                break;
            }
            }
        }
        else
        {
            sprintf(displayMsg, "Get Operation Mode has failed!\n");
            sendStop = true;
        }

        if (sendStop)
        {
            packet_number = smpt_packet_number_generator_next(&device_ri);
            smpt_stop = smpt_send_dl_stop(&device_ri, packet_number);
            Sleep(10);
            Smpt_dl_power_module dl_power_module = { 0 };
            fill_dl_poweroff_module(&device_ri, &dl_power_module);
            smpt_send_dl_power_module(&device_ri, &dl_power_module);
            Sleep(10);
        }
        // Request ID Data
        packet_number = smpt_packet_number_generator_next(&device_ri);
        smpt_send_get_device_id(&device_ri, packet_number);
        bool getID = false;
        int getID_cnt = 0;
        Smpt_ack getID_ack = { 0 };
        memset(ID_dev, '\0', 64);

        while (!getID && getID_cnt < 100)
        {
            if (smpt_new_packet_received(&device_ri))
            {
                smpt_last_ack(&device_ri, &getID_ack);
                getID = smpt_get_get_device_id_ack(&device_ri, &device_id_ack);
            }
            Sleep(1);
            getID_cnt++;
        }
        sprintf(ID_dev, "%s", device_id_ack.device_id);
        bool id_comp = strcmp(ID_dev, ID_INGEST) == 0;

        smpt_next = smpt_check && smpt_port && smpt_stop && getID && id_comp;
        // smpt_next = connection to the device was successful

        if (smpt_next && !smpt_end && !abort) {
            if (display) { sprintf(displayMsg, "Device RehaIngest found."); }
            recorder_emg1.clear();
            found = true;
            error = false;
        }
        else if (!smpt_next)
        {
            sprintf(displayMsg, "Error - Reha Ingest: Device not found. Turn it on and/or check connection.");
            smpt_stop = smpt_send_dl_stop(&device_ri, packet_number);
            smpt_close_serial_port(&device_ri);
        }
        else if (abort)
        {
            smpt_end = true;
        }

    };


};

//==================================================================
using namespace std;

// ====================== Global variables ====================== 
RehaMove3 stimulator;
RehaIngest recorder;

// ====================== Function headers ====================== 
void findStimulator();
void testStimulator();

void findRecorder();
void testRecorder();
void testRecorder2();

// ====================== main ====================== 
int main()
{
    //testStimulator();
    //testRecorder();
    testRecorder2();

    return 0;
}

// ====================== Function definitions: stimulator ====================== 

void findStimulator()
{
    char PORT_STIM[5] = "COM3";
    char PORT_STIM_UI[5] = "COM3";
    int countPort = 0, nrPort = 3;
    bool success = false;

    while (!success)
    {

        // Select port from the GUI
        if (PORT_STIM_UI[3] >= '1' && PORT_STIM_UI[3] <= '9')
        {
            PORT_STIM[3] = PORT_STIM_UI[3];
        }
        countPort++;

        // Connect to the device and initialize settings
        printf("Starting stimulator on port %s, try nr %d, nrPort %d, %s\n", PORT_STIM, countPort, nrPort, PORT_STIM_UI);
        stimulator.display = true;
        stimulator.init(PORT_STIM);
        printf("%s\n", stimulator.displayMsg);

        stimulator.fq[Smpt_Channel_Red] = 30;

        success = stimulator.ready;


        // others
        if (countPort >= 3)
        {
            countPort = 0;

            nrPort++;
            if (nrPort > 9)
            {
                nrPort = 1;
            }
            char cPort[8];
            sprintf(cPort, "%d", nrPort);
            PORT_STIM_UI[3] = cPort[0];
        }

        // loop settings
        Sleep(100);
    }
}

void testStimulator()
{
    /*
    char PORT_STIM[5] = "COM3";
    char PORT_STIM_UI[5] = "COM3";
    int countPort = 0, nrPort = 3;
    bool success = false;
    // initialization

    while (!success)
    {

        // Select port from the GUI
        if (PORT_STIM_UI[3] >= '1' && PORT_STIM_UI[3] <= '9')
        {
            PORT_STIM[3] = PORT_STIM_UI[3];
        }
        countPort++;

        // Connect to the device and initialize settings
        printf("Starting stimulator on port %s, try nr %d, nrPort %d, %s\n", PORT_STIM, countPort, nrPort, PORT_STIM_UI);
        stimulator.display = true;
        stimulator.init(PORT_STIM);
        printf("%s\n", stimulator.displayMsg);

        stimulator.fq[Smpt_Channel_Red] = 30;

        success = stimulator.ready;


        // others
        if (countPort >= 3)
        {
            countPort = 0;

            nrPort++;
            if (nrPort > 9)
            {
                nrPort = 1;
            }
            char cPort[8];
            sprintf(cPort, "%d", nrPort);
            PORT_STIM_UI[3] = cPort[0];
        }

        // loop settings
        Sleep(100);


        // loop settings
        Sleep(100);
    }
*/
    
    findStimulator();

    printf("\n\n---------------Stimulator found----------------------\n\n");
    stimulator.display = true;
    bool restart_need = false;
    bool devAvaliable = false;

    for (int j = 0;j < 200;j++)
    {
        devAvaliable = stimulator.checkStatus();

        if (devAvaliable && !restart_need)
        {
            printf("Stimulieren.\n");
            //stimulator.update();
        }
        else if (!devAvaliable && !restart_need)
        {
            printf("Stimulator was disconnected\n");
            stimulator.end();
            restart_need = true;
            //stimulator.init();
        }
        else if (restart_need)
        {
            findStimulator();
            restart_need = !stimulator.ready;
        }

        Sleep(1000);
    }

    printf("%s\n-------------------------------\n", stimulator.displayMsg);

}

// ====================== Function definitions: recorder ====================== 

void findRecorder()
{
    char PORT_REC[5] = "COM4";
    char PORT_REC_UI[5] = "COM4";
    int countPort = 0, nrPort = 4;
    bool success = false;

    while (!success)
    {
        // Select port from the GUI
        if (PORT_REC_UI[3] >= '1' && PORT_REC_UI[3] <= '6')
        {
            PORT_REC[3] = PORT_REC_UI[3];
        }
        countPort++;

        // Connect to the device and initialize settings
        printf("Starting recorder on port %s, try nr %d, nrPort %d, %s\n", PORT_REC, countPort, nrPort, PORT_REC_UI);
        recorder.display = true;
        recorder.init(PORT_REC);
        if (recorder.found)
        {
            recorder.start();
        }


        printf("%s\n", recorder.displayMsg);


        success = recorder.ready;

        // others
        if (countPort >= 3)
        {
            countPort = 0;

            nrPort++;
            if (nrPort > 7)
            {
                nrPort = 1;
            }
            char cPort[8];
            sprintf(cPort, "%d", nrPort);
            PORT_REC_UI[3] = cPort[0];
        }

        // loop settings
        Sleep(100);


        if (!success)
        {
            recorder.display = true;
           // recorder.end();
            printf("%s\n-----\n", recorder.displayMsg);
        }



        // loop settings
        Sleep(100);

    }

}

void testRecorder()
{

    for (int k = 0; k < 3; k++)
    {

    
    findRecorder();

    printf("\n\n-------------- DONE SOME STUFF --------------\n\n");

    // record some stuff
    int dummy = 0;
    for (int j = 0; j < 3; j++)
    {
        printf("recording\n");
        recorder.record();
        Sleep(1000);
    }

    // checking periodically connection
    int schleife = 0, j=0, rec_cnt = 0;   // schleife (DE) = loop (ENG)
    bool devAvailable = false, received_data = false;
    //for (int j = 0; j < 30; j++)
    while(j<30)
    {
        recorder.record();
        //recorder.record();
        devAvailable = (received_data && recorder.data_start) || (!recorder.data_start && recorder.ready);
        Sleep(33);
        
        Sleep(33);
    }

    printf("Disconnect\n");
    //recorder.end();
    printf("%s", recorder.displayMsg);
    Sleep(2000);


    printf("Starting again...\n");
    Sleep(5000);
    } // for loop
}

void testRecorder2()
{
    bool success = false;
    int countPort = 0;

    char PORT_REC[5] = "COM4";

    // start recorder
    while (!success)
    {
        // Connect to the device and initialize settings
        printf("Starting recorder on port %s, try nr %d\n", PORT_REC, countPort);
        recorder.display = true;
        recorder.init(PORT_REC);
        if (recorder.found)
        {
            recorder.start();
        }
        printf("%s\n", recorder.displayMsg);

        countPort++;

        success = recorder.ready;
    }

    // checking periodically connection
    int schleife = 0, j = 0, rec_cnt = 0;   // schleife (DE) = loop (ENG)
    bool devAvailable = false, received_data = false;
    //for (int j = 0; j < 30; j++)j
    while (j < 10000)
    {
        //received_data = recorder.record(rec_cnt);
        //devAvailable = (received_data && recorder.data_start) || (!recorder.data_start && recorder.ready);
        devAvailable = recorder.record();
        // = !recorder.error;
        Sleep(100);
        printf("dataStart %d, error cnt %d, packet %d, error %d, j %d\n", recorder.data_start, recorder.error_cnt, recorder.packet_cnt, recorder.error, j);
        j++;

        if (recorder.error)
        {
            printf("Connection lost. Ending device... ");
            recorder.display = true;
            recorder.end();
            printf("%s\n", recorder.displayMsg);

            Sleep(2000);

            success = false;

            printf("Re-connecting... ");
            while (!success)
            {
                recorder.init(PORT_REC);
                printf("%s\n", recorder.displayMsg);

                if (recorder.found)
                {
                    recorder.start();
                    if (recorder.ready)
                    {
                        printf("Connection re-stored\n");
                        success = true;
                    }
                }
                Sleep(1000);
            }

        }
    }

    printf("Disconnecting\n");
    //devAvailable = recorder.end(schleife);
    recorder.end();
    printf("dev %d, cnt%d, %s",devAvailable,schleife, recorder.displayMsg);
}

