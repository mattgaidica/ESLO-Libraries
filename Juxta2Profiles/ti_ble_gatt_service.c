/*******************************************************************************
 * Filename:    ble_gatt_service.c
 *
 * Description:    This file contains the implementation of the service.
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

/*********************************************************************
 * INCLUDES
 *********************************************************************/
#include <string.h>
#include <icall.h>
#include "util.h"
/* This Header file contains all BLE API and icall structure definition */
#include "icall_ble_api.h"
#include "ti_ble_gatt_service.h"

#ifdef SYSCFG
#include "ti_ble_config.h"
#endif
/*********************************************************************
 * MACROS
 *********************************************************************/

/*********************************************************************
 * CONSTANTS
 *********************************************************************/

/*********************************************************************
 * TYPEDEFS
 *********************************************************************/

/*********************************************************************
 * GLOBAL VARIABLES
 *********************************************************************/

// simpleProfile Service UUID
CONST uint8 simpleProfileServUUID[ATT_BT_UUID_SIZE] = {
        LO_UINT16(SIMPLEPROFILE_SERV_UUID), HI_UINT16(SIMPLEPROFILE_SERV_UUID) };

// Char1 UUID
CONST uint8 juxtaProfile_logCountUUID[ATT_BT_UUID_SIZE] = {
        LO_UINT16(JUXTAPROFILE_LOGCOUNT_UUID), HI_UINT16(
                JUXTAPROFILE_LOGCOUNT_UUID) };

// Char2 UUID
CONST uint8 juxtaProfile_metaCountUUID[ATT_BT_UUID_SIZE] = {
        LO_UINT16(JUXTAPROFILE_METACOUNT_UUID), HI_UINT16(
                JUXTAPROFILE_METACOUNT_UUID) };

// Char3 UUID
CONST uint8 juxtaProfile_localTimeUUID[ATT_BT_UUID_SIZE] = {
        LO_UINT16(JUXTAPROFILE_LOCALTIME_UUID), HI_UINT16(
                JUXTAPROFILE_LOCALTIME_UUID) };

// Char4 UUID
CONST uint8 juxtaProfile_vBattUUID[ATT_BT_UUID_SIZE] = {
        LO_UINT16(JUXTAPROFILE_VBATT_UUID), HI_UINT16(JUXTAPROFILE_VBATT_UUID) };

// Char5 UUID
CONST uint8 juxtaProfile_tempUUID[ATT_BT_UUID_SIZE] = {
        LO_UINT16(JUXTAPROFILE_TEMP_UUID), HI_UINT16(JUXTAPROFILE_TEMP_UUID) };

// Char6 UUID
CONST uint8 juxtaProfile_advModeUUID[ATT_BT_UUID_SIZE] = {
        LO_UINT16(JUXTAPROFILE_ADVMODE_UUID), HI_UINT16(
                JUXTAPROFILE_ADVMODE_UUID) };

// Char7 UUID
CONST uint8 juxtaProfile_dataUUID[ATT_BT_UUID_SIZE] = {
        LO_UINT16(JUXTAPROFILE_DATA_UUID), HI_UINT16(JUXTAPROFILE_DATA_UUID) };

// Char8 UUID
CONST uint8 juxtaProfile_commandUUID[ATT_BT_UUID_SIZE] = {
        LO_UINT16(JUXTAPROFILE_COMMAND_UUID), HI_UINT16(
                JUXTAPROFILE_COMMAND_UUID) };

// Char9 UUID
CONST uint8 juxtaProfile_subjectUUID[ATT_BT_UUID_SIZE] = {
        LO_UINT16(JUXTAPROFILE_SUBJECT_UUID), HI_UINT16(
                JUXTAPROFILE_SUBJECT_UUID) };

/*********************************************************************
 * LOCAL VARIABLES
 *********************************************************************/
simpleProfileCBs_t *simpleProfile_AppCBs = NULL;

/*********************************************************************
 * Profile Attributes - variables
 *********************************************************************/

// Service declaration
static CONST gattAttrType_t simpleProfileService = { ATT_BT_UUID_SIZE,
                                                     simpleProfileServUUID };

// Characteristic "Char1" Properties (for declaration)
static uint8 juxtaProfile_logCountProps = GATT_PROP_READ | GATT_PROP_WRITE;
// Characteristic "Char1" Value variable
uint8 juxtaProfile_logCount[JUXTAPROFILE_LOGCOUNT_LEN] = { 0 };
// Characteristic "Char1" User Description
static uint8 juxtaProfile_logCountUserDesp[] = "Log Count";

// Characteristic "Char2" Properties (for declaration)
static uint8 juxtaProfile_metaCountProps = GATT_PROP_READ | GATT_PROP_WRITE;
// Characteristic "Char2" Value variable
uint8 juxtaProfile_metaCount[JUXTAPROFILE_METACOUNT_LEN] = { 0 };
// Characteristic "Char2" User Description
static uint8 juxtaProfile_metaCountUserDesp[] = "Meta Count";

// Characteristic "Char3" Properties (for declaration)
static uint8 juxtaProfile_localTimeProps = GATT_PROP_READ | GATT_PROP_WRITE;
// Characteristic "Char3" Value variable
uint8 juxtaProfile_localTime[JUXTAPROFILE_LOCALTIME_LEN] = { 0 };
// Characteristic "Char3" User Description
static uint8 juxtaProfile_localTimeUserDesp[] = "Local Time";

// Characteristic "Char4" Properties (for declaration)
static uint8 juxtaProfile_vBattProps = GATT_PROP_READ | GATT_PROP_NOTIFY;
// Characteristic "Char4" Value variable
uint8 juxtaProfile_vBatt[JUXTAPROFILE_VBATT_LEN] = { 0 };
// Characteristic "Char4" User Description
static uint8 juxtaProfile_vBattUserDesp[] = "Battery Voltage";
// Characteristic "Char4" CCCD
static gattCharCfg_t *juxtaProfile_vBattConfig;

// Characteristic "Char5" Properties (for declaration)
static uint8 juxtaProfile_tempProps = GATT_PROP_READ | GATT_PROP_NOTIFY;
// Characteristic "Char5" Value variable
uint8 juxtaProfile_temp[JUXTAPROFILE_TEMP_LEN] = { 0 };
// Characteristic "Char5" User Description
static uint8 juxtaProfile_tempUserDesp[] = "Temperature";
// Characteristic "Char5" CCCD
static gattCharCfg_t *juxtaProfile_tempConfig;

// Characteristic "Char6" Properties (for declaration)
static uint8 juxtaProfile_advModeProps = GATT_PROP_READ | GATT_PROP_WRITE;
// Characteristic "Char6" Value variable
uint8 juxtaProfile_advMode = 0x0;
// Characteristic "Char6" User Description
static uint8 juxtaProfile_advModeUserDesp[] = "Advertise Mode";

// Characteristic "Char7" Properties (for declaration)
static uint8 juxtaProfile_dataProps = GATT_PROP_NOTIFY;
// Characteristic "Char7" Value variable
uint8 juxtaProfile_data[JUXTAPROFILE_DATA_LEN] = { 0 };
// Characteristic "Char7" User Description
static uint8 juxtaProfile_dataUserDesp[] = "Juxta Data";
// Characteristic "Char7" CCCD
static gattCharCfg_t *juxtaProfile_dataConfig;

// Characteristic "Char8" Properties (for declaration)
static uint8 juxtaProfile_commandProps = GATT_PROP_WRITE;
// Characteristic "Char8" Value variable
uint8 juxtaProfile_command = 0x0;
// Characteristic "Char8" User Description
static uint8 juxtaProfile_commandUserDesp[] = "Command";

// Characteristic "Char9" Properties (for declaration)
static uint8 juxtaProfile_subjectProps = GATT_PROP_READ | GATT_PROP_WRITE;
// Characteristic "Char9" Value variable
uint8 juxtaProfile_subject[JUXTAPROFILE_SUBJECT_LEN] = { 0 };
// Characteristic "Char9" User Description
static uint8 juxtaProfile_subjectUserDesp[] = "Subject";

/*********************************************************************
 * Profile Attributes - Table
 *********************************************************************/
static gattAttribute_t simpleProfileAttrTbl[] = { {
        { ATT_BT_UUID_SIZE, primaryServiceUUID },
        GATT_PERMIT_READ,
        0, (uint8*) &simpleProfileService },

                                                  // LOG COUNT
                                                  // Char1 Characteristic Declaration
        { { ATT_BT_UUID_SIZE, characterUUID },
        GATT_PERMIT_READ,
          0, &juxtaProfile_logCountProps },

        // Char1 Characteristic Value
        { { ATT_BT_UUID_SIZE, juxtaProfile_logCountUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
          0, juxtaProfile_logCount },

        // Characteristic Char1 User Description
        { { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
          0, juxtaProfile_logCountUserDesp },

        // META COUNT
        // Char2 Characteristic Declaration
        { { ATT_BT_UUID_SIZE, characterUUID },
        GATT_PERMIT_READ,
          0, &juxtaProfile_metaCountProps },

        // Char2 Characteristic Value
        { { ATT_BT_UUID_SIZE, juxtaProfile_metaCountUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
          0, juxtaProfile_metaCount },

        // Characteristic Char2 User Description
        { { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
          0, juxtaProfile_metaCountUserDesp },

        // LOCAL TIME
        // Char3 Characteristic Declaration
        { { ATT_BT_UUID_SIZE, characterUUID },
        GATT_PERMIT_READ,
          0, &juxtaProfile_localTimeProps },

        // Char3 Characteristic Value
        { { ATT_BT_UUID_SIZE, juxtaProfile_localTimeUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
          0, juxtaProfile_localTime },

        // Characteristic Char3 User Description
        { { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
          0, juxtaProfile_localTimeUserDesp },

        // BATTERY
        // Char4 Characteristic Declaration
        { { ATT_BT_UUID_SIZE, characterUUID },
        GATT_PERMIT_READ,
          0, &juxtaProfile_vBattProps },

        // Char4 Characteristic Value
        { { ATT_BT_UUID_SIZE, juxtaProfile_vBattUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
          0, juxtaProfile_vBatt },

        // Characteristic Char4 User Description
        { { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
          0, juxtaProfile_vBattUserDesp },

        // Char4 configuration
        { { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
          0, (uint8*) &juxtaProfile_vBattConfig },

        // TEMPERATURE
        // Char5 Characteristic Declaration
        { { ATT_BT_UUID_SIZE, characterUUID },
        GATT_PERMIT_READ,
          0, &juxtaProfile_tempProps },

        // Char5 Characteristic Value
        { { ATT_BT_UUID_SIZE, juxtaProfile_tempUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
          0, juxtaProfile_temp },

        // Characteristic Char5 User Description
        { { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
          0, juxtaProfile_tempUserDesp },

        // Char5 configuration
        { { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
          0, (uint8*) &juxtaProfile_tempConfig },

        // ADVERTISE
        // Char6 Characteristic Declaration
        { { ATT_BT_UUID_SIZE, characterUUID },
        GATT_PERMIT_READ,
          0, &juxtaProfile_advModeProps },

        // Char6 Characteristic Value
        { { ATT_BT_UUID_SIZE, juxtaProfile_advModeUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
          0, &juxtaProfile_advMode },

        // Characteristic Char6 User Description
        { { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
          0, juxtaProfile_advModeUserDesp },

        // DATA
        // Char7 Characteristic Declaration
        { { ATT_BT_UUID_SIZE, characterUUID },
        GATT_PERMIT_READ,
          0, &juxtaProfile_dataProps },

        // Char7 Characteristic Value
        { { ATT_BT_UUID_SIZE, juxtaProfile_dataUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
          0, juxtaProfile_data },

        // Characteristic Char7 User Description
        { { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
          0, juxtaProfile_dataUserDesp },

        // Char7 configuration
        { { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
          0, (uint8*) &juxtaProfile_dataConfig },

        // COMMAND
        // Char8 Characteristic Declaration
        { { ATT_BT_UUID_SIZE, characterUUID },
        GATT_PERMIT_READ,
          0, &juxtaProfile_commandProps },

        // Char8 Characteristic Value
        { { ATT_BT_UUID_SIZE, juxtaProfile_commandUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
          0, &juxtaProfile_command },

        // Characteristic Char8 User Description
        { { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
          0, juxtaProfile_commandUserDesp },

        // SUBJECT
        // Char3 Characteristic Declaration
        { { ATT_BT_UUID_SIZE, characterUUID },
        GATT_PERMIT_READ,
          0, &juxtaProfile_subjectProps },

        // Char3 Characteristic Value
        { { ATT_BT_UUID_SIZE, juxtaProfile_subjectUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
          0, juxtaProfile_subject },

        // Characteristic Char3 User Description
        { { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
          0, juxtaProfile_subjectUserDesp },

};

/*********************************************************************
 * LOCAL FUNCTIONS
 *********************************************************************/
static bStatus_t gatt_simpleProfile_ReadAttrCB(uint16_t connHandle,
                                               gattAttribute_t *pAttr,
                                               uint8_t *pValue, uint16_t *pLen,
                                               uint16_t offset, uint16_t maxLen,
                                               uint8_t method);
static bStatus_t gatt_simpleProfile_WriteAttrCB(uint16_t connHandle,
                                                gattAttribute_t *pAttr,
                                                uint8_t *pValue, uint16_t len,
                                                uint16_t offset, uint8_t method);
static bStatus_t gatt_simpleProfile_AuthorizationCB(uint16_t connHandle,
                                                    gattAttribute_t *pAttr,
                                                    uint8_t opcode);

extern bStatus_t simpleProfile_ReadAttrCB(uint16_t connHandle,
                                          gattAttribute_t *pAttr,
                                          uint8_t *pValue, uint16_t *pLen,
                                          uint16_t offset, uint16_t maxLen,
                                          uint8_t method);
extern bStatus_t simpleProfile_WriteAttrCB(uint16_t connHandle,
                                           gattAttribute_t *pAttr,
                                           uint8_t *pValue, uint16_t len,
                                           uint16_t offset, uint8_t method);

/*********************************************************************
 * PROFILE CALLBACKS
 *********************************************************************/
// simpleProfile Service Callbacks
CONST gattServiceCBs_t simpleProfileCBs = { gatt_simpleProfile_ReadAttrCB, // Read callback function pointer
        gatt_simpleProfile_WriteAttrCB, // Write callback function pointer
        gatt_simpleProfile_AuthorizationCB // Authorization callback function pointer
        };

/*********************************************************************
 * PUBLIC FUNCTIONS
 *********************************************************************/
/*
 * AddService - Initializes the services by registering
 * GATT attributes with the GATT server.
 *
 */
bStatus_t simpleProfile_AddService(uint32 services)
{
    uint8 status;

    // BATTERY
    // Allocate Client Characteristic Configuration table
    juxtaProfile_vBattConfig = (gattCharCfg_t*) ICall_malloc(
            sizeof(gattCharCfg_t) * MAX_NUM_BLE_CONNS);
    if (juxtaProfile_vBattConfig == NULL)
    {
        return ( bleMemAllocError);
    }
    // Initialize Client Characteristic Configuration attributes
    GATTServApp_InitCharCfg( LINKDB_CONNHANDLE_INVALID,
                            juxtaProfile_vBattConfig);

    // TEMPERATURE
    // Allocate Client Characteristic Configuration table
    juxtaProfile_tempConfig = (gattCharCfg_t*) ICall_malloc(
            sizeof(gattCharCfg_t) * MAX_NUM_BLE_CONNS);
    if (juxtaProfile_tempConfig == NULL)
    {
        return ( bleMemAllocError);
    }
    // Initialize Client Characteristic Configuration attributes
    GATTServApp_InitCharCfg( LINKDB_CONNHANDLE_INVALID,
                            juxtaProfile_tempConfig);

    // Allocate Client Characteristic Configuration table
    juxtaProfile_dataConfig = (gattCharCfg_t*) ICall_malloc(
            sizeof(gattCharCfg_t) * MAX_NUM_BLE_CONNS);
    if (juxtaProfile_dataConfig == NULL)
    {
        return ( bleMemAllocError);
    }
    // Initialize Client Characteristic Configuration attributes
    GATTServApp_InitCharCfg( LINKDB_CONNHANDLE_INVALID,
                            juxtaProfile_dataConfig);
    if (services)
    {
        // Register GATT attribute list and CBs with GATT Server App
        status = GATTServApp_RegisterService(
                simpleProfileAttrTbl, GATT_NUM_ATTRS( simpleProfileAttrTbl ),
                GATT_MAX_ENCRYPT_KEY_SIZE, &simpleProfileCBs);
    }
    else
    {
        status = SUCCESS;
    }
    return (status);
}

/***************************************************************
 * RegisterAppCBs - Registers the application callback function.
 *                  Only call this function once.
 *
 * appCallbacks - pointer to application callbacks.
 ***************************************************************/

bStatus_t simpleProfile_RegisterAppCBs(simpleProfileCBs_t *appCallbacks)
{
    if (appCallbacks)
    {
        simpleProfile_AppCBs = appCallbacks;
        return ( SUCCESS);
    }
    else
    {
        return ( bleAlreadyInRequestedMode);
    }
}

/*******************************************************************
 * SetParameter - Set a service parameter.
 *
 *    param - Profile parameter ID
 *    len - length of data to right
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 ********************************************************************/
bStatus_t simpleProfile_SetParameter(uint8 param, uint8 len, void *value)
{
    bStatus_t ret = SUCCESS;
    switch (param)
    {

    case JUXTAPROFILE_LOGCOUNT: // LOG COUNT
        if (len == JUXTAPROFILE_LOGCOUNT_LEN)
        {
            VOID memcpy(juxtaProfile_logCount, value, len);
        }
        else
        {
            ret = bleInvalidRange;
        }
        break;

    case JUXTAPROFILE_METACOUNT: // META COUNT
        if (len == JUXTAPROFILE_METACOUNT_LEN)
        {
            VOID memcpy(juxtaProfile_metaCount, value, len);
        }
        else
        {
            ret = bleInvalidRange;
        }
        break;

    case JUXTAPROFILE_LOCALTIME: // LOCAL TIME
        if (len == JUXTAPROFILE_LOCALTIME_LEN)
        {
            VOID memcpy(juxtaProfile_localTime, value, len);
        }
        else
        {
            ret = bleInvalidRange;
        }
        break;

    case JUXTAPROFILE_VBATT: // BATTERY
        if (len == JUXTAPROFILE_VBATT_LEN)
        {
            memcpy(juxtaProfile_vBatt, value, len);
            // Try to send notification.
            GATTServApp_ProcessCharCfg(juxtaProfile_vBattConfig,
                                       (uint8*) juxtaProfile_vBatt, FALSE,
                                       simpleProfileAttrTbl,
                                       GATT_NUM_ATTRS(simpleProfileAttrTbl),
                                       INVALID_TASK_ID,
                                       gatt_simpleProfile_ReadAttrCB);
        }
        else
        {
            ret = bleInvalidRange;
        }
        break;

    case JUXTAPROFILE_TEMP: // TEMP
        if (len == JUXTAPROFILE_TEMP_LEN)
        {
            VOID memcpy(juxtaProfile_temp, value, len);
            // Try to send notification.
            GATTServApp_ProcessCharCfg(juxtaProfile_tempConfig,
                                       (uint8*) juxtaProfile_temp, FALSE,
                                       simpleProfileAttrTbl,
                                       GATT_NUM_ATTRS(simpleProfileAttrTbl),
                                       INVALID_TASK_ID,
                                       gatt_simpleProfile_ReadAttrCB);
        }
        else
        {
            ret = bleInvalidRange;
        }
        break;

    case JUXTAPROFILE_ADVMODE: // MODE
        if (len == JUXTAPROFILE_ADVMODE_LEN)
        {
            memcpy(&juxtaProfile_advMode, value, len);
        }
        else
        {
            ret = bleInvalidRange;
        }
        break;

    case JUXTAPROFILE_DATA: // DATA
        if (len == JUXTAPROFILE_DATA_LEN)
        {
            VOID memcpy(juxtaProfile_data, value, len);
            // Try to send notification.
            GATTServApp_ProcessCharCfg(juxtaProfile_dataConfig,
                                       (uint8*) juxtaProfile_data, FALSE,
                                       simpleProfileAttrTbl,
                                       GATT_NUM_ATTRS(simpleProfileAttrTbl),
                                       INVALID_TASK_ID,
                                       gatt_simpleProfile_ReadAttrCB);
        }
        else
        {
            ret = bleInvalidRange;
        }
        break;

    case JUXTAPROFILE_SUBJECT: // LOCAL TIME
        if (len == JUXTAPROFILE_SUBJECT_LEN)
        {
            VOID memcpy(juxtaProfile_subject, value, len);
        }
        else
        {
            ret = bleInvalidRange;
        }
        break;

    case JUXTAPROFILE_COMMAND: // no reason to set this (write only)
    default:
        ret = INVALIDPARAMETER;
        break;
    }
    return ret;
}

/******************************************************************
 * GetParameter - Get a service parameter.
 *
 * param - Profile parameter ID
 * value - pointer to data to write.  This is dependent on
 *         the parameter ID and WILL be cast to the appropriate
 *         data type (example: data type of uint16 will be cast to
 *         uint16 pointer).
 ******************************************************************/
bStatus_t simpleProfile_GetParameter(uint8 param, void *value)
{
    bStatus_t ret = SUCCESS;
    switch (param)
    {
    case JUXTAPROFILE_LOGCOUNT: // LOG COUNT
    {
        memcpy(value, juxtaProfile_logCount, JUXTAPROFILE_LOGCOUNT_LEN);
        break;
    }

    case JUXTAPROFILE_METACOUNT: // META COUNT
    {
        memcpy(value, juxtaProfile_metaCount, JUXTAPROFILE_METACOUNT_LEN);
        break;
    }

    case JUXTAPROFILE_LOCALTIME: // LOCAL TIME
    {
        memcpy(value, juxtaProfile_localTime, JUXTAPROFILE_LOCALTIME_LEN);
        break;
    }
    case JUXTAPROFILE_ADVMODE: // MODE
    {
        memcpy(value, &juxtaProfile_advMode, JUXTAPROFILE_ADVMODE_LEN);
        break;
    }
    case JUXTAPROFILE_DATA: // DATA
    {
        memcpy(value, juxtaProfile_data, JUXTAPROFILE_DATA_LEN);
        break;
    }
    case JUXTAPROFILE_COMMAND: // COMMAND
    {
        memcpy(value, &juxtaProfile_command, JUXTAPROFILE_COMMAND_LEN);
        break;
    }
    case JUXTAPROFILE_SUBJECT: // LOCAL TIME
    {
        memcpy(value, juxtaProfile_subject, JUXTAPROFILE_SUBJECT_LEN);
        break;
    }
    case JUXTAPROFILE_VBATT: // BATT, no write
    case JUXTAPROFILE_TEMP: // TEMP, no write
    default:
    {
        ret = INVALIDPARAMETER;
        break;
    }
    }
    return ret;
}
/*********************************************************************
 * @fn          ReadAttrCB
 *
 * @brief       Read an attribute.
 *
 * @param       connHandle - connection message was received on
 * @param       pAttr - pointer to attribute
 * @param       pValue - pointer to data to be read
 * @param       pLen - length of data to be read
 * @param       offset - offset of the first octet to be read
 * @param       maxLen - maximum length of data to be read
 * @param       method - type of read message
 *
 * @return      SUCCESS, blePending or Failure
 ********************************************************************/
static bStatus_t gatt_simpleProfile_ReadAttrCB(uint16_t connHandle,
                                               gattAttribute_t *pAttr,
                                               uint8_t *pValue, uint16_t *pLen,
                                               uint16_t offset, uint16_t maxLen,
                                               uint8_t method)
{
    return simpleProfile_ReadAttrCB(connHandle, pAttr, pValue, pLen, offset,
                                    maxLen, method);
}

/*********************************************************************
 * @fn      WriteAttrCB
 *
 * @brief   Validate attribute data prior to a write operation
 *
 * @param   connHandle - connection message was received on
 * @param   pAttr - pointer to attribute
 * @param   pValue - pointer to data to be written
 * @param   len - length of data
 * @param   offset - offset of the first octet to be written
 * @param   method - type of write message
 *
 * @return  SUCCESS, blePending or Failure
 ********************************************************************/
static bStatus_t gatt_simpleProfile_WriteAttrCB(uint16_t connHandle,
                                                gattAttribute_t *pAttr,
                                                uint8_t *pValue, uint16_t len,
                                                uint16_t offset, uint8_t method)
{
    return simpleProfile_WriteAttrCB(connHandle, pAttr, pValue, len, offset,
                                     method);
}
/*********************************************************************
 * @fn      AuthorizationCB
 *
 * @brief   Validate attribute data prior to a write operation
 *
 * @param   connHandle - connection message was received on
 * @param   pAttr - pointer to attribute
 * @param   opcode
 *
 * @return  SUCCESS, blePending or Failure
 ********************************************************************/
static bStatus_t gatt_simpleProfile_AuthorizationCB(uint16_t connHandle,
                                                    gattAttribute_t *pAttr,
                                                    uint8_t opcode)
{
    return SUCCESS;
}
/*********************************************************************
 * @fn          simpleProfile_ReadAttrCB
 *
 * @brief       Read an attribute.
 *
 * @param       connHandle - connection message was received on
 * @param       pAttr - pointer to attribute
 * @param       pValue - pointer to data to be read
 * @param       pLen - length of data to be read
 * @param       offset - offset of the first octet to be read
 * @param       maxLen - maximum length of data to be read
 * @param       method - type of read message
 *
 * @return      SUCCESS, blePending or Failure
 */
bStatus_t simpleProfile_ReadAttrCB(uint16_t connHandle, gattAttribute_t *pAttr,
                                   uint8_t *pValue, uint16_t *pLen,
                                   uint16_t offset, uint16_t maxLen,
                                   uint8_t method)
{
    bStatus_t status = SUCCESS;

    // Make sure it's not a blob operation (no attributes in the profile are long)
    if (offset > 0)
    {
        return ( ATT_ERR_ATTR_NOT_LONG);
    }

    if (pAttr->type.len == ATT_BT_UUID_SIZE)
    {
        // 16-bit UUID
        uint16 uuid = BUILD_UINT16(pAttr->type.uuid[0], pAttr->type.uuid[1]);
        switch (uuid)
        {
        // No need for "GATT_SERVICE_UUID" or "GATT_CLIENT_CHAR_CFG_UUID" cases;
        // gattserverapp handles those reads
        case JUXTAPROFILE_LOGCOUNT_UUID: // LOG COUNT
            *pLen = JUXTAPROFILE_LOGCOUNT_LEN;
            VOID memcpy(pValue, pAttr->pValue, JUXTAPROFILE_LOGCOUNT_LEN);
            break;

        case JUXTAPROFILE_METACOUNT_UUID: // META COUNT
            *pLen = JUXTAPROFILE_METACOUNT_LEN;
            VOID memcpy(pValue, pAttr->pValue, JUXTAPROFILE_METACOUNT_LEN);
            break;

        case JUXTAPROFILE_LOCALTIME_UUID: // LOCAL TIME
            *pLen = JUXTAPROFILE_LOCALTIME_LEN;
            VOID memcpy(pValue, pAttr->pValue, JUXTAPROFILE_LOCALTIME_LEN);
            break;

        case JUXTAPROFILE_VBATT_UUID: // BATTERY
            *pLen = JUXTAPROFILE_VBATT_LEN;
            VOID memcpy(pValue, pAttr->pValue, JUXTAPROFILE_VBATT_LEN);
            break;

        case JUXTAPROFILE_TEMP_UUID: // TEMP
            *pLen = JUXTAPROFILE_TEMP_LEN;
            VOID memcpy(pValue, pAttr->pValue, JUXTAPROFILE_TEMP_LEN);
            break;

        case JUXTAPROFILE_ADVMODE_UUID: // ADVERTISE
            *pLen = sizeof(uint8_t);
            pValue[0] = *pAttr->pValue;
            break;

        case JUXTAPROFILE_DATA_UUID: // DATA, no read but required for notify
            *pLen = JUXTAPROFILE_DATA_LEN;
            VOID memcpy(pValue, pAttr->pValue, JUXTAPROFILE_DATA_LEN);
            break;

        case JUXTAPROFILE_SUBJECT_UUID: // LOCAL TIME
            *pLen = JUXTAPROFILE_SUBJECT_LEN;
            VOID memcpy(pValue, pAttr->pValue, JUXTAPROFILE_SUBJECT_LEN);
            break;

        case JUXTAPROFILE_COMMAND_UUID: // COMMAND, no read perms
        default:
            *pLen = 0;
            status = ATT_ERR_ATTR_NOT_FOUND;
            break;
        }
    }
    else
    {
        // 128-bit UUID
        *pLen = 0;
        status = ATT_ERR_INVALID_HANDLE;
    }

    return (status);
}

/*********************************************************************
 * @fn      simpleProfile_WriteAttrCB
 *
 * @brief   Validate attribute data prior to a write operation
 *
 * @param   connHandle - connection message was received on
 * @param   pAttr - pointer to attribute
 * @param   pValue - pointer to data to be written
 * @param   len - length of data
 * @param   offset - offset of the first octet to be written
 * @param   method - type of write message
 *
 * @return  SUCCESS, blePending or Failure
 */
bStatus_t simpleProfile_WriteAttrCB(uint16_t connHandle, gattAttribute_t *pAttr,
                                    uint8_t *pValue, uint16_t len,
                                    uint16_t offset, uint8_t method)
{
    bStatus_t status = SUCCESS;
    uint8 notifyApp = 0xFF;

    if (pAttr->type.len == ATT_BT_UUID_SIZE)
    {
        // 16-bit UUID
        uint16 uuid = BUILD_UINT16(pAttr->type.uuid[0], pAttr->type.uuid[1]);
        switch (uuid)
        {

        case JUXTAPROFILE_LOGCOUNT_UUID: // LOG COUNT
        {
            if (offset == 0)
            {
                if (len != JUXTAPROFILE_LOGCOUNT_LEN)
                {
                    status = ATT_ERR_INVALID_VALUE_SIZE;
                }
            }
            else
            {
                status = ATT_ERR_ATTR_NOT_LONG;
            }
            //Write the value
            if (status == SUCCESS)
            {
                uint8 *pCurValue = (uint8*) pAttr->pValue;
                memcpy(pCurValue, pValue, len);
                notifyApp = JUXTAPROFILE_LOGCOUNT;
            }
            break;
        }
        case JUXTAPROFILE_METACOUNT_UUID: // META COUNT
        {
            if (offset == 0)
            {
                if (len != JUXTAPROFILE_METACOUNT_LEN)
                {
                    status = ATT_ERR_INVALID_VALUE_SIZE;
                }
            }
            else
            {
                status = ATT_ERR_ATTR_NOT_LONG;
            }
            //Write the value
            if (status == SUCCESS)
            {
                uint8 *pCurValue = (uint8*) pAttr->pValue;
                memcpy(pCurValue, pValue, len);
                notifyApp = JUXTAPROFILE_METACOUNT;
            }
            break;
        }

        case JUXTAPROFILE_LOCALTIME_UUID: // LOCAL TIME
        {
            if (offset == 0)
            {
                if (len != JUXTAPROFILE_LOCALTIME_LEN)
                {
                    status = ATT_ERR_INVALID_VALUE_SIZE;
                }
            }
            else
            {
                status = ATT_ERR_ATTR_NOT_LONG;
            }
            //Write the value
            if (status == SUCCESS)
            {
                uint8 *pCurValue = (uint8*) pAttr->pValue;
                memcpy(pCurValue, pValue, len);
                notifyApp = JUXTAPROFILE_LOCALTIME;
            }
            break;
        }

        case JUXTAPROFILE_ADVMODE_UUID: // ADVERTISE MODE, single byte
        {
            if (offset == 0)
            {
                if (len != JUXTAPROFILE_ADVMODE_LEN)
                {
                    status = ATT_ERR_INVALID_VALUE_SIZE;
                }
            }
            else
            {
                status = ATT_ERR_ATTR_NOT_LONG;
            }
            //Write the value
            if (status == SUCCESS)
            {
                uint8 *pCurValue = (uint8*) pAttr->pValue;
                *pCurValue = pValue[0]; // single byte
                notifyApp = JUXTAPROFILE_ADVMODE;
            }
            break;
        }

        case JUXTAPROFILE_COMMAND_UUID: // COMMAND
        {
            if (offset == 0)
            {
                if (len != JUXTAPROFILE_COMMAND_LEN)
                {
                    status = ATT_ERR_INVALID_VALUE_SIZE;
                }
            }
            else
            {
                status = ATT_ERR_ATTR_NOT_LONG;
            }
            //Write the value
            if (status == SUCCESS)
            {
                uint8 *pCurValue = (uint8*) pAttr->pValue;
                *pCurValue = pValue[0]; // single byte
                notifyApp = JUXTAPROFILE_COMMAND;
            }
            break;
        }

        case JUXTAPROFILE_SUBJECT_UUID: // LOCAL TIME
        {
            if (offset == 0)
            {
                if (len != JUXTAPROFILE_SUBJECT_LEN)
                {
                    status = ATT_ERR_INVALID_VALUE_SIZE;
                }
            }
            else
            {
                status = ATT_ERR_ATTR_NOT_LONG;
            }
            //Write the value
            if (status == SUCCESS)
            {
                uint8 *pCurValue = (uint8*) pAttr->pValue;
                memcpy(pCurValue, pValue, len);
                notifyApp = JUXTAPROFILE_SUBJECT;
            }
            break;
        }

        case GATT_CLIENT_CHAR_CFG_UUID:
            status = GATTServApp_ProcessCCCWriteReq(connHandle, pAttr, pValue,
                                                    len, offset,
                                                    GATT_CLIENT_CFG_NOTIFY);
            break;

        case JUXTAPROFILE_VBATT_UUID: // BATTERY, no write
        case JUXTAPROFILE_TEMP_UUID: // TEMP, no write
        case JUXTAPROFILE_DATA_UUID: // DATA, no write
        default:
            status = ATT_ERR_ATTR_NOT_FOUND;
            break;
        }
    }
    else
    {
        // 128-bit UUID
        status = ATT_ERR_INVALID_HANDLE;
    }

    // If a characteristic value changed then callback function to notify application of change
    if ((notifyApp != 0xFF) && simpleProfile_AppCBs
            && simpleProfile_AppCBs->pfnSimpleProfileChange)
    {
        simpleProfile_AppCBs->pfnSimpleProfileChange(notifyApp);
    }

    return (status);
}

/*********************************************************************
 *********************************************************************/
