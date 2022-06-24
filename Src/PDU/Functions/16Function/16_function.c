#include "16_function.h"

typedef struct __attribute__((scalar_storage_order("big-endian"), packed))
{
   uint16_t DataRegisterAddress;
   uint16_t RegistersToWriteCount;
   uint8_t BytesToFollow;
   ModbusRegister RegistersBuffer[];
} Function16RequestData;

typedef struct __attribute__((scalar_storage_order("big-endian"), packed))
{
   uint16_t DataRegisterAddress;
   uint16_t WrittenRegistersCount;
} Function16ResponseData;

ModbusStatus ProcessModbus16FunctionRequest(SDeviceCommonHandle *handle,
                                            ModbusProcessingParameters parameters,
                                            ModbusRequest *request,
                                            ModbusResponse *response)
{
   if(request->Size < sizeof(Function16RequestData))
   {
      SDeviceRuntimeErrorRaised(handle, MODBUS_RUNTIME_ERROR_WRONG_REQUEST_SIZE);
      return MODBUS_STATUS_NON_MODBUS_ERROR;
   }

   const Function16RequestData *requestData = (const Function16RequestData *)request->Bytes;
   Function16ResponseData *responseData = (Function16ResponseData *)response->Bytes;
   if(requestData->BytesToFollow / sizeof(ModbusRegister) != requestData->RegistersToWriteCount ||
      requestData->BytesToFollow % sizeof(ModbusRegister) != 0                                  ||
      requestData->BytesToFollow != request->Size - sizeof(Function16RequestData))
   {
      SDeviceRuntimeErrorRaised(handle, MODBUS_RUNTIME_ERROR_WRONG_REGISTERS_COUNT);
      return MODBUS_STATUS_ILLEGAL_DATA_ERROR;
   }

   uint16_t address = requestData->DataRegisterAddress;
   size_t registersCount = requestData->RegistersToWriteCount;
   const __SDEVICE_INIT_DATA(Modbus) *initData = (__SDEVICE_INIT_DATA(Modbus) *)handle->Init;
   ModbusStatus status = initData->WriteRegisters(handle,
                                                  requestData->RegistersBuffer,
                                                  &(ModbusOperationParameters)
                                                  {
                                                     .RegisterAddress = address,
                                                     .RegistersCount = registersCount,
                                                     .RequestContext = parameters.RequestContext
                                                  });

   if(status != MODBUS_STATUS_OK)
   {
      SDeviceRuntimeErrorRaised(handle, MODBUS_RUNTIME_ERROR_REGISTER_ACCESS_FAIL);
      return status;
   }

   responseData->DataRegisterAddress = address;
   responseData->WrittenRegistersCount = registersCount;

   response->Size = sizeof(Function16ResponseData);
   return MODBUS_STATUS_OK;
}
