// fixDevices.cpp : This file contains the 'main' function. Program execution begins and ends there.
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
#include "smpt_ml_client.h"
#include "smpt_dl_client.h"
#include "smpt_definitions.h"

// ------------------ Gobal variables ------------------
// For recorder
static std::vector<double> recorder_emg1;
static std::vector<double> recorder_emg2;


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
// Recorder
class RehaIngest
{
private:
    const char* port_name_ri;
    uint8_t packet_number = 0;
    Smpt_device device_ri = { 0 };
    Smpt_ml_init ml_init = { 0 }; // Struct for ml_init command
    Smpt_get_device_id_ack device_id_ack;
    //Process variables
    bool smpt_port, smpt_check, smpt_stop, smpt_next, smpt_end;
    char ID_dev[64], ID_INGEST[64] = "190751110";

public:
    bool abort, ready, found, display;
    bool data_received, data_start, data_printed;
    char displayMsg[512];

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
    }
    // Functions
    void init(char* port)
    {
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
        if (display) { sprintf(displayMsg, "Device ready."); }
    };
    void record()
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
            // Sleep(2); // <--- maybe add this?
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
};

//==================================================================
using namespace std;

// ====================== Global variables ====================== 
RehaIngest recorder;

// ====================== Function headers ====================== 
void findRecorder();    //<- it goes through all the com ports until it finds the recorder
void testRecorder();    // it tests the recorder multiple times and it uses the findRecorder()
void testRecorder2();   // only test the recorder once

// ====================== main ====================== 
int main()
{
    //testRecorder();
    testRecorder2();
    return 0;
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
    for (int j = 0; j < 3; j++)
    {
        printf("recording\n");
        recorder.record();
        Sleep(1000);
    }

    // checking periodically connection
    int schleife = 0, j=0;   // schleife (DE) = loop (ENG)
    bool devAvailable = false;
    //for (int j = 0; j < 30; j++)
    while(j<30)
    {
        recorder.record();
        if (recorder.data_start)
        {
            devAvailable = recorder.checkStatus(schleife);
            printf("av %d, schleife %d \n", devAvailable, schleife);
            j++;
        }
        
        Sleep(33);
    }

    printf("Disconnect\n");
    recorder.end();
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
    int schleife = 0, j = 0;   // schleife (DE) = loop (ENG)
    bool devAvailable = false;
    //for (int j = 0; j < 30; j++)
    while (j < 300)
    {
        recorder.record();
        if (recorder.data_start)
        {
            devAvailable = recorder.checkStatus(schleife);
            printf("av %d, schleife %d \n", devAvailable, schleife);
            j++;
        }

        Sleep(33);
    }

    printf("Disconnecting\n");
    recorder.end();
    printf("%s", recorder.displayMsg);
}

void testIDLE()
{
    RehaIngest recorder;
    char PORT_REC[5] = "COM4";
    char PORT_REC_UI[5] = "COM4";
    int countPort = 0, nrPort = 4;
    bool success = false;

    // initialization
    recorder.init(PORT_REC);
    recorder.start();

    for (int j = 0; j < 1000; j++)
    {
        recorder.record();

        if (recorder.data_start && success)
        {
            printf("Das Geraet hat die Messung angefangen\n");
            success = true;
        }
        else if (j >= 500)
        {
            printf("Immer noch kein Datenpacket bekommen\n");
        }
        Sleep(100);
    }

    recorder.end();

}
