#ifndef SMPT_DM_PACKET_VALIDITY_H
#define SMPT_DM_PACKET_VALIDITY_H

/***************************************************************************//**
* \file smpt_dm_packet_validity.h
* \author Bjoern Kuberski
* \copyright Hasomed GmbH 2013-2017
*
* This file contains functions for validity checks of the command structs.
*******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include "smpt_dm_definitions.h"

SMPT_API bool smpt_is_valid_dm_init(const Smpt_dm_init *const dm_init);

SMPT_API bool smpt_is_valid_dm_init_ack(const Smpt_dm_init_ack *const dm_init_ack);

SMPT_API bool smpt_is_valid_dm_data(const Smpt_dm_data *const dm_data);

#ifdef __cplusplus
}
#endif

#endif /* SMPT_DM_PACKET_VALIDITY_H */

