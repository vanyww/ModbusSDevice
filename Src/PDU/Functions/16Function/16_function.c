#include "16_function.h"

typedef struct __attribute__((scalar_storage_order("big-endian"), packed))
{
   uint16_t DataRegisterAddress;
   uint16_t RegistersToWriteCount;
   uint8_t BytesToFollow;
   ModbusSDeviceRegister RegistersBuffer[];
} ModbusFunction16RequestData;

typedef struct __attribute__((scalar_storage_order("big-endian"), packed))
{
   uint16_t DataRegisterAddress;
   uint16_t WrittenRegistersCount;
} ModbusFunction16ResponseData;

ModbusSDeviceStatus ModbusProcess16FunctionRequest(__SDEVICE_HANDLE(Modbus) *handle,
                                                   ModbusProcessingParameters parameters,
                                                   ModbusSDeviceRequest *request,
                                                   ModbusSDeviceResponse *response)
{
   if(request->BytesCount < sizeof(ModbusFunction16RequestData))
   {
      SDeviceRuntimeErrorRaised(handle, MODBUS_SDEVICE_RUNTIME_REQUEST_SIZE_ERROR);
      return MODBUS_SDEVICE_STATUS_NON_MODBUS_ERROR;
   }

   const ModbusFunction16RequestData *requestData = request->Bytes;
   ModbusFunction16ResponseData *responseData = response->Bytes;

   if(requestData->BytesToFollow / sizeof(ModbusSDeviceRegister) != requestData->RegistersToWriteCount ||
      requestData->BytesToFollow % sizeof(ModbusSDeviceRegister) != 0                                  ||
      requestData->BytesToFollow != request->BytesCount - sizeof(ModbusFunction16RequestData))
   {
      SDeviceRuntimeErrorRaised(handle, MODBUS_SDEVICE_RUNTIME_REQUEST_REGISTER_COUNT_ERROR);
      return MODBUS_SDEVICE_STATUS_ILLEGAL_DATA_ERROR;
   }

   ModbusSDeviceStatus status =
            handle->Constant->WriteRegistersFunction(handle,
                                                     requestData->RegistersBuffer,
                                                     &(ModbusSDeviceOperationParameters)
                                                     {
                                                        .RegisterAddress = requestData->DataRegisterAddress,
                                                        .RegistersCount = requestData->RegistersToWriteCount,
                                                        .RequestContext = parameters.RequestContext
                                                     });

   if(status != MODBUS_SDEVICE_STATUS_OK)
   {
      SDeviceRuntimeErrorRaised(handle, MODBUS_SDEVICE_RUNTIME_REGISTER_ACCESS_ERROR);
      return status;
   }

   responseData->DataRegisterAddress = requestData->DataRegisterAddress;
   responseData->WrittenRegistersCount = requestData->RegistersToWriteCount;

   response->BytesCount = sizeof(ModbusFunction16ResponseData);
   return MODBUS_SDEVICE_STATUS_OK;
}
