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
#define JUXTAPROFILE_LOGCOUNT   0
#define JUXTAPROFILE_LOGCOUNT_UUID 0xFFF1
#define JUXTAPROFILE_LOGCOUNT_LEN  4
// Characteristic defines, Meta Count
#define JUXTAPROFILE_METACOUNT   1
#define JUXTAPROFILE_METACOUNT_UUID 0xFFF2
#define JUXTAPROFILE_METACOUNT_LEN  4
// Characteristic defines, Local Time
#define JUXTAPROFILE_LOCALTIME   2
#define JUXTAPROFILE_LOCALTIME_UUID 0xFFF3
#define JUXTAPROFILE_LOCALTIME_LEN  4
// Characteristic defines, Battery Voltage
#define JUXTAPROFILE_VBATT   3
#define JUXTAPROFILE_VBATT_UUID 0xFFF4
#define JUXTAPROFILE_VBATT_LEN  4
// Characteristic defines, Temperature
#define JUXTAPROFILE_TEMP   4
#define JUXTAPROFILE_TEMP_UUID 0xFFF5
#define JUXTAPROFILE_TEMP_LEN  4
// Characteristic defines, Advertise Mode
#define JUXTAPROFILE_ADVMODE   5
#define JUXTAPROFILE_ADVMODE_UUID 0xFFF6
#define JUXTAPROFILE_ADVMODE_LEN  1
// Characteristic defines, Juxta Data
#define JUXTAPROFILE_DATA   6
#define JUXTAPROFILE_DATA_UUID 0xFFF7
#define JUXTAPROFILE_DATA_LEN  128
// Characteristic defines, Command
#define JUXTAPROFILE_COMMAND   7
#define JUXTAPROFILE_COMMAND_UUID 0xFFF8
#define JUXTAPROFILE_COMMAND_LEN  1
// Characteristic defines, Subject
#define JUXTAPROFILE_SUBJECT   8
#define JUXTAPROFILE_SUBJECT_UUID 0xFFF9
#define JUXTAPROFILE_SUBJECT_LEN  16

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
extern uint8 juxtaProfile_logCount[JUXTAPROFILE_LOGCOUNT_LEN];
extern uint8 juxtaProfile_metaCount[JUXTAPROFILE_METACOUNT_LEN];
extern uint8 juxtaProfile_localTime[JUXTAPROFILE_LOCALTIME_LEN];
extern uint8 juxtaProfile_vBatt[JUXTAPROFILE_VBATT_LEN];
extern uint8 juxtaProfile_temp[JUXTAPROFILE_TEMP_LEN];
extern uint8 juxtaProfile_advMode;
extern uint8 juxtaProfile_data[JUXTAPROFILE_DATA_LEN];
extern uint8 juxtaProfile_command;
extern uint8 juxtaProfile_subject[JUXTAPROFILE_SUBJECT_LEN];
/*********************************************************************
 *********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _SERVICES_H_ */
