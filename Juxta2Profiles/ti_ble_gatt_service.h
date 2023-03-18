/*******************************************************************************
 * Filename:    ble_gatt_service.h
 *
 * Description:    This file contains the services definitions and
 *                 prototypes.
 *
 * Copyright (c) 2015-2019, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************/

#ifndef _SERVICES_H_
#define _SERVICES_H_

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************
 * INCLUDES
 *********************************************************************/
#include <stdint.h>
#include <bcomdef.h>
/*********************************************************************
 * CONSTANTS
 *********************************************************************/
// Profile Parameters
// Service UUID
#define SIMPLEPROFILE_SERV_UUID 0xFFF0
// Characteristic defines, Log Count
#define SIMPLEPROFILE_CHAR1   0
#define SIMPLEPROFILE_CHAR1_UUID 0xFFF1
#define SIMPLEPROFILE_CHAR1_LEN  4
// Characteristic defines, Meta Count
#define SIMPLEPROFILE_CHAR2   1
#define SIMPLEPROFILE_CHAR2_UUID 0xFFF2
#define SIMPLEPROFILE_CHAR2_LEN  4
// Characteristic defines, Local Time
#define SIMPLEPROFILE_CHAR3   2
#define SIMPLEPROFILE_CHAR3_UUID 0xFFF3
#define SIMPLEPROFILE_CHAR3_LEN  4
// Characteristic defines, Battery Voltage
#define SIMPLEPROFILE_CHAR4   3
#define SIMPLEPROFILE_CHAR4_UUID 0xFFF4
#define SIMPLEPROFILE_CHAR4_LEN  4
// Characteristic defines, Temperature
#define SIMPLEPROFILE_CHAR5   4
#define SIMPLEPROFILE_CHAR5_UUID 0xFFF5
#define SIMPLEPROFILE_CHAR5_LEN  4
// Characteristic defines, Advertise Mode
#define SIMPLEPROFILE_CHAR6   5
#define SIMPLEPROFILE_CHAR6_UUID 0xFFF6
#define SIMPLEPROFILE_CHAR6_LEN  1
// Characteristic defines, Juxta Data
#define SIMPLEPROFILE_CHAR7   6
#define SIMPLEPROFILE_CHAR7_UUID 0xFFF7
#define SIMPLEPROFILE_CHAR7_LEN  128

/*********************************************************************
 * TYPEDEFS
 *********************************************************************/

/*********************************************************************
 * MACROS
 *********************************************************************/

/*********************************************************************
 * Profile Callbacks
 *********************************************************************/

// Callback when a characteristic value has changed
typedef void (*simpleProfileChange_t)(uint8_t paramID);

typedef struct
{
    simpleProfileChange_t pfnSimpleProfileChange; // Called when characteristic value changes
    simpleProfileChange_t pfnCfgChangeCb;
} simpleProfileCBs_t;
/*********************************************************************
 * API FUNCTIONS
 *********************************************************************/
/*
 * _AddService- Initializes the service by registering
 *          GATT attributes with the GATT server.
 *
 */
extern bStatus_t simpleProfile_AddService(uint32 services);

/*
 * _RegisterAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
extern bStatus_t simpleProfile_RegisterAppCBs(simpleProfileCBs_t *appCallbacks);

/*
 * _SetParameter - Set a service parameter.
 *
 *    param - Profile parameter ID
 *    len - length of data to right
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 */
extern bStatus_t simpleProfile_SetParameter(uint8 param, uint8 len,
                                            void *value);

/*
 * _GetParameter - Get a service parameter.
 *
 *    param - Profile parameter ID
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 */
extern bStatus_t simpleProfile_GetParameter(uint8 param, void *value);

/*****************************************************
 Extern variables
 *****************************************************/
extern simpleProfileCBs_t *simpleProfile_AppCBs;
extern uint8 simpleProfileChar1[SIMPLEPROFILE_CHAR1_LEN];
extern uint8 simpleProfileChar2[SIMPLEPROFILE_CHAR2_LEN];
extern uint8 simpleProfileChar3[SIMPLEPROFILE_CHAR3_LEN];
extern uint8 simpleProfileChar4[SIMPLEPROFILE_CHAR4_LEN];
extern uint8 simpleProfileChar5[SIMPLEPROFILE_CHAR5_LEN];
extern uint8 simpleProfileChar6;
extern uint8 simpleProfileChar7[SIMPLEPROFILE_CHAR7_LEN];
/*********************************************************************
 *********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _SERVICES_H_ */
