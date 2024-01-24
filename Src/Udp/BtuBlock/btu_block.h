#pragma once

#include "../private.h"

#include "SDeviceCore/common.h"

#include <memory.h>

#define EMPTY_UDP_BTU_BLOCK_SIZE sizeof(UdpBtuBlock)

typedef struct __attribute__((packed, may_alias))
{
   uint8_t BtuAddress[MODBUS_UDP_SDEVICE_BTU_ADDRESS_SIZE];
   uint8_t PduBytes[];
} UdpBtuBlock;

static bool TryProcessRequestUdpBtuBlock(void                 *handle,
                                         ProcessingStageInput  input,
                                         ProcessingStageOutput output)
{
   if(input.RequestSize < EMPTY_UDP_BTU_BLOCK_SIZE)
   {
      SDeviceLogStatus(handle, MODBUS_UDP_SDEVICE_STATUS_WRONG_REQUEST_SIZE);
      return false;
   }

   const UdpBtuBlock *request = input.RequestData;
   UdpBtuBlock *response = output.ResponseData;
   ThisHandle *_handle = handle;

   if(memcmp(_handle->Runtime->BtuAddress, request->BtuAddress, MODBUS_UDP_SDEVICE_BTU_ADDRESS_SIZE) != 0)
      return false;

   size_t responseDataSize;
   bool wasPduProcessingSuccessful =
         ModbusSDeviceBaseTryProcessRequestPdu(handle,
                                               (ProcessingStageInput)
                                               {
                                                  .RequestData       = request->PduBytes,
                                                  .OperationContext  = input.OperationContext,
                                                  .RequestSize       = input.RequestSize - EMPTY_UDP_BTU_BLOCK_SIZE,
                                                  .IsOutputMandatory = input.IsOutputMandatory
                                               },
                                               (ProcessingStageOutput)
                                               {
                                                  .ResponseData = response->PduBytes,
                                                  .ResponseSize = &responseDataSize
                                               });

   if(wasPduProcessingSuccessful)
   {
      memcpy(response->BtuAddress, _handle->Runtime->BtuAddress, MODBUS_UDP_SDEVICE_BTU_ADDRESS_SIZE);

      *output.ResponseSize = EMPTY_UDP_BTU_BLOCK_SIZE + responseDataSize;

      return true;
   }

   return false;
}
