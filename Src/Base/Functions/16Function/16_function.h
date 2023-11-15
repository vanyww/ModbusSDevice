#pragma once

#include "../../private.h"

static ModbusSDeviceProtocolException ProcessRequest16Function(SDEVICE_HANDLE(Modbus) *handle,
                                                               const void             *operationContext,
                                                               FunctionInput           input,
                                                               FunctionOutput          output)
{
   if(input.RequestSize < sizeof(Function16Request))
   {
      SDeviceLogStatus(handle, MODBUS_SDEVICE_STATUS_WRONG_REQUEST_SIZE);
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
      SDeviceLogStatus(handle, MODBUS_SDEVICE_STATUS_WRONG_REGISTERS_COUNT);
      return MODBUS_SDEVICE_PROTOCOL_EXCEPTION_ILLEGAL_DATA_VALUE;
   }

   ModbusSDeviceProtocolException operationException =
         handle->Init->WriteOperation(handle,
                                      &(const ModbusSDeviceWriteOperationParameters)
                                      {
                                         .RegistersData    = registersData,
                                         .RegistersAddress = registersAddress,
                                         .RegistersCount   = registersCount
                                      },
                                      operationContext);

   if(operationException == MODBUS_SDEVICE_PROTOCOL_EXCEPTION_OK)
   {
      response->RegistersAddress = __builtin_bswap16(registersAddress);
      response->RegistersCount = __builtin_bswap16(registersCount);

      *output.ResponseSize = sizeof(Function16Response);
   }
   else
   {
      SDeviceLogStatus(handle, MODBUS_SDEVICE_STATUS_REGISTERS_ACCESS_FAIL);
   }

   return operationException;
}
