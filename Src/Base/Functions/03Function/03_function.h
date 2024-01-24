#pragma once

#include "../../private.h"

#define FUNCTION_03_MAX_REGISTERS_COUNT ((MAX_PDU_SIZE - sizeof(Function03Response)) /                                 \
                                         MODBUS_SDEVICE_BASE_REGISTER_SIZE)

static ModbusSDeviceBaseProtocolException ProcessRequest03Function(void          *handle,
                                                                   const void    *operationContext,
                                                                   FunctionInput  input,
                                                                   FunctionOutput output)
{
   if(input.RequestSize != sizeof(Function03Request))
   {
      SDeviceLogStatus(handle, MODBUS_SDEVICE_STATUS_WRONG_REQUEST_SIZE);
      return MODBUS_SDEVICE_BASE_PROTOCOL_EXCEPTION_NON_PROTOCOL_ERROR;
   }

   Function03Response *response = &output.Response->AsFunction03Response;

   uint16_t registersCount = __builtin_bswap16(input.Request->AsFunction03Request.RegistersCount);
   uint16_t registersAddress = __builtin_bswap16(input.Request->AsFunction03Request.RegistersAddress);

   if(registersCount > FUNCTION_03_MAX_REGISTERS_COUNT)
   {
      SDeviceLogStatus(handle, MODBUS_SDEVICE_STATUS_WRONG_REGISTERS_COUNT);
      return MODBUS_SDEVICE_BASE_PROTOCOL_EXCEPTION_ILLEGAL_DATA_VALUE;
   }

   const ModbusSDeviceBaseInitData *init = SDeviceGetHandleInitData(handle);
   ModbusSDeviceBaseProtocolException operationException =
         init->ReadOperation(handle,
                             &(const ModbusSDeviceBaseReadOperationParameters)
                             {
                                .RegistersData    = response->RegistersData,
                                .RegistersAddress = registersAddress,
                                .RegistersCount   = registersCount
                             },
                             operationContext);

   if(operationException == MODBUS_SDEVICE_BASE_PROTOCOL_EXCEPTION_OK)
   {
      size_t readRegistersSize = registersCount * MODBUS_SDEVICE_BASE_REGISTER_SIZE;
      response->FollowingDataBytes = readRegistersSize;

      *output.ResponseSize = readRegistersSize + sizeof(Function03Response);
   }

   return operationException;
}
