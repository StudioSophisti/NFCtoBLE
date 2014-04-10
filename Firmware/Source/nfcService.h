
#ifndef SWTIMESERVICE_H
#define SWTIMESERVICE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

#include "hal_types.h"
  
#define NFC_SERVICE_UUID                    0xFA01   
#define NFC_DATA_CHAR_UUID                  0xFB01   
      
// Length of Characteristics in bytes
#define NFC_DATA_CHAR_LEN                   18  

extern bStatus_t NFCService_AddService( void );
void NFCSendNotification(uint8* data, uint8 dataLen);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* SWTIMESERVICE_H */
