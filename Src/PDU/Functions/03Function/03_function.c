#include "03_function.h"

#define __MODBUS_FUNCTION_03_MAX_REGISTERS_COUNT                                                                       \
   ((__MODBUS_MAX_PDU_SIZE - sizeof(ModbusFunction03ResponseData)) / sizeof(ModbusSDeviceRegister))

typedef struct __attribute__((scalar_storage_order("big-endian"), packed))
{
   uint16_t DataRegisterAddress;
   uint16_t RegistersToReadCount;
} ModbusFunction03RequestData;

typedef struct __attribute__((scalar_storage_order("big-endian"), packed))
{
   uint8_t FollowingDataBytes;
   ModbusSDeviceRegister RegistersBuffer[];
} ModbusFunction03ResponseData;

ModbusSDeviceStatus ModbusProcess03FunctionRequest(__SDEVICE_HANDLE(Modbus) *handle,
                                                   ModbusProcessingParameters parameters,
                                                   ModbusSDeviceRequest *request,
                                                   ModbusSDeviceResponse *response)
{
   if(request->BytesCount != sizeof(ModbusFunction03RequestData))
   {
      SDeviceRuntimeErrorRaised(handle, MODBUS_SDEVICE_RUNTIME_REQUEST_SIZE_ERROR);
      return MODBUS_SDEVICE_STATUS_NON_MODBUS_ERROR;
   }

   const ModbusFunction03RequestData *requestData = request->Bytes;
   ModbusFunction03ResponseData *responseData = response->Bytes;

   if(requestData->RegistersToReadCount > __MODBUS_FUNCTION_03_MAX_REGISTERS_COUNT)
   {
      SDeviceRuntimeErrorRaised(handle, MODBUS_SDEVICE_RUNTIME_REQUEST_REGISTER_COUNT_ERROR);
      return MODBUS_SDEVICE_STATUS_ILLEGAL_DATA_ERROR;
   }

   ModbusSDeviceStatus status =
            handle->Constant->ReadRegistersFunction(handle,
                                                    responseData->RegistersBuffer,
                                                    &(ModbusSDeviceOperationParameters)
                                                    {
                                                       .RegisterAddress = requestData->DataRegisterAddress,
                                                       .RegistersCount = requestData->RegistersToReadCount,
                                                       .RequestContext = parameters.RequestContext
                                                    });

   if(status != MODBUS_SDEVICE_STATUS_OK)
   {
      SDeviceRuntimeErrorRaised(handle, MODBUS_SDEVICE_RUNTIME_REGISTER_ACCESS_ERROR);
      return status;
   }

   responseData->FollowingDataBytes = requestData->RegistersToReadCount * sizeof(ModbusSDeviceRegister);

   response->BytesCount = sizeof(ModbusFunction03ResponseData) + responseData->FollowingDataBytes;
   return MODBUS_SDEVICE_STATUS_OK;
}
