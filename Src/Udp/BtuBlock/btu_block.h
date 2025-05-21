#pragma once

#include "../private.h"

#include <memory.h>

#define EMPTY_BTU_BLOCK_SIZE sizeof(BtuBlock)

typedef struct __attribute__((packed, may_alias))
{
   SDEVICE_PROPERTY_TYPE(ModbusUdp, BtuAddress) BtuAddress;
   uint8_t                                      PduBytes[];
} BtuBlock;

static bool TryProcessRequestBtuBlock(
      void                                 *this,
      ModbusSDevicePduProcessingStageInput  input,
      ModbusSDevicePduProcessingStageOutput output)
{
   ThisHandle *_this = this;

   if(input.RequestSize < EMPTY_BTU_BLOCK_SIZE)
      return false;

   const BtuBlock *request = input.RequestData;
   BtuBlock *response = output.ResponseData;

   if(memcmp(&_this->Runtime->BtuAddress, &request->BtuAddress, sizeof(request->BtuAddress)))
      return false;

   size_t responseDataSize;
   bool wasPduProcessingSuccessful =
         ModbusSDeviceInternalTryProcessRequestPdu(
               _this,
               (ModbusSDevicePduProcessingStageInput)
               {
                  .RequestData       = request->PduBytes,
                  .CallParameters    = input.CallParameters,
                  .RequestSize       = input.RequestSize - EMPTY_BTU_BLOCK_SIZE,
                  .IsOutputMandatory = input.IsOutputMandatory
               },
               (ModbusSDevicePduProcessingStageOutput)
               {
                  .ResponseData = response->PduBytes,
                  .ResponseSize = &responseDataSize
               });

   if(wasPduProcessingSuccessful)
   {
      memcpy(&response->BtuAddress, &_this->Runtime->BtuAddress, sizeof(request->BtuAddress));

      *output.ResponseSize = EMPTY_BTU_BLOCK_SIZE + responseDataSize;

      return true;
   }

   return false;
}
