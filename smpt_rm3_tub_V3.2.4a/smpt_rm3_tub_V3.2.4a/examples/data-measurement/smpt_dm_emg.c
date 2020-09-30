#include <stdio.h>
#include "smpt_dm_client.h"

static void test_dm_emg(const char *port_name);

static void send_dm_init(Smpt_device *const device);
static void get_dm_init_ack(Smpt_device *const device);

static void send_dm_start(Smpt_device *const device);
static void get_dm_data(Smpt_device *const device);
static void send_dm_stop(Smpt_device *const device);

int main(void)
{
    const char *port_name = "COM5";
    test_dm_emg(port_name);
    return 0;
}

void test_dm_emg(const char *port_name)
{   
    Smpt_device device = {0};
    smpt_open_serial_port(&device, port_name);

    send_dm_init(&device);
    get_dm_init_ack(&device);

    send_dm_start(&device);

    get_dm_data(&device);
    /* get more data */

    send_dm_stop(&device);
}

void send_dm_init(Smpt_device *const device)
{
    Smpt_dm_init dm_init = {0};
    smpt_clear_dm_init(&dm_init);

    dm_init.number_of_slots = 2;
    dm_init.slots_init[0].type              = Smpt_Dm_Slot_Emg_Raw_Red;
    dm_init.slots_init[0].number_of_samples = 1;
    dm_init.slots_init[1].type              = Smpt_Dm_Slot_Emg_Raw_Blue;
    dm_init.slots_init[1].number_of_samples = 1;

    dm_init.packet_number = smpt_packet_number_generator_next(device);

    if (!smpt_send_dm_init(device, &dm_init))
    {
        printf("Could not send dm init\n");
    }
}

void get_dm_init_ack(Smpt_device *const device)
{
    Smpt_dm_init_ack dm_init_ack = {0};
    smpt_get_dm_init_ack(device, &dm_init_ack);
}

void send_dm_start(Smpt_device *const device)
{
    if (!smpt_send_dm_start(device, smpt_packet_number_generator_next(device)))
    {
        printf("Could not send dm start\n");
    }
}

void get_dm_data(Smpt_device *const device)
{
    Smpt_dm_data dm_data = {0};
    int i, n;
    for (i = 0; i < dm_data.number_of_slots; i++)
    {
        Smpt_dm_slot slot = dm_data.slots_dm[i];

        for (n = 0; n < slot.number_of_samples; n++)
        {
            Smpt_dm_sample sample = slot.samples[n];

            int value = sample.value_int;
            /* do something with value */
            value++;
        }
    }

    smpt_get_dm_data(device, &dm_data);
}

void send_dm_stop(Smpt_device *const device)
{
    if (!smpt_send_dm_stop(device, smpt_packet_number_generator_next(device)))
    {
        printf("Could not send dm stop\n");
    }
}
