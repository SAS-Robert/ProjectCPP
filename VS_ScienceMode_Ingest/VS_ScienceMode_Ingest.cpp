//ScienceMode_ingest.cpp : 
//Author: Dirk Berholz
//Company: HASOMED GmbH
///06-2020

#include "smpt_dl_client.h"
#include "smpt_definitions.h"
#include <iostream>
#include <fstream>
#include <conio.h>

static void fill_dl_init(Smpt_device* const device, Smpt_dl_init* const dl_init);
static void fill_dl_power_module(Smpt_device* const device, Smpt_dl_power_module* const dl_power_module);
static void handle_dl_packet_global(Smpt_device* const device);
static void handleInitAckReceived(Smpt_device* const device, const Smpt_ack& ack);
static void handlePowerModuleAckReceived(Smpt_device* const device, const Smpt_ack& ack);
static void handleStopAckReceived(Smpt_device* const device, const Smpt_ack& ack);
static void handleSendLiveDataReceived(Smpt_device* const device, const Smpt_ack& ack);

std::ofstream mMyfile("example.txt");
int one_to_escape = 0;

int main()
{
    /* EDIT: Change to the virtual com port of your device */
    const char* port_name = "COM4";
    uint8_t packet_number = 0;


    Smpt_device device = { 0 };
    smpt_open_serial_port(&device, port_name);

    /* First step */
    std::cout << "First step\n==========================\n";
    /* Send stop, because it could be, that the measurement is not stopped last time in your program*/
    std::cout << "send stop\n-------------------\n";
    packet_number = smpt_packet_number_generator_next(&device);
    smpt_send_dl_stop(&device, packet_number);

    /* Clean the input buffer */
    while (smpt_new_packet_received(&device))
    {
        handle_dl_packet_global(&device);
    }

    /* Second step */
    std::cout << "\nSecond step\n==========================\n";
    /* Enable the internal measurement unit*/
    std::cout << "Enable the internal measurement unit\n-------------------\n";
    Smpt_dl_power_module dl_power_module = { 0 };
    fill_dl_power_module(&device, &dl_power_module);
    smpt_send_dl_power_module(&device, &dl_power_module);

    /* wait, because the enabling takes some time (normal up to 4ms) */
    Sleep(10);

    while (smpt_new_packet_received(&device))
    {
        handle_dl_packet_global(&device);
    }
    /* Third step*/
    std::cout << "\nThird step\n==========================\n";
    /* initialize internal measurement unit*/
    std::cout << "initialize internal measurement unit\n-------------------\n";
    Smpt_dl_init dl_init = { 0 };
    fill_dl_init(&device, &dl_init);
    smpt_send_dl_init(&device, &dl_init);

    Sleep(10);

    while (smpt_new_packet_received(&device))
    {
        handle_dl_packet_global(&device);
    }

    /* Fourth step*/
    std::cout << "\nFourth step\n==========================\n";
    std::cout << "After this, the loop is running! press any key to start.\n";
    _getch();
    /* send measurement start cmd*/
    packet_number = smpt_packet_number_generator_next(&device);
    smpt_send_dl_start(&device, packet_number);

    while (one_to_escape == 0)
    {
        while (smpt_new_packet_received(&device))
        {
            handle_dl_packet_global(&device);
            one_to_escape = _kbhit();
        }

    }

    /*fifth step*/
    std::cout << "\nFifth step\n==========================\n";
    /* send stop cmd*/
    std::cout << "send stop cmd\n-------------------\n";
    packet_number = smpt_packet_number_generator_next(&device);
    smpt_send_dl_stop(&device, packet_number);

    mMyfile.close();

    return 0;
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

static void handle_dl_packet_global(Smpt_device* const device)
{
    Smpt_ack ack;
    smpt_last_ack(device, &ack);
    Smpt_Cmd cmd = (Smpt_Cmd)ack.command_number;

    std::cout << "handle_dl_packet_global cmd:" << cmd << "\n";

    switch (cmd)
    {
    case Smpt_Cmd_Dl_Power_Module_Ack:
    {
        handlePowerModuleAckReceived(device, ack);
        break;
    }
    case Smpt_Cmd_Dl_Get_Ack:
    {
        //handleGetAckReceived(ack);
        break;
    }
    case Smpt_Cmd_Dl_Init_Ack:
    {
        handleInitAckReceived(device, ack);
        break;
    }
    case Smpt_Cmd_Dl_Start_Ack:
    {
        //handleStartAckReceived(ack);
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
void handleSendLiveDataReceived(Smpt_device* const device, const Smpt_ack& ack)
{
    Smpt_dl_send_live_data live_data;
    float values[5] = { 0 };
    smpt_get_dl_send_live_data(device, &live_data);

    for (int i = 0; i < live_data.n_channels; i++)
    {
        values[i] = live_data.electrode_samples[i].value;
    }
    //uint32_t timeDiff = live_data.time_offset - m_lastTimeOffset;
    values[4] = (float)live_data.time_offset;

    if (mMyfile.is_open())
    {
        //value[0] : channel 1, bioimpedance measurement
        //value[1] : channel 2, emg 1 measurement 
        //value[2] : channel 3, emg 2
        //value[3] : channel 4, analog signal.
        //value[4] : time_ofset between last sample and actual sample
        mMyfile << values[0] << ", " << values[1] << ", " << values[2] << ", " << values[3] << ", " << values[4] << "\n";
    }
}