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
CONST uint8 simpleProfileChar1UUID[ATT_BT_UUID_SIZE] =
        { LO_UINT16(SIMPLEPROFILE_CHAR1_UUID), HI_UINT16(
                SIMPLEPROFILE_CHAR1_UUID) };

// Char2 UUID
CONST uint8 simpleProfileChar2UUID[ATT_BT_UUID_SIZE] =
        { LO_UINT16(SIMPLEPROFILE_CHAR2_UUID), HI_UINT16(
                SIMPLEPROFILE_CHAR2_UUID) };

// Char3 UUID
CONST uint8 simpleProfileChar3UUID[ATT_BT_UUID_SIZE] =
        { LO_UINT16(SIMPLEPROFILE_CHAR3_UUID), HI_UINT16(
                SIMPLEPROFILE_CHAR3_UUID) };

// Char4 UUID
CONST uint8 simpleProfileChar4UUID[ATT_BT_UUID_SIZE] =
        { LO_UINT16(SIMPLEPROFILE_CHAR4_UUID), HI_UINT16(
                SIMPLEPROFILE_CHAR4_UUID) };

// Char5 UUID
CONST uint8 simpleProfileChar5UUID[ATT_BT_UUID_SIZE] =
        { LO_UINT16(SIMPLEPROFILE_CHAR5_UUID), HI_UINT16(
                SIMPLEPROFILE_CHAR5_UUID) };

// Char6 UUID
CONST uint8 simpleProfileChar6UUID[ATT_BT_UUID_SIZE] =
        { LO_UINT16(SIMPLEPROFILE_CHAR6_UUID), HI_UINT16(
                SIMPLEPROFILE_CHAR6_UUID) };

// Char7 UUID
CONST uint8 simpleProfileChar7UUID[ATT_BT_UUID_SIZE] =
        { LO_UINT16(SIMPLEPROFILE_CHAR7_UUID), HI_UINT16(
                SIMPLEPROFILE_CHAR7_UUID) };

// Char8 UUID
CONST uint8 simpleProfileChar8UUID[ATT_BT_UUID_SIZE] =
        { LO_UINT16(SIMPLEPROFILE_CHAR8_UUID), HI_UINT16(
                SIMPLEPROFILE_CHAR8_UUID) };

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
static uint8 simpleProfileChar1Props = GATT_PROP_READ | GATT_PROP_WRITE;
// Characteristic "Char1" Value variable
uint8 simpleProfileChar1[SIMPLEPROFILE_CHAR1_LEN] = { 0 };
// Characteristic "Char1" User Description
static uint8 simpleProfileChar1UserDesp[] = "Log Count";

// Characteristic "Char2" Properties (for declaration)
static uint8 simpleProfileChar2Props = GATT_PROP_READ | GATT_PROP_WRITE;
// Characteristic "Char2" Value variable
uint8 simpleProfileChar2[SIMPLEPROFILE_CHAR2_LEN] = { 0 };
// Characteristic "Char2" User Description
static uint8 simpleProfileChar2UserDesp[] = "Meta Count";

// Characteristic "Char3" Properties (for declaration)
static uint8 simpleProfileChar3Props = GATT_PROP_READ | GATT_PROP_WRITE;
// Characteristic "Char3" Value variable
uint8 simpleProfileChar3[SIMPLEPROFILE_CHAR3_LEN] = { 0 };
// Characteristic "Char3" User Description
static uint8 simpleProfileChar3UserDesp[] = "Local Time";

// Characteristic "Char4" Properties (for declaration)
static uint8 simpleProfileChar4Props = GATT_PROP_READ | GATT_PROP_NOTIFY;
// Characteristic "Char4" Value variable
uint8 simpleProfileChar4[SIMPLEPROFILE_CHAR4_LEN] = { 0 };
// Characteristic "Char4" User Description
static uint8 simpleProfileChar4UserDesp[] = "Battery Voltage";
// Characteristic "Char4" CCCD
static gattCharCfg_t *simpleProfileChar4Config;

// Characteristic "Char5" Properties (for declaration)
static uint8 simpleProfileChar5Props = GATT_PROP_READ | GATT_PROP_NOTIFY;
// Characteristic "Char5" Value variable
uint8 simpleProfileChar5[SIMPLEPROFILE_CHAR5_LEN] = { 0 };
// Characteristic "Char5" User Description
static uint8 simpleProfileChar5UserDesp[] = "Temperature";
// Characteristic "Char5" CCCD
static gattCharCfg_t *simpleProfileChar5Config;

// Characteristic "Char6" Properties (for declaration)
static uint8 simpleProfileChar6Props = GATT_PROP_READ | GATT_PROP_WRITE;
// Characteristic "Char6" Value variable
uint8 simpleProfileChar6 = 0x0;
// Characteristic "Char6" User Description
static uint8 simpleProfileChar6UserDesp[] = "Advertise Mode";

// Characteristic "Char7" Properties (for declaration)
static uint8 simpleProfileChar7Props = GATT_PROP_NOTIFY;
// Characteristic "Char7" Value variable
uint8 simpleProfileChar7[SIMPLEPROFILE_CHAR7_LEN] = { 0 };
// Characteristic "Char7" User Description
static uint8 simpleProfileChar7UserDesp[] = "Juxta Data";
// Characteristic "Char7" CCCD
static gattCharCfg_t *simpleProfileChar7Config;

// Characteristic "Char8" Properties (for declaration)
static uint8 simpleProfileChar8Props = GATT_PROP_WRITE;
// Characteristic "Char8" Value variable
uint8 simpleProfileChar8 = 0x0;
// Characteristic "Char8" User Description
static uint8 simpleProfileChar8UserDesp[] = "Command";

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
          0, &simpleProfileChar1Props },

        // Char1 Characteristic Value
        { { ATT_BT_UUID_SIZE, simpleProfileChar1UUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
          0, simpleProfileChar1 },

        // Characteristic Char1 User Description
        { { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
          0, simpleProfileChar1UserDesp },

        // META COUNT
        // Char2 Characteristic Declaration
        { { ATT_BT_UUID_SIZE, characterUUID },
        GATT_PERMIT_READ,
          0, &simpleProfileChar2Props },

        // Char2 Characteristic Value
        { { ATT_BT_UUID_SIZE, simpleProfileChar2UUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
          0, simpleProfileChar2 },

        // Characteristic Char2 User Description
        { { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
          0, simpleProfileChar2UserDesp },

        // LOCAL TIME
        // Char3 Characteristic Declaration
        { { ATT_BT_UUID_SIZE, characterUUID },
        GATT_PERMIT_READ,
          0, &simpleProfileChar3Props },

        // Char3 Characteristic Value
        { { ATT_BT_UUID_SIZE, simpleProfileChar3UUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
          0, simpleProfileChar3 },

        // Characteristic Char3 User Description
        { { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
          0, simpleProfileChar3UserDesp },

        // BATTERY
        // Char4 Characteristic Declaration
        { { ATT_BT_UUID_SIZE, characterUUID },
        GATT_PERMIT_READ,
          0, &simpleProfileChar4Props },

        // Char4 Characteristic Value
        { { ATT_BT_UUID_SIZE, simpleProfileChar4UUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
          0, simpleProfileChar4 },

        // Characteristic Char4 User Description
        { { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
          0, simpleProfileChar4UserDesp },

        // Char4 configuration
        { { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
          0, (uint8*) &simpleProfileChar4Config },

        // TEMPERATURE
        // Char5 Characteristic Declaration
        { { ATT_BT_UUID_SIZE, characterUUID },
        GATT_PERMIT_READ,
          0, &simpleProfileChar5Props },

        // Char5 Characteristic Value
        { { ATT_BT_UUID_SIZE, simpleProfileChar5UUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
          0, simpleProfileChar5 },

        // Characteristic Char5 User Description
        { { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
          0, simpleProfileChar5UserDesp },

        // Char5 configuration
        { { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
          0, (uint8*) &simpleProfileChar5Config },

        // ADVERTISE
        // Char6 Characteristic Declaration
        { { ATT_BT_UUID_SIZE, characterUUID },
        GATT_PERMIT_READ,
          0, &simpleProfileChar6Props },

        // Char6 Characteristic Value
        { { ATT_BT_UUID_SIZE, simpleProfileChar6UUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
          0, &simpleProfileChar6 },

        // Characteristic Char6 User Description
        { { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
          0, simpleProfileChar6UserDesp },

        // DATA
        // Char7 Characteristic Declaration
        { { ATT_BT_UUID_SIZE, characterUUID },
        GATT_PERMIT_READ,
          0, &simpleProfileChar7Props },

        // Char7 Characteristic Value
        { { ATT_BT_UUID_SIZE, simpleProfileChar7UUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
          0, simpleProfileChar7 },

        // Characteristic Char7 User Description
        { { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
          0, simpleProfileChar7UserDesp },

        // Char7 configuration
        { { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
          0, (uint8*) &simpleProfileChar7Config },

        // COMMAND
        // Char8 Characteristic Declaration
        { { ATT_BT_UUID_SIZE, characterUUID },
        GATT_PERMIT_READ,
          0, &simpleProfileChar8Props },

        // Char8 Characteristic Value
        { { ATT_BT_UUID_SIZE, simpleProfileChar8UUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
          0, &simpleProfileChar8 },

        // Characteristic Char8 User Description
        { { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
          0, simpleProfileChar8UserDesp },

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
    simpleProfileChar4Config = (gattCharCfg_t*) ICall_malloc(
            sizeof(gattCharCfg_t) * MAX_NUM_BLE_CONNS);
    if (simpleProfileChar4Config == NULL)
    {
        return ( bleMemAllocError);
    }
    // Initialize Client Characteristic Configuration attributes
    GATTServApp_InitCharCfg( LINKDB_CONNHANDLE_INVALID,
                            simpleProfileChar4Config);

    // TEMPERATURE
    // Allocate Client Characteristic Configuration table
    simpleProfileChar5Config = (gattCharCfg_t*) ICall_malloc(
            sizeof(gattCharCfg_t) * MAX_NUM_BLE_CONNS);
    if (simpleProfileChar5Config == NULL)
    {
        return ( bleMemAllocError);
    }
    // Initialize Client Characteristic Configuration attributes
    GATTServApp_InitCharCfg( LINKDB_CONNHANDLE_INVALID,
                            simpleProfileChar5Config);

    // Allocate Client Characteristic Configuration table
    simpleProfileChar7Config = (gattCharCfg_t*) ICall_malloc(
            sizeof(gattCharCfg_t) * MAX_NUM_BLE_CONNS);
    if (simpleProfileChar7Config == NULL)
    {
        return ( bleMemAllocError);
    }
    // Initialize Client Characteristic Configuration attributes
    GATTServApp_InitCharCfg( LINKDB_CONNHANDLE_INVALID,
                            simpleProfileChar7Config);
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

    case SIMPLEPROFILE_CHAR1: // LOG COUNT
        if (len == SIMPLEPROFILE_CHAR1_LEN)
        {
            VOID memcpy(simpleProfileChar1, value, len);
        }
        else
        {
            ret = bleInvalidRange;
        }
        break;

    case SIMPLEPROFILE_CHAR2: // META COUNT
        if (len == SIMPLEPROFILE_CHAR2_LEN)
        {
            VOID memcpy(simpleProfileChar2, value, len);
        }
        else
        {
            ret = bleInvalidRange;
        }
        break;

    case SIMPLEPROFILE_CHAR3: // LOCAL TIME
        if (len == SIMPLEPROFILE_CHAR3_LEN)
        {
            VOID memcpy(simpleProfileChar3, value, len);
        }
        else
        {
            ret = bleInvalidRange;
        }
        break;

    case SIMPLEPROFILE_CHAR4: // BATTERY
        if (len == SIMPLEPROFILE_CHAR4_LEN)
        {
            memcpy(simpleProfileChar4, value, len);
            // Try to send notification.
            GATTServApp_ProcessCharCfg(simpleProfileChar4Config,
                                       (uint8*) simpleProfileChar4, FALSE,
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

    case SIMPLEPROFILE_CHAR5: // TEMP
        if (len == SIMPLEPROFILE_CHAR5_LEN)
        {
            VOID memcpy(simpleProfileChar5, value, len);
            // Try to send notification.
            GATTServApp_ProcessCharCfg(simpleProfileChar5Config,
                                       (uint8*) simpleProfileChar5, FALSE,
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

    case SIMPLEPROFILE_CHAR6: // MODE
        if (len == SIMPLEPROFILE_CHAR6_LEN)
        {
            memcpy(&simpleProfileChar6, value, len);
        }
        else
        {
            ret = bleInvalidRange;
        }
        break;

    case SIMPLEPROFILE_CHAR7: // DATA
        if (len == SIMPLEPROFILE_CHAR7_LEN)
        {
            VOID memcpy(simpleProfileChar7, value, len);
            // Try to send notification.
            GATTServApp_ProcessCharCfg(simpleProfileChar7Config,
                                       (uint8*) simpleProfileChar7, FALSE,
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

    case SIMPLEPROFILE_CHAR8: // no reason to set this (write only)
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
    case SIMPLEPROFILE_CHAR1: // LOG COUNT
    {
        memcpy(value, simpleProfileChar1, SIMPLEPROFILE_CHAR1_LEN);
        break;
    }

    case SIMPLEPROFILE_CHAR2: // META COUNT
    {
        memcpy(value, simpleProfileChar2, SIMPLEPROFILE_CHAR2_LEN);
        break;
    }

    case SIMPLEPROFILE_CHAR3: // LOCAL TIME
    {
        memcpy(value, simpleProfileChar3, SIMPLEPROFILE_CHAR3_LEN);
        break;
    }
    case SIMPLEPROFILE_CHAR6: // MODE
    {
        memcpy(value, &simpleProfileChar6, SIMPLEPROFILE_CHAR6_LEN);
        break;
    }
    case SIMPLEPROFILE_CHAR7: // DATA
    {
        memcpy(value, simpleProfileChar7, SIMPLEPROFILE_CHAR7_LEN);
        break;
    }
    case SIMPLEPROFILE_CHAR8: // COMMAND
    {
        memcpy(value, &simpleProfileChar8, SIMPLEPROFILE_CHAR8_LEN);
        break;
    }
    case SIMPLEPROFILE_CHAR4: // BATT, no write
    case SIMPLEPROFILE_CHAR5: // TEMP, no write
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
        case SIMPLEPROFILE_CHAR1_UUID: // LOG COUNT
            *pLen = SIMPLEPROFILE_CHAR1_LEN;
            VOID memcpy(pValue, pAttr->pValue, SIMPLEPROFILE_CHAR1_LEN);
            break;

        case SIMPLEPROFILE_CHAR2_UUID: // META COUNT
            *pLen = SIMPLEPROFILE_CHAR2_LEN;
            VOID memcpy(pValue, pAttr->pValue, SIMPLEPROFILE_CHAR2_LEN);
            break;

        case SIMPLEPROFILE_CHAR3_UUID: // LOCAL TIME
            *pLen = SIMPLEPROFILE_CHAR3_LEN;
            VOID memcpy(pValue, pAttr->pValue, SIMPLEPROFILE_CHAR3_LEN);
            break;

        case SIMPLEPROFILE_CHAR4_UUID: // BATTERY
            *pLen = SIMPLEPROFILE_CHAR4_LEN;
            VOID memcpy(pValue, pAttr->pValue, SIMPLEPROFILE_CHAR4_LEN);
            break;

        case SIMPLEPROFILE_CHAR5_UUID: // TEMP
            *pLen = SIMPLEPROFILE_CHAR5_LEN;
            VOID memcpy(pValue, pAttr->pValue, SIMPLEPROFILE_CHAR5_LEN);
            break;

        case SIMPLEPROFILE_CHAR6_UUID: // ADVERTISE
            *pLen = sizeof(uint8_t);
            pValue[0] = *pAttr->pValue;
            break;

        case SIMPLEPROFILE_CHAR7_UUID: // DATA, no read but required for notify
            *pLen = SIMPLEPROFILE_CHAR7_LEN;
            VOID memcpy(pValue, pAttr->pValue, SIMPLEPROFILE_CHAR7_LEN);
            break;

        case SIMPLEPROFILE_CHAR8_UUID: // COMMAND, no read perms
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

        case SIMPLEPROFILE_CHAR1_UUID: // LOG COUNT
        {
            if (offset == 0)
            {
                if (len != SIMPLEPROFILE_CHAR1_LEN)
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
                notifyApp = SIMPLEPROFILE_CHAR1;
            }
            break;
        }
        case SIMPLEPROFILE_CHAR2_UUID: // META COUNT
        {
            if (offset == 0)
            {
                if (len != SIMPLEPROFILE_CHAR2_LEN)
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
                notifyApp = SIMPLEPROFILE_CHAR2;
            }
            break;
        }

        case SIMPLEPROFILE_CHAR3_UUID: // LOCAL TIME
        {
            if (offset == 0)
            {
                if (len != SIMPLEPROFILE_CHAR3_LEN)
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
                notifyApp = SIMPLEPROFILE_CHAR3;
            }
            break;
        }

        case SIMPLEPROFILE_CHAR6_UUID: // ADVERTISE MODE, single byte
        {
            if (offset == 0)
            {
                if (len != SIMPLEPROFILE_CHAR6_LEN)
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
                notifyApp = SIMPLEPROFILE_CHAR6;
            }
            break;
        }

        case SIMPLEPROFILE_CHAR8_UUID: // COMMAND
        {
            if (offset == 0)
            {
                if (len != SIMPLEPROFILE_CHAR8_LEN)
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
                notifyApp = SIMPLEPROFILE_CHAR8;
            }
            break;
        }

        case GATT_CLIENT_CHAR_CFG_UUID:
            status = GATTServApp_ProcessCCCWriteReq(connHandle, pAttr, pValue,
                                                    len, offset,
                                                    GATT_CLIENT_CFG_NOTIFY);
            break;

        case SIMPLEPROFILE_CHAR4_UUID: // BATTERY, no write
        case SIMPLEPROFILE_CHAR5_UUID: // TEMP, no write
        case SIMPLEPROFILE_CHAR7_UUID: // DATA, no write
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