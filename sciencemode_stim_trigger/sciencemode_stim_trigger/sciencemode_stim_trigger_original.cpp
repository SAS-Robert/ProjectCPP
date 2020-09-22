// sciencemode_stim_trigger.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.

#include "smpt_ml_client.h"
#include "smpt_dl_client.h"
#include "smpt_definitions.h"
#include "iir.h"
#include <iostream>
#include <fstream>
#include <conio.h>

//static void mid_level_stimulation(const char* port_name);
static void fill_ml_init(Smpt_device* const device, Smpt_ml_init* const ml_init);
static void fill_ml_update(Smpt_device* const device, Smpt_ml_update* const ml_update);
static void fill_ml_update_off(Smpt_device* const device, Smpt_ml_update* const ml_update);
static void fill_ml_get_current_data(Smpt_device* const device, Smpt_ml_get_current_data* const ml_get_current_data);

static void fill_dl_init(Smpt_device* const device, Smpt_dl_init* const dl_init);
static void fill_dl_power_module(Smpt_device* const device, Smpt_dl_power_module* const dl_power_module);
static void handle_dl_packet_global(Smpt_device* const device, Smpt_device* const device_stim);
static void handleInitAckReceived(Smpt_device* const device, const Smpt_ack& ack);
static void handlePowerModuleAckReceived(Smpt_device* const device, const Smpt_ack& ack);
static void handleStopAckReceived(Smpt_device* const device, const Smpt_ack& ack);
static void handleSendLiveDataReceived(Smpt_device* const device, const Smpt_ack& ack, Smpt_device* const device_stim);

std::ofstream mMyfile("example.txt");
const int order = 8; // 4th order (=2 biquads)
Iir::Butterworth::LowPass<order> m_f;

const int order1 = 4; // 4th order (=2 biquads)
Iir::Butterworth::BandStop<order1> m_f1;

const int order2 = 4; // 4th order (=2 biquads)
Iir::Butterworth::HighPass<order2> m_f2;

const int order3 = 4; // 4th order (=2 biquads)
Iir::Butterworth::LowPass<order2> m_f3;



int one_to_escape = 0;

int main()
{
    const char* port_name_inge = "COM4";
    const char* port_name_move = "COM3";

    uint8_t packet_number_inge = 0;
    uint8_t packet_number_move = 0;


    // 1# open comport
    //###################
    Smpt_device dev_inge = { 0 };
    if (!smpt_open_serial_port(&dev_inge, port_name_inge))
    {
        /* could not open Inge */
        std::cout << "ERR: could not open ComPort Inge\n";
        return 0;
    }

    Smpt_device dev_move = { 0 };
    if (!smpt_open_serial_port(&dev_move, port_name_move))
    {
        /* could not open Move3 */
        std::cout << "ERR: could not open ComPort Move\n";
      //  return 0;
    }
    //##################


    // Filter ini
    const float samplingrate = 4000; // Hz
    const float cutoff_frequency = 90; // Hz
    m_f.setup(samplingrate, cutoff_frequency);

    const float cutoff_frequency1 = 50; // Hz
    const float width_frequency = 8;
    m_f1.setup(samplingrate, cutoff_frequency1, width_frequency);

    const float cutoff_frequency2 = 5; // Hz
    m_f2.setup(samplingrate, cutoff_frequency2);

    const float cutoff_frequency3 = 2; // Hz
    m_f3.setup(samplingrate, cutoff_frequency3);

    // 2# init stim
    //#################
    Smpt_ml_init ml_init = { 0 };           /* Struct for ml_init command */
    fill_ml_init(&dev_move, &ml_init);
    if (!smpt_send_ml_init(&dev_move, &ml_init))
    {
        /* could not init move */
        std::cout << "ERR: could not init Move\n";
        return 0;
    }/* Send the ml_init command to the stimulation unit */
    //#################


    /* First step */
    std::cout << "First step\n==========================\n";
    /* Send stop, because it could be, that the measurement is not stopped last time in your program*/
    std::cout << "send stop\n-------------------\n";
    packet_number_inge = smpt_packet_number_generator_next(&dev_inge);
    smpt_send_dl_stop(&dev_inge, packet_number_inge);

    /* Clean the input buffer */
    while (smpt_new_packet_received(&dev_inge))
    {
        handle_dl_packet_global(&dev_inge, &dev_move);
    }

    /* Second step */
    std::cout << "\nSecond step\n==========================\n";
    /* Enable the internal measurement unit*/
    std::cout << "Enable the internal measurement unit\n-------------------\n";
    Smpt_dl_power_module dl_power_module = { 0 };
    fill_dl_power_module(&dev_inge, &dl_power_module);
    smpt_send_dl_power_module(&dev_inge, &dl_power_module);

    /* wait, because the enabling takes some time (normal up to 4ms) */
    Sleep(10);

    while (smpt_new_packet_received(&dev_inge))
    {
        handle_dl_packet_global(&dev_inge, &dev_move);
    }
    /* Third step*/
    std::cout << "\nThird step\n==========================\n";
    /* initialize internal measurement unit*/
    std::cout << "initialize internal measurement unit\n-------------------\n";
    Smpt_dl_init dl_init = { 0 };
    fill_dl_init(&dev_inge, &dl_init);
    smpt_send_dl_init(&dev_inge, &dl_init);

    Sleep(10);

    while (smpt_new_packet_received(&dev_inge))
    {
        handle_dl_packet_global(&dev_inge, &dev_move);
    }

    /* Fourth step*/
    std::cout << "\nFourth step\n==========================\n";
    std::cout << "After this, the loop is running! press any key to start.\n";
    _getch();
    /* send measurement start cmd*/
    packet_number_inge = smpt_packet_number_generator_next(&dev_inge);
    smpt_send_dl_start(&dev_inge, packet_number_inge);

    while (one_to_escape == 0)
    {
        while (smpt_new_packet_received(&dev_inge))
        {
            handle_dl_packet_global(&dev_inge, &dev_move);
            one_to_escape = _kbhit();
        }

    }
    /*fifth step*/
    std::cout << "\nFifth step\n==========================\n";
    /* send stop cmd*/
    std::cout << "send stop cmd\n-------------------\n";
    packet_number_inge = smpt_packet_number_generator_next(&dev_inge);
    smpt_send_dl_stop(&dev_inge, packet_number_inge);

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

static void handle_dl_packet_global(Smpt_device* const device, Smpt_device* const device_stim)
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
        handleSendLiveDataReceived(device, ack, device_stim);
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
void handleSendLiveDataReceived(Smpt_device* const device, const Smpt_ack& ack, Smpt_device* const device_stim)
{
    Smpt_dl_send_live_data live_data;
    Smpt_ml_update ml_update = { 0 };       /* Struct for ml_update command */
    float values[5] = { 0 };
    smpt_get_dl_send_live_data(device, &live_data);

    for (int i = 0; i < live_data.n_channels; i++)
    {
        values[i] = live_data.electrode_samples[i].value;
    }
    //uint32_t timeDiff = live_data.time_offset - m_lastTimeOffset;
    values[4] = (float)live_data.time_offset;

    values[1] = m_f.filter(values[1]);
    values[1] = m_f1.filter(values[1]);
    values[1] = m_f2.filter(values[1]);
    values[1] = sqrt(values[1] * values[1]);
    values[1] = m_f3.filter(values[1]);

    if (mMyfile.is_open())
    {
        //value[0] : channel 1, bioimpedance measurement
        //value[1] : channel 2, emg 1 measurement
        //value[2] : channel 3, emg 2
        //value[3] : channel 4, analog signal.
        //value[4] : time_ofset between last sample and actual sample
        mMyfile << values[0] << ", " << values[1] << ", " << values[2] << ", " << values[3] << ", " << values[4] << "\n";
    }
    if (values[1] >= 0.02)
    {
        fill_ml_update(device_stim, &ml_update);
        if(!smpt_send_ml_update(device_stim, &ml_update))
            {
                /* could not init move */
                std::cout << "ERR: could not send ml update Move\n";
            }
    }
    else
    {
        fill_ml_update_off(device_stim, &ml_update);
        if (!smpt_send_ml_update(device_stim, &ml_update))
        {
            /* could not init move */
            std::cout << "ERR: could not send ml update off Move \n";
        }

    }
}

//void mid_level_stimulation(const char* port_name)
//{
//    Smpt_device device = { 0 };
//    smpt_open_serial_port(&device, port_name);
//
//    Smpt_ml_init ml_init = { 0 };           /* Struct for ml_init command */
//    fill_ml_init(&device, &ml_init);
//    smpt_send_ml_init(&device, &ml_init); /* Send the ml_init command to the stimulation unit */
//
//    Smpt_ml_update ml_update = { 0 };       /* Struct for ml_update command */
//    fill_ml_update(&device, &ml_update);
//    smpt_send_ml_update(&device, &ml_update);
//
//    UINT i = 0;
//    while (i <= 60000)
//    {
//        Smpt_ml_get_current_data ml_get_current_data = { 0 };
//        fill_ml_get_current_data(&device, &ml_get_current_data);
//        smpt_send_ml_get_current_data(&device, &ml_get_current_data);
//        Sleep(1);
//        i++;
//    }
//
//    Smpt_ml_get_current_data ml_get_current_data = { 0 };
//    fill_ml_get_current_data(&device, &ml_get_current_data);
//    smpt_send_ml_get_current_data(&device, &ml_get_current_data);
//
//    smpt_send_ml_stop(&device, smpt_packet_number_generator_next(&device));
//
//    smpt_close_serial_port(&device);
//}

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
    ml_update->channel_config[Smpt_Channel_Red].period = 100;           /* Frequency: 50 Hz */

    /* Set the stimulation pulse */
    /* First point, current: 20 mA, positive, pulse width: 200 µs */
    ml_update->channel_config[Smpt_Channel_Red].points[0].current = 20;
    ml_update->channel_config[Smpt_Channel_Red].points[0].time = 200;

    /* Second point, pause 100 µs */
    ml_update->channel_config[Smpt_Channel_Red].points[1].time = 100;

    /* Third point, current: -20 mA, negative, pulse width: 200 µs */
    ml_update->channel_config[Smpt_Channel_Red].points[2].current = -20;
    ml_update->channel_config[Smpt_Channel_Red].points[2].time = 200;
}

void fill_ml_get_current_data(Smpt_device* const device, Smpt_ml_get_current_data* const ml_get_current_data)
{
    ml_get_current_data->packet_number = smpt_packet_number_generator_next(device);
    ml_get_current_data->data_selection[Smpt_Ml_Data_Stimulation] = true; /* get stimulation data */
}

void fill_ml_update_off(Smpt_device* const device, Smpt_ml_update* const ml_update)
{
    /* Clear ml_update and set the data */
    smpt_clear_ml_update(ml_update);
    ml_update->enable_channel[Smpt_Channel_Red] = true;  /* Enable channel red */
    ml_update->packet_number = smpt_packet_number_generator_next(device);

    ml_update->channel_config[Smpt_Channel_Red].number_of_points = 0;  /* Set the number of points */
    ml_update->channel_config[Smpt_Channel_Red].ramp = 0;              /* Three lower pre-pulses   */
    ml_update->channel_config[Smpt_Channel_Red].period = 0;           /* Frequency: 50 Hz */

    /* Set the stimulation pulse */
    /* First point, current: 20 mA, positive, pulse width: 200 µs */
    ml_update->channel_config[Smpt_Channel_Red].points[0].current = 0;
    ml_update->channel_config[Smpt_Channel_Red].points[0].time = 0;

    /* Second point, pause 100 µs */
    ml_update->channel_config[Smpt_Channel_Red].points[1].time = 0;

    /* Third point, current: -20 mA, negative, pulse width: 200 µs */
    ml_update->channel_config[Smpt_Channel_Red].points[2].current = 0;
    ml_update->channel_config[Smpt_Channel_Red].points[2].time = 0;
}
