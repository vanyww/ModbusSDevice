#include "03_function.h"

#define __FUNCTION_03_MAX_REGISTERS_COUNT                                                                              \
   ((__MODBUS_MAX_PDU_SIZE - sizeof(Function03ResponseData)) / sizeof(ModbusRegister))

typedef struct __attribute__((scalar_storage_order("big-endian"), packed))
{
   uint16_t DataRegisterAddress;
   uint16_t RegistersToReadCount;
} Function03RequestData;

typedef struct __attribute__((scalar_storage_order("big-endian"), packed))
{
   uint8_t FollowingDataBytes;
   ModbusRegister RegistersBuffer[];
} Function03ResponseData;

ModbusStatus ProcessModbus03FunctionRequest(SDeviceCommonHandle *handle,
                                            ModbusProcessingParameters parameters,
                                            ModbusRequest *request,
                                            ModbusResponse *response)
{
   if(request->Size != sizeof(Function03RequestData))
   {
      SDeviceRuntimeErrorRaised(handle, MODBUS_RUNTIME_ERROR_WRONG_REQUEST_SIZE);
      return MODBUS_STATUS_NON_MODBUS_ERROR;
   }

   const Function03RequestData *requestData = (const Function03RequestData *)request->Bytes;
   Function03ResponseData *responseData = (Function03ResponseData *)response->Bytes;
   if(requestData->RegistersToReadCount > __FUNCTION_03_MAX_REGISTERS_COUNT)
   {
      SDeviceRuntimeErrorRaised(handle, MODBUS_RUNTIME_ERROR_WRONG_REGISTERS_COUNT);
      return MODBUS_STATUS_ILLEGAL_DATA_ERROR;
   }

   size_t registersCount = requestData->RegistersToReadCount;
   const __SDEVICE_INIT_DATA(Modbus) *initData = (__SDEVICE_INIT_DATA(Modbus) *)handle->Data;
   ModbusStatus status = initData->ReadRegisters(handle,
                                                 responseData->RegistersBuffer,
                                                 &(ModbusOperationParameters)
                                                 {
                                                    .RegisterAddress = requestData->DataRegisterAddress,
                                                    .RegistersCount = registersCount,
                                                    .RequestContext = parameters.RequestContext
                                                 });

   if(status != MODBUS_STATUS_OK)
   {
      SDeviceRuntimeErrorRaised(handle, MODBUS_RUNTIME_ERROR_REGISTER_ACCESS_FAIL);
      return status;
   }

   responseData->FollowingDataBytes = registersCount * sizeof(ModbusRegister);

   response->Size = sizeof(Function03ResponseData) + responseData->FollowingDataBytes;
   return MODBUS_STATUS_OK;
}
