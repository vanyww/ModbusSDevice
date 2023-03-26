#pragma once

#include "data_types.h"

#include "SDeviceCore/errors.h"

static ModbusSDeviceProtocolException Process16FunctionRequest(ModbusSDeviceCommonHandle handle,
                                                               const ModbusProcessingParameters *parameters,
                                                               const ModbusSDeviceRequest *request,
                                                               ModbusSDeviceResponse *response)
{
   SDeviceDebugAssert(request != NULL);
   SDeviceDebugAssert(response != NULL);
   SDeviceDebugAssert(parameters != NULL);
   SDeviceDebugAssert(handle.AsAny != NULL);

   if(request->Size < sizeof(Function16Request))
   {
      SDeviceLogStatus(handle.AsAny, MODBUS_SDEVICE_STATUS_CORRUPTED_REQUEST);
      return MODBUS_SDEVICE_PROTOCOL_EXCEPTION_NON_PROTOCOL_ERROR;
   }

   const Function16Request *requestData = (const Function16Request *)request->Data;
   Function16Response *responseData = (Function16Response *)response->Data;

   if(requestData->BytesToFollow / sizeof(ModbusSDeviceRegister) != requestData->RegistersCount ||
      requestData->BytesToFollow % sizeof(ModbusSDeviceRegister) != 0                           ||
      requestData->BytesToFollow != request->Size - sizeof(Function16Request))
   {
      SDeviceLogStatus(handle.AsAny, MODBUS_SDEVICE_STATUS_CORRUPTED_REQUEST);
      return MODBUS_SDEVICE_PROTOCOL_EXCEPTION_ILLEGAL_DATA_ERROR;
   }

   uint16_t registersAddress = requestData->RegistersAddress;
   size_t registersCount = requestData->RegistersCount;
   ModbusSDeviceProtocolException status =
            parameters->RegistersCallbacks->WriteRegisters(handle,
                                                           &(const ModbusSDeviceRegistersWriteParameters)
                                                           {
                                                              .Data.AsPlain = requestData->RegistersBuffer,
                                                              .RequestContext = parameters->RequestContext,
                                                              .RegistersAddress = registersAddress,
                                                              .RegistersCount = registersCount,
                                                           });

   if(status != MODBUS_SDEVICE_PROTOCOL_EXCEPTION_OK)
   {
      SDeviceLogStatus(handle.AsAny, MODBUS_SDEVICE_STATUS_REGISTER_ACCESS_FAIL);
      return status;
   }

   responseData->RegistersAddress = registersAddress;
   responseData->RegistersCount = registersCount;

   response->Size = sizeof(Function16Response);

   return MODBUS_SDEVICE_PROTOCOL_EXCEPTION_OK;
}
