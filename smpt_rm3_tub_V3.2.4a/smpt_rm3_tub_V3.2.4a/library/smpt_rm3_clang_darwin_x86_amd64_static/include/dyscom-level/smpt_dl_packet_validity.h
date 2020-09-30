#ifndef SMPT_DL_PACKET_VALIDITY_H
#define SMPT_DL_PACKET_VALIDITY_H

/***************************************************************************//**
* \file smpt_dl_packet_validity.h
* \author Christoph Petzold
* \copyright Hasomed GmbH 2014-2016
*
* This file contains functions for validity checks of the dyscom-level command
* structs.
*******************************************************************************/

#include "smpt_dl_definitions_data_types.h"

/***************************************************************************//**
* \brief Checks the validity of the passed struct.
*
* \param[in] dl_init
* \returns True if all parameters are valid.
*******************************************************************************/
bool smpt_is_valid_dl_init(const Smpt_dl_init *const dl_init);

/***************************************************************************//**
* \brief Checks the validity of the passed struct.
*
* \param[in] dl_init_ack
* \returns True if all parameters are valid.
*******************************************************************************/
bool smpt_is_valid_dl_init_ack(const Smpt_dl_init_ack *const dl_init_ack);

/***************************************************************************//**
* \brief Checks the validity of the passed struct.
*
* \param[in] dl_power_module_ack
* \returns True if all parameters are valid.
*******************************************************************************/
bool smpt_is_valid_dl_power_module_ack(const Smpt_dl_power_module_ack *const dl_pwer_module_ack);

/***************************************************************************//**
* \brief Checks the validity of the passed struct.
*
* \param[in] dl_start_ack
* \returns True if all parameters are valid.
*******************************************************************************/
bool smpt_is_valid_dl_start_ack(const Smpt_dl_start_ack *const dl_start_ack);

bool smpt_is_valid_dl_send_mmi(const Smpt_dl_send_mmi *const dl_send_mmi);

/***************************************************************************//**
* \brief Checks the validity of the passed struct.
*
* \param[in] dl_stop_ack
* \returns True if all parameters are valid.
*******************************************************************************/
bool smpt_is_valid_dl_stop_ack(const Smpt_dl_stop_ack *const dl_stop_ack);

/***************************************************************************//**
* \brief Checks the validity of the passed struct.
*
* \param[in] dl_get
* \returns True if all parameters are valid.
*******************************************************************************/
bool smpt_is_valid_dl_get(const Smpt_dl_get *const dl_get);

/***************************************************************************//**
* \brief Checks the validity of the passed struct.
*
* \param[in] dl_power_module
* \returns True if all parameters are valid.
*******************************************************************************/
bool smpt_is_valid_dl_power_module(const Smpt_dl_power_module*const dl_power_module);

/***************************************************************************//**
* \brief Checks the validity of the passed struct.
*
* \param[in] dl_get_ack
* \returns True if all parameters are valid.
*******************************************************************************/
bool smpt_is_valid_dl_get_ack(const Smpt_dl_get_ack *const dl_get_ack);

/***************************************************************************//**
* \brief Checks the validity of the passed struct.
*
* \param[in] dl_send_file
* \returns True if all parameters are valid.
*******************************************************************************/
bool smpt_is_valid_dl_send_file(const Smpt_dl_send_file *const dl_send_file);

/***************************************************************************//**
* \brief Checks the validity of the passed struct.
*
* \param[in] live_data
* \returns True if all parameters are valid.
*******************************************************************************/
bool smpt_is_valid_dl_send_live_data(const Smpt_dl_send_live_data *const live_data);

/***************************************************************************//**
* \brief Checks the validity of the passed struct.
*
* \param[in] dl_sys
* \returns True if all parameters are valid.
*******************************************************************************/
bool smpt_is_valid_dl_sys(const Smpt_dl_sys *const dl_sys);


/***************************************************************************//**
* \brief Checks the validity of the passed struct.
*
* \param[in] dl_sys_ack
* \returns True if all parameters are valid.
*******************************************************************************/
bool smpt_is_valid_dl_sys_ack(const Smpt_dl_sys_ack *const dl_sys_ack);
#endif /* SMPT_DL_PACKET_VALIDITY_H */
