#pragma once

#include "../private.h"

typedef struct __attribute__((packed, may_alias))
{
   uint16_t RegistersAddress;
   uint16_t RegistersCount;
   uint8_t  BytesToFollow;
   uint8_t  RegistersData[];
} Function16Request;

typedef struct __attribute__((packed, may_alias))
{
   uint16_t RegistersAddress;
   uint16_t RegistersCount;
} Function16Response;

static BaseProtocolException Process16FunctionRequest(void                 *handle,
                                                      ProcessingStageInput  input,
                                                      ProcessingStageOutput output)
{
   if(input.RequestSize < sizeof(Function16Request))
   {
      SDeviceLogStatus(handle, MODBUS_SDEVICE_STATUS_WRONG_REQUEST_SIZE);
      return MODBUS_SDEVICE_BASE_PROTOCOL_EXCEPTION_NON_PROTOCOL_ERROR;
   }

   const Function16Request *request = input.RequestData;
   Function16Response *response = output.ResponseData;

   uint8_t bytesToFollow = request->BytesToFollow;
   BiEndianUInt16 registersCount = BI_ENDIAN_UINT16_FROM_NETWORK(request->RegistersCount);
   BiEndianUInt16 registersAddress = BI_ENDIAN_UINT16_FROM_NETWORK(request->RegistersAddress);

   if(bytesToFollow != input.RequestSize - sizeof(Function16Request)               ||
      bytesToFollow / MODBUS_SDEVICE_BASE_REGISTER_SIZE != registersCount.AsNative ||
      bytesToFollow % MODBUS_SDEVICE_BASE_REGISTER_SIZE != 0)
   {
      SDeviceLogStatus(handle, MODBUS_SDEVICE_STATUS_WRONG_REGISTERS_COUNT);
      return MODBUS_SDEVICE_BASE_PROTOCOL_EXCEPTION_ILLEGAL_DATA_VALUE;
   }

   const BaseInitData *init = SDeviceGetHandleInitData(handle);
   BaseProtocolException operationException =
         init->WriteOperation(handle,
                              &(const BaseWriteOperationParameters)
                              {
                                 .RegistersData    = request->RegistersData,
                                 .RegistersAddress = registersAddress.AsNative,
                                 .RegistersCount   = registersCount.AsNative
                              },
                              input.OperationContext);

   if(operationException != MODBUS_SDEVICE_BASE_PROTOCOL_EXCEPTION_OK)
      return operationException;

   if(input.IsOutputMandatory)
   {
      response->RegistersAddress = registersAddress.AsNetwork;
      response->RegistersCount = registersCount.AsNetwork;

      *output.ResponseSize = sizeof(Function16Response);
   }

   return MODBUS_SDEVICE_BASE_PROTOCOL_EXCEPTION_OK;
}
