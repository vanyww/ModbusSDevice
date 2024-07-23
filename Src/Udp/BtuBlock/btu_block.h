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

static bool TryProcessRequestUdpBtuBlock(
      void                    *handle,
      PduProcessingStageInput  input,
      PduProcessingStageOutput output)
{
   if(input.RequestSize < EMPTY_UDP_BTU_BLOCK_SIZE)
   {
      SDeviceLogStatus(handle, MODBUS_UDP_SDEVICE_STATUS_WRONG_REQUEST_SIZE);
      return false;
   }

   ThisHandle        *_handle  = handle;
   const UdpBtuBlock *request  = input.RequestData;
   UdpBtuBlock       *response = output.ResponseData;

   if(memcmp(_handle->Runtime->BtuAddress, request->BtuAddress, MODBUS_UDP_SDEVICE_BTU_ADDRESS_SIZE))
      return false;

   size_t responseDataSize;
   bool wasPduProcessingSuccessful =
         ModbusSDeviceBaseTryProcessRequestPdu(
               handle,
               (PduProcessingStageInput)
               {
                  .RequestData       = request->PduBytes,
                  .OperationContext  = input.OperationContext,
                  .RequestSize       = input.RequestSize - EMPTY_UDP_BTU_BLOCK_SIZE,
                  .IsOutputMandatory = input.IsOutputMandatory
               },
               (PduProcessingStageOutput)
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
