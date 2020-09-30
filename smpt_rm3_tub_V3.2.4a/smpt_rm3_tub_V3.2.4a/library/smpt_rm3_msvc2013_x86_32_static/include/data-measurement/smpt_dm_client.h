#ifndef SMPT_DM_CLIENT_H
#define SMPT_DM_CLIENT_H

/***************************************************************************//**
* \file smpt_dm_client.h
* \author Bjoern Kuberski
* \copyright Hasomed GmbH 2013-2017
*
* This file contains functions for using the continuous data measurement.
*******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32) || defined(__linux__)

#include "smpt_dm_definitions.h"
#include "smpt_dm_packet_validity.h"
#include "smpt_client.h"

/***************************************************************************//**
* \brief Builds the dm_init packet and sends it to the serial port.
* \param[in,out] device
* \param[in]     dm_init
* \returns True if the command was send successfully
*******************************************************************************/
SMPT_API bool smpt_send_dm_init(Smpt_device *const device, const Smpt_dm_init *const dm_init);

/***************************************************************************//**
* \brief Builds the dm_start packet and sends it to the serial port.
* \param[in,out] device
* \param[in]     packet_number
* \returns True if the command was send successfully
*******************************************************************************/
SMPT_API bool smpt_send_dm_start(Smpt_device *const device, uint8_t packet_number);

/***************************************************************************//**
* \brief Builds the dm_stop packet and sends it to the serial port.
* \param[in,out] device
* \param[in]     packet_number
* \returns True if the command was send successfully
*******************************************************************************/
SMPT_API bool smpt_send_dm_stop(Smpt_device *const device, uint8_t packet_number);

/***************************************************************************//**
* \brief Writes the last received dm_init_ack response in the given struct
*
* \param[in,out] device
* \param[in,out] dm_init_ack
*******************************************************************************/
SMPT_API bool smpt_get_dm_init_ack(Smpt_device *const device,
                                   Smpt_dm_init_ack *const dm_init_ack);

/***************************************************************************//**
* \brief Writes the last received dm_data packet in the given struct
*
* \param[in,out] device
* \param[in,out] dm_data
*******************************************************************************/
SMPT_API bool smpt_get_dm_data(Smpt_device *const device,
                               Smpt_dm_data *const dm_data);

#endif /* defined(_WIN32) || defined(__linux__) */

#ifdef __cplusplus
}
#endif

#endif /* SMPT_DM_CLIENT_H */

