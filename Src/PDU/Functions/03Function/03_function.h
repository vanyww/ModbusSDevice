#pragma once

#include "data_types.h"

#include "SDeviceCore/errors.h"

#define FUNCTION_03_MAX_REGISTERS_COUNT ((MAX_PDU_SIZE - sizeof(Function03Response)) / sizeof(ModbusSDeviceRegister))

static ModbusSDeviceProtocolException Process03FunctionRequest(ModbusSDeviceCommonHandle handle,
                                                               const ModbusProcessingParameters *parameters,
                                                               const ModbusSDeviceRequest *request,
                                                               ModbusSDeviceResponse *response)
{
   SDeviceDebugAssert(request != NULL);
   SDeviceDebugAssert(response != NULL);
   SDeviceDebugAssert(parameters != NULL);
   SDeviceDebugAssert(handle.AsAny != NULL);

   if(request->Size != sizeof(Function03Request))
   {
      SDeviceLogStatus(handle.AsAny, MODBUS_SDEVICE_STATUS_CORRUPTED_REQUEST);
      return MODBUS_SDEVICE_PROTOCOL_EXCEPTION_NON_PROTOCOL_ERROR;
   }

   const Function03Request *requestData = (const Function03Request *)request->Data;
   Function03Response *responseData = (Function03Response *)response->Data;

   if(requestData->RegistersCount > FUNCTION_03_MAX_REGISTERS_COUNT)
   {
      SDeviceLogStatus(handle.AsAny, MODBUS_SDEVICE_STATUS_CORRUPTED_REQUEST);
      return MODBUS_SDEVICE_PROTOCOL_EXCEPTION_ILLEGAL_DATA_ERROR;
   }

   uint16_t registersCount = requestData->RegistersCount;
   ModbusSDeviceProtocolException status =
         parameters->RegistersCallbacks->ReadRegisters(handle,
                                                       &(const ModbusSDeviceRegistersReadParameters)
                                                       {
                                                          .RegistersAddress = requestData->RegistersAddress,
                                                          .Data.AsPlain = responseData->RegistersBuffer,
                                                          .RequestContext = parameters->RequestContext,
                                                          .RegistersCount = registersCount
                                                       });

   if(status != MODBUS_SDEVICE_PROTOCOL_EXCEPTION_OK)
   {
      SDeviceLogStatus(handle.AsAny, MODBUS_SDEVICE_STATUS_REGISTER_ACCESS_FAIL);
      return status;
   }

   uint8_t responseValueSize = registersCount * sizeof(ModbusSDeviceRegister);
   responseData->FollowingDataBytes = responseValueSize;

   response->Size = sizeof(Function03Response) + responseValueSize;

   return MODBUS_SDEVICE_PROTOCOL_EXCEPTION_OK;
}
