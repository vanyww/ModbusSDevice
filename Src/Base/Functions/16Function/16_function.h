#pragma once

#include "../../private.h"

static ModbusSDeviceBaseProtocolException ProcessRequest16Function(void          *handle,
                                                                   const void    *operationContext,
                                                                   FunctionInput  input,
                                                                   FunctionOutput output)
{
   if(input.RequestSize < sizeof(Function16Request))
   {
      SDeviceLogStatus(handle, MODBUS_SDEVICE_STATUS_WRONG_REQUEST_SIZE);
      return MODBUS_SDEVICE_BASE_PROTOCOL_EXCEPTION_NON_PROTOCOL_ERROR;
   }

   Function16Response *response = &output.Response->AsFunction16Response;

   uint16_t registersCount = __builtin_bswap16(input.Request->AsFunction16Request.RegistersCount);
   uint16_t registersAddress = __builtin_bswap16(input.Request->AsFunction16Request.RegistersAddress);
   const void *registersData = input.Request->AsFunction16Request.RegistersData;
   uint8_t bytesToFollow = input.Request->AsFunction16Request.BytesToFollow;

   if(bytesToFollow / MODBUS_SDEVICE_BASE_REGISTER_SIZE != registersCount ||
      bytesToFollow % MODBUS_SDEVICE_BASE_REGISTER_SIZE != 0              ||
      bytesToFollow != input.RequestSize - sizeof(Function16Request))
   {
      SDeviceLogStatus(handle, MODBUS_SDEVICE_STATUS_WRONG_REGISTERS_COUNT);
      return MODBUS_SDEVICE_BASE_PROTOCOL_EXCEPTION_ILLEGAL_DATA_VALUE;
   }

   const ModbusSDeviceBaseInitData *init = SDeviceGetHandleInitData(handle);
   ModbusSDeviceBaseProtocolException operationException =
         init->WriteOperation(handle,
                              &(const ModbusSDeviceBaseWriteOperationParameters)
                              {
                                 .RegistersData    = registersData,
                                 .RegistersAddress = registersAddress,
                                 .RegistersCount   = registersCount
                              },
                              operationContext);

   if(operationException == MODBUS_SDEVICE_BASE_PROTOCOL_EXCEPTION_OK)
   {
      response->RegistersAddress = __builtin_bswap16(registersAddress);
      response->RegistersCount = __builtin_bswap16(registersCount);

      *output.ResponseSize = sizeof(Function16Response);
   }

   return operationException;
}
