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
uint8 simpleProfileChar1[SIMPLEPROFILE_CHAR1_LEN] = { 0x0, 0x0, 0x0, 0x0 };
// Characteristic "Char1" User Description
static uint8 simpleProfileChar1UserDesp[] = "Characteristic 1";

// Characteristic "Char2" Properties (for declaration)
static uint8 simpleProfileChar2Props = GATT_PROP_READ | GATT_PROP_WRITE;
// Characteristic "Char2" Value variable
uint8 simpleProfileChar2[SIMPLEPROFILE_CHAR2_LEN] = { 0x0, 0x0, 0x0, 0x0 };
// Characteristic "Char2" User Description
static uint8 simpleProfileChar2UserDesp[] = "Characteristic 2";

// Characteristic "Char3" Properties (for declaration)
static uint8 simpleProfileChar3Props = GATT_PROP_READ | GATT_PROP_WRITE;
// Characteristic "Char3" Value variable
uint8 simpleProfileChar3 = 0x0;
// Characteristic "Char3" User Description
static uint8 simpleProfileChar3UserDesp[] = "Characteristic 3";

// Characteristic "Char4" Properties (for declaration)
static uint8 simpleProfileChar4Props = GATT_PROP_NOTIFY | GATT_PROP_WRITE;
// Characteristic "Char4" Value variable
uint8 simpleProfileChar4 = 0x0;
// Characteristic "Char4" User Description
static uint8 simpleProfileChar4UserDesp[] = "Characteristic 4";

// Characteristic "Char4" CCCD
static gattCharCfg_t *simpleProfileChar4Config;
/*********************************************************************
 * Profile Attributes - Table
 *********************************************************************/
static gattAttribute_t simpleProfileAttrTbl[] = { {
        { ATT_BT_UUID_SIZE, primaryServiceUUID },
        GATT_PERMIT_READ,
        0, (uint8*) &simpleProfileService },

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

        // Char3 Characteristic Declaration
        { { ATT_BT_UUID_SIZE, characterUUID },
        GATT_PERMIT_READ,
          0, &simpleProfileChar3Props },

        // Char3 Characteristic Value
        { { ATT_BT_UUID_SIZE, simpleProfileChar3UUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
          0, &simpleProfileChar3 },

        // Characteristic Char3 User Description
        { { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
          0, simpleProfileChar3UserDesp },

        // Char4 Characteristic Declaration
        { { ATT_BT_UUID_SIZE, characterUUID },
        GATT_PERMIT_READ,
          0, &simpleProfileChar4Props },

        // Char4 Characteristic Value
        { { ATT_BT_UUID_SIZE, simpleProfileChar4UUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
          0, &simpleProfileChar4 },

        // Characteristic Char4 User Description
        { { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
          0, simpleProfileChar4UserDesp },

        // Char4 configuration
        { { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
          0, (uint8*) &simpleProfileChar4Config }, };

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

    case SIMPLEPROFILE_CHAR1:
        if (len == SIMPLEPROFILE_CHAR1_LEN)
        {
            VOID memcpy(simpleProfileChar1, value, SIMPLEPROFILE_CHAR1_LEN);
        }
        else
        {
            ret = bleInvalidRange;
        }
        break;

    case SIMPLEPROFILE_CHAR2:
        if (len == SIMPLEPROFILE_CHAR1_LEN)
        {
            VOID memcpy(simpleProfileChar2, value, SIMPLEPROFILE_CHAR2_LEN);
        }
        else
        {
            ret = bleInvalidRange;
        }
        break;

    case SIMPLEPROFILE_CHAR3:
        if (len == sizeof(uint8))
        {
            memcpy(&simpleProfileChar3, value, len);
        }
        else
        {
            ret = bleInvalidRange;
        }
        break;

    case SIMPLEPROFILE_CHAR4:
        if (len == sizeof(uint8))
        {
            memcpy(&simpleProfileChar4, value, len);
            // Try to send notification.
            GATTServApp_ProcessCharCfg(simpleProfileChar4Config,
                                       (uint8*) &simpleProfileChar4, FALSE,
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
    case SIMPLEPROFILE_CHAR1:
    {
        memcpy(value, simpleProfileChar1, SIMPLEPROFILE_CHAR1_LEN);
        break;
    }

    case SIMPLEPROFILE_CHAR2:
    {
        memcpy(value, simpleProfileChar2, SIMPLEPROFILE_CHAR2_LEN);
        break;
    }

    case SIMPLEPROFILE_CHAR3:
    {
        memcpy(value, &simpleProfileChar3, SIMPLEPROFILE_CHAR3_LEN);
        break;
    }

    case SIMPLEPROFILE_CHAR4:
    {
        memcpy(value, &simpleProfileChar4, SIMPLEPROFILE_CHAR4_LEN);
        break;
    }

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

        case SIMPLEPROFILE_CHAR3_UUID: // single byte
        case SIMPLEPROFILE_CHAR4_UUID:
            *pLen = 1;
            pValue[0] = *pAttr->pValue;
            break;

        case SIMPLEPROFILE_CHAR1_UUID:
            *pLen = SIMPLEPROFILE_CHAR1_LEN;
            VOID memcpy(pValue, pAttr->pValue, SIMPLEPROFILE_CHAR1_LEN);
            break;

        case SIMPLEPROFILE_CHAR2_UUID:
            *pLen = SIMPLEPROFILE_CHAR2_LEN;
            VOID memcpy(pValue, pAttr->pValue, SIMPLEPROFILE_CHAR2_LEN);
            break;

        default:
            // Should never get here! (characteristics 3 and 4 do not have read permissions)
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

        case SIMPLEPROFILE_CHAR1_UUID: // multi-byte
        {

            // Validate the value
            // Make sure it's not a blob oper
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
        case SIMPLEPROFILE_CHAR2_UUID: // multi-byte
        {

            // Validate the value
            // Make sure it's not a blob oper
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
        case SIMPLEPROFILE_CHAR3_UUID: // single byte
        {

            // Validate the value
            // Make sure it's not a blob oper
            if (offset == 0)
            {
                if (len != 1)
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
                *pCurValue = pValue[0];
                notifyApp = SIMPLEPROFILE_CHAR3;
            }

            break;
        }
        case SIMPLEPROFILE_CHAR4_UUID: // technically multi-byte, but I
        {

            // Validate the value
            // Make sure it's not a blob oper
            if (offset == 0)
            {
                if (len != 1)
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
                *pCurValue = pValue[0];
                notifyApp = SIMPLEPROFILE_CHAR4;

            }

            break;
        }

        case GATT_CLIENT_CHAR_CFG_UUID:
            status = GATTServApp_ProcessCCCWriteReq(connHandle, pAttr, pValue,
                                                    len, offset,
                                                    GATT_CLIENT_CFG_NOTIFY);
            break;

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
