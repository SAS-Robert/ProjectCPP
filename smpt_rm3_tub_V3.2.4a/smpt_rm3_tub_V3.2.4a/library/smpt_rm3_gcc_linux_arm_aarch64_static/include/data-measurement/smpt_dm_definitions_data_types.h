#ifndef SMPT_DM_DEFINITIONS_DATA_TYPES_H
#define SMPT_DM_DEFINITIONS_DATA_TYPES_H

/***************************************************************************//**
* \file smpt_dm_definitions_data_types.h
* \author Bjoern Kuberski
* \copyright Hasomed GmbH 2013-2017
*
* This file defines structs and enum for the data measurement (dm) used for
* EMG and other measurements.
*******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include "smpt_definitions.h"

typedef enum
{
    Smpt_Length_Dm_Number_Of_Slots     = 10,
    Smpt_Length_Dm_Number_Of_Registers = 26,
    Smpt_Length_Dm_Max_Meta_Data       = 512,
    Smpt_Length_Dm_Number_of_Samples   = 50

} Smpt_Dm_Length;

typedef enum
{
    Smpt_Dm_Registers_Type_None = 0,
    Smpt_Dm_Registers_Type_Emg  = 1 /**< The EMG Chip of type ADS129x, 26 registers */
}
Smpt_Dm_Registers_Type;

typedef enum
{
    Smpt_Dm_Slot_None          = 0, /**< Slot is unused */
    Smpt_Dm_Slot_Emg_Raw_Red   = 1, /**< Raw EMG value, value_int is used */
    Smpt_Dm_Slot_Emg_Raw_Blue  = 2, /**< Raw EMG value, value_int is used */
    Smpt_Dm_Slot_Emg_Raw_Black = 3, /**< Raw EMG value, value_int is used (not yet possible) */
    Smpt_Dm_Slot_Emg_Raw_White = 4, /**< Raw EMG value, value_int is used (not yet possible) */
    Smpt_Dm_Slot_Emg_mV_Red    = 5, /**< EMG Value in mV, value_float is used (not yet implemented) */
    Smpt_Dm_Slot_Emg_mV_Blue   = 6, /**< EMG Value in mV, value_float is used (not yet implemented) */
    Smpt_Dm_Slot_Emg_mV_Black  = 7, /**< EMG Value in mV, value_float is used (not yet implemented) */
    Smpt_Dm_Slot_Emg_mV_White  = 8, /**< EMG Value in mV, value_float is used (not yet implemented) */

    Smpt_Dm_Slot_Last_Item     = Smpt_Dm_Slot_Emg_mV_White
    /* maybe added in future revisions: filtered EMG, temperature, 9D sensor data */
} Smpt_Dm_Slot_Type;

typedef enum
{
    Smpt_Dm_Electrode_Status_None           = 0,
    Smpt_Dm_Electrode_Status_Positive       = 1, /**< (1 adhesive, 0 off) for positive electrode */
    Smpt_Dm_Electrode_Status_Negative       = 2, /**< (1 adhesive, 0 off) for negative electrode */
    Smpt_Dm_Electrode_Status_Maximum_Valid  = 3  /**< (1, 1) means both electrodes are adhesive. */
} Smpt_Dm_Electrode_Status;

typedef struct
{
    Smpt_Dm_Slot_Type type;              /**< type of the slot */
    uint8_t           number_of_samples; /**< Number of samples in the Smpt_dm_data packet during
                                              measurement. If the number is too large the dm_init
                                              is invalid. Maximum: Smpt_Length_Dm_Number_of_Samples
                                            */
} Smpt_dm_slot_init; /**< \struct Smpt_dm_slot
                     Contains the configuration for a slot (selects the channel). */

typedef struct
{
    uint8_t  packet_number;     /**< [0 - 63] Packet number */

    uint16_t meta_data_length;  /**< [0 - Smpt_Length_Dm_Max_Meta_Data] String length of meta data
                                                                        (not yet implemented) */
    char     meta_data[Smpt_Length_Dm_Max_Meta_Data]; /**< Meta data (not yet implemented) */

    uint8_t           number_of_slots; /**< [0 - Smpt_Length_Dm_Number_Of_Slots] */
    Smpt_dm_slot_init slots_init[Smpt_Length_Dm_Number_Of_Slots]; /**< \see Smpt_dm_slot_init */

    bool do_not_send_live_data; /**< [true, false] If true then live data is NOT send during
                                                   measurement. */
    bool write_to_memory_card; /**< [true, false] If true then the measured data will be written on
                                                  the memory card (not yet implemented). */
    bool sync_with_stimulation; /**< [true, false] If true then the measured data will be sent
                                                   after the stimulation. */
    bool read_registers;  /**< [true, false] If true, the registers will be read. */
    bool write_registers; /**< [true, false] If true, the registers are transfered
                                             and written to the measurement chip. */
    Smpt_Dm_Registers_Type registers_type;  /**< \see Smpt_Dm_Registers_Type */
    uint8_t                registers[Smpt_Length_Dm_Number_Of_Registers]; /**< Array of registers
                                                                               to be written */
} Smpt_dm_init; /**< \struct Smpt_dm_init
                     Contains the initialisation data for the measurement. */

typedef struct
{
    uint8_t     packet_number; /**< Packet number echo */
    Smpt_Result result;        /**< \see Smpt_Result */

    bool read_registers; /**< [true, false] If true, the registers have been read and are transfered */

    Smpt_Dm_Registers_Type registers_type; /**< Registers type echo */
    uint8_t                registers[Smpt_Length_Dm_Number_Of_Registers]; /**< Registers read from
                                                                               the selected chip */
    uint16_t file_name_length; /**< String length of the file name */
    char     file_name[Smpt_Length_File_Name]; /**< File name of the measurement on sd card. */

} Smpt_dm_init_ack; /**< \struct Smpt_dm_init_ack
                     Contains the response for the Smpt_dm_init command. */

typedef struct
{
    int32_t value_int;   /**< Measurement value for some  slot types  */
    float   value_float; /**< Measurement value for other slot types  */
    bool    stim_status[Smpt_Length_Number_Of_Channels]; /**< [true, false] If true the was
                                                              stimulation on the channel (index) */
} Smpt_dm_sample; /**< \struct Smpt_dm_sample
                     Contains one measurement value and the stimulation status at that time. */

typedef struct
{
    uint8_t           number_of_samples; /**< Number of sample in this slot */
    Smpt_dm_sample    samples[Smpt_Length_Dm_Number_of_Samples]; /**< \see Smpt_dm_sample */
    Smpt_Dm_Slot_Type type;  /**< Type from initialization \see Smpt_Dm_Slot_Type */

} Smpt_dm_slot; /**< \struct Smpt_dm_slot
                     Contains the measurement value for each slot */

typedef struct
{
    uint8_t      packet_number;      /**< [0 - 63] Packet number */

    uint8_t      number_of_slots;    /**< Number of slots as specified in dm_init */
    Smpt_dm_slot slots_dm[Smpt_Length_Dm_Number_Of_Slots]; /**< Values for each slot */

    uint8_t      electrode_status; /**< (not yet implemented) */

} Smpt_dm_data;


/***************************************************************************//**
* \brief Clears the struct (sets all values to 0).
*
* \param[in] dm_init
*******************************************************************************/
SMPT_API void smpt_clear_dm_init(Smpt_dm_init *const dm_init);

/***************************************************************************//**
* \brief Clears the struct (sets all values to 0).
*
* \param[in] dm_init_ack
*******************************************************************************/
SMPT_API void smpt_clear_dm_init_ack(Smpt_dm_init_ack *const dm_init_ack);

/***************************************************************************//**
* \brief Clears the struct (sets all values to 0).
*
* \param[in] dm_data
*******************************************************************************/
SMPT_API void smpt_clear_dm_data(Smpt_dm_data *const dm_data);

/***************************************************************************//**
* \brief Checks if a slot type has int as data type.
*
* \param[in] slot_type
* \returns True, if slot_type uses value_int in Smpt_dm_sample, else false
*******************************************************************************/
SMPT_API bool smpt_dm_is_slot_type_int(Smpt_Dm_Slot_Type slot_type);

/***************************************************************************//**
* \brief Checks if a slot type has float as data type.
*
* \param[in] slot_type
* \returns True, if slot_type uses value_float in Smpt_dm_sample, else false
*******************************************************************************/
SMPT_API bool smpt_dm_is_slot_type_float(Smpt_Dm_Slot_Type slot_type);

#ifdef __cplusplus
}
#endif

#endif /* SMPT_DM_DEFINITIONS_DATA_TYPES_H */
