#pragma once

#include "../private.h"

#define FUNCTION_03_MAX_REGISTERS_COUNT                                                                                \
   ((MAX_PDU_SIZE - sizeof(Function03Response)) / MODBUS_SDEVICE_REGISTER_SIZE)

typedef struct __attribute__((packed))
{
   uint16_t RegistersAddress;
   uint16_t RegistersCount;
} __attribute__((may_alias)) Function03Request;

typedef struct __attribute__((packed))
{
   uint8_t FollowingDataBytes;
   uint8_t RegistersData[];
} __attribute__((may_alias)) Function03Response;

static ModbusSDeviceProtocolException Process03FunctionRequest(
      void                    *handle,
      PduProcessingStageInput  input,
      PduProcessingStageOutput output)
{
   if(input.RequestSize != sizeof(Function03Request))
      return MODBUS_SDEVICE_PROTOCOL_EXCEPTION_NON_PROTOCOL_ERROR;

   const Function03Request *request  = input.RequestData;
   Function03Response      *response = output.ResponseData;

   uint16_t registersCount   = SWAP_UINT16_BYTES(request->RegistersCount);
   uint16_t registersAddress = SWAP_UINT16_BYTES(request->RegistersAddress);

   if(registersCount > FUNCTION_03_MAX_REGISTERS_COUNT)
      return MODBUS_SDEVICE_PROTOCOL_EXCEPTION_ILLEGAL_DATA_VALUE;

   const ModbusSDeviceInitData *init = SDeviceGetHandleInitData(handle);
   ModbusSDeviceProtocolException operationException =
         init->ReadOperation(
               handle,
               &(const ModbusSDeviceReadOperationParameters)
               {
                  .RegistersData            = response->RegistersData,
                  .RegistersCount           = registersCount,
                  .RegistersAddress         = registersAddress,
                  .IsRegistersDataMandatory = input.IsOutputMandatory
               },
               input.CallParameters);

   if(operationException != MODBUS_SDEVICE_PROTOCOL_EXCEPTION_OK)
      return operationException;

   if(input.IsOutputMandatory)
   {
      size_t readRegistersSize = registersCount * MODBUS_SDEVICE_REGISTER_SIZE;

      response->FollowingDataBytes = readRegistersSize;

      *output.ResponseSize = readRegistersSize + sizeof(Function03Response);
   }

   return MODBUS_SDEVICE_PROTOCOL_EXCEPTION_OK;
}
