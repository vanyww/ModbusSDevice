#pragma once

#include "../../private.h"

#define FUNCTION_03_MAX_REGISTERS_COUNT ((MAX_PDU_SIZE - sizeof(Function03Response)) / MODBUS_SDEVICE_REGISTER_SIZE)

static ModbusSDeviceProtocolException ProcessRequest03Function(SDEVICE_HANDLE(Modbus) *handle,
                                                               const void             *operationContext,
                                                               FunctionInput           input,
                                                               FunctionOutput          output)
{
   SDeviceDebugAssert(handle != NULL);
   SDeviceDebugAssert(input.Request != NULL);
   SDeviceDebugAssert(output.Response != NULL);
   SDeviceDebugAssert(output.ResponseSize != NULL);

   if(input.RequestSize != sizeof(Function03Request))
   {
      SDeviceLogStatus(handle, MODBUS_SDEVICE_STATUS_CORRUPTED_REQUEST);
      return MODBUS_SDEVICE_PROTOCOL_EXCEPTION_NON_PROTOCOL_ERROR;
   }

   Function03Response *response = &output.Response->AsFunction03Response;

   uint16_t registersCount = __builtin_bswap16(input.Request->AsFunction03Request.RegistersCount);
   uint16_t registersAddress = __builtin_bswap16(input.Request->AsFunction03Request.RegistersAddress);

   if(registersCount > FUNCTION_03_MAX_REGISTERS_COUNT)
   {
      SDeviceLogStatus(handle, MODBUS_SDEVICE_STATUS_CORRUPTED_REQUEST);
      return MODBUS_SDEVICE_PROTOCOL_EXCEPTION_ILLEGAL_DATA_VALUE;
   }

   ModbusSDeviceProtocolException operationException =
         handle->Init->ReadOperation(handle,
                                     &(const ModbusSDeviceReadOperationParameters)
                                     {
                                        .RegistersData    = response->RegistersData,
                                        .RegistersAddress = registersAddress,
                                        .RegistersCount   = registersCount
                                     },
                                     operationContext);

   if(operationException == MODBUS_SDEVICE_PROTOCOL_EXCEPTION_OK)
   {
      size_t readRegistersSize = registersCount * MODBUS_SDEVICE_REGISTER_SIZE;
      response->FollowingDataBytes = readRegistersSize;

      *output.ResponseSize = readRegistersSize + sizeof(Function03Response);
   }
   else
   {
      SDeviceLogStatus(handle, MODBUS_SDEVICE_STATUS_REGISTERS_ACCESS_FAIL);
   }

   return operationException;
}
