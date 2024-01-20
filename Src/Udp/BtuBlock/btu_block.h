#pragma once

#include "../private.h"

#include "SDeviceCore/common.h"

#include <memory.h>

#define EMPTY_UDP_BTU_BLOCK_SIZE (sizeof(UdpBtuBlock) - SIZEOF_MEMBER(UdpBtuBlock, Pdu))

typedef struct __attribute__((packed, __may_alias__))
{
   uint8_t    BtuAddress[MODBUS_UDP_SDEVICE_BTU_ADDRESS_SIZE];
   MessagePdu Pdu;
} UdpBtuBlock;

typedef struct
{
   const UdpBtuBlock *BtuBlock;
   size_t             BtuBlockSize;
} UdpBtuBlockInput;

typedef struct
{
   UdpBtuBlock *BtuBlock;
   size_t      *BtuBlockSize;
} UdpBtuBlockOutput;

static bool TryProcessRequestUdpBtuBlock(ThisHandle       *handle,
                                         const void       *operationContext,
                                         UdpBtuBlockInput  input,
                                         UdpBtuBlockOutput output)
{
   if(input.BtuBlockSize < EMPTY_UDP_BTU_BLOCK_SIZE)
   {
      SDeviceLogStatus(handle, MODBUS_UDP_SDEVICE_STATUS_WRONG_REQUEST_SIZE);
      return false;
   }

   if(memcmp(handle->Runtime->BtuAddress, input.BtuBlock->BtuAddress, MODBUS_UDP_SDEVICE_BTU_ADDRESS_SIZE) != 0)
      return false;

   size_t responseDataSize;
   bool wasPduProcessingSuccessful =
         ModbusSDeviceBaseTryProcessRequestPdu(handle,

   if(wasPduProcessingSuccessful)
   {
      memcpy(output.BtuBlock->BtuAddress, handle->Runtime->BtuAddress, MODBUS_UDP_SDEVICE_BTU_ADDRESS_SIZE);
      *output.BtuBlockSize = EMPTY_UDP_BTU_BLOCK_SIZE + responseDataSize;
   }

   return wasPduProcessingSuccessful;
}
