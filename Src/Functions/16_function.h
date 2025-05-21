#pragma once

#include "../private.h"

typedef struct __attribute__((packed))
{
   uint16_t RegistersAddress;
   uint16_t RegistersCount;
   uint8_t  BytesToFollow;
   uint8_t  RegistersData[];
} __attribute__((may_alias)) Function16Request;

typedef struct __attribute__((packed))
{
   uint16_t RegistersAddress;
   uint16_t RegistersCount;
} __attribute__((may_alias)) Function16Response;

static ModbusSDeviceProtocolException Process16FunctionRequest(
      void                                 *this,
      ModbusSDevicePduProcessingStageInput  input,
      ModbusSDevicePduProcessingStageOutput output)
{
   if(input.RequestSize < sizeof(Function16Request))
      return MODBUS_SDEVICE_PROTOCOL_EXCEPTION_NON_PROTOCOL_ERROR;

   const Function16Request *request = input.RequestData;
   Function16Response *response = output.ResponseData;

   uint8_t  bytesToFollow = request->BytesToFollow;
   uint16_t registersCount = __builtin_bswap16(request->RegistersCount);
   uint16_t registersAddress = __builtin_bswap16(request->RegistersAddress);

   if(bytesToFollow != input.RequestSize - sizeof(Function16Request) ||
         bytesToFollow / MODBUS_SDEVICE_REGISTER_SIZE != registersCount ||
         bytesToFollow % MODBUS_SDEVICE_REGISTER_SIZE)
   {
      return MODBUS_SDEVICE_PROTOCOL_EXCEPTION_ILLEGAL_DATA_VALUE;
   }

   const ModbusSDeviceInitData *init = SDeviceGetHandleInitData(this);
   ModbusSDeviceProtocolException operationException =
         init->WriteOperation(
               this,
               &(const ModbusSDeviceWriteOperationParameters)
               {
                  .RegistersData    = request->RegistersData,
                  .RegistersAddress = registersAddress,
                  .RegistersCount   = registersCount
               },
               input.CallParameters);

   if(operationException != MODBUS_SDEVICE_PROTOCOL_EXCEPTION_OK)
      return operationException;

   if(input.IsOutputMandatory)
   {
      response->RegistersAddress = __builtin_bswap16(registersAddress);
      response->RegistersCount = __builtin_bswap16(registersCount);

      *output.ResponseSize = sizeof(Function16Response);
   }

   return MODBUS_SDEVICE_PROTOCOL_EXCEPTION_OK;
}
