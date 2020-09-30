#include "smpt_ll_client.h"

static void ll_demux(const char *port_name);

int main(void)
{
    /* EDIT: Change to the virtual com port of your device */
    const char *port_name = "COM5";

    ll_demux(port_name);
    return 0;
}

void ll_demux(const char *port_name)
{
    Smpt_ll_init ll_init = {0};       /* Struct for ll_init command */
    Smpt_ll_channel_config ll_channel_config = {0};   /* Struct for ll_channel_config command */
    Smpt_ll_demux ll_demux = {0};     /* Struct for demux configuration */

    Smpt_device device = {0};
    smpt_open_serial_port(&device, port_name);

    /* Clear ll_init struct and set the data */
    smpt_clear_ll_init(&ll_init);
    ll_init.packet_number = smpt_packet_number_generator_next(&device);

    /* Enable demux */
    ll_init.enable_demux  = true;

    /* Send the ll_init command to stimulation unit */
    smpt_send_ll_init(&device, &ll_init);

    /* Configure the demux and put it into ll_channel_config */
    ll_demux.demux_id = Smpt_Demux_Rt_72;
    ll_demux.electrodes[0]  = Smpt_Ll_Demux_Positive;
    ll_demux.electrodes[60] = Smpt_Ll_Demux_Negative;
    smpt_ll_demux_convert(&ll_channel_config, &ll_demux);

    /* Set the variable for transmitting the demux configuration */
    ll_channel_config.modify_demux = true;

    /* Set the stimulation data */
    ll_channel_config.enable_stimulation = true;
    ll_channel_config.channel = Smpt_Channel_Red;  /* Use blue channel */
    ll_channel_config.number_of_points = 3;         /* Set the number of points*/
    ll_channel_config.packet_number = smpt_packet_number_generator_next(&device);

    /* Set the stimulation pulse */
    /* First point, current: 20 mA, positive, pulse width: 200 µs */
    ll_channel_config.points[0].current =  20;
    ll_channel_config.points[0].time    = 200;

    /* Second point, pause 100 µs */
    ll_channel_config.points[1].time = 100;

    /* Third point, current: -20 mA, negative, pulse width: 200 µs */
    ll_channel_config.points[2].current = -20;
    ll_channel_config.points[2].time    = 200;

    /* Send the ll_channel_list command to the stimulation unit */
    smpt_send_ll_channel_config(&device, &ll_channel_config);

    /* Send the ll_stop command to the stimulation unit */
    smpt_send_ll_stop(&device, smpt_packet_number_generator_next(&device));

    smpt_close_serial_port(&device);
}
