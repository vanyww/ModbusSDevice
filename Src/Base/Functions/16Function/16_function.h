#pragma once

#include "../../private_base.h"

#include "SDeviceCore/errors.h"

static ModbusSDeviceProtocolException ProcessRequest16Function(ThisHandle    *handle,
                                                               const void    *operationContext,
                                                               FunctionInput  input,
                                                               FunctionOutput output)
{
   SDeviceDebugAssert(handle != NULL);
   SDeviceDebugAssert(input.Request!= NULL);
   SDeviceDebugAssert(output.Response != NULL);
   SDeviceDebugAssert(output.ResponseSize != NULL);

   if(input.RequestSize < sizeof(Function16Request))
   {
      SDeviceLogStatus(handle, MODBUS_SDEVICE_STATUS_CORRUPTED_REQUEST);
      return MODBUS_SDEVICE_PROTOCOL_EXCEPTION_NON_PROTOCOL_ERROR;
   }

   Function16Response *response = &output.Response->AsFunction16Response;

   uint16_t registersCount = __builtin_bswap16(input.Request->AsFunction16Request.RegistersCount);
   uint16_t registersAddress = __builtin_bswap16(input.Request->AsFunction16Request.RegistersAddress);
   const void *registersData = input.Request->AsFunction16Request.RegistersData;
   uint8_t bytesToFollow = input.Request->AsFunction16Request.BytesToFollow;

   if(bytesToFollow / MODBUS_SDEVICE_REGISTER_SIZE != registersCount ||
      bytesToFollow % MODBUS_SDEVICE_REGISTER_SIZE != 0              ||
      bytesToFollow != input.RequestSize - sizeof(Function16Request))
   {
      SDeviceLogStatus(handle, MODBUS_SDEVICE_STATUS_CORRUPTED_REQUEST);
      return MODBUS_SDEVICE_PROTOCOL_EXCEPTION_ILLEGAL_DATA_VALUE;
   }

   ModbusSDeviceProtocolException operationException =
         handle->Init.WriteOperation(handle,
                                     &(const ModbusSDeviceWriteOperationParameters)
                                     {
                                        .RegistersData    = registersData,
                                        .RegistersAddress = registersAddress,
                                        .RegistersCount   = registersCount,
                                        .Context          = operationContext
                                     });

   if(operationException != MODBUS_SDEVICE_PROTOCOL_EXCEPTION_OK)
   {
      SDeviceLogStatus(handle, MODBUS_SDEVICE_STATUS_REGISTERS_ACCESS_FAIL);
      return operationException;
   }

   response->RegistersAddress = __builtin_bswap16(registersAddress);
   response->RegistersCount = __builtin_bswap16(registersCount);

   *output.ResponseSize = sizeof(Function16Response);

   return MODBUS_SDEVICE_PROTOCOL_EXCEPTION_OK;
}
