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

static ThisBaseProtocolException Process16FunctionRequest(
      void                    *handle,
      PduProcessingStageInput  input,
      PduProcessingStageOutput output)
{
   if(input.RequestSize < sizeof(Function16Request))
   {
      SDeviceLogStatus(handle, MODBUS_SDEVICE_STATUS_WRONG_REQUEST_SIZE);
      return MODBUS_SDEVICE_BASE_PROTOCOL_EXCEPTION_NON_PROTOCOL_ERROR;
   }

   const Function16Request *request  = input.RequestData;
   Function16Response      *response = output.ResponseData;

   uint8_t  bytesToFollow    = request->BytesToFollow;
   uint16_t registersCount   = SWAP_UINT16_BYTES(request->RegistersCount);
   uint16_t registersAddress = SWAP_UINT16_BYTES(request->RegistersAddress);

   if(bytesToFollow != input.RequestSize - sizeof(Function16Request)      ||
      bytesToFollow / MODBUS_SDEVICE_BASE_REGISTER_SIZE != registersCount ||
      bytesToFollow % MODBUS_SDEVICE_BASE_REGISTER_SIZE)
   {
      SDeviceLogStatus(handle, MODBUS_SDEVICE_STATUS_WRONG_REGISTERS_COUNT);
      return MODBUS_SDEVICE_BASE_PROTOCOL_EXCEPTION_ILLEGAL_DATA_VALUE;
   }

   const ThisBaseInitData *init = SDeviceGetHandleInitData(handle);
   ThisBaseProtocolException operationException =
         init->WriteOperation(
               handle,
               &(const ThisBaseWriteOperationParameters)
               {
                  .RegistersData    = request->RegistersData,
                  .RegistersAddress = registersAddress,
                  .RegistersCount   = registersCount
               },
               input.OperationContext);

   if(operationException != MODBUS_SDEVICE_BASE_PROTOCOL_EXCEPTION_OK)
      return operationException;

   if(input.IsOutputMandatory)
   {
      response->RegistersAddress = SWAP_UINT16_BYTES(registersAddress);
      response->RegistersCount   = SWAP_UINT16_BYTES(registersCount);

      *output.ResponseSize = sizeof(Function16Response);
   }

   return MODBUS_SDEVICE_BASE_PROTOCOL_EXCEPTION_OK;
}
