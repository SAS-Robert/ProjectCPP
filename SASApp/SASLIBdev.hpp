/** Library for RehaMove3 and RehaIngest devices Connection.
  * This library is based on the examples provided by
  * Hasomed GmbH
  *
*/

#ifndef SASLIBdev_H_ // Include guards
#define SASLIBdev_H_

#include <iostream>
#include <vector>
// ------------------------------------------------------------------------

using namespace std;

// ------------------ Additional libraries ------------------
#include "smpt_ml_client.h"
#include "smpt_dl_client.h"
#include "smpt_definitions.h"

// ------------------ Gobal variables ------------------
static std::vector<double> recorder_emg1;
static std::vector<double> recorder_emg2;

// ------------------ Functions definition ------------------
void fill_ml_init(Smpt_device *const device, Smpt_ml_init *const ml_init)
{
    /* Clear ml_init struct and set the data */
    smpt_clear_ml_init(ml_init);
    ml_init->packet_number = smpt_packet_number_generator_next(device);
}

void fill_ml_update(Smpt_device *const device, Smpt_ml_update *const ml_update, Smpt_Channel sel_ch, bool enable, Smpt_ml_channel_config values)
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

void enable_ml_update(Smpt_device *const device, Smpt_ml_update *const ml_update, Smpt_Channel sel_ch, bool enable)
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
    ml_update->channel_config[sel_ch].points[0].time = 200;
    ml_update->channel_config[sel_ch].points[1].time = 200;
    ml_update->channel_config[sel_ch].points[2].current = 0.0;
    ml_update->channel_config[sel_ch].points[2].time = 200;
}

void fill_ml_get_current_data(Smpt_device *const device, Smpt_ml_get_current_data *const ml_get_current_data)
{
    ml_get_current_data->packet_number = smpt_packet_number_generator_next(device);
    ml_get_current_data->data_selection[Smpt_Ml_Data_Stimulation] = true; /* get stimulation data */
}

void fill_dl_init(Smpt_device *const device, Smpt_dl_init *const dl_init)
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

void fill_dl_power_module(Smpt_device *const device, Smpt_dl_power_module *const dl_power_module)
{
    dl_power_module->hardware_module = Smpt_Dl_Hardware_Module_Measurement;
    dl_power_module->switch_on_off = true;
}

void handleInitAckReceived(Smpt_device *const device, const Smpt_ack &ack)
{
    Smpt_dl_init_ack init_ack;
    smpt_get_dl_init_ack(device, &init_ack);
}

void handlePowerModuleAckReceived(Smpt_device *const device, const Smpt_ack &ack)
{
    Smpt_dl_power_module_ack power_module_ack;
    smpt_get_dl_power_module_ack(device, &power_module_ack);
}

void handleStopAckReceived(Smpt_device *const device, const Smpt_ack &ack)
{
    Smpt_dl_power_module_ack power_module_ack;
    smpt_get_dl_power_module_ack(device, &power_module_ack);
}

// Modified from original Hasomeds examples:
static void handleSendLiveDataReceived(Smpt_device *const device, const Smpt_ack &ack)
{
    Smpt_dl_send_live_data live_data;
    float values[5] = {0};
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
}

static bool handle_dl_packet_global(Smpt_device *const device)
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
    char *port_name_rm;
    Smpt_device device;
    Smpt_ml_init ml_init; // Struct for ml_init command *
    Smpt_ml_get_current_data ml_get_current_data;
    uint8_t packet;
    int turn_on = 0; //Time if the device gets turned on in the middle of the process
    bool smpt_port, smpt_check, smpt_next, smpt_end, smpt_get;
    Smpt_ml_update ml_update; // Struct for ml_update command

public:
    bool ready, active;
    //Smpt_ml_channel_config stim;
    Smpt_ml_channel_config stim[4]; // New
    bool stim_act[4] = {true, false, false, false};
    // From main:
    bool abort;

    // Constructor
    RehaMove3(char *port)
    {
        port_name_rm = port;
        device = {0};
        ml_init = {0};
        ml_get_current_data = {0};
        smpt_port = false;
        smpt_check = false;
        smpt_next = false;
        smpt_end = false;
        smpt_get = false;
        ready = false;
        abort = false;
        active = false;
    }
    // Functions
    void init()
    {
        // Old
        // Stimulation values
        stim[Smpt_Channel_Red].number_of_points = 3; // Set the number of points
        stim[Smpt_Channel_Red].ramp = 3;             // Three lower pre-pulses
        stim[Smpt_Channel_Red].period = 20;          // Frequency: 50 Hz
        // Set the stimulation pulse
        stim[Smpt_Channel_Red].points[0].current = 0;
        stim[Smpt_Channel_Red].points[0].time = 200;
        stim[Smpt_Channel_Red].points[1].time = 200;
        stim[Smpt_Channel_Red].points[2].current = 0;
        stim[Smpt_Channel_Red].points[2].time = 200;

        // Start Process
        printf("Reha Move3 message: Initializing device on port %s... ", port_name_rm); //<<port_name_rm<<endl;
        while (!smpt_next)
        {
            smpt_check = smpt_check_serial_port(port_name_rm);
            smpt_port = smpt_open_serial_port(&device, port_name_rm);
            // Request ID Data
            packet = smpt_packet_number_generator_next(&device);
            smpt_send_get_device_id(&device, packet);

            fill_ml_init(&device, &ml_init);
            smpt_send_ml_init(&device, &ml_init);
            fill_ml_update(&device, &ml_update, Smpt_Channel_Red, stim_act[Smpt_Channel_Red], stim[Smpt_Channel_Red]); // <- comment out this one
            //enable_ml_update(&device, &ml_update, Smpt_Channel_Red, true); //New
            smpt_send_ml_update(&device, &ml_update);
            fill_ml_get_current_data(&device, &ml_get_current_data);
            // This last command check if it's received all the data requested
            smpt_get = smpt_send_ml_get_current_data(&device, &ml_get_current_data);

            // smpt_next = go to next step -> Process running
            smpt_next = smpt_check && smpt_port && smpt_get;
            if (!smpt_next)
            {
                smpt_send_ml_stop(&device, smpt_packet_number_generator_next(&device));
                smpt_close_serial_port(&device);
                if (smpt_check)
                {
                    std::cout << "\nError - Reha Move3: Device does not respond. Turn it on or restart it.\n";
                    turn_on = 2500;
                }
                else
                {
                    std::cout << "\nError - Reha Move3: Device not found. Check connection.\n";
                }
                Sleep(5000); // waits for 5 seconds to give you time to regret your life
                std::cout << "\nReha Move 3 message: Retrying... ";
                smpt_port = true;
            }
            if (abort)
            {
                smpt_end = true;
                break;
            }
        }

        // Run Process
        if (!smpt_end)
        {
            Sleep(turn_on); // wait for it to be properly started
            smpt_port = false;
            fill_ml_init(&device, &ml_init);
            smpt_send_ml_init(&device, &ml_init);
            ready = true;
        }
        // Initialize stim channels values
        for (int k = 0; k < 4; k++)
        {
            stim[k].number_of_points = 3; //* Set the number of points
            stim[k].ramp = 3;             //* Three lower pre-pulses
            stim[k].period = 20;          //* Frequency: 50 Hz
            // Set the stimulation pulse
            stim[k].points[0].current = 5.0;
            stim[k].points[0].time = 200;
            stim[k].points[1].time = 200;
            stim[k].points[2].current = -5.0;
            stim[k].points[2].time = 200;
        }

        printf("Device RehaMove3 ready.\n");
    };
    void update()
    {
        fill_ml_update(&device, &ml_update, Smpt_Channel_Red, stim_act[Smpt_Channel_Red], stim[Smpt_Channel_Red]);
        smpt_send_ml_update(&device, &ml_update);

        fill_ml_get_current_data(&device, &ml_get_current_data);
        smpt_send_ml_get_current_data(&device, &ml_get_current_data);
        active = stim_act[Smpt_Channel_Red];
    };
    //New
    void update2(Smpt_Channel sel_ch)
    {
        fill_ml_update(&device, &ml_update, sel_ch, stim_act[sel_ch], stim[sel_ch]);
        smpt_send_ml_update(&device, &ml_update);

        fill_ml_get_current_data(&device, &ml_get_current_data);
        smpt_send_ml_get_current_data(&device, &ml_get_current_data);
        active = stim_act[Smpt_Channel_Red] || stim_act[Smpt_Channel_Blue] || stim_act[Smpt_Channel_Black] || stim_act[Smpt_Channel_White];
    };

    void enable(Smpt_Channel sel_ch, bool enable)
    {
        stim_act[sel_ch] = enable;

        if (enable)
        {
            printf("Chanel %i is enabled\n", sel_ch);
        }
        else
        {
            printf("Chanel %i is not enabled\n", sel_ch);
        }
    }
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
        if (smpt_check)
        {
            std::cout << "Reha Move3 message: Process finished.\n";
        }
    };
};

// Recorder
class RehaIngest
{
private:
    const char *port_name_ri;
    uint8_t packet_number = 0;
    Smpt_device device_ri = {0};
    Smpt_ml_init ml_init = {0}; // Struct for ml_init command
    //Process variables
    bool smpt_port, smpt_check, smpt_stop, smpt_next, smpt_end;

public:
    bool abort, ready;
    bool data_received, data_start, data_printed;

    // Constructor
    RehaIngest(char *port)
    {
        port_name_ri = port;
        device_ri = {0};
        ml_init = {0};
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
    }
    // Functions
    void init()
    {
        // First step
        std::cout << "Reha Ingest message: Setting communication on port " << port_name_ri << "... ";
        while (!smpt_next)
        {
            smpt_check = smpt_check_serial_port(port_name_ri);
            smpt_port = smpt_open_serial_port(&device_ri, port_name_ri);
            packet_number = smpt_packet_number_generator_next(&device_ri);
            smpt_stop = smpt_send_dl_stop(&device_ri, packet_number);
            smpt_next = smpt_check && smpt_port && smpt_stop;
            if (!smpt_next)
            {
                std::cout << "Error - Reha Ingest: Device not found. Turn it on and/or check connection.\n";
                smpt_stop = smpt_send_dl_stop(&device_ri, packet_number);
                smpt_close_serial_port(&device_ri);
                Sleep(5000); // waits for 5 seconds to give you time to regret your life
                std::cout << "Reha Ingest message: Retrying... ";
            }
            if (abort)
            {
                smpt_end = true;
                break;
            }
        }
        std::cout << "Device RehaIngest found." << endl;
        ready = true;
        recorder_emg1.clear();
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
        std::cout << "Reha Ingest message: Enabling and initializing device... ";
        Smpt_dl_power_module dl_power_module = {0};
        fill_dl_power_module(&device_ri, &dl_power_module);
        smpt_send_dl_power_module(&device_ri, &dl_power_module);

        // wait, because the enabling takes some time (normal up to 4ms)
        Sleep(10);

        while (smpt_new_packet_received(&device_ri))
        {
            handle_dl_packet_global(&device_ri);
        }
        // Third step: initialize device
        Smpt_dl_init dl_init = {0};
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
        std::cout << "Device ready.\n";
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
        }
        smpt_port = smpt_close_serial_port(&device_ri);
        smpt_check = smpt_check_serial_port(port_name_ri);
        ready = false;
        printf("Reha Ingest message: Process finished.\n");
    };
};

// ------------------------------------------------------------------------
#endif // SASLIBdev_H_