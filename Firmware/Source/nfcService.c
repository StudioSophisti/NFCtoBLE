/*********************************************************************
 * INCLUDES
 */
#include "bcomdef.h"
#include "OSAL.h"
#include "linkdb.h"
#include "att.h"
#include "gatt.h"
#include "gatt_uuid.h"
#include "gattservapp.h"
#include "gapbondmgr.h"
#include "global.h"
#include "nfcService.h"
#include "OnBoard.h"
#include "peripheral.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
// NFC Service UUID: 
CONST uint8 NFCServUUID[ATT_UUID_SIZE] = {
  BASE_UUID_128(NFC_SERVICE_UUID)
};

// NFC Data Characteristic UUID: 
CONST uint8 NFCDataCharUUID[ATT_UUID_SIZE] = {
  BASE_UUID_128(NFC_DATA_CHAR_UUID)
};

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
   
#define INVALID_CONN                   0xFFFF
   
/*********************************************************************
 * Service Attributes - variables
 */

// NFC Service attribute
static CONST gattAttrType_t NFCService = { ATT_UUID_SIZE, NFCServUUID };

// NFC Data Characteristic
static uint8 NFCDataCharProps = GATT_PROP_READ | GATT_PROP_NOTIFY;
static uint8 NFCDataCharValue[NFC_DATA_CHAR_LEN];
static gattCharCfg_t NFCDataCtrlConfig[GATT_MAX_NUM_CONN];
static CONST uint8 NFCDataCharUserDesc[] = "NFC Data";


/*********************************************************************
 * Service Attributes - Table
 */

static gattAttribute_t NFCServiceAttrTbl[] = 
{
  // NFC Service
  { 
    { ATT_BT_UUID_SIZE, primaryServiceUUID }, /* type */
    GATT_PERMIT_READ,                         /* permissions */
    0,                                        /* handle */
    (uint8 *)&NFCService              /* pValue */
  },

    // Characteristic 1 Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &NFCDataCharProps 
    },

      // Characteristic Value 1
      { 
        { ATT_UUID_SIZE, NFCDataCharUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE, 
        0, 
        NFCDataCharValue 
      },

      // Characteristic configuration
      {
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        (uint8 *)NFCDataCtrlConfig
      },
      
      // Characteristic 1 User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0, 
        (uint8 *)NFCDataCharUserDesc 
      }

   
};


/*********************************************************************
 * LOCAL FUNCTIONS
 */
static uint8 NFCService_ReadAttrCB( uint16 connHandle, gattAttribute_t *pAttr, 
                            uint8 *pValue, uint8 *pLen, uint16 offset, uint8 maxLen );
static bStatus_t NFCService_WriteAttrCB( uint16 connHandle, gattAttribute_t *pAttr,
                                 uint8 *pValue, uint8 len, uint16 offset );

/*********************************************************************
 * PROFILE CALLBACKS
 */
// SB Config Profile Service Callbacks
CONST gattServiceCBs_t NFCServiceCBs =
{
  NFCService_ReadAttrCB,  // Read callback function pointer
  NFCService_WriteAttrCB, // Write callback function pointer
  NULL                       // Authorization callback function pointer
};
/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      NFCService_AddService
 *
 * @brief   Initializes the service by registering
 *          GATT attributes with the GATT server.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 *
 * @return  Success or Failure
 */
bStatus_t NFCService_AddService( void )
{
  uint8 status = SUCCESS;
  
  GATTServApp_InitCharCfg( INVALID_CONNHANDLE, NFCDataCtrlConfig );
  
  // Register GATT attribute list and CBs with GATT Server App
  status = GATTServApp_RegisterService( NFCServiceAttrTbl, 
                                          GATT_NUM_ATTRS( NFCServiceAttrTbl ),
                                          &NFCServiceCBs );
  return ( status );
}

/*********************************************************************
 * @fn          NFCService_ReadAttrCB
 *
 * @brief       Read an attribute.
 *
 * @param       connHandle - connection message was received on
 * @param       pAttr - pointer to attribute
 * @param       pValue - pointer to data to be read
 * @param       pLen - length of data to be read
 * @param       offset - offset of the first octet to be read
 * @param       maxLen - maximum length of data to be read
 *
 * @return      Success or Failure
 */
static uint8 NFCService_ReadAttrCB( uint16 connHandle, gattAttribute_t *pAttr, 
                            uint8 *pValue, uint8 *pLen, uint16 offset, uint8 maxLen )
{
  bStatus_t status = SUCCESS;

  // If attribute permissions require authorization to read, return error
  if ( gattPermitAuthorRead( pAttr->permissions ) )
  {
    // Insufficient authorization
    return ( ATT_ERR_INSUFFICIENT_AUTHOR );
  }
  
  // Make sure it's not a blob operation (no attributes in the profile are long)
  if ( offset > 0 )
  {
    return ( ATT_ERR_ATTR_NOT_LONG );
  }
 
  if ( pAttr->type.len == ATT_UUID_SIZE )
  {
    // 128-bit UUID
    
    if (osal_memcmp(pAttr->type.uuid, NFCDataCharUUID, ATT_UUID_SIZE)) {
      //NFC Data UUID
             
      *pLen = NFC_DATA_CHAR_LEN;    
      VOID osal_memcpy( pValue, pAttr->pValue, NFC_DATA_CHAR_LEN );  
        
    } else {
      // Should never get here! 
      *pLen = 0;
      status = ATT_ERR_ATTR_NOT_FOUND;
    } 
               
  }
  else
  {
    // 16-bit UUID
    *pLen = 0;
    status = ATT_ERR_INVALID_HANDLE;
  }

  return ( status );
}

/*********************************************************************
 * @fn      NFCService_WriteAttrCB
 *
 * @brief   Validate attribute data prior to a write operation
 *
 * @param   connHandle - connection message was received on
 * @param   pAttr - pointer to attribute
 * @param   pValue - pointer to data to be written
 * @param   len - length of data
 * @param   offset - offset of the first octet to be written
 * @param   complete - whether this is the last packet
 * @param   oper - whether to validate and/or write attribute value  
 *
 * @return  Success or Failure
 */
static bStatus_t NFCService_WriteAttrCB( uint16 connHandle, gattAttribute_t *pAttr,
                                 uint8 *pValue, uint8 len, uint16 offset )
{
  bStatus_t status = SUCCESS;
  
  // If attribute permissions require authorization to write, return error
  if ( gattPermitAuthorWrite( pAttr->permissions ) )
  {
    // Insufficient authorization
    return ( ATT_ERR_INSUFFICIENT_AUTHOR );
  }
  
  
  if ( pAttr->type.len == ATT_UUID_SIZE )
  {
    // 128-bit UUID
    
    if (osal_memcmp(pAttr->type.uuid, NFCDataCharUUID, ATT_UUID_SIZE)) {
      //NFC Data UUID
     
      //Write the value
      if ( len == NFC_DATA_CHAR_LEN )
      {               
        osal_memcpy( pAttr->pValue, pValue, NFC_DATA_CHAR_LEN );
            
        
      }
        
    }     
    
    else {
      // Should never get here!  
        status = ATT_ERR_ATTR_NOT_FOUND;
    } 
    
  }
  else
  {
    // 16-bit UUID   
    uint16 uuid = BUILD_UINT16( pAttr->type.uuid[0], pAttr->type.uuid[1]);
    if ( uuid == GATT_CLIENT_CHAR_CFG_UUID)
    {
      status = GATTServApp_ProcessCCCWriteReq( connHandle, pAttr, pValue, len,
                                               offset, GATT_CLIENT_CFG_NOTIFY );
    }
    else
    {
      status = ATT_ERR_ATTR_NOT_FOUND; // Should never get here!
    }  
  }

  return ( status );
}

void NFCSendNotification(uint8* data, uint8 dataLen)
{
  uint16 connHandle;
  GAPRole_GetParameter( GAPROLE_CONNHANDLE, &connHandle );
  
  if (connHandle == INVALID_CONN) 
    return;
  
  uint16 value = GATTServApp_ReadCharCfg( connHandle, NFCDataCtrlConfig );
    
  // If notifications enabled
  if ( value & GATT_CLIENT_CFG_NOTIFY )
  {
    
    attHandleValueNoti_t noti;
    gattAttribute_t *pAttr = GATTServApp_FindAttr(NFCServiceAttrTbl, GATT_NUM_ATTRS(NFCServiceAttrTbl),
                                                  NFCDataCharValue);
    noti.handle = pAttr->handle;
    noti.len = dataLen;
    osal_memcpy(noti.value, data, dataLen);

    VOID GATT_Notification(connHandle, &noti, FALSE);
  }
}
    

/*********************************************************************
*********************************************************************/
